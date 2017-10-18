
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

#include "http.hpp"
#include "http_request.hpp"
#include "http_response.hpp"

namespace tp {
namespace http {


	std::string SessionStorage::generateSessionId() {
		while ( true ) {
			int m = uniform_dist( e1 );
			if ( sessions.count(std::to_string( m )) < 1 ) {
				return std::to_string( m );
			}
		}
	}
	
	Session &SessionStorage::getSessionForRequest ( Request &req ) {
		std::string sid = "";
		std::string cookiestring = req.header["cookie"];
		
		std::regex pieces_regex( ".*sessionId=([0123456789]*)[^0123456789]*$" );
		std::smatch pieces_match;
		if ( std::regex_match ( cookiestring, pieces_match, pieces_regex ) ) {
			for (size_t i = 0; i < pieces_match.size(); ++i) {
				std::ssub_match sub_match = pieces_match[i];
				auto fsid=sub_match.str();
				if (sessions.count(fsid) > 0) sid = fsid;
			}   
			
		}
	
	//    std::cout  << cookiestring << std::endl;
		if ( sid == "" ) {
			sid = generateSessionId();
			sessions[sid].setId(sid);
		}
		return sessions[sid];
	}
	
	
	t_Response SessionStorage::storeSessionForRequest ( Session session, t_Response &res ) {
		sessions[session.getId()] = session;
		std::stringstream setcookie;
		setcookie << "sessionId" << "=" << session.getId();
		res.getHeader()["Set-Cookie"] = setcookie.str();
		return res;
	}
	
	
	SessionStorage::SessionStorage() : e1( r() ), uniform_dist( 1000001, 1 << ( sizeof( int ) * 8 - 1 ) ) {
	
	}
	
		


}
}
