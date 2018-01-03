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

#include <map>

namespace tp {
    namespace http {
    
class Response {
    
public:
    
    /**
     * Returns fragment of response body
     */
    Response();
    virtual ~Response();

    virtual std::vector < char > getBytes(size_t partSize ) = 0;

    virtual int &getCode()  = 0;
    virtual std::string &getCodeComment()  = 0;
    virtual std::map < std::string, std::string > &getHeader()  = 0;

    virtual int getCode() const = 0;
    virtual std::string getCodeComment() const = 0;
    virtual std::map < std::string, std::string > getHeader() const = 0;

    
};



class ResponseStringStream : public Response {
public:
    int _code;
    std::string _codeComment;
	std::map < std::string, std::string > _header;

    std::stringstream ss;
    std::vector < char > getBytes(size_t partSize);

    int getCode() const;
    std::string getCodeComment() const;
    std::map < std::string, std::string > getHeader() const;
    int &getCode() ;
    std::string &getCodeComment() ;
    std::map < std::string, std::string > &getHeader() ;

    ResponseStringStream() {
        _code = 200;
        _codeComment = "ok";
        _header["Content-Type"] = "text/html; charset=utf8";
    }    
};

class t_Response : public Response {
public:
    std::shared_ptr<Response> p_Response;
    std::vector < char > getBytes(size_t partSize );
    int &getCode() ;
    std::string &getCodeComment() ;
    std::map < std::string, std::string > &getHeader() ;
    int getCode() const;
    std::string getCodeComment() const;
    std::map < std::string, std::string > getHeader() const;

    t_Response(Response *newResponsePointer) : p_Response(newResponsePointer) {

    }
    t_Response() {
        
    }
};

//std::shared_ptr<Response> newResponsePtr(std::string responseType);
// typedef std::shared_ptr<Response> t_Response;
std::ostream& operator << ( std::ostream& os, t_Response & value_ );


namespace ResponseFactory {
    t_Response response( const std::string &responseString, const int code = 200, const std::string &comment = "OK");
    t_Response responseFile( const std::string &fname, const int code = 200, const std::string &comment = "OK");
    t_Response response( const std::vector < char > &responseString, const int code = 200, const std::string &comment = "OK");
};



    
    }
}    

#endif
