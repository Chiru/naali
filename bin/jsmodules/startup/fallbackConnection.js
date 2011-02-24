client.FallbackConnection.connect(fallbackConnection);

function fallbackConnection()
{
    //const QString& address, unsigned short port, const QString& username, const QString& password, const QString &protocol = QString()
    client.Login("localhost",2345,"user","","udp");
}
