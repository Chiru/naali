import naali
from circuits import Component

import PythonQt.QtGui
import PythonQt.QtCore

from logindialog import LoginDialog
from user import UserDialog, User
from messagedialogs import ChatDialog

class Xmpp(Component):
    def __init__(self):
        self.xmpp = naali.framework.GetModuleQObj("XMPP")
        if not self.xmpp:
            return
        
        self.chatDialogs = []
        self.createLoginDialog()
        
        # Add 'XMPP' menu item under 'View'-menu
        self.menuToggleAction = PythonQt.QtGui.QAction("XMPP", 0)
        self.menuToggleAction.connect("triggered()", self.menuActionToggled)
        mainWindow = naali.ui.MainWindow()
        viewMenu = mainWindow.GetMenu("&View")
        if not viewMenu:
            viewMenu = mainWindow.AddMenu("&View")
        viewMenu.addAction(self.menuToggleAction)
    
    def createLoginDialog(self):
        self.loginDialog = LoginDialog(self.xmpp.getAccountManager())
        self.loginDialog.connectButton.connect('clicked(bool)', self.connectToServer)
        self.loginDialog.cancelButton.connect('clicked(bool)', self.loginDialog.hideDialog)
        self.loginDialogEnabled = True
        
    def createUserDialog(self):
        self.userDialog = UserDialog(self.client)
        self.client.connect('rosterChanged()', self.userDialog.populateUserList)
        self.client.connect('presenceChanged(QString)', self.userDialog.updateUser)
        self.client.connect('vCardChanged(QString)', self.userDialog.updateUser)
        self.userDialog.listWidget.connect('itemDoubleClicked(QListWidgetItem*)', self.__openChatDialog__)
        self.userDialog.showDialog()
        self.userDialogEnabled = True
        
    def createChatDialog(self, remotejid):
        # TODO: add icons back to chat dialogs
        chatDialog = ChatDialog(remotejid, self.chatExtension)
        chatDialog.setVisible(True)
        self.chatDialogs.append(chatDialog)
        return chatDialog
        
    def menuActionToggled(self):
        if(self.loginDialogEnabled):
            self.loginDialog.showDialog()
        if(self.userDialogEnabled and self.userDialog):
            self.userDialog.showDialog()
            
    def connectToServer(self):
        self.client = self.xmpp.newClient(self.loginDialog.serverEdit.text, self.loginDialog.jidEdit.text, self.loginDialog.passwordEdit.text)
        self.chatExtension = self.client.addExtension("Chat")
        self.client.connect('connected()', self.handleConnected())
        self.chatExtension.connect('messageReceived(QString,QString)', self.__handleMessageReceived__)
        
    def handleConnected(self):
        self.loginDialog.hideDialog()
        self.loginDialogEnabled = False
        self.createUserDialog()
        
    def __openChatDialog__(self):
        selectedjid = self.userDialog.getSelectedJid()
        if not self.__getChatDialog(selectedjid):
            self.createChatDialog(selectedjid)
        else:
            self.__getChatDialog(selectedjid).setVisible(True)
        
    def __handleMessageReceived__(self, remotejid, message):
        if not self.__getChatDialog(remotejid):
            chatDialog = createChatDialog(remotejid)
            # Dialog wasn't created when the message arrived, send the message to it now.
            chatDialog.__handleMessageReceived__(remotejid, message)
        
    def __getChatDialog(self, remotejid):
        for dialog in self.chatDialogs:
            if remotejid == dialog.getJid():
                return dialog
        return False
        
