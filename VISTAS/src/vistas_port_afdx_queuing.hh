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
 * Afdx queuing port
 */
#ifndef _VISTAS_PORT_AFDX_QUEUING_HH_
#define _VISTAS_PORT_AFDX_QUEUING_HH_
#include "vistas_port_application.hh"
#include "vistas_message_queuing_afdx.hh"

namespace vistas
{
class port_afdx_queuing;
typedef shared_ptr<port_afdx_queuing> port_afdx_queuing_ptr;

class port_afdx_queuing: public port_application<message_queuing_afdx_ptr>
{
public:
    port_afdx_queuing(context_weak_ptr context, socket_ptr socket, uint32_t max_size, uint32_t expected_max_size, uint32_t queue_depth);
    ~port_afdx_queuing();

    // Lookup/initialize the message associated with this port
    ims::message_ptr get_message(std::string name,
                                 std::string     local_name);

    // Read/write data from socket
    void send();
    void receive();

    // Return port protocol
    virtual ims_protocol_t get_protocol() { return ims_afdx; }

    // Add / Remove message. May throw.
    void push(const char* message, uint32_t message_size)
    throw(ims::exception);
    uint32_t pop(char* message, uint32_t message_max_size);

    uint32_t get_queue_data(char      *data,
                            uint32_t  max_size,
                            uint32_t  queue_index = 0)
    throw(ims::exception);

    inline uint32_t pending_count() { return _nb_messages; }

    // Empty queue
    void reset();

private:
    struct queued_message_t {
        char*    data;
        uint32_t size;
    };

    uint32_t           _max_size;
    uint32_t           _expected_max_size;
    uint32_t           _message_queue_depth;
    uint32_t           _queue_depth;
    queued_message_t*  _queue;
    uint32_t           _nb_messages;
    uint32_t           _begin;
    uint32_t           _fifo_size;
    char*              _fifo;
};

}

#endif

