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
 * DISCRETE port
 */
#include "vistas_port_discrete.hh"
#include "vistas_message_wrapper.hh"


namespace vistas
{
//
// Ctor / Dtor
//
port_discrete::port_discrete(context_weak_ptr context, socket_ptr socket, uint32_t fifo_size, uint32_t send_period_us) :
    port_application<message_discrete_ptr>(context, socket),
    _fifo_size(fifo_size+VISTAS_HEADER_SIZE),
    _fifo(new uint8_t[_fifo_size]),
    _send_period_us(send_period_us),
    _send_next_date_us(0)
{
    memset(_fifo, 0, _fifo_size);
}

port_discrete::~port_discrete()
{
    delete[] _fifo;
}

//
// Find and check associated message
//
ims::message_ptr port_discrete::get_discrete_signal(std::string name,
                                                    uint32_t    offset,
                                                    uint32_t    validity_duration,
                                                    uint32_t    true_state,
                                                    uint32_t    false_state,
                                                    std::string     local_name,
                                                    std::string     bus_name,
                                                    uint32_t        period_us)
{
    message_map_t::iterator imessage = _message_map.find(offset);
    if (imessage != _message_map.end()) {
        // Message already exists, check its consistency and return it
        if (imessage->second->_true_state != true_state) {
            THROW_IMS_ERROR(ims_init_failure, "Different true state for same signal " << name << " !");
        }
        if (imessage->second->_false_state != false_state) {
            THROW_IMS_ERROR(ims_init_failure, "Different false state for same signal " << name << " !");
        }

      // return a wrapper of the message with the correct local_name
      return message_wrapper_ptr(new message_wrapper(imessage->second, local_name) );
    }

    // Message doesn't exists, create it
    if (offset + VISTAS_HEADER_SIZE >= _fifo_size) {
        THROW_IMS_ERROR(ims_init_failure, "Offset '" << offset << "' of signal " << name <<
                        " is too big for port fifo size '" << _fifo_size - VISTAS_HEADER_SIZE << "' !");
    }

    message_discrete_ptr message =
            message_discrete_ptr(new message_discrete(_context,
                                                      name,
                                                      _socket->get_address()->get_direction(),
                                                      this,
                                                      validity_duration,
                                 true_state,
                                 false_state,
                                 local_name,
                                 bus_name,
                                 period_us));

    _message_map[offset] = message;
    _message_list.push_back(message);

    return message;
}

//
// Receive
//
void port_discrete::receive()
{
    _socket->receive((char*)_fifo, _fifo_size);
    
    message_map_t::iterator imessage = _message_map.begin();
    while ( imessage != _message_map.end() )
    {
      imessage->second->port_set_data(&_fifo[VISTAS_HEADER_SIZE + imessage->first]);
      
      imessage++;
    }
}

//
// Send
//
void port_discrete::send()
{
    if (_send_period_us == 0 || _send_next_date_us <= _context->get_time_us()) {
        prepare_header(_fifo);
        
        message_map_t::iterator imessage = _message_map.begin();
        while ( imessage != _message_map.end() )
        {
          imessage->second->port_read_data(&_fifo[VISTAS_HEADER_SIZE + imessage->first]);
          
          imessage++;
        }
        
        _socket->send((const char*)_fifo, _fifo_size);
        if (_send_period_us > 0) {
            _send_next_date_us = (_context->get_time_us() / (uint64_t)_send_period_us + 1) * (uint64_t)_send_period_us;
        }
    }
}

//
// Mark this port as modified
//
void port_discrete::set_modified()
{
    _send_next_date_us = 0;
}
}
