
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

#ifndef __PUZNIAKOWSKI_SESSION_HTTP__
#define __PUZNIAKOWSKI_SESSION_HTTP__

#include "http.hpp"
#include "http_request.hpp"
#include "http_response.hpp"

#include <chrono>
#include <ctime>
#include <unordered_map>
#include <random>


namespace tp {
namespace http {

// default session container
class i_SessionData {
public:
	/**
	 * Method for getting string representation of session data
	 * */
	virtual std::string toString() = 0;
	/**
	 * Saving data to session represented by string
	 * */
	virtual void fromString(const std::string  &str) = 0;
};

class StringSessionData : public i_SessionData {
public:
	std::string data;
	std::string toString() {
		return data;
	}
	void fromString(const std::string  &str) {
		data = str;
	}
};

/**
 * The session meta data. It records session ID, timestamp of session creation and 
 * session data.
 * */
class Session {
protected:
	std::chrono::time_point<std::chrono::system_clock> createTime; // time since epoch
	std::string _id;
public:
	std::shared_ptr < i_SessionData > data;
	virtual i_SessionData *getSessionData() {
		return data.get();
	}

	virtual void setSessionData(i_SessionData *d) {
		data = std::shared_ptr < i_SessionData >(d);
	}
	virtual std::string getId() const {
		return _id;
	}
	virtual void setId( const std::string &id ) {
		_id = id;
	}
	virtual double getSecondsOfLife() {
		return std::chrono::duration_cast<std::chrono::seconds>(
				   std::chrono::system_clock::now() - createTime ).count();
	}
	Session() {
		createTime = std::chrono::system_clock::now();
	}
};

class i_SessionStorage {
public:
	virtual Session &getSessionForRequest ( tp::http::Request &req ) = 0;
	virtual tp::http::t_Response storeSessionForRequest ( Session session, tp::http::t_Response &res ) = 0;
};


class SessionStorage : public i_SessionStorage {
protected:
	std::mutex session_mutex;
	
	std::unordered_map< std::string, Session > sessions;
	std::random_device r;
	std::default_random_engine e1;
	std::uniform_int_distribution<int> uniform_dist;

    std::string generateSessionId();    
public:

    tp::http::t_Response storeSessionForRequest ( Session session, tp::http::t_Response &res );        
	Session &getSessionForRequest ( tp::http::Request &req );
	SessionStorage();
};



class HttpWithSession : public Http {
private:
// std::function < t_Response ( Request &, Session & )>
public:	
	SessionStorage sessionStorage;
	
	Session &getSession(tp::http::Request &req) {
		return sessionStorage.getSessionForRequest(req);
	}
	tp::http::t_Response saveSession(Session &session, tp::http::t_Response &res) {
		return sessionStorage.storeSessionForRequest(session, res);
	}

	void sGET( const std::string &mapping, std::function < t_Response ( Request &, Session & ) > f );

	void sPOST( const std::string &mapping, std::function < t_Response ( Request &, Session & ) > f );


	HttpWithSession  (std::string hostname = "localhost", int port = 8080, int async = false) 
		: Http( hostname, port, async ) {

	}
	
};

}
}


#endif
