import PythonQt.QtGui
import PythonQt.QtCore

class ChatDialog():
    def __init__(self, remotejid, chatextension, icon=0):
        self.dialog = PythonQt.QtGui.QDialog()
        self.remoteJid = remotejid
        self.chatExtension = chatextension
        self.dialog.setWindowTitle(self.remoteJid)
        if icon != 0:
            self.dialog.setWindowIcon(icon)
        self.layout = PythonQt.QtGui.QVBoxLayout(self.dialog)
        
        self.scrollArea = PythonQt.QtGui.QScrollArea()
        self.listWidget = PythonQt.QtGui.QListWidget()
        self.scrollArea.setWidget(self.listWidget)
        self.layout.addWidget(self.scrollArea)
        
        self.sendLayout = PythonQt.QtGui.QHBoxLayout()
        self.lineEdit = PythonQt.QtGui.QLineEdit()
        self.sendButton = PythonQt.QtGui.QPushButton("Send")
        self.sendLayout.addWidget(self.lineEdit)
        self.sendLayout.addWidget(self.sendButton)
        self.layout.addLayout(self.sendLayout)
        
        self.listWidget.setFixedHeight(350)
        self.listWidget.setFixedWidth(490)
        self.dialog.setFixedHeight(350)
        self.dialog.setFixedWidth(500)
        
        self.sendButton.connect('clicked(bool)', self.__sendMessage__)
        self.chatExtension.connect('messageReceived(QString,QString)', self.__handleMessageReceived__)
        
    def isVisible(self):
        return self.dialog.visible
        
    def setVisible(self, visible):
        self.dialog.setVisible(visible)
        
    def getJid(self):
        return self.remoteJid
        
    def __handleMessageReceived__(self, userjid, message):
        if not userjid == self.remoteJid:
            return
        if not self.isVisible():
            self.setVisible(True)
        self.dialog.activateWindow()
        self.__appendMessage(userjid, message)
        
    def __sendMessage__(self):
        message = self.__getLine()
        self.chatExtension.sendMessage(self.remoteJid, message)
        self.__appendMessage("You", message)
        
    def __appendMessage(self, user, message):
        displayedMessage = "[" + user + "] " + message
        self.listWidget.addItem(displayedMessage)
        
    def __getLine(self):
        line = self.lineEdit.text
        self.lineEdit.text = ""
        return line

class ChatroomDialog():
    def __init__(self, mucextension, roomname, nickname):
        self.dialog = PythonQt.QtGui.QDialog()
        self.mucExtension = mucextension
        self.roomName = roomname
        self.nickname = nickname
        self.particpants = self.mucExtension.getParticipants(self.roomName)
        self.dialog.setWindowTitle(self.roomName)
        self.layout = PythonQt.QtGui.QVBoxLayout(self.dialog)
        
        # Create list widgets for messages and userlist 
        self.listLayout = PythonQt.QtGui.QHBoxLayout()
        
        self.messageScroll = PythonQt.QtGui.QScrollArea()
        self.messageArea = PythonQt.QtGui.QListWidget()
        self.messageScroll.setWidget(self.messageArea)
        self.messageArea.setFixedHeight(330)
        self.messageArea.setFixedWidth(450)
        self.listLayout.addWidget(self.messageScroll)
        
        self.userlistScroll = PythonQt.QtGui.QScrollArea()
        self.userlistArea = PythonQt.QtGui.QListWidget()
        self.userlistScroll.setWidget(self.userlistArea)
        self.userlistArea.setFixedHeight(330)
        self.userlistArea.setFixedWidth(140)
        self.listLayout.addWidget(self.userlistScroll)
        
        self.layout.addLayout(self.listLayout)
        
        # Create line-edit and pushbutton for sending messages
        self.sendLayout = PythonQt.QtGui.QHBoxLayout()
        self.lineEdit = PythonQt.QtGui.QLineEdit()
        self.sendButton = PythonQt.QtGui.QPushButton("Send")
        self.sendLayout.addWidget(self.lineEdit)
        self.sendLayout.addWidget(self.sendButton)
        self.layout.addLayout(self.sendLayout)
        
        self.dialog.setFixedHeight(350)
        self.dialog.setFixedWidth(640)
        
        self.sendButton.connect('clicked(bool)', self.__sendMessage__)
        self.mucExtension.connect('messageReceived(QString,QString,QString)', self.__handleMessageReceived__)
        self.mucExtension.connect('userJoinedRoom(QString,QString)', self.__handleUserJoined__)
        self.mucExtension.connect('userLeftRoom(QString,QString)', self.__handleUserLeft__)
        
    def showDialog(self):
        self.dialog.show()
        
    def getJid(self):
        return self.roomName
    
    def __sendMessage__(self):
        message = self.__getLine()
        self.mucExtension.sendMessage(self.roomName, message)
        #self.__appendMessage("You", message)
    
    def __handleMessageReceived__(self, room, sender, message):
        if not room == self.roomName:
            return
        #if sender == self.nickname:
        #    return
        self.__appendMessage(sender, message)
        
    def __handleUserJoined__(self, room, user):
        if not room == self.roomName:
            return
        self.__updateUserlist()
    
    def __handleUserLeft__(self, room, user):
        if not room == self.roomName:
            return
        self.__updateUserlist()
    
    def __updateUserlist(self):
        if not self.userlistArea.count == 0:
            for i in range(0, self.userlistArea.count()):
                self.userlistArea.takeItem(i)
        for p in self.mucExtension.getParticipants(self.roomName):
            self.userlistArea.addItem(p)
        
    def __getLine(self):
        line = self.lineEdit.text
        self.lineEdit.text = ""
        return line
        
    def __appendMessage(self, user, message):
        displayedMessage = "[" + user + "] " + message
        self.messageArea.addItem(displayedMessage)
        
