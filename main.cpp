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

/* This file is an example usage of Http class */

#include <iostream>
#include <sstream>
#include <string>
#include <http.hpp>
#include <chrono>
#include <thread>
#include <fstream>


using namespace tp::http;
using namespace std;

int main ( int argc, char **argv ) {
    {
	Http srv( "localhost", 8090, true );
    bool loggedin = false;
    
    srv.filter_GET("/", [&]( Request &req )->void {
        req.queryString = "/index.html";
    });

    srv.filter_GET("/main.cpp", [&]( Request &req )->void {
        if (!loggedin) req.queryString = "/";
    });

    srv.GET( "/login", [&loggedin]( Request &req )->t_Response {
        loggedin = true;
        return ResponseFactory::response("Logged in :D");
    } );
    srv.GET( "/hello", []( Request &req )->t_Response {
        std::stringstream ss;
        ss << "<p>Hello world!!</p><table>";
        for (auto p : req.getParams()) ss << "<tr><td>" << p.first << "</td><td>" << p.second << "<td></tr>\r\n";
        ss << "</table>";
        return ResponseFactory::response(ss.str(), 200, "OK");
    } );

    srv.GET("/(.*)",getStaticFileHandler("",true));


    
    
    std::thread t([&](){srv.start();});
    t.detach();

    // just for test purposes:
    {
        Request req;
        req.method = "GET";
        req.proto = "HTTP/1.1";
        req.queryString = "/hello?witaj=januszu";
        req.remoteAddress = "localhost:8090";
        t_Response res = Http::doHttpQuery(req);
        std::cout << res << std::endl;
        
    }
    



    std::cout << "---server " << VER << " started. type some text to exit --" << std::endl;
    std::string i;
    std::cin >> i;
    std::cout << "Closing server" << std::endl;
    srv.stop();
    }
    std::cout << "Closing server - OK" << std::endl;
	return 0;

}
