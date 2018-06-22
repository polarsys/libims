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
 * ANALOGUE port
 */
#include "vistas_port_analogue.hh"
#include "vistas_message_wrapper.hh"

#define ANALOGUE_NETWORK_SIZE (sizeof(float))

namespace vistas
{
//
// Ctor / Dtor
//
port_analogue::port_analogue(context_weak_ptr context, socket_ptr socket, uint32_t fifo_size, uint32_t send_period_us) :
    port_application<message_analogue_ptr>(context, socket),
    _fifo_size(fifo_size+VISTAS_HEADER_SIZE),
    _fifo(new uint8_t[_fifo_size]),
    _send_period_us(send_period_us),
    _send_next_date_us(0)
{
    memset(_fifo, 0, _fifo_size);
}

port_analogue::~port_analogue()
{
    delete[] _fifo;
}

//
// Find and check associated message
//
ims::message_ptr port_analogue::get_analogue_signal(std::string name,
                                                    uint32_t    offset,
                                                    uint32_t    validity_duration,
                                                    float       scale_factor,
                                                    float       gain,
                                                    std::string     local_name,
                                                    std::string     bus_name,
                                                    uint32_t        period_us)
{
    message_map_t::iterator imessage = _message_map.find(offset);
    if (imessage != _message_map.end()) {
        // Message already exists, check its consistency and return it
        if (imessage->second->_scale_factor != scale_factor) {
            THROW_IMS_ERROR(ims_init_failure, "Different scale factor for same signal " << name << " !");
        }
        if (imessage->second->_gain != gain) {
            THROW_IMS_ERROR(ims_init_failure, "Different signal offset for same signal " << name << " !");
        }

      // return a wrapper of the message with the correct local_name
      return message_wrapper_ptr(new message_wrapper(imessage->second, local_name) );
    }

    // Message doesn't exists, create it
    if (offset + VISTAS_HEADER_SIZE > _fifo_size - ANALOGUE_NETWORK_SIZE) {
        THROW_IMS_ERROR(ims_init_failure, "Offset '" << offset << "' of signal " << name <<
                        " is too big for port fifo size '" << _fifo_size - VISTAS_HEADER_SIZE << "' !");
    }

    message_analogue_ptr message =
            message_analogue_ptr(new message_analogue(_context,
                                                      name,
                                                      _socket->get_address()->get_direction(),
                                                      this,
                                                      validity_duration,
                                                      scale_factor,
                                                      gain,
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
void port_analogue::receive()
{
    _socket->receive((char*)_fifo, _fifo_size);
    
    message_map_t::iterator imessage = _message_map.begin();
    while ( imessage != _message_map.end() )
    {
      imessage->second->port_set_data(&_fifo[VISTAS_HEADER_SIZE + imessage->first] );
      
      imessage++;
    }
}

//
// Send
//
void port_analogue::send()
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
}
