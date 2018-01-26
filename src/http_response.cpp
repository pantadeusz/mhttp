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

#include "http_response.hpp"

#include "hhelpers.hpp"

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iostream>

namespace tp {
namespace http {


std::ostream& operator << ( std::ostream& os, Response & value ) {
	value.readContent( [&]( const std::vector < char > &bread, const response_status_t& ) {
		for ( char c : bread ) os << c;
	} );
	return os;
}

int Response::code() const {
	return _code;
}
std::string Response::comment() const {
	return _codeComment;
}
std::map < std::string, std::string > Response::header() const {
	return _header;
}

void Response::code( const int c ) {
	_code = c;
}
void Response::comment( const std::string &cc ) {
	_codeComment = cc;
}
void Response::header( const std::map < std::string, std::string > &h ) {
	_header = h;
}
void Response::header( const std::string &k, const std::string &v ) {
	_header[k] = v;
}


ResponseStringBuffer::ResponseStringBuffer( const std::string &s_ ) {
	_code = 200;
	_codeComment = "ok";
	_header["Content-Type"] = "text/html; charset=utf8";
	std::string sr = s_;
	readContent = [sr]( const response_callback_t &f ) {
		std::vector < char > ret( sr.begin(), sr.end() );
		f( ret, false );
	};
}

class ResponseFileStream : public Response {
protected:

public:
	ResponseFileStream( const std::string &fname_ ) {
		_code = 200;
		_codeComment = "ok";

		std::string fname = fname_;

		std::ifstream ss( fname, std::ios::binary );

		if ( ss.fail() ) {
			throw std::length_error( "could not read file!" );
		}
		auto mime = getMimeType( fname );
		if ( mime.substr( 0, mime.find( "/" ) ) == "text" ) {
			_header["Content-Type"] = mime + "; charset=utf8";
		} else {
			_header["Content-Type"] = mime;
		}


		readContent = [fname]( const response_callback_t &f ) {
			std::ifstream ss( fname, std::ios::binary );

			if ( ss.fail() ) {
				throw std::length_error( "could not read file!" );
			}
			size_t partSize = 4096;
			size_t s = 0;
			while ( ss.good() ) {
				std::vector < char > ret( partSize );
				s = ss.readsome ( ret.data(), partSize );
				if (s == 0) break;
				ret.resize( s );
				f( ret, false );
			};
		};
	};
};


////////////////////////////////////////////////////////////////////////

namespace ResponseFactory {

Response response( const std::string &responseString, const int code, const std::string &comment ) {
	ResponseStringBuffer res( responseString );
	res.comment( comment );
	res.code( code );
	return res;
}

Response response( const std::vector < char > &responseString, const int code, const std::string &comment ) {
	ResponseStringBuffer res( std::string( responseString.begin(), responseString.end() ) );
	res.comment( comment );
	res.code( code );
	return res;
}

Response responseFile( const std::string &fname, const int code, const std::string &comment ) {
	ResponseFileStream res( fname );
	res.comment( comment );
	res.code( code );
	return res ;
}
}
}
}