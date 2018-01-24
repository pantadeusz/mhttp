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
	std::list < char > buf;
	do {
		auto buf = value.nextContentPart( 1024 );
		for ( char c : buf ) os << c;
	} while ( buf.size() > 0 );
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

class ResponseFileStream : public Response {
protected:
	
public:
	ResponseFileStream( const std::string &fname ) {
    	std::shared_ptr < std::ifstream > data = std::make_shared<std::ifstream> (fname, std::ios::binary );
		_code = 200;
		_codeComment = "ok";
		auto &ss = *data.get();

		if ( ss.fail() ) {
			throw std::length_error( "could not read file!" );
		}
		auto mime = getMimeType( fname );
		if ( mime.substr( 0, mime.find( "/" ) ) == "text" ) {
			_header["Content-Type"] = mime + "; charset=utf8";
		} else {
			_header["Content-Type"] = mime;
		}

		nextContentPart = [ data ]( size_t partSize )  {
			std::vector < char > ret( partSize );
			std::ifstream &ss = *data.get();

			size_t s = ss.readsome ( ret.data(), partSize );
			ret.resize( s );
			return std::list<char>( ret.begin(), ret.end() );
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