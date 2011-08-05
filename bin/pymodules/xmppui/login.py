import PythonQt.QtGui
import PythonQt.QtCore

class LoginDialog():
    def __init__(self, accountManager):
        self.visible = False
        self.accountManager = accountManager
        self.servers = self.accountManager.getServers()
    
        self.dialog = PythonQt.QtGui.QDialog()
        self.dialog.setWindowTitle("XMPP Login")
        self.layout = PythonQt.QtGui.QVBoxLayout(self.dialog)
        
        self.serverLabel = PythonQt.QtGui.QLabel("Host:")
        self.serverEdit = PythonQt.QtGui.QLineEdit()
        self.serverLabel.setBuddy(self.serverEdit)
        self.layout.addWidget(self.serverLabel)
        self.layout.addWidget(self.serverEdit)
        self.serverEdit.connect('editingFinished()', self.setUserCompleter)
        self.serverCompleter = PythonQt.QtGui.QCompleter(self.servers)
        self.serverCompleter.setCompletionMode(PythonQt.QtGui.QCompleter.InlineCompletion)
        self.serverEdit.setCompleter(self.serverCompleter)
        
        self.jidLabel = PythonQt.QtGui.QLabel("JabberID:")
        self.jidEdit = PythonQt.QtGui.QLineEdit()
        self.jidLabel.setBuddy(self.jidEdit)
        self.layout.addWidget(self.jidLabel)
        self.layout.addWidget(self.jidEdit)
        
        self.passwordLabel = PythonQt.QtGui.QLabel("Password:")
        self.passwordEdit = PythonQt.QtGui.QLineEdit()
        self.passwordEdit.setEchoMode(PythonQt.QtGui.QLineEdit.Password)
        self.jidLabel.setBuddy(self.passwordEdit)
        self.layout.addWidget(self.passwordLabel)
        self.layout.addWidget(self.passwordEdit)
        
        self.buttonLayout = PythonQt.QtGui.QHBoxLayout(self.dialog)
        self.connectButton = PythonQt.QtGui.QPushButton("Connect")
        self.connectButton.setDefault(True)
        self.cancelButton = PythonQt.QtGui.QPushButton("Cancel")
        self.buttonLayout.addWidget(self.cancelButton)
        self.buttonLayout.addWidget(self.connectButton)
        self.layout.addLayout(self.buttonLayout)
        #self.connectButton.connect('clicked(bool)', self.connectToServer)
        #self.cancelButton.connect('clicked(bool)', self.hideDialog)
        
    def hideDialog(self):
        self.dialog.hide()
        self.visible = False
        
    def setUserCompleter(self):
        self.users = self.accountManager.getUserJids(self.serverEdit.text)
        self.userCompleter = PythonQt.QtGui.QCompleter(self.users)
        self.userCompleter.setCompletionMode(PythonQt.QtGui.QCompleter.InlineCompletion)
        self.jidEdit.setCompleter(self.userCompleter)
        
    def showDialog(self):
        self.dialog.show()
        self.visible = True
