/*

    Copyright (C) 2016,2017 Tadeusz Puźniakowski

    This file is part of cpphttpsrv.

    cpphttpsrv is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    cpphttpsrv is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cpphttpsrv; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Ten plik jest częścią cpphttpsrv.

    cpphttpsrv jest wolnym oprogramowaniem; możesz go rozprowadzać dalej
    i/lub modyfikować na warunkach Powszechnej Licencji Publicznej GNU,
    wydanej przez Fundację Wolnego Oprogramowania - według wersji 2 tej
    Licencji lub (według twojego wyboru) którejś z późniejszych wersji.

    Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
    użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
    gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
    ZASTOSOWAŃ. W celu uzyskania bliższych informacji sięgnij do
    Powszechnej Licencji Publicznej GNU.

    Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
    Powszechnej Licencji Publicznej GNU (GNU General Public License);
    jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
    Place, Fifth Floor, Boston, MA  02110-1301  USA

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

    {
        Request req;
        req.method = "GET";
        req.proto = "HTTP/1.1";
        req.queryString = "/hello?witaj=januszu";
        req.remoteAddress = "localhost:8090";
        t_Response res = Http::doHttpQuery(req);
        std::cout << res << std::endl;
        
    }
    



    std::cout << "---server started. type some text to exit --" << std::endl;
    std::string i;
    std::cin >> i;
    std::cout << "Closing server" << std::endl;
    srv.stop();
    }
    std::cout << "Closing server - OK" << std::endl;
	return 0;

}
