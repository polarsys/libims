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
#include "vistas_port_nad.hh"
#include "vistas_message_wrapper.hh"

namespace vistas
{
//
// Ctor / Dtor
//
port_nad::port_nad(context_weak_ptr context, socket_ptr socket, uint32_t fifo_size, uint32_t send_period_us) :
    port_application<message_nad_ptr>(context, socket),
    _fifo_size(fifo_size+VISTAS_HEADER_SIZE),
    _fifo(new uint8_t[_fifo_size]),
    _send_period_us(send_period_us),
    _send_next_date_us(0)
{
    memset(_fifo, 0, _fifo_size);
}

port_nad::~port_nad()
{
    delete[] _fifo;
}

//
// Find and check associated message
//
ims::message_ptr port_nad::get_nad_message(std::string name,
                                           uint32_t    size,
                                           uint32_t    offset,
                                           std::string     local_name,
                                           std::string     bus_name,
                                           uint32_t        period_us,
                                           ims_nad_type_t        nad_type,
                                           uint32_t              nad_dim1,
                                           uint32_t              nad_dim2)
{
    message_map_t::iterator imessage = _message_map.find(offset);
    if (imessage != _message_map.end()) {
      // Message already exists, check its consistency and return it
      if (imessage->second->_size != size) {
        THROW_IMS_ERROR(ims_init_failure, "Different size for same message " << name << " !");        
      }
      // return a wrapper of the message with the correct local_name
      return message_wrapper_ptr(new message_wrapper(imessage->second, local_name) );
    }

    // Message doesn't exists, create it
    if (offset + VISTAS_HEADER_SIZE > _fifo_size - size) {
        THROW_IMS_ERROR(ims_init_failure, "Offset '" << offset << "' of message " << name <<
                        " is too big for port fifo size '" << _fifo_size - VISTAS_HEADER_SIZE << "' !");
    }

    message_nad_ptr message =
            message_nad_ptr(new message_nad(_context,
                                            name,
                                            _socket->get_address()->get_direction(),
                                            this,
                                            _fifo + offset + VISTAS_HEADER_SIZE,
                                            size,
                                            local_name,
                                            bus_name,
                                            period_us,
                                            nad_type,
                                            nad_dim1,
                                            nad_dim2));
    
    _message_map[offset] = message;
    _message_list.push_back(message);

    return message;
}

//
// Receive
//
void port_nad::receive()
{
    _socket->receive((char*)_fifo, _fifo_size);

    for (message_list_t::iterator imessage = _message_list.begin();
         imessage != _message_list.end();
         imessage++)
    {
        (*imessage)->_validity = ims_valid;
    }
}

//
// Send
//
void port_nad::send()
{
    if (_send_period_us == 0 || _send_next_date_us <= _context->get_time_us()) {
        prepare_header(_fifo);
        _socket->send((const char*)_fifo, _fifo_size);
        if (_send_period_us > 0) {
            _send_next_date_us = (_context->get_time_us() / (uint64_t)_send_period_us + 1) * (uint64_t)_send_period_us;
        }
    }
}
}
