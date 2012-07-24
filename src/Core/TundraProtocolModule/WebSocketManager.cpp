#include "WebSocketManager.h"
#include <boost/version.hpp>


WebSocketManager::WebSocketManager(unsigned short port)
{
    this->port = port;
    server::handler::ptr h(this);
    endpoint = new server(h);
    startServer();
}

WebSocketManager::~WebSocketManager() {

}

string WebSocketManager::getConId(connection_ptr con) {
    stringstream endpoint;
    //endpoint << con->get_endpoint();
    endpoint << con;
    return endpoint.str();
}

int WebSocketManager::parseJSON(string s, ptree &pt){
    try
    {
        stringstream ss;
        ss << s;
        boost::property_tree::json_parser::read_json(ss, pt);
        return 0;
    }
    catch(const boost::property_tree::json_parser::json_parser_error& e)
    {
        cerr << "WSManager JSON parse error: " << e.what() << endl;
        return -1;
    }


}

// Events

void WebSocketManager::on_validate(connection_ptr con){
    cout << "Request for resource: " << con->get_resource() << endl;
    cout << "origin: " << con->get_origin() << endl;

}

void WebSocketManager::on_open(connection_ptr con){
    cout << "Web Client " << con << " connected." << endl;
    //Adding the new connection to the list of connections
    connections.insert(pair<connection_ptr,string>(con,getConId(con)));
    cout << "Web clients connected: " << connections.size() << endl;

    /*
    for ( std::map<connection_ptr,std::string>::const_iterator iter = connections.begin();
       iter != connections.end(); ++iter )
       std::cout << iter->first << '\t' << iter->second << '\n';

    std::cout << std::endl;
    */

}

void WebSocketManager::on_close(connection_ptr con){
    cout << "Web Client " << con << " disconnected." << endl;

    map<connection_ptr,string>::iterator it = connections.find(con);

    if (it == connections.end()) {
        /* This client has already disconnected, we can ignore this.
        This happens when there is a deliberate "soft" disconnection
        preceeding the "hard" socket read fail or disconnect ack message. */
        return;
    }

    connections.erase(it);

}

void WebSocketManager::on_message(connection_ptr con, message_ptr msg){
    cout << "Got message: " << msg->get_payload() << endl;
    ptree pt;

    if(parseJSON(msg->get_payload(), pt) != -1) {
        cout << "Variable1?: " << pt.get<string>("variable1") << endl;

        //Echoes the message back
        con->send(msg->get_payload(),msg->get_opcode());
    }

}

void WebSocketManager::startServer()
{
    cout << "Boost version used: " << BOOST_LIB_VERSION << endl;
    cout << "Starting WebSocket server on port " << port << endl;

    try {
        //Initializing server endpoint
        //server::handler::ptr h(this);
        //server endpoint(h);

        using namespace websocketpp::log;
        endpoint->alog().unset_level(alevel::ALL);
        endpoint->elog().unset_level(elevel::ALL);

        endpoint->alog().set_level(alevel::CONNECT);
        endpoint->alog().set_level(alevel::DISCONNECT);

        endpoint->elog().set_level(elevel::RERROR);
        endpoint->elog().set_level(elevel::FATAL);

        //Getting pointer to the right function
        void(websocketpp::role::server<websocketpp::server>::*f)(uint16_t,size_t) =
                &websocketpp::role::server<websocketpp::server>::listen;

        cout << "Starting WSServer thread... \n";
        boost::shared_ptr<boost::thread> ptr(new boost::thread(f, endpoint, port, 1));
        t = ptr;
        t->detach();


    } catch (exception& e) {
        cerr << "WebSocketManager Exception: " << e.what() << endl;
    }

}


//Sync events

void WebSocketManager::addEntity(unsigned int id) {
    std::stringstream response;
    response << "{\"event\":\"addEntity\", \"id\":"  << id << "}";

    for ( std::map<connection_ptr,std::string>::const_iterator iter = connections.begin();
          iter != connections.end(); ++iter ){
        cout << "Sending response: " << response.str() << " to webclient: " << iter->second << endl;
        iter->first->send(response.str());
    }

}



