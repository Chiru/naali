#pragma once
#include <websocketpp/websocketpp.hpp>
#include <boost/thread.hpp>
//Define this before including json_parser, if there is boost spirit grammar initialized on multiple threads
//#define BOOST_SPIRIT_THREADSAFE //requires boost threads
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp> //uses spirit headers
//#include <boost/foreach.hpp>
#include <string>
#include <cstring>
#include <sstream>
#include <set>
#include <exception>
#include <qstring.h>


using websocketpp::server;
using boost::property_tree::ptree;

using namespace std;


class WebSocketManager : public server::handler
{
public:
    WebSocketManager(unsigned short port);
    ~WebSocketManager();
    void startServer();
    string getConId(connection_ptr con);

    void sendUserConnected();

    int loadCollada(string path, string &data);
    int parseJSON(string s, ptree&pt);
    void createEventMsg(string event, string data, string &jsonString);

    //Sync event handlers
    void addEntity(unsigned int id);

private:
    unsigned short port;
    //List of current connections
    map<connection_ptr, string> connections;
    boost::shared_ptr<boost::thread> t;
    //server::handler::ptr handler;
    websocketpp::server *endpoint;


    //Socket events
    void on_validate(connection_ptr con);
    void on_open(connection_ptr con);
    void on_close(connection_ptr con);
    void on_message(connection_ptr con, message_ptr msg);

};
