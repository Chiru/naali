// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "XMLRPCException.h"
#include "XMLRPCConnection.h"
#include "Poco/URI.h"
#include "boost/lexical_cast.hpp"

#include "HttpRequest.h"

namespace
{
/// The writer callback function used by cURL.
size_t WriteCallback(char *data, size_t size, size_t nmemb, std::vector<char> *buffer)  
{  
	if (buffer)
	{
		buffer->insert(buffer->end(), data, data + size * nmemb);
		return size * nmemb;
	}
	else
        return 0;
}
}

XMLRPCConnection::XMLRPCConnection(const std::string& address, const std::string& port)
{
    SetServerAddress(address, port);
}

void XMLRPCConnection::SetServerAddress(const std::string& address, const std::string& port) 
{ 
    std::string address_copy = address;
    if (address_copy.find("://") == std::string::npos)
        address_copy = "http://" + address_copy;
        
    Poco::URI uri = Poco::URI(address_copy);
    uri.setPort( boost::lexical_cast<int>(port) );
    
    strUrl_ = uri.toString();
}

XMLRPC_REQUEST XMLRPCConnection::Send(const char* data)
{
    HttpUtilities::HttpRequest request;
    request.SetUrl(strUrl_);
    request.SetRequestBody("text/xml", data);
    request.SetMethod(HttpUtilities::HttpRequest::Post);
    request.Perform();
    
    const std::vector<Core::u8> response_data = request.GetResponseBody();
    
    if (!request.GetSuccess())
        throw XMLRPCException(std::string("XMLRPCEPI exception in XMLRPCConnection::Send() " + request.GetReason()));

    if (response_data.size() == 0)
        throw XMLRPCException(std::string("XMLRPCEPI exception in XMLRPCConnection::Send() response data size was zero: "));			

    // Convert the XML string to a XMLRPC reply structure.
    return XMLRPC_REQUEST_FromXML((const char*)&response_data[0], (int)(response_data.size()), 0);
}
