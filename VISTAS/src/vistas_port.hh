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
 * Base class of all ports
 */
#ifndef _VISTAS_PORT_HH_
#define _VISTAS_PORT_HH_

#include "vistas_socket.hh"

namespace vistas
{
class context;
typedef context* context_weak_ptr;

class port;
typedef shared_ptr<port> port_ptr;
typedef port*            port_weak_ptr;

class port
{
public:
    //Ctor
    port(context_weak_ptr context, socket_ptr socket);

    // Called by the socket input pool when this
    // port has to read the socket.
    // The input pool garantee that the socket will not lock.
    virtual void receive() = 0;

    // Called when this port has to be send
    virtual void send() = 0;

    // Socket access
    inline socket_ptr get_socket()            { return _socket;   }
    inline void set_socket(socket_ptr socket) { _socket = socket; }

    // Dtor
    virtual ~port() {}

protected:
    context_weak_ptr  _context;
    socket_ptr        _socket;

    // Intrusive list of queued ports (see output_queue)
private:
    friend class output_queue;
    port_weak_ptr _next_queued_port;
};

}

#endif
