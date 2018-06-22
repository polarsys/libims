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

//
// Unicast input socket
//
#include "vistas_socket_unicast_input.hh"
#include <errno.h>
#include <string.h>

namespace vistas
{

/*
* Constant for setsockopt calls
*/
static const __attribute__((__unused__)) int zero = 0;
static const __attribute__((__unused__)) int one = 1;

void socket_unicast_input::create(socket_address_ptr address)
throw(ims::exception)
{
    socket::create(address, SOCK_DGRAM);

    // Filter on the given multicast port
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(sockaddr_in));
    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(address->get_port());
#ifdef _WIN32
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
#else
    saddr.sin_addr.s_addr = inet_addr(address->get_ip().c_str());
#endif
    if (bind(_sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) != 0) {
        close();
        THROW_IMS_ERROR(ims_init_failure, to_string() << ": Error while binding socket to port '" << address->get_port() << "'.");
    }

}

//
// Receive from the socket
//
uint32_t socket_unicast_input::receive(char* buffer, uint32_t buffer_size, client* client)
throw(ims::exception)
{
    int res;
    if (client) {
        socklen_t socklen = sizeof(client->saddr);
        res = recvfrom(_sock, buffer, buffer_size, 0, (struct sockaddr *)&client->saddr, &socklen);
    } else {
        res = recvfrom(_sock, buffer, buffer_size, 0, NULL, NULL);
    }

    if (res < 0) {
#ifdef _WIN32
        // Data truncated to buffer size
        if (WSAGetLastError() == WSAEMSGSIZE) return buffer_size;
#endif
        THROW_IMS_ERROR(ims_implementation_specific_error, to_string() << ": Failed to read from socket. error: " << socket::getlasterror());
    }

#ifdef ENABLE_INSTRUMENTATION
    // Call handler
    if(ims_socket_handler_recv){
        ims_socket_handler_recv(buffer,
                                res,
                                inet_ntoa(client->saddr.sin_addr),
                                ntohs(client->saddr.sin_port));
    }
#endif

    return res;
}

//
// Send
//
void socket_unicast_input::send(__attribute__((__unused__)) const char* buffer,
                                __attribute__((__unused__)) uint32_t size)
throw(ims::exception)
{
    THROW_IMS_ERROR(ims_invalid_configuration, to_string() << ": Cannot send an input socket!");
}

//
// Reply to the emmiter
//
void socket_unicast_input::reply(client& client, const char* buffer, uint32_t size)
throw(ims::exception)
{
    socklen_t socklen = sizeof(client.saddr);

#ifdef ENABLE_INSTRUMENTATION
    // Call handler
    if(ims_socket_handler_reply){
        ims_socket_handler_reply(buffer,
                                 size,
                                 inet_ntoa(client.saddr.sin_addr),
                                 ntohs(client.saddr.sin_port));
    }
#endif

    int32_t sent_size = sendto(_sock, buffer, size, 0, (struct sockaddr *)&client.saddr, socklen);
    
    if (sent_size < 0 || (unsigned)sent_size != size) {
        THROW_IMS_ERROR(ims_implementation_specific_error, to_string() << ": Failed to write to socket! errno:" << socket::getlasterror());
    }
}
}
