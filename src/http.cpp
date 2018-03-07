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

#include "tpnet.hpp"
#include "http.hpp"
#include "hhelpers.hpp"


#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <signal.h>

//#include <sys/wait.h>

using namespace std;


namespace tp {
namespace http {


std::function<void( const std::string & )> errlog = []( const std::string &s ) {
	std::cerr << "ERROR: " << s << std::endl;
};

std::function<void( const std::string & )> stdlog = []( const std::string &s ) {
	std::cout << "LOG: " << s << std::endl;
};




// void Http::sigchld_handler( int ) {
// 	while( waitpid( -1, NULL, WNOHANG ) > 0 );
// }

void Http::GET( const std::string &mapping, t_requHandler f ) {
	urlMappings["GET"].push_back( { std::regex( mapping ), f } );
}
void Http::POST( const std::string &mapping, t_requHandler f ) {
	urlMappings["POST"].push_back( { std::regex( mapping ), f } );
}
void Http::DELETE( const std::string &mapping, t_requHandler f ) {
	urlMappings["DELETE"].push_back( { std::regex( mapping ), f } );
}



void Http::filter_GET( const std::string &mapping, t_filterHandler f ) {
	filterMappings["GET"].push_back( { std::regex( mapping ), f } );
}

void Http::filter_POST( const std::string &mapping, t_filterHandler f ) {
	filterMappings["POST"].push_back( { std::regex( mapping ), f } );
}

void Http::filter_DELETE( const std::string &mapping, t_filterHandler f ) {
	filterMappings["DELETE"].push_back( { std::regex( mapping ), f } );
}

std::string Http::readHttpRequestLine( Socket & s ) {
	char c[2] = {0, 0};
	std::string ret = "";
	auto bytesRead = s.read(  c, 1 );
	while ( bytesRead > 0 ) {
		ret = ret + c[0];
		if ( ( c[0] == '\n' ) && ( c[1] == '\r' ) ) {
			break;
		}
		c[1] = c[0];
		bytesRead = s.read(  c, 1 );
		if ( bytesRead < 0 ) {
			std::cout << "bytesRead = " << bytesRead << "; ERRNO " << strerror( errno ) << std::endl;
		}
	}
	if ( ret.length() > 2 ) return ret.substr( 0, ret.length() - 2 );
	return ret;
}

std::map<std::string, std::string> Http::readHttpHeader( Socket & sock ) {
	char c;
	std::map<std::string, std::string> retTxt;

	std::string keyvalue = "";

	while ( sock.read( &c, 1 ) > 0 ) {
		keyvalue = keyvalue + c;
		auto i = keyvalue.length();
		if ( i >= 2 ) {
			if ( ( keyvalue[i - 2] == 13 ) && ( keyvalue[i - 1] == 10 ) ) {
				if ( keyvalue == "\r\n" ) break;
				auto s = keyvalue.substr( 0, keyvalue.find( ":" ) );
				std::transform( s.begin(), s.end(), s.begin(),  ::tolower );
				retTxt[s] = trim( keyvalue.substr( keyvalue.find( ":" ) + 1, i - 2 ) );
				keyvalue = "";
			}
		}
	}
	return retTxt;
}



std::vector <char> Http::readHttpData( Socket & clientsocket, size_t dataSize ) {
	std::vector <char> ret;
	ret.reserve( dataSize );
	char dataPart[512];
	size_t dataPartSize = 0;
	while ( ( dataPartSize = clientsocket.read( dataPart, 512 ) ) > 0 ) {
		for ( size_t i = 0; i < dataPartSize; i++ ) {
			ret.push_back( dataPart[i] );
		}
		if ( dataSize <= ret.size() ) break;
	}
	if ( ret.size() < dataSize )throw std::length_error( "could not read whole request contents" );
	//if (clientsocket.read( ret.data(), dataSize ) < dataSize) throw std::length_error("could not read whole request contents");
	return ret;
}

Request Http::getRequest( Socket &clientsocket ) {
	Request requ;
	requ.remoteAddress = clientsocket.getHost();
	std::string requestLine = readHttpRequestLine( clientsocket );

	if ( requestLine.length() <= 0 ) {
		clientsocket.close(  );
		Request errrequ;
		return errrequ;
	}

	requ.header = readHttpHeader( clientsocket );

	std::stringstream ss( trim( requestLine ) );
	ss >> requ.method >> requ.queryString >> requ.proto;

	stdlog( "REQUEST[" + requ.remoteAddress + "]: " + requ.method  + " :: " + requ.queryString + " :: " + requ.proto );

	if ( requ.getContentLength() > 0 ) requ.content = readHttpData( clientsocket, requ.getContentLength() );

	return requ;
}

Request Http::applyRequestFilters( const Request req_ ) {
	// find matching path
	Request requ = req_;
	auto pth = requ.getPath();
	for ( auto &mapping : filterMappings[requ.method] ) {
		std::regex pieces_regex = mapping.first ;
		if ( std::regex_match ( pth, pieces_regex ) ) {
			mapping.second( requ ); // assign handler
			break;
		}
	}
	return requ;
}


Response Http::processRequests( const Request req ) {
	Request requ = req;

	// find matching path
	t_requHandler handler = notFoundHandler;
	auto pth = requ.getPath();
	for ( auto &mapping : urlMappings[requ.method] ) {
		std::regex pieces_regex( mapping.first );
		std::smatch pieces_match;
		if ( std::regex_match ( pth, pieces_match, pieces_regex ) ) {
			handler = mapping.second; // assign handler
			for ( size_t i = 0; i < pieces_match.size(); ++i ) {
				std::ssub_match sub_match = pieces_match[i];
				requ.pathSubmatches.push_back( sub_match.str() );
			}
			break;
		}
	}


	Response response; // odpowiedz
	try {
		response = handler( requ ); ///< actual response prepare
	} catch ( const ifstream::failure& e ) {
		std::stringstream responseStream;
		responseStream << "<p>Error: " << std::string( e.what() ) << "</p>" << std::endl;
		responseStream << "<p>path: " << requ.getPath() << "</p>" << std::endl;
		responseStream << "<p>file reading exception.</p>" << std::endl;
		response =  ResponseFactory::response( responseStream.str(), 404,  "selected file not found or file opening error" );
	} catch ( std::runtime_error &e ) {
		std::stringstream responseStream;
		responseStream << "<p>Error std::runtime_error: " << std::string( e.what() ) << "</p>" << std::endl;
		responseStream << "<p>path: " << requ.getPath() << "</p>" << std::endl;
		response =  ResponseFactory::response( responseStream.str(), 500, std::string( e.what() ) );
	} catch ( std::exception &e ) {
		std::stringstream responseStream;
		responseStream << "<p>Error std::exception: " << std::string( e.what() ) << "</p>" << std::endl;
		responseStream << "<p>path: " << requ.getPath() << "</p>" << std::endl;
		response =  ResponseFactory::response( responseStream.str(), 500,  "selected file not found" );
	} catch ( ... ) {
		response =  ResponseFactory::response( "Unknown server error!", 500,  "internal server error" );
	}
	return response;
}

int Http::sendResponse( Socket & clientsocket, Response resp ) {
	std::stringstream sent;
	sent << "HTTP/1.1 " <<  resp.code()  << " - " << resp.comment();
	sent << "\r\nServer: PuzniakowskiHttp " << VER;
	sent << "\r\nConnection: close";
	for ( auto & h : resp.header() ) {
		sent << "\r\n" << h.first << ": " << h.second;
	}
	sent << "\r\n\r\n";

	std::string sdata = sent.str();
	clientsocket.write( sdata.data(), sdata.length() );
	{
		resp.readContent( [&]( const std::vector < char > &bread, const response_status_t& ) {
			for ( char c : bread ) clientsocket.write( &c, 1 );
		} );
	}
	clientsocket.shutdown();
	return 1;
}

int Http::acceptConnection() {
	//auto now = std::chrono::system_clock::now(); // set current time

	try {
		auto clientsocket = listeningSocket.accept();
		{
			std::lock_guard<std::mutex> guard( workers_mutex );
			for ( auto it = workers.begin(); it != workers.end(); ) {
				auto status = ( *it ).wait_for( std::chrono::milliseconds( 0 ) );
				if ( status == std::future_status::ready ) {
					//auto result =
					( *it ).get(); /// TODO: getting result - this is not really useful
					it = workers.erase( it );
				} else {
					++it;
				}
			}
		}

		{
			std::lock_guard<std::mutex> guard( workers_mutex );
			workers.push_back( std::async( std::launch::async, [this, clientsocket]()->int{
				try {

					auto s = clientsocket;
					Request requ = getRequest( s );
					requ = applyRequestFilters( requ );
					Response response = processRequests( requ );
					return sendResponse( s, response );

				} catch ( ... ) {
					errlog( "cought exception in processRequests( requ )" );
					return -200;
				}

			} ) );
		}

	} catch ( ... ) {
		std::cout << "Internal server error!!!";
		return -1;
	}

	return 0;
}

Http::Http( std::string hostname, int port, int async ) : listeningSocket( hostname, std::to_string( port ) ) {
	this->async = async;
	this->port = port;
	urlMappings["GET"] = std::vector< std::pair < std::regex, t_requHandler > > ();
	urlMappings["POST"] = std::vector< std::pair < std::regex, t_requHandler > > ();
	urlMappings["DELETE"] = std::vector< std::pair < std::regex, t_requHandler > > ();
	filterMappings["GET"] = std::vector< std::pair < std::regex, t_filterHandler > > ();
	filterMappings["POST"] = std::vector< std::pair < std::regex, t_filterHandler > > ();
	filterMappings["DELETE"] = std::vector< std::pair < std::regex, t_filterHandler > > ();
	signal( SIGPIPE, SIG_IGN );
	workers.reserve( 128 );
	stdlog( "Web server at " + hostname + " : " + std::to_string( port ) );

}
Http::~Http () {
	listeningSocket.close();
	std::lock_guard<std::mutex> guard( workers_mutex );
	for ( auto it = workers.begin(); it != workers.end(); ) {
		auto status = ( *it ).wait_for( std::chrono::milliseconds( 100 ) );
		if ( status == std::future_status::ready || status == std::future_status::timeout ) {
			//auto result =
			( *it ).get(); /// TODO: getting result - this is not really useful
			it = workers.erase( it );
		} else {
			++it;
		}
	}
	stdlog( "server closed" );
}


t_requHandler getStaticFileHandler( const std::string sprefix, const bool safe_path ) {
	return [ = ]( Request & req )->Response {
		std::string fn;
		if ( req.pathSubmatches.size() > 1 ) fn = req.pathSubmatches[1];
		else fn = req.getPath();
		if ( safe_path ) {
			std::regex pieces_regex( ".*/*[.][.]/.*" );
			if ( std::regex_match ( fn, pieces_regex ) ) {
				throw std::runtime_error( "The path contains .. element. This is not allowed!!" );
			}
		}
		return ResponseFactory::responseFile( sprefix + fn );
	};
}



t_requHandler Http::notFoundHandler = []( Request & )->Response {
	return ResponseFactory::response( "Page not found", 404,  "not found" );
};

void Http::start() {
	terminateServerLoop = false;
	while( acceptConnection() >= 0 ) {
		if ( terminateServerLoop ) break;
	}
}

Response Http::doHttpQuery( Request req ) {
	std::string port = "80";
	std::string host = req.remoteAddress;
	if ( req.remoteAddress.find( ":" ) != std::string::npos ) {
		host = req.remoteAddress.substr( 0, req.remoteAddress.find( ":" ) );
		port = req.remoteAddress.substr( req.remoteAddress.find( ":" ) + 1 );
	}
	ClientSocket srv( port, host );

	{
		std::stringstream ss;

		ss << req.method << " " << req.queryString << " HTTP/1.1\r\n";
		ss << "Host: " << host << "\r\n";
		ss << "Connection: close\r\n";
		for ( auto kv : req.header )	 {
			if ( kv.first != "content-length" ) ss << kv.first << ": " << kv.second << "\r\n";
		}
		if ( req.content.size() > 0 ) {
			ss << "content-length: " << req.content.size() << "\r\n";
		}
		ss << "\r\n";
		for ( auto v : req.content )	 {
			ss << v;
		}

		std::string s = ss.str();
		srv.write( s.data(), s.size() ); // send request
	}
	srv.shutdown();
	std::string response = Http::readHttpRequestLine( srv );
	std::map<std::string, std::string> header = Http::readHttpHeader( srv );
	std::vector <char> data;
	if ( header.count( "content-length" ) > 0 ) {
		size_t s =  std::stoul( header["content-length"] );
		data = Http::readHttpData( srv, s );
		//std::cout << "readHttpData: " << std::string( data.begin(), data.end() ) << std::endl;
	} else {
		if ( header["transfer-encoding"] == "chunked" ) {

			while ( true ) {
				size_t chunkSize = 0;
				{
					std::vector < char > chunkDesc;
					char dataPart;
					int size = srv.read( &dataPart, 1 );
					while ( size == 1 ) {
						chunkDesc.push_back( dataPart );
						if ( ( chunkDesc[chunkDesc.size() - 2] == '\r' ) && ( chunkDesc[chunkDesc.size() - 1] == '\n' ) ) break;
						size = srv.read( &dataPart, 1 );
					}
					std::stringstream ss ( std::string( chunkDesc.begin(), chunkDesc.end() ) );
					ss >> std::hex >> chunkSize;
					//std::cout << "SIZE!! : " << chunkSize << std::endl;
				}
				if ( chunkSize > 0 ) {
					char dataPart[chunkSize + 1];
					int size = srv.read( dataPart, chunkSize );
					if ( size == ( int )chunkSize ) {
						data.insert( data.end(), dataPart, dataPart + size );
					}
				} else {
					break;
				}
			}
			{
				char dataPart;
				int size = srv.read( &dataPart, 1 );
				while ( size == 1 ) {
					//chunkDesc.push_back( dataPart );
					//if ( ( chunkDesc[chunkDesc.size() - 2] == '\r' ) && ( chunkDesc[chunkDesc.size() - 1] == '\n' ) ) break;
					size = srv.read( &dataPart, 1 );
				}
			}
			//std::cout << "CHUNKED!!" << std::endl;

		} else {
			char dataPart[32];
			int size = srv.read( dataPart, 32 );
			while ( ( size > 0 )  && ( size <= 32 ) ) {
				data.insert( data.end(), dataPart, dataPart + size );
				size = srv.read( dataPart, 32 );
			}
		}
		//std::cout << "dataPart: " << std::string( data.begin(), data.end() ) << std::endl;

	}
	std::string rproto;
	int rcode;
	std::string rcomment;
	std::stringstream ss( trim( response ) );
	ss >> rproto >> rcode >>  rcomment;
	auto ret = ResponseFactory::response( data, rcode, rcomment );
	ret.header( header );
	return ret;
}
}
}

