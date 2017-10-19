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

#include <http_session.hpp>
#include <http_memorysessionstorage.hpp>

#include <catch.hpp>
#include <fakeit.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <chrono>
#include <thread>

using namespace Catch;
using namespace fakeit;
using namespace tp::http;
using namespace std::chrono_literals;

TEST_CASE( "http server with session support", "[mhttp][http][session]" ) {
    errlog = [](const std::string & e){};
    stdlog = [](const std::string & e){};
    static int port = 10990;
    port++;
    HttpWithSession srv( "localhost", port, true );
    
    srv.sGET( "/", []( Request &req, Session session )->t_Response {
        return ResponseFactory::response(session.getId());
    } );
    srv.GET( "/nosess", []( Request &req )->t_Response {
        return ResponseFactory::response("no session");
    } );

    std::thread t([&](){srv.start();});
    t.detach();
    std::this_thread::sleep_for(1s);
    SECTION("setting up session cookie") {
        Request req;
        req.method = "GET";
        req.proto = "HTTP/1.1";
        req.queryString = "/";
        req.remoteAddress = "localhost:" + std::to_string(port);
        t_Response res1 = Http::doHttpQuery(req);
        std::stringstream ss;
        ss << res1;
        std::string sessionID = ss.str();
        //for (auto h : res1.getHeader()) {
        //    std::cout << "h: " << h.first << " = " << h.second << std::endl;
        //}
        REQUIRE(("sessionId="+sessionID)==res1.getHeader()["set-cookie"]);
    }


    SECTION("session persist between requests") {
        Request req;
        req.method = "GET";
        req.proto = "HTTP/1.1";
        req.queryString = "/";
        req.remoteAddress = "localhost:" + std::to_string(port);
        t_Response res1 = Http::doHttpQuery(req);
        std::string setCookieString = res1.getHeader()["set-cookie"];
        std::stringstream ss;
        ss << res1;
        std::string sessionID = ss.str();
//        for (auto h : res1.getHeader()) {
//            std::cout << "h: " << h.first << " = " << h.second << std::endl;
//        }
        REQUIRE(("sessionId="+sessionID)==setCookieString);
        Request req2;
        req2.method = "GET";
        req2.proto = "HTTP/1.1";
        req2.queryString = "/";
        req2.remoteAddress = "localhost:" + std::to_string(port);
        req2.header["cookie"] = setCookieString;
        t_Response res2 = Http::doHttpQuery(req2);
        std::stringstream ss2;
        ss2 << res2;
        std::string sessionID2 = ss2.str();
//         for (auto h : res2.getHeader()) {
//             std::cout << "h2: " << h.first << " = " << h.second << std::endl;
//         }
//         std::cout << "assertion" << std::endl;
        REQUIRE(("sessionId="+sessionID2)==res2.getHeader()["set-cookie"]);
        REQUIRE(sessionID2==sessionID);
    }
    srv.stop();
}



TEST_CASE( "http server with session handling errors and cleanups", "[mhttp][http][session]" ) {
    errlog = [](const std::string & e){};
    stdlog = [](const std::string & e){};
    static int port = 11990;
    port++;
    HttpWithSession srv( "localhost", port, true, new MemorySessionStorage(3, 5, 5));
    
    srv.sGET( "/", []( Request &req, Session session )->t_Response {
        return ResponseFactory::response(session.getId());
    } );
    srv.GET( "/nosess", []( Request &req )->t_Response {
        return ResponseFactory::response("no session");
    } );

    std::thread t([&](){srv.start();});
    t.detach();
    std::this_thread::sleep_for(1s);

    SECTION("too many sessions test") {
        for (int i = 0; i < 10; i++) {
            Request req;
            req.method = "GET";
            req.proto = "HTTP/1.1";
            req.queryString = "/";
            req.remoteAddress = "localhost:" + std::to_string(port);
            t_Response res1 = Http::doHttpQuery(req);
            std::string setCookieString = res1.getHeader()["set-cookie"];
            std::stringstream ss;
            ss << res1;
            std::string sessionID = ss.str();
            if (i == 0) {
                REQUIRE(("sessionId="+sessionID)==res1.getHeader()["set-cookie"]);
            }
        }
        for (int i = 0; i < 10; i++) {
            Request req;
            req.method = "GET";
            req.proto = "HTTP/1.1";
            req.queryString = "/";
            req.remoteAddress = "localhost:" + std::to_string(port);
            t_Response res1 = Http::doHttpQuery(req);
            std::string setCookieString = res1.getHeader()["set-cookie"];
            std::stringstream ss;
            ss << res1;
            std::string sessionID = ss.str();
            REQUIRE_THAT( sessionID, Contains( "could not allocate another session" ) ); 
            
        }
    }
    srv.stop();
}