class CallDialog():
    def __init__(self, callextension, remotejid, remoteresource, outgoing):
        self.callExtension = callextension
        self.remoteJid = remotejid
        self.remoteResource = remoteresource
        self.dialog = PythonQt.QtGui.QDialog()
        self.dialog.setWindowTitle("Call with " + self.remoteJid)
        self.layout = PythonQt.QtGui.QVBoxLayout(self.dialog)
        
        self.statusLabel = PythonQt.QtGui.QLabel("Connecting...")
        self.layout.addWidget(self.statusLabel)
        
        self.buttonLayout = PythonQt.QtGui.QHBoxLayout()
        self.pauseButton = PythonQt.QtGui.QPushButton("Pause")
        self.disconnectButton = PythonQt.QtGui.QPushButton("Disconnect")
        self.redialButton = PythonQt.QtGui.QPushButton("Re-dial")
        self.redialButton.setEnabled(False)
        self.buttonLayout.addWidget(self.pauseButton)
        self.buttonLayout.addWidget(self.disconnectButton)
        self.buttonLayout.addWidget(self.redialButton)
        
        self.layout.addLayout(self.buttonLayout)
        
        self.callExtension.connect('activeCallChanged(QString)', self.__handleCallConnected__)
        self.callExtension.connect('callDisconnected(QString)', self.__handleCallDisconnected__)
        self.pauseButton.connect('clicked(bool)', self.__muteCall__)
        self.disconnectButton.connect('clicked(bool)', self.__disconnectCall__)
        self.redialButton.connect('clicked(bool)', self.__callUser__)
        
        if(outgoing):
            self.__callUser__()
        
    def showDialog(self):
        self.dialog.show()
        
    def getJid(self):
        return self.peerJid()
        
    def __handleCallConnected__(self, remoteJid):
        if not remoteJid == self.remoteJid:
            print("derp, wrong call: {0}".format(remoteJid))
            return
        self.statusLabel.setText("Connected")
        self.redialButton.setEnabled(False)
        self.disconnectButton.setEnabled(True)
        self.pauseButton.setEnabled(True)
    
    def __handleCallDisconnected__(self, remoteJid):
        if not remoteJid == self.remoteJid:
            print("derp, wrong call: {0}".format(remoteJid))
            return
        self.statusLabel.setText("Disconnected")
        self.redialButton.setEnabled(True)
        self.pauseButton.setEnabled(False)
        self.disconnectButton.setEnabled(False)
        
    def __callUser__(self):
        self.callExtension.callUser(self.remoteJid, self.remoteResource)
        self.statusLabel.setText("Connecting")
        
    def __muteCall__(self):
        print("Not implemented")
    
    def __disconnectCall__(self):
        self.callExtension.disconnectCall(self.remoteJid)
