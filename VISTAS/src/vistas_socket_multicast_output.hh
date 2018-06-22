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
// Multicast output socket
//
#ifndef _VISTAS_SOCKET_MULTICAST_OUTPUT_HH_
#define _VISTAS_SOCKET_MULTICAST_OUTPUT_HH_
#include "vistas_socket.hh"

namespace vistas
{
class socket_multicast_output : public vistas::socket
{
public:

    // Create an non-initilized socket
    // You have to call create to initialize it.
    inline socket_multicast_output();

    //
    // Create and initilize a socket
    // @see create
    //
    inline socket_multicast_output(socket_address_ptr address)
    throw(ims::exception);

    //
    // Initialize an output socket
    //
    void create(socket_address_ptr address)
    throw(ims::exception);

    // Receive from the socket
    // Will always thow error::invalid_direction.
    virtual uint32_t receive(char* buffer, uint32_t buffer_size, client* client = NULL)
    throw(ims::exception);

    // Write to the socket
    virtual void send(const char* buffer, uint32_t size) throw(ims::exception);

    // Reply to the emmiter
    // Will always thow error::invalid_direction.
    virtual void reply(client& client,const char* buffer, uint32_t size)
    throw(ims::exception);

private:
    struct sockaddr_in _saddr;
    socklen_t          _socklen;
};

//***************************************************************************
// Inlines
//***************************************************************************
socket_multicast_output::socket_multicast_output() :
    vistas::socket()
{
}

socket_multicast_output::socket_multicast_output(socket_address_ptr address)
throw(ims::exception) :
    vistas::socket()
{
    create(address);
}

}
#endif
