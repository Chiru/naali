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
