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


#include "tpnet.hpp"


#include <iostream>
#include <netdb.h>
#include <sstream>
#include <stdexcept>
#include <unistd.h>


namespace tp {
    namespace http {
    
ConnectedSocket::ConnectedSocket(int s) : refcounter(new int), sfd(s){

}
ConnectedSocket::ConnectedSocket() : refcounter(new int), sfd(-1){

}

ConnectedSocket::~ConnectedSocket() {
    if (refcounter.use_count() == 1) {
        ::close (sfd);
    }
}

int ConnectedSocket::write (const void * buf, size_t bufsize) {
    return ::send( (int)sfd, (const void *)buf, (size_t)bufsize, MSG_NOSIGNAL );
}

int ConnectedSocket::read(void * buf, size_t bufsize) {
    return ::read(sfd, (char *)buf, bufsize);
}

int ConnectedSocket::close() {
    return ::close (sfd);
}

ListeningSocket::ListeningSocket(std::string port) {
    int yes = 1;
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s;

    for (unsigned int i = 0; i < sizeof(struct addrinfo); i++) ((char *)&hints)[i] = 0;
    hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints.ai_protocol = 0;          /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    s = getaddrinfo(NULL, port.c_str(), &hints, &result);
    if (s != 0) {
        std::stringstream ss;
        ss << "ListeningSocket getaddrinfo:: " << gai_strerror(s) << "; port= " << port;
        throw std::invalid_argument(ss.str());
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (sfd == -1) continue;

        if ( setsockopt( sfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof( yes ) ) == -1 ) {
            throw std::invalid_argument("error at: setsockopt( sfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof( int ) )");
        }

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break; 
        }
        ::close(sfd);
    }

    if (rp == NULL) {
        std::stringstream ss;
        ss << "ListeningSocket unable to bind address:: " << gai_strerror(s) << "; " << port;
        throw std::invalid_argument(ss.str());
    }
    freeaddrinfo(result);
    if ( listen( sfd, 32 ) == -1 ) {
        throw std::invalid_argument("listen error");
	}

}

ConnectedSocket ListeningSocket::accept() {
    struct sockaddr_storage peer_addr;	
    socklen_t  peer_addr_len = sizeof(struct sockaddr_storage);
    
    std::string host_s;
    std::string service_s;
    int csfd;
    
    if ( ( csfd = ::accept( sfd, ( struct sockaddr * )&peer_addr, &peer_addr_len ) ) == -1 ) {
        throw std::invalid_argument("could not accept connection!");
    } else {
        char host[NI_MAXHOST], service[NI_MAXSERV];

        //int s = 
        getnameinfo((struct sockaddr *) &peer_addr,
                        peer_addr_len, host, NI_MAXHOST,
                        service, NI_MAXSERV, NI_NUMERICSERV);
        host_s	= host;
        service_s = service;
    }
    ConnectedSocket ret(csfd);
    ret._host = host_s;
    ret._service = service_s;
    return ret;
}


ClientSocket::ClientSocket(int s) : ConnectedSocket(s) {}

ClientSocket ClientSocket::connectTo(const std::string proto, const std::string remoteAddress) {
    int sfd;

    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s;

    for (unsigned i = 0; i < sizeof(struct addrinfo); i++) ((char *)&hints)[i] = 0;
    
    hints.ai_family = AF_UNSPEC;    // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // Stream socket
    hints.ai_flags = 0;
    hints.ai_protocol = 0;          /// Any protocol

    s = getaddrinfo(remoteAddress.c_str(), proto.c_str(), &hints, &result);
    if (s != 0) {
        std::stringstream ss;
        ss << "getaddrinfo:: " << gai_strerror(s) << "; " << proto << "://" << remoteAddress;
        throw std::invalid_argument(ss.str());
    }
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
                        rp->ai_protocol);
        if (sfd == -1)
            continue;
        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
            break;                  // Success
        ::close(sfd);
    }

    if (rp == NULL) {               // No address succeeded
        throw std::invalid_argument("connection error");
    }

    freeaddrinfo(result);
    ClientSocket ret(sfd);
    ret._host = remoteAddress;
    ret._service = proto;
    return ret;
}



}
}

