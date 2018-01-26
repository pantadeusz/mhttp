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

#ifndef __HTTP_RESPONSE__TP__HPP___
#define  __HTTP_RESPONSE__TP__HPP___

#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <sstream>
#include <list>
#include <map>
#include <functional>
#include <iostream>

namespace tp {
namespace http {

using response_status_t = int;
using response_callback_t = std::function < void ( const std::vector < char > &, const response_status_t & ) >;

class Response {
protected:
	int _code; // response code
	std::string _codeComment; // response code comment
	std::map < std::string, std::string > _header; // the headers
	bool _data_available; // additional information about availablility of data to read.
public:

	/**
	 * function for reading content. It will call response callback multiple times with chunks of data.
	 * This is similar to the solution given in NodeJS fs
	 * */
	std::function < void ( const response_callback_t & ) > readContent;
	/**
	 * get response HTTP code
	 * */
	int code() const;
	/**
	 * set response HTTP code
	 * */
	void code( const int c ) ;
	/**
	 * get response HTTP code comment
	 * */
	std::string comment() const;
	/**
	 * set response HTTP code commment
	 * */
	void comment( const std::string & ) ;

	/**
	 * get response HTTP headers
	 * */
	std::map < std::string, std::string > header() const;
	/**
	 * set response HTTP headers
	 * */
	void header( const std::map < std::string, std::string > &h ) ;
	/**
	 * set response HTTP header value
	 * */
	void header( const std::string &k, const std::string &v ) ;
};

/**
 * Response as a string buffer. It can be read multiple times.
 * */
class ResponseStringBuffer : public Response {
protected:
public:
	ResponseStringBuffer( const std::string &s_ ) ;
};

std::ostream& operator << ( std::ostream& os, Response & value_ );


namespace ResponseFactory {
Response response( const std::string &responseString, const int code = 200, const std::string &comment = "OK" );
Response responseFile( const std::string &fname, const int code = 200, const std::string &comment = "OK" );
Response response( const std::vector < char > &responseString, const int code = 200, const std::string &comment = "OK" );
};




}
}

#endif
