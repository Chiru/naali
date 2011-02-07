client.FallbackConnection.connect(fallbackConnection);

function fallbackConnection()
{
    console.ExecuteCommand("connect(localhost,2345,Pertti)" );
}
