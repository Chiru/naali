import PythonQt.QtGui
import PythonQt.QtCore

class UserDialog():
    def __init__(self, client):
        self.client = client
        self.users = []
        self.dialog = PythonQt.QtGui.QDialog()
        self.dialog.setWindowTitle("Tundra XMPP")
        self.dialog.setFixedWidth(204)
        self.dialog.setFixedHeight(301)
        self.layout = PythonQt.QtGui.QVBoxLayout(self.dialog)
        self.layout.setContentsMargins(0,0,0,0)
        self.layout.setSpacing(0)
        
        self.menuBar = PythonQt.QtGui.QMenuBar(self.dialog)
        self.menu = PythonQt.QtGui.QMenu("&File")
        self.menuBar.addMenu(self.menu)
        self.layout.addWidget(self.menuBar)
        
        self.listWidget = PythonQt.QtGui.QListWidget()
        self.scrollArea = PythonQt.QtGui.QScrollArea()
        self.scrollArea.setWidget(self.listWidget)
        self.listWidget.setFixedWidth(200)
        self.listWidget.setFixedHeight(270)
        self.layout.addWidget(self.scrollArea)
        
    # Returns the menu object
    def getMenu(self):
        return self.menu
    
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
    
    # Populate listwidget with available users in the roster
    def populateUserList(self):
        roster = self.client.getRoster()
        for userJid in roster:
            userItem = self.client.getUser(userJid)
            user = User(userItem, self.client, self.listWidget)
            if not userItem.isAvailable():
                user.__availabilityChanged__(False)
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
        userItem.connect('availabilityChanged(bool)', self.__availabilityChanged__)
    
    # Set listwidgetitem hidden when user unavailable
    def __availabilityChanged__(self, available):
        self.listItem.setHidden(not available)
        
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
