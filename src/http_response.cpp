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


namespace tp {
    namespace http {


std::ostream& operator << ( std::ostream& os, t_Response & value ) {
    std::vector < char > buf;
    do {
        buf = value.getBytes(1024);
        for (char c : buf) os << c;
    } while (buf.size() > 0);
    return os;
}

Response::Response() {
}

Response::~Response() {
}


std::vector < char > ResponseStringStream::getBytes(size_t partSize) {
    std::vector < char > ret(partSize);
    size_t s = ss.readsome (ret.data(), partSize);
    ret.resize(s);
    return ret;
}

int ResponseStringStream::getCode() const{
    return _code;
}
std::string ResponseStringStream::getCodeComment() const{
    return _codeComment;
}
std::map < std::string, std::string > ResponseStringStream::getHeader() const{
    return _header;
}

int &ResponseStringStream::getCode() {
    return _code;
}
std::string &ResponseStringStream::getCodeComment() {
    return _codeComment;
}
std::map < std::string, std::string > &ResponseStringStream::getHeader() {
    return _header;
}

//////////////////////////////////////////////////////////////////////////////////

std::vector < char > t_Response::getBytes(size_t partSize) {
    return p_Response.get()->getBytes(partSize);
}

int &t_Response::getCode() {
    return p_Response.get()->getCode();
}
std::string &t_Response::getCodeComment() {
    return p_Response.get()->getCodeComment();
}
std::map < std::string, std::string > &t_Response::getHeader() {
    return p_Response.get()->getHeader();
}

int t_Response::getCode() const{
    return p_Response.get()->getCode();
}
std::string t_Response::getCodeComment() const{
    return p_Response.get()->getCodeComment();
}
std::map < std::string, std::string > t_Response::getHeader() const{
    return p_Response.get()->getHeader();
}


///////////////////////////////////////////////////////////////////////////////////


class ResponseFileStream : public Response {
public:
    int _code;
    std::string _codeComment;
	std::map < std::string, std::string > _header;

    std::ifstream ss;
    std::vector < char > getBytes(size_t partSize);
    ResponseFileStream(const std::string &fname) : ss(fname, std::ios::binary) {
        _code = 200;
        _codeComment = "ok";
    
        if (ss.fail()) {
            throw std::length_error("could not read file!"); 
        }
        auto mime = getMimeType(fname);
        if (mime.substr(0,mime.find("/")) == "text") {
            _header["Content-Type"] = mime + "; charset=utf8";
        } else {
            _header["Content-Type"] = mime;
        }
            
    };

    int getCode() const;
    std::string getCodeComment() const;
    std::map < std::string, std::string > getHeader() const;
    int &getCode() ;
    std::string &getCodeComment() ;
    std::map < std::string, std::string > &getHeader() ;
};
std::vector < char > ResponseFileStream::getBytes(size_t partSize)  {
    std::vector < char > ret(partSize);
    size_t s = ss.readsome (ret.data(), partSize);
    ret.resize(s);
    return ret;
};

int &ResponseFileStream::getCode() {
    return _code;
}
std::string &ResponseFileStream::getCodeComment() {
    return _codeComment;
}
std::map < std::string, std::string > &ResponseFileStream::getHeader() {
    return _header;
}
int ResponseFileStream::getCode() const{
    return _code;
}
std::string ResponseFileStream::getCodeComment() const{
    return _codeComment;
}
std::map < std::string, std::string > ResponseFileStream::getHeader() const{
    return _header;
}



////////////////////////////////////////////////////////////////////////

namespace ResponseFactory {

t_Response response(const std::string &responseString, const int code, const std::string &comment) {
    ResponseStringStream &res = *(new ResponseStringStream());
    res.ss << responseString;
    res.getCodeComment() = comment;
    res.getCode() = code;
    return t_Response(&res);
}

t_Response response(const std::vector < char > &responseString, const int code, const std::string &comment) {
    ResponseStringStream &res = *(new ResponseStringStream());
    for (char c : responseString) res.ss << c;
    res.getCodeComment() = comment;
    res.getCode() = code;
    return t_Response(&res);
}

t_Response responseFile( const std::string &fname, const int code, const std::string &comment) {
    
    
    ResponseFileStream &res = *(new ResponseFileStream(fname));
    res.getCodeComment() = comment;
    res.getCode() = code;
    return t_Response(&res);
}
}
    }
}