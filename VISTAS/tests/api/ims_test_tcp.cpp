/*******************************************************************************
 * Copyright (c) 2018 Airbus Operations S.A.S                                  *
 *                                                                             *
 * This program and the accompanying materials are made available under the    *
 * terms of the Eclipse Public License v. 2.0 which is available at            *
 * http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License   *
 * v. 1.0 which is available at                                                *
 * http://www.eclipse.org/org/documents/edl-v10.php.                           *
 *                                                                             *
 * SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause                            *
 *******************************************************************************/

/*
 * IMS test tcp
 */
#include "ims_test_tcp.hh"
#include "ims_test_actor.hh"

#ifdef __linux
#include <arpa/inet.h>
#include <netinet/tcp.h>
#endif
#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#endif

namespace ims_test
{

/*
   * Constant for setsockopt calls
   */
static const __attribute__((__unused__)) int zero = 0;
static const __attribute__((__unused__)) int one = 1;

//
// Socket Ctor
//
test_tcp::test_tcp(ims_test_actor_t actor_base, __attribute__((unused)) std::string ip, uint32_t port) : _clientsock(INVALID_SOCKET)
{
    ims_test::actor* actor = static_cast<ims_test::actor*>(actor_base);

    // Open a TCP socket
    _sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_IP);
    if ( _sock == INVALID_SOCKET ) {
        TEST_ABORT(actor, "Error creating test socket.");
    }

    // Reuse the addr
    if (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &one, sizeof(one)) != 0) {
        close();
        TEST_ABORT(actor, "Failed to set SO_REUSEADDR on socket !");
    }
    
    // no delay, because we want to send the packets ASAP
    if (setsockopt(_sock, IPPROTO_TCP, TCP_NODELAY, (const char *) &one, sizeof(one)) != 0) {
        close();
        TEST_ABORT(actor, "Failed to set TCP_NODELAY on socket !");
    }

    // bind the socket
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(sockaddr_in));
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(port);
#ifdef _WIN32
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
#else
    saddr.sin_addr.s_addr = inet_addr(ip.c_str());
#endif
    if (bind(_sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) != 0) {
        close();
        TEST_ABORT(actor, "Error while binding socket to port %d.", port);
    }

    if(listen(_sock, 5) != 0)
    {
        close();
        TEST_ABORT(actor, "Error while listening socket to port %d.", port);
    }

    _clientsock = accept(_sock, NULL, NULL);
    if (_clientsock <= 0)
    {
        close();
        TEST_ABORT(actor, "Error while accepting socket to port %d.", port);
    }
    
}

//
// Receive from the socket
//
uint32_t test_tcp::receive(char* buffer, uint32_t buffer_size, uint32_t timeout_us)
{
    struct timeval tv;
    tv.tv_sec = timeout_us / 1000000;
    tv.tv_usec = timeout_us % 1000000;
    
    fd_set select_set;
    FD_ZERO(&select_set);
    FD_SET(_clientsock, &select_set);
    int result = select(_clientsock + 1, &select_set, NULL, NULL, &tv);
    if (result > 0) {
        return recv(_clientsock, buffer, buffer_size, 0);
    } else {
        return 0;
    }
}

uint32_t test_tcp::send(char* buffer, uint32_t buffer_size)
{
    return ::send(_clientsock, buffer, buffer_size, 0);
}

//
// Close the socket
//
void test_tcp::close()
{
    if (_sock != INVALID_SOCKET) {
        shutdown(_sock, 2);
#ifdef _WIN32
        closesocket(_sock);
#else
        ::close(_sock);
#endif
        _sock = INVALID_SOCKET;
    }
    if (_clientsock != INVALID_SOCKET) {
        shutdown(_clientsock, 2);
#ifdef _WIN32
        closesocket(_clientsock);
#else
        ::close(_clientsock);
#endif
        _clientsock = INVALID_SOCKET;
    }
}

}
