
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

#ifndef __PUZNIAKOWSKI_MEMORYSESSIONSTORAGE_HTTP__
#define __PUZNIAKOWSKI_MEMORYSESSIONSTORAGE_HTTP__

#include "http_sessionstorage.hpp"
#include "http.hpp"
#include "http_request.hpp"
#include "http_response.hpp"

#include <chrono>
#include <ctime>
#include <unordered_map>
#include <random>


namespace tp {
namespace http {

class MemorySessionStorage : public i_SessionStorage {
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
	MemorySessionStorage();
};

}
}


#endif
