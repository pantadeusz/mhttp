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

#ifndef __TPNET__TP__HPP___
#define  __TPNET__TP__HPP___

#include <string>

#include <memory>

namespace tp {
namespace http {

class Socket {
protected:
    Socket(){}
public:
	std::string _host;
	std::string _port;
	std::shared_ptr<int> s;

	int write ( const void * buf, size_t bufsize );
	int read( void * buf, size_t bufsize );
    int shutdown();
    int close();

	std::string getHost() {
		return _host;
	};

	int sfd();
	Socket( int s_ );
};

class ListeningSocket : public Socket {
public:
	ListeningSocket( std::string host, std::string port );
	Socket accept();
};

class ClientSocket : public Socket {
public:

	ClientSocket( const std::string service, const std::string host );

};


}
}

#endif
