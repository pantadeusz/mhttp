
/*

    Copyright (C) 2017 Tadeusz Puźniakowski

    This file is part of mhttp.

	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	you can contact me via http://www.puzniakowski.pl

*/

#ifndef __PUZNIAKOWSKI_HTTP__
#define __PUZNIAKOWSKI_HTTP__

#include "http_request.hpp"
#include "http_response.hpp"
#include "tpnet.hpp"


#include <string>
#include <functional>
#include <map>
#include <vector>
#include <future>
#include <utility>
#include <regex>

namespace tp {
namespace http {



typedef std::function < t_Response ( Request & )> t_requHandler;
typedef std::function < void( Request & ) > t_filterHandler;



typedef class Http {
private:

	// method
	std::map < std::string, std::vector < std::pair < std::regex, t_requHandler > > > urlMappings;
	std::map < std::string, std::vector < std::pair < std::regex, t_filterHandler > > > filterMappings;

	/// tasks
	std::mutex workers_mutex;
	std::vector< std::future < int > > workers;

	// accepting socket
	ListeningSocket listeningSocket;

	// current port
	int port;
	// is async
	int async;

	std::future < int > acceptingWorker;

	bool terminateServerLoop; ///< does the main server loop should be terminated?
	

	static Request getRequest( SocketInterface & clientsocket );
	Request applyRequestFilters( const Request req_ );
	t_Response processRequests( const Request req ) ;
	static int sendResponse( SocketInterface & clientsocket, t_Response response );


public:

	/**
	 * @brief Reads first line of HTTP request
	 * 
	 * @param s connected socket
	 * @return  std::string first line of http transmission
	 ************/
	static std::string readHttpRequestLine( SocketInterface & s );

	/**
	 * @brief Read HTTP header lines and form map of the Key - Value pairs of header lines
	 * 
	 * @param sock connected socket
	 * @return std::map<std::string, std::string> 
	 ************/
	static std::map<std::string, std::string> readHttpHeader( SocketInterface & sock );

	/**
	 * @brief Read HTTP data - especially for POST requests
	 * 
	 * @param clientsocket conneced socket
	 * @param dataSize the size of data determined by content-size
	 * @return std::vector <char> returned data
	 ************/
	static std::vector <char> readHttpData( SocketInterface & clientsocket, size_t dataSize );



	/**
	 * @brief adds handler for GET requests. It will check mappings in order
	 * 
	 * @param mapping the URL pattern (regex) for request
	 * @param f request handler for mapping
	 ************/
	virtual void GET( const std::string &mapping, t_requHandler f );
	/**
	 * @brief adds handler for POST requests. It will check mappings in order
	 * 
	 * @param mapping the URL pattern (regex) for request
	 * @param f request handler for mapping
	 ************/
	virtual void POST( const std::string &mapping, t_requHandler f );
		/**
	 * @brief adds handler for DELETE requests. It will check mappings in order
	 * 
	 * @param mapping the URL pattern (regex) for request
	 * @param f request handler for mapping
	 ************/
	virtual void DELETE( const std::string &mapping, t_requHandler f );

	/**
	 * @brief adds filter for mapping. Filter will transform Request. It chan change path and other elements of the request
	 * 
	 * @param mapping the regex for matchin URL
	 * @param f handler - the function that will transform requests
	 ************/
	void filter_GET( const std::string &mapping, t_filterHandler f );
	/**
	 * @brief adds filter for mapping. Filter will transform Request. It chan change path and other elements of the request
	 * 
	 * @param mapping the regex for matchin URL
	 * @param f handler - the function that will transform requests
	 ************/
	void filter_POST( const std::string &mapping, t_filterHandler f );
	/**
	 * @brief adds filter for mapping. Filter will transform Request. It chan change path and other elements of the request
	 * 
	 * @param mapping the regex for matchin URL
	 * @param f handler - the function that will transform requests
	 ************/
	void filter_DELETE( const std::string &mapping, t_filterHandler f );


	/**
	 * function that accepts new connection. It also parses the request and dispatches new task to handle it
	 */
	int acceptConnection();

	/**
	 * constructor creates working server
	*/
	Http( std::string hostname = "localhost", int port = 8080, int async = false );
	virtual ~Http ();

	/**
	 * The default handler for 404 error
	 * */
	static t_requHandler notFoundHandler;

	void start();
	void stop() {
		terminateServerLoop = true;
	};

	// performs get operation
	static t_Response doHttpQuery( Request req );
} Http;


/**
 * Get static file handler for serving static sites.
 *
 * @param prefix path to data
 * @param safe_path shold throw exception on any /../ elements
 * @return returns newly created t_requestHandler
 * */
t_requHandler getStaticFileHandler( const std::string sprefix = "", const bool safe_path = true );



/**
 * @brief the function that will accept error logs
 ************/
extern std::function<void( const std::string & )> errlog;
/**
 * @brief the function that will accept logs
 ************/
extern std::function<void( const std::string & )> stdlog;



}
}


#endif
