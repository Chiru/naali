import PythonQt.QtGui
import PythonQt.QtCore

class UserDialog():
    def __init__(self, client):
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
    
    # Returns User object for given jid   
    def getUser(self, userJid):
        for user in self.users:
            if(user.jid == userJid):
                return user
    
    # Returns all jids in userlist
    def getUserList(self):
        users = []
        for user in self.users:
            users.append(user.jid())
        return users
        
    # Get jid of the currently selected user
    def getSelectedJid(self):
        item = self.listWidget.currentItem()
        return item.text()
    
    # Fetch   
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
        self.dialog.show()
        
    def hideDialog(self):
        self.dialog.hide()   
        
class User():
    def __init__(self, userItem, client, listWidget):
        self.userItem = userItem
        self.client = client
        self.listWidget = listWidget
        self.jid = userItem.getJid()
        self.listItem = PythonQt.QtGui.QListWidgetItem(listWidget)
        self.listItem.setText(self.jid)
    
    # Update user's listitem 
    def update(self):
        pixmap = PythonQt.QtGui.QPixmap()
        if not(pixmap.loadFromData(self.userItem.getPhoto())):
            pixmap = PythonQt.QtGui.QPixmap(30,30)
            pixmap.fill()
        self.picture = PythonQt.QtGui.QIcon()
        self.picture.addPixmap(pixmap)
        self.listItem.setIcon(self.picture)
            
    # Returns users fullname if set
    def fullname(self):
        return self.userItem.getFullName()
