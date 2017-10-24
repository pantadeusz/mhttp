
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

#ifndef __PUZNIAKOWSKI_SESSIONSTORAGE_HTTP__
#define __PUZNIAKOWSKI_SESSIONSTORAGE_HTTP__

#include "http_request.hpp"
#include "http_response.hpp"

#include <chrono>
#include <ctime>
#include <unordered_map>



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
	virtual void fromString( const std::string  &str ) = 0;
};

class StringSessionData : public i_SessionData {
public:
	std::string data;
	std::string toString() ;
	void fromString( const std::string  &str ) ;
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
	virtual i_SessionData *getSessionData();
	/**
	 * @brief 
	 * 
	 * @param d 
	 * @return  void 
	 **/
	virtual void setSessionData( i_SessionData *d );
	virtual std::string getId() const;
	virtual void setId( const std::string &id );
	virtual double getSecondsOfLife();
	Session();
};

class i_SessionStorage {
public:
	virtual std::string generateSessionId() = 0;
	virtual Session &getSessionForRequest ( tp::http::Request &req ) = 0;
	virtual tp::http::t_Response storeSessionForRequest ( Session session, tp::http::t_Response &res ) = 0;
	virtual void storeSession ( Session session ) = 0;
};


}
}


#endif
