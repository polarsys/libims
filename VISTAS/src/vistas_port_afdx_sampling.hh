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
 * Afdx sampling port
 */
#ifndef _VISTAS_PORT_AFDX_SAMPLING_HH_
#define _VISTAS_PORT_AFDX_SAMPLING_HH_
#include "vistas_port_application.hh"
#include "vistas_message_sampling.hh"

namespace vistas
{
class port_afdx_sampling;
typedef shared_ptr<port_afdx_sampling> port_afdx_sampling_ptr;

class port_afdx_sampling: public port_application<message_sampling_ptr>
{
public:
    port_afdx_sampling(context_weak_ptr context, socket_ptr socket);
    ~port_afdx_sampling();

    // Lookup/initialize the message associated with this port
    ims::message_ptr get_message(std::string name, uint32_t size, uint32_t expected_size, uint32_t validity_duration_us, std::string local_name, uint32_t period_us);

    // Read/write data from socket
    void send();
    void receive();

    // Return port protocol
    virtual ims_protocol_t get_protocol() { return ims_afdx; }
    
    uint32_t           _fifo_size;
    char*              _fifo;
};
}

#endif

