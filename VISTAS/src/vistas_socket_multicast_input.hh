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
// Multicast input socket
//
#ifndef _VISTAS_SOCKET_MULTICAST_INPUT_HH_
#define _VISTAS_SOCKET_MULTICAST_INPUT_HH_
#include "vistas_socket.hh"

namespace vistas
{

class socket_multicast_input : public socket
{
public:

    // Create an non-initilized socket
    // You have to call create to initialize it.
    inline socket_multicast_input();

    //
    // Create and initialize input socket
    // @see create
    //
    inline socket_multicast_input(socket_address_ptr address)
    throw(ims::exception);

    //
    // Initialize an input socket
    //
    void create(socket_address_ptr address)
    throw(ims::exception);

    // Receive from the socket
    uint32_t receive(char* buffer, uint32_t buffer_size, client* client = NULL)
    throw(ims::exception);

    // Write to the socket
    // Will always thow error::invalid_direction.
    void send(const char* buffer, uint32_t size)
    throw(ims::exception);


    // Reply to the emmiter
    virtual void reply(client& client, const char* buffer, uint32_t size)
    throw(ims::exception);
};

//***************************************************************************
// Inlines
//***************************************************************************
socket_multicast_input::socket_multicast_input() :
    socket()
{
}

socket_multicast_input::socket_multicast_input(socket_address_ptr address)
throw(ims::exception)
{
    create(address);
}

}
#endif
