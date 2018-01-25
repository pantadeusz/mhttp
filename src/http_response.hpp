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


class ResponseData {
public:
	virtual void *get() = 0;
};

using response_status_t = int;
using response_callback_t = std::function < void ( const std::list < char > &, const response_status_t & ) >;

class Response {
protected:

	int _code;
	std::string _codeComment;
	std::map < std::string, std::string > _header;

	bool _data_available; // additional information about availablility of data to read.
public:

	std::function < void ( const response_callback_t & ) > readContent;

	void code( const int c ) ;
	int code() const;
	std::string comment() const;
	void comment( const std::string & ) ;

	std::map < std::string, std::string > header() const;
	void header( const std::map < std::string, std::string > &h ) ;
};

class ResponseStringBuffer : public Response {
protected:
public:

	ResponseStringBuffer( const std::string &s_ ) {
		_code = 200;
		_codeComment = "ok";
		_header["Content-Type"] = "text/html; charset=utf8";
		std::string sr = s_;
		readContent = [sr]( const response_callback_t &f ) {
			size_t partSize = 100;
			size_t s = 0;
			std::stringstream ss (sr );
			do {
				std::list < char > ret;
				char retB[partSize];
				s = ss.readsome ( retB, partSize );
				for ( unsigned int i = 0; i < s; i++ ) ret.push_back( retB[i] );
				if (s == partSize) {
					f(ret, false);
				} else {
					f(ret, true);
				}
			} while (s == partSize);
		};
	}
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
