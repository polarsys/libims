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
 * NAD port
 */
#ifndef _VISTAS_PORT_NAD_HH_
#define _VISTAS_PORT_NAD_HH_
#include "vistas_port_application.hh"
#include "vistas_message_nad.hh"

namespace vistas
{
class port_nad;
typedef shared_ptr<port_nad> port_nad_ptr;

class port_nad: public port_application<message_nad_ptr>
{
public:
    port_nad(context_weak_ptr context, socket_ptr socket, uint32_t fifo_size, uint32_t send_period_us);
    ~port_nad();

    // Lookup/initialize the message associated with this port
    ims::message_ptr get_nad_message(std::string name,
                                     uint32_t    size,
                                     uint32_t    offset,
                                     std::string     local_name,
                                     std::string     bus_name,
                                     uint32_t        period_us,
                                     ims_nad_type_t        nad_type,
                                     uint32_t              nad_dim1,
                                     uint32_t              nad_dim2);

    
    // Send data to socket
    // The port discret will effectivelly send only if time has reach its period.
    void send();

    // Read data from socket
    void receive();

    // Return port protocol
    virtual ims_protocol_t get_protocol() { return ims_nad; }

    // Return true if this port is a periodic output one
    virtual bool is_periodic_output() { return _socket->get_address()->get_direction() == ims_output; }

    // "ims_reset_all" also resets the simulation time, so we must reset our date of next send
    virtual ims_return_code_t reset_messages() throw(ims::exception)
    {
        _send_next_date_us = 0;

        return port_application<message_nad_ptr>::reset_messages();
    }

private:
    uint32_t       _fifo_size;
    uint8_t*       _fifo;
    uint32_t       _send_period_us;
    uint64_t       _send_next_date_us;


    // Message lookup from offset (same content as _message_list in port_application base class)
    typedef std::tr1::unordered_map<uint32_t, message_nad_ptr> message_map_t;
    message_map_t _message_map;
};
}
#endif

