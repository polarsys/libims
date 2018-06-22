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
 * Base vistas message class
 */
#ifndef _VISTAS_MESSAGE_HH_
#define _VISTAS_MESSAGE_HH_
#include "ims_message.hh"
#include "vistas_context.hh"

namespace vistas 
{
class message : public ims::message
{
protected:
    inline message(context_weak_ptr context,
                   std::string      name,
                   ims_protocol_t   protocol,
                   ims_direction_t  direction,
                   ims_mode_t       mode,
                   std::string     local_name,
                   std::string     bus_name,
                   uint32_t        period_us,
                   port_weak_ptr    port);

    context_weak_ptr _context;  // Vistas context
    port_weak_ptr   _port;     // Associated port

    
    virtual void dump(std::string prefix = std::string());
};

//***************************************************************************
// Inlines
//***************************************************************************
message::message(context_weak_ptr context,
                 std::string      name,
                 ims_protocol_t   protocol,
                 ims_direction_t  direction,
                 ims_mode_t       mode,
                 std::string     local_name,
                 std::string     bus_name,
                 uint32_t        period_us,
                 port_weak_ptr    port) :
    ims::message(name, protocol, direction, mode, local_name, bus_name, period_us),
    _context(context),
    _port(port)
{
}

}

#endif
