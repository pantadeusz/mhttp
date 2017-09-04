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


std::ostream& operator << ( std::ostream& os, t_Response const& value_ ) {
    std::vector < char > buf;
    Response &value = *(value_.get());
    do {
        buf = value.getBytes(1024);
        for (char c : buf) os << c;
    } while (buf.size() > 0);
    return os;
}

Response::Response() {
    code = 200;
    codeComment = "ok";
    header["Content-Type"] = "text/html; charset=utf8";
}

Response::~Response() {
}


std::vector < char > ResponseStringStream::getBytes(size_t partSize) {
    std::vector < char > ret(partSize);
    size_t s = ss.readsome (ret.data(), partSize);
    ret.resize(s);
    return ret;
}


t_Response ResponseFactory::response(const std::string &responseString, const int code, const std::string &comment) {
    ResponseStringStream &res = *(new ResponseStringStream());
    res.ss << responseString;
    res.codeComment = comment;
    res.code = code;
    return t_Response(&res);
}

t_Response ResponseFactory::response(const std::vector < char > &responseString, const int code, const std::string &comment) {
    ResponseStringStream &res = *(new ResponseStringStream());
    for (char c : responseString) res.ss << c;
    res.codeComment = comment;
    res.code = code;
    return t_Response(&res);
}




t_Response ResponseFactory::responseFile( const std::string &fname, const int code, const std::string &comment) {
    class ResponseFileStream : public Response {
    public:
        std::ifstream ss;
        std::vector < char > getBytes(size_t partSize)  {
            std::vector < char > ret(partSize);
            size_t s = ss.readsome (ret.data(), partSize);
            ret.resize(s);
            return ret;
        };
        ResponseFileStream(const std::string &fname) : ss(fname, std::ios::binary) {
            if (ss.fail()) {
                throw std::length_error("could not read file!"); 
            }
            auto mime = getMimeType(fname);
            if (mime.substr(0,mime.find("/")) == "text") {
                header["Content-Type"] = mime + "; charset=utf8";
            } else {
                header["Content-Type"] = mime;
            }
                
        };
    };
    
    ResponseFileStream &res = *(new ResponseFileStream(fname));
    res.codeComment = comment;
    res.code = code;
    return t_Response(&res);
}
    }
}