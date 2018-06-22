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
 * IMS test Muticast input
 */
#include "ims_test_mc_input.hh"
#include "ims_test_actor.hh"

#ifdef __linux
#include <arpa/inet.h>
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
mc_input::mc_input(ims_test_actor_t actor_base, std::string ip, uint32_t port)
{
    ims_test::actor* actor = static_cast<ims_test::actor*>(actor_base);

    // Open a UDP socket
    _sock = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if ( _sock == INVALID_SOCKET) {
        TEST_ABORT(actor, "Error creating test socket.");
    }

    // Reuse the addr
    if (setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, (const char*) &one, sizeof(one)) != 0) {
        close();
        TEST_ABORT(actor, "Failed to set SO_REUSEADDR on socket !");
    }

    // Filter on the given multicast port
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

    // Join the multicast group
    struct ip_mreq imreq;
    memset(&imreq, 0, sizeof(struct ip_mreq));
    imreq.imr_multiaddr.s_addr = inet_addr(ip.c_str());
    imreq.imr_interface.s_addr = htonl(INADDR_ANY); // First interface
    if (setsockopt(_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char *)&imreq, sizeof(struct ip_mreq)) != 0) {
        close();
        TEST_ABORT(actor, "Failed to join multicast group %s!", ip.c_str());
    }

}

//
// Receive from the socket
//
uint32_t mc_input::receive(char* buffer, uint32_t buffer_size, uint32_t timeout_us)
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_us;
    
    fd_set select_set;
    FD_ZERO(&select_set);
    FD_SET(_sock, &select_set);

    int result = select(_sock + 1, &select_set, NULL, NULL, &tv);
    if (result > 0) {
        return recvfrom(_sock, buffer, buffer_size, 0, NULL, NULL);
    } else {
        return 0;
    }
}

//
// Close the socket
//
void mc_input::close()
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
}

}
