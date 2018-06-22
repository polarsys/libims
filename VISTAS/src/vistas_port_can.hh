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
 * CAN port
 */
#ifndef _VISTAS_PORT_CAN_HH_
#define _VISTAS_PORT_CAN_HH_
#include "vistas_port_application.hh"
#include "vistas_message_sampling_can.hh"

namespace vistas
{
class port_can;
typedef shared_ptr<port_can> port_can_ptr;

//
// The port class itself
//
class port_can: public port_application<message_sampling_can_ptr>
{
public:
    port_can(context_weak_ptr context, socket_ptr socket, std::string bus_name, uint32_t fifo_size);
    ~port_can();

    // Lookup/initialize a message associated with this port
    ims::message_ptr get_message(uint32_t id, uint32_t size, uint32_t validity_duration_us, std::string local_name, uint32_t  period_us);

    // Read/write data from socket
    void send();
    void receive();

    // Return port protocol
    virtual ims_protocol_t get_protocol() { return ims_can; }

private:
    std::string _bus_name;
    uint32_t    _fifo_size;
    char*       _fifo;
    uint32_t    _total_messages_size;

    // Map message id => message (same content as _message_list in port_application base class)
    typedef std::tr1::unordered_map<uint32_t, message_sampling_can_ptr> message_map_t;
    message_map_t _message_map;
};

}
#endif

