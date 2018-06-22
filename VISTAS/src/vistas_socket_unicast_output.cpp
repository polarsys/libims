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
// Unicast output socket
//
#include "vistas_socket_unicast_output.hh"
#include <errno.h>
#include <string.h>

namespace vistas
{

/*
   * Constant for setsockopt calls
   */
static const __attribute__((__unused__)) int zero = 0;
static const __attribute__((__unused__)) int one = 1;

void socket_unicast_output::create(socket_address_ptr address)
throw(ims::exception)
{
    socket::create(address, SOCK_DGRAM);

    // If the outgoing port is specified, bind the socket to this port
    if (address->get_outgoing_port()) {
        struct sockaddr_in saddr;
        memset(&saddr, 0, sizeof(sockaddr_in));
        saddr.sin_family = PF_INET;
        saddr.sin_port = htons(address->get_outgoing_port());
        saddr.sin_addr.s_addr = htonl(INADDR_ANY); // This parameter seem ignored. It don't specify the outgoing interface.
        if (bind(_sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in)) != 0) {
            close();
            THROW_IMS_ERROR(ims_init_failure, to_string() << ": Error while binding socket to port '" << address->get_outgoing_port() << "'.");
        }
    }

    // Set destination multicast address
    _saddr.sin_family = PF_INET;
    _saddr.sin_addr.s_addr = inet_addr(address->get_ip().c_str());
    _saddr.sin_port = htons(address->get_port());
    _socklen = sizeof(struct sockaddr_in);
}

//
// Receive from the socket
//
uint32_t socket_unicast_output::receive(__attribute__((__unused__)) char* buffer,
                                        __attribute__((__unused__)) uint32_t buffer_size,
                                        __attribute__((__unused__)) client* client)
throw(ims::exception)
{
    THROW_IMS_ERROR(ims_invalid_configuration, to_string() << ": We can't receive from an output socket!");
}

//
// Write to the socket
//
void socket_unicast_output::send(const char* buffer, uint32_t size)
throw(ims::exception)
{
#ifdef ENABLE_INSTRUMENTATION
    // Call handler
    if(ims_socket_handler_send){
        ims_socket_handler_send(buffer,
                                size,
                                inet_ntoa(_saddr.sin_addr),
                                ntohs(_saddr.sin_port));
    }
#endif

    int32_t sent_size = sendto(_sock, buffer, size, 0, (struct sockaddr *)&_saddr, _socklen);

    if (sent_size < 0 || (unsigned)sent_size != size) {
        THROW_IMS_ERROR(ims_implementation_specific_error, to_string() << ": Failed to write to socket! errno:" << socket::getlasterror());
    }
}

//
// Reply to the emmiter
//
void socket_unicast_output::reply(__attribute__((__unused__)) client& client,
                                  __attribute__((__unused__)) const char* buffer,
                                  __attribute__((__unused__)) uint32_t size)
throw(ims::exception)
{
    THROW_IMS_ERROR(ims_invalid_configuration, to_string() << ": We can't reply from an output socket!");
}

}
