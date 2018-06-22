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
 * Discrete message class
 */
#include "vistas_message_discrete.hh"
#include "ims_log.hh"
#include <string.h>
#include "vistas_port_discrete.hh"

#define INVALID_DATE ((uint64_t)-1)

namespace vistas
{
//
// Ctor
//
message_discrete::message_discrete(context_weak_ptr              context,
                                   std::string                   name,
                                   ims_direction_t               direction,
                                   port_weak_ptr                 port,
                                   uint32_t                      validity_duration,
                                   uint32_t                      true_state,
                                   uint32_t                      false_state,
                                   std::string     local_name,
                                   std::string     bus_name,
                                   uint32_t        period_us) :
    message(context, name, ims_discrete, direction, ims_sampling, local_name, bus_name, period_us, port),
    _data(0),
    _has_data(false),
    _user_payload_size(1),
    _true_state(true_state),
    _false_state(false_state),
    _init_data(0),
    _has_init_data(false),
    _validity_duration_us(validity_duration),
    _data_time_us(INVALID_DATE)
{
    // even if there is no init data, we must send "false_state"
    _init_data = _false_state;
    
    reset();
}

void message_discrete::init_data(const char* data,
                                 uint32_t size) throw(ims::exception)
{
    if (size == 1 )
    {
      _init_data = *data ? _true_state : _false_state;
      _data = _init_data;
      _has_init_data = true;
    }
}

//
// Time/validity handling
//
ims_return_code_t message_discrete::set_validity_duration(uint32_t validity_duration_us)
throw(ims::exception)
{
    _validity_duration_us = validity_duration_us;
    return ims_no_error;
}

ims_return_code_t message_discrete::get_validity_duration(uint32_t* validity_duration_us)
throw(ims::exception)
{
    *validity_duration_us = _validity_duration_us;
    return ims_no_error;
}


//
// Fill data and mark port as to be send
//
ims_return_code_t message_discrete::write_sampling(const char* message_addr,
                                                   uint32_t    message_size)
throw(ims::exception)
{
    if (get_direction() == ims_input) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot write to an INPUT message !");
    }

    ims_return_code_t result = ims_no_error;

    if (message_size != _user_payload_size) {
        return ims_message_invalid_size;
    }

    uint32_t value =  (message_size == 4)? *(uint32_t*)message_addr : *message_addr;
    value = (value)? _true_state : _false_state;
    
    _has_data = true;

    if (_data != value) {
        _data = value;
        static_cast<port_discrete*>(_port)->set_modified();
    }

    return result;
}

//
// Read data
//
ims_return_code_t message_discrete::read_sampling(char*           message_addr,
                                                  uint32_t*       message_size,
                                                  ims_validity_t* message_validity)
throw(ims::exception)
{
    if (get_direction() == ims_output) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot read from an OUTPUT message !");
    }
    
    
    if (_has_data)
    {
      memset(message_addr, 0, _user_payload_size);
      if ( _data == _true_state) {
          *(message_addr + _user_payload_size - 1) = discrete::value_true;
      }
      *message_size = _user_payload_size;
      if (_validity_duration_us == 0 ||
              (_data_time_us != INVALID_DATE &&
               _context->get_time_us() - _data_time_us < _validity_duration_us)) {
          *message_validity = ims_valid;
      } else {
          *message_validity = ims_invalid;
      }
    }
    else if (_has_init_data)
    {
      memset(message_addr, 0, _user_payload_size);
      if ( _init_data == _true_state) {
          *(message_addr + _user_payload_size - 1) = discrete::value_true;
      }
      *message_validity = ims_never_received_but_initialized;
      *message_size = _user_payload_size;
      
    }
    else
    {
      *message_validity = ims_never_received;
      *message_size = 0;
    }


    return ims_no_error;
}

//
// Reset this message
//
ims_return_code_t message_discrete::reset()
throw(ims::exception)
{
    _has_data = false;
    _data = _init_data;
    _data_time_us = INVALID_DATE;
    static_cast<port_discrete*>(_port)->set_modified();
    return ims_no_error;
}

//
// Invalidate this message
//
ims_return_code_t message_discrete::invalidate()
throw(ims::exception)
{
    _data_time_us = INVALID_DATE;
    return ims_no_error;
}

//
// Check message
//
bool message_discrete::check(ims_protocol_t   protocol,
                             uint32_t         max_size,
                             uint32_t         depth,
                             ims_direction_t  direction)
{
    
    if (protocol != ims_discrete) {
        LOG_ERROR("Message " << get_name() << " is an discrete message, not a " <<
                  ims_protocol_string(protocol) << "!");
        return false;
    }

    if (max_size != 1 &&
            max_size != 4)
    {
        LOG_ERROR("Discrete message " << get_name() << " should have a max size of 1 or 4, not " << max_size << "!");
        return false;
    }
    _user_payload_size = max_size;

    if (depth != 1) {
        LOG_ERROR("Message " << get_name() << " is a discrete message, then it has a depth of 1, not " << depth << "!");
        return false;
    }

    if (direction != get_direction()) {
        LOG_ERROR("Message " << get_name() << " is an " << ims_direction_string(get_direction()) << " message, not an " <<
                  ims_direction_string(direction) << " one!");
        return false;
    }

    return true;
}

}
