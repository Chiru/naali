import PythonQt.QtGui
import PythonQt.QtCore

class MessageDialog():
    def __init__(self, title, icon):
        self.dialog = PythonQt.QtGui.QDialog()
        self.dialog.setWindowTitle(title)
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
        
    def showDialog(self):
        self.dialog.show()
        
    def isVisible(self):
        return self.dialog.visible
        
    def setVisible(self, visible):
        self.dialog.setVisible(visible)
        
    def appendMessage(self, message):
        self.listWidget.addItem(message)
        
    def getLine(self):
        line = self.lineEdit.text
        self.lineEdit.text = ""
        return line
