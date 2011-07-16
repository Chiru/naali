engine.ImportExtension("qt.core");
engine.ImportExtension("qt.gui");

var label = new QLabel();
label.indent = 10;
label.text = "This scene demonstrates the use of the Menu component.\nFor using, activate 3DMenu attribute from 3DCanvasSource component\nunder \"Screen\" entity.\nMenu opens when canvas is clicked.";
label.resize(660,70);
label.setStyleSheet("QLabel {background-color: transparent; font-size: 16px; }");

var proxy = new UiProxyWidget(label);
ui.AddProxyWidgetToScene(proxy);
proxy.x = 150;
proxy.y = 50;
proxy.windowFlags = 0;
proxy.visible = true;
