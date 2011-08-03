import naali
from circuits import Component

import PythonQt.QtGui
import PythonQt.QtCore

from login import LoginDialog
from userlist import UserDialog

mainWindow = naali.ui.MainWindow()
xmpp = naali.framework.GetModuleQObj("XMPPModule")

class xmppUi(Component):
    def __init__(self):
        self.loginDialogEnabled = True
        self.userDialogEnabled = False
        self.loginDialog = LoginDialog(xmpp.getAccountManager())
        self.loginDialog.connectButton.connect('clicked(bool)', self.connectToServer)
        self.loginDialog.cancelButton.connect('clicked(bool)', self.loginDialog.hideDialog)
        
        self.menuToggleAction = PythonQt.QtGui.QAction("XMPP", 0)
        self.menuToggleAction.connect("triggered()", self.menuActionToggled)
        viewMenu = mainWindow.GetMenu("&View")
        if not viewMenu:
            viewMenu = mainWindow.AddMenu("&View")
        viewMenu.addAction(self.menuToggleAction)
        
    def menuActionToggled(self):
        if(self.loginDialogEnabled):
            self.loginDialog.showDialog()
        if(self.userDialogEnabled and self.userDialog):
            self.userDialog.showDialog()

    def connectToServer(self):
        self.client = xmpp.newClient(self.loginDialog.serverEdit.text, self.loginDialog.jidEdit.text, self.loginDialog.passwordEdit.text)
        self.client.connect('PrivateMessageReceived(QString,QString)', self.messageReceived)
        self.client.connect('Connected()', self.createUserDialog())
        
    def createUserDialog(self):
        self.loginDialog.hideDialog()
        self.loginDialogEnabled = False
        self.userDialog = UserDialog(self.client)
        self.userDialog.listWidget.connect('itemDoubleClicked(QListWidgetItem*)', self.openMessageDialog)
        self.userDialog.showDialog()
        self.userDialogEnabled = True
            
    def messageReceived(self, userJid, message):
        user = self.userDialog.getUserByJid(userJid)
        if(user):
            user.messageReceived(message)

    def openMessageDialog(self):
        item = self.userDialog.listWidget.currentItem()
        user = self.userDialog.getUserByName(item.text())
        if(user):
            user.createDialog()
        else:
            print "User not found"
