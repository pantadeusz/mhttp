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

#include "http_session.hpp"
#include "http_memorysessionstorage.hpp"

#include "http_request.hpp"
#include "http_response.hpp"

#include <iostream>
#include <list>

namespace tp {
namespace http {


std::string MemorySessionStorage::generateSessionId() {
	while ( true ) {
		int m = uniform_dist( e1 ) & 0x07fffffff;
		if ( sessions.count( std::to_string( m ) ) < 1 ) {
			return std::to_string( m );
		}
	}
}

Session &MemorySessionStorage::getSessionForRequest ( Request &req ) {
	std::lock_guard<std::mutex> guard( session_mutex );
	std::string sid = "";
	std::string cookiestring = req.header["cookie"];

	//std::cout << "getSessionForRequest cookie: " << cookiestring << " ; " << std::endl;

	std::regex pieces_regex( ".*sessionId=([0123456789][0123456789]*).*" );
	std::smatch pieces_match;

	if ( std::regex_match ( cookiestring, pieces_match, pieces_regex ) ) {
		for ( size_t i = 1; i < pieces_match.size(); ++i ) {
			std::ssub_match sub_match = pieces_match[i];
			auto fsid = sub_match.str();
			sid = fsid;
			if ( sessions.count( fsid ) > 0 ) {
				break;
			}
		}
	}
	//std::cout << "getSessionForRequest cookie - sid: " << sid << " ; " << std::endl;

	//    std::cout  << cookiestring << std::endl;
	if (( sid == "" ) || (sessions.count( sid ) == 0)) {
		if (sid == "") sid = generateSessionId();
		if ((sessions.size() % sessionCleanupCycle) == 0) {
			std::list < std::string > sessionToDelete;
			for (auto &s : sessions) {
				if (s.second.getSecondsOfLife() > sessionTimeout) {
					sessionToDelete.push_back(s.first);
				}	
			}
			for (auto &sidd : sessionToDelete) {
				sessions.erase(sidd);
			}
		}
		if ((maxSessions == -1) || ((int)sessions.size() > maxSessions)) {
			throw std::overflow_error("could not allocate another session"); 
		}
		sessions[sid].setId( sid );
	}
	return sessions[sid];
}


void MemorySessionStorage::storeSession ( Session session ) {
	std::lock_guard<std::mutex> guard( session_mutex );
	sessions[session.getId()] = session;
}


t_Response MemorySessionStorage::storeSessionForRequest ( Session session, t_Response &res ) {
	storeSession ( session );
	std::stringstream setcookie;
	setcookie << "sessionId" << "=" << session.getId();
	res.getHeader()["set-cookie"] = setcookie.str();
	//std::cout << "set-cookie header: " << res.getHeader()["set-cookie"] << std::endl;
	return res;
}


MemorySessionStorage::MemorySessionStorage(int sessionLifeTimeSeconds,int maxSessions_, int sessionCleanupCycle_)
		 : e1( r() ), 
		 uniform_dist( 1001, 1 << ( sizeof( int ) * 8 - 2 ) ),
		 sessionTimeout(sessionLifeTimeSeconds),
		 maxSessions(maxSessions_),
		 sessionCleanupCycle(sessionCleanupCycle_) {
}





}
}
