import naali
from circuits import Component

import PythonQt.QtGui
import PythonQt.QtCore

from logindialog import LoginDialog
from user import UserDialog, User
from messagedialogs import ChatDialog, ChatroomDialog

class Xmpp(Component):
    def __init__(self):
        self.xmpp = naali.framework.GetModuleQObj("XMPP")
        if not self.xmpp:
            return
        
        self.chatroomDialogs = []
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
        self.userDialogEnabled = False
        
    def createUserDialog(self):
        self.userDialog = UserDialog(self.client)
        self.joinroomDialog = JoinRoomDialog(self.client.getHost())
        
        if self.mucExtension:
            filemenu = self.userDialog.getMenu()
            self.joinRoomAction = PythonQt.QtGui.QAction("Join room", 0)
            self.joinRoomAction.connect("triggered()", self.__selectChatroom__)
            filemenu.addAction(self.joinRoomAction)
        
        self.client.connect('rosterChanged()', self.userDialog.populateUserList)
        self.client.connect('presenceChanged(QString)', self.userDialog.updateUser)
        self.client.connect('vCardChanged(QString)', self.userDialog.updateUser)
        
        self.joinroomDialog.okButton.connect('clicked(bool)', self.__joinChatroom__)
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
        self.mucExtension = self.client.addExtension("Muc")
        self.client.connect('connected()', self.handleConnected())
        self.chatExtension.connect('messageReceived(QString,QString)', self.__handleMessageReceived__)
        self.mucExtension.connect('roomAdded(QString,QString)', self.__createChatroomDialog__)
        self.mucExtension.connect('roomRemoved(QString,QString)', self.__handleChatroomRemoved__)
        
    def handleConnected(self):
        self.loginDialog.hideDialog()
        self.loginDialogEnabled = False
        self.createUserDialog()
        
    def __selectChatroom__(self):
        self.joinroomDialog.showDialog()
        
    def __joinChatroom__(self):
        roomname = self.joinroomDialog.getRoom()
        nickname = self.joinroomDialog.getNickname()
        if roomname == "" or nickname == "":
            return
        self.mucExtension.joinRoom(roomname, nickname)
        self.joinroomDialog.hideDialog()
        
    def __createChatroomDialog__(self, roomjid, nickname):
        if not self.__getChatroomDialog(roomjid):
            roomdialog = ChatroomDialog(self.mucExtension, roomjid, nickname)
            roomdialog.showDialog()
            self.chatroomDialogs.append(roomdialog)
        
    def __openChatDialog__(self):
        selectedjid = self.userDialog.getSelectedJid()
        if not self.__getChatDialog(selectedjid):
            self.createChatDialog(selectedjid)
        else:
            self.__getChatDialog(selectedjid).setVisible(True)
            
    def __handleChatroomRemoved__(self, room, reason):
        if not self.__getChatroomDialog(room):
            return
        self.chatroomDialogs.remove(self.__getChatroomDialog(room))
        print("Chatroom \"{0}\" removed, reason: {1}".format(room, reason))
        
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
        
    def __getChatroomDialog(self, roomjid):
        for dialog in self.chatroomDialogs:
            if roomjid == dialog.getJid():
                return dialog
        return False
        
class JoinRoomDialog():
    def __init__(self, host):
        self.host = host
        self.dialog = PythonQt.QtGui.QDialog()
        self.dialog.setWindowTitle("Join chatroom")
        self.layout = PythonQt.QtGui.QVBoxLayout(self.dialog)
        
        self.roomLabel = PythonQt.QtGui.QLabel("Room:")
        self.roomEdit = PythonQt.QtGui.QLineEdit()
        self.roomLabel.setBuddy(self.roomEdit)
        self.layout.addWidget(self.roomLabel)
        self.layout.addWidget(self.roomEdit)
        
        self.hostLabel = PythonQt.QtGui.QLabel("Server:")
        self.hostEdit = PythonQt.QtGui.QLineEdit("conference." + self.host)
        self.hostLabel.setBuddy(self.hostEdit)
        self.layout.addWidget(self.hostLabel)
        self.layout.addWidget(self.hostEdit)
        
        self.nickLabel = PythonQt.QtGui.QLabel("Nickname:")
        self.nickEdit = PythonQt.QtGui.QLineEdit()
        self.nickLabel.setBuddy(self.nickEdit)
        self.layout.addWidget(self.nickLabel)
        self.layout.addWidget(self.nickEdit)
        
        self.vSpacer = PythonQt.QtGui.QSpacerItem(1,10)
        self.layout.addSpacerItem(self.vSpacer)
        
        self.buttonLayout = PythonQt.QtGui.QHBoxLayout()
        self.cancelButton = PythonQt.QtGui.QPushButton("Cancel")
        self.okButton = PythonQt.QtGui.QPushButton("Join")
        self.okButton.setDefault(True)
        self.buttonLayout.addWidget(self.cancelButton)
        self.buttonLayout.addWidget(self.okButton)
        
        self.layout.addLayout(self.buttonLayout)
        
        self.cancelButton.connect('clicked(bool)', self.hideDialog)
        
    def showDialog(self):
        self.dialog.show()
        
    def hideDialog(self):
        self.roomEdit.text = ""
        self.roomEdit.text = ""
        self.dialog.setVisible(False)
        
    def getRoom(self):
        if self.roomEdit.text == "":
            return ""
        return self.roomEdit.text + "@" + self.hostEdit.text
        
    def getNickname(self):
        return self.nickEdit.text
        
        
