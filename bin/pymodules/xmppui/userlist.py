import PythonQt.QtGui
import PythonQt.QtCore

from message import MessageDialog

class UserDialog():
    def __init__(self, client):
        self.visible = False
        self.client = client
        self.users = []
        self.dialog = PythonQt.QtGui.QDialog()
        self.dialog.setWindowTitle("Tundra XMPP")
        self.dialog.setFixedWidth(204)
        self.dialog.setFixedHeight(304)
        self.layout = PythonQt.QtGui.QVBoxLayout(self.dialog)
        self.layout.setContentsMargins(0,0,0,0)
        self.layout.setSpacing(0)
        
        self.listWidget = PythonQt.QtGui.QListWidget()
        self.scrollArea = PythonQt.QtGui.QScrollArea()
        self.scrollArea.setWidget(self.listWidget)
        self.listWidget.setFixedWidth(200)
        self.listWidget.setFixedHeight(270)
        self.layout.addWidget(self.scrollArea)
        
        self.buttonLayout = PythonQt.QtGui.QHBoxLayout()
        self.addContactButton = PythonQt.QtGui.QPushButton("+")
        self.settingsButton = PythonQt.QtGui.QPushButton("S")
        self.buttonSpacer = PythonQt.QtGui.QSpacerItem(180, 1)
        self.buttonLayout.addWidget(self.addContactButton)
        self.buttonLayout.addSpacerItem(self.buttonSpacer)
        self.buttonLayout.addWidget(self.settingsButton)
        self.layout.addLayout(self.buttonLayout)
        
        self.client.connect('rosterChanged()', self.populateUserList)
        self.client.connect('presenceChanged(QString)', self.updateUser)
        self.client.connect('vCardChanged(QString)', self.updateUser)

    def getUserByName(self, name):
        for user in self.users:
            if(user.fullName == name):
                    return user
                    
    def getUserByJid(self, userJid):
        for user in self.users:
            if(user.jid == userJid):
                return user
        
    def populateUserList(self):
        roster = self.client.getRoster()
        for userJid in roster:
            userItem = self.client.getUser(userJid)
            user = User(userItem, self.client, self.listWidget)
            self.users.append(user)
            
    def updateUser(self, userJid):
        for user in self.users:
            if(user.jid == userJid):
                user.update()
                break
            
    def showDialog(self):
        self.visible = True
        self.dialog.show()
        
        
class User():
    def __init__(self, userItem, client, listWidget):
        self.userItem = userItem
        self.jid = userItem.getJid()
        self.client = client
        self.listItem = PythonQt.QtGui.QListWidgetItem(listWidget)
        self.hasDialog = False
        
    def update(self):
        self.fullName = self.userItem.getFullName()
        print "debug: ", len(self.fullName)
        pixmap = PythonQt.QtGui.QPixmap()
        if not(pixmap.loadFromData(self.userItem.getPhoto())):
            pixmap = PythonQt.QtGui.QPixmap(30,30)
            pixmap.fill()
        self.picture = PythonQt.QtGui.QIcon()
        self.picture.addPixmap(pixmap)
        self.listItem.setIcon(self.picture)
        self.listItem.setText(self.fullName)
        
    def createDialog(self):
        self.dialog = MessageDialog(self.fullName, self.picture)
        self.dialog.sendButton.connect('clicked(bool)', self.sendMessage)
        self.hasDialog = True
        self.dialog.showDialog()
    
    def sendMessage(self):
        message = self.dialog.getLine()
        self.client.sendMessage(self.jid, message)
        
        displayedMessage = "[You] " + message
        self.dialog.appendMessage(displayedMessage)
        
    def messageReceived(self, message):
        if not(self.hasDialog):
            self.createDialog()
            
        if not(self.dialog.isVisible()):
            self.dialog.setVisible(True)
            
        displayedMessage = "[" + self.fullName + "] " + message
        self.dialog.appendMessage(displayedMessage)
