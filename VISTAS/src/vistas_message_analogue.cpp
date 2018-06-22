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
 * Analogue message class
 */
#include "vistas_message_analogue.hh"
#include "ims_log.hh"
#include <string.h>
#include "vistas_port.hh"
#include "htonl.h"

#define ANALOGUE_USER_SIZE (sizeof(float))

#define INVALID_DATE ((uint64_t)-1)

namespace vistas
{
//
// Ctor
//
message_analogue::message_analogue(context_weak_ptr context,
                                   std::string      name,
                                   ims_direction_t  direction,
                                   port_weak_ptr    port,
                                   uint32_t         validity_duration,
                                   float            scale_factor,
                                   float            gain,
                                   std::string     local_name,
                                   std::string     bus_name,
                                   uint32_t        period_us) :
    message(context, name, ims_analogue, direction, ims_sampling, local_name, bus_name, period_us, port),
    _data(0),
    _has_data(false),
    _scale_factor(scale_factor),
    _gain(gain),
    _init_data(0),
    _has_init_data(false),
    _validity_duration_us(validity_duration),
    _data_time_us(INVALID_DATE)
{
    // even if there is no init data, we must not send "0", but send "0*_scale_factor + _gain", i.e. "_gain"
    MyUnion u;
    u.vf = _gain;
    _init_data = htonl(u.vi);
      
    reset();
}

void message_analogue::init_data(const char* data,
                                 uint32_t size) throw(ims::exception)
{
    if (size == sizeof(float) )
    {
      float user_value;
      memcpy(&user_value, data, sizeof(float) );
      
      MyUnion u;
      u.vf = user_value * _scale_factor + _gain;
      
      _init_data = htonl(u.vi);
      _data = _init_data;
      
      _has_init_data = true;
    }
}

//
// Time/validity handling
//
ims_return_code_t message_analogue::set_validity_duration(uint32_t validity_duration_us)
throw(ims::exception)
{
    _validity_duration_us = validity_duration_us;
    return ims_no_error;
}

ims_return_code_t message_analogue::get_validity_duration(uint32_t* validity_duration_us)
throw(ims::exception)
{
    *validity_duration_us = _validity_duration_us;
    return ims_no_error;
}


//
// Fill data and mark port as to be send
//
ims_return_code_t message_analogue::write_sampling(const char* message_addr,
                                                   uint32_t    message_size)
throw(ims::exception)
{
    if (get_direction() == ims_input) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot write to an INPUT message !");
    }

    if (message_size != ANALOGUE_USER_SIZE) {
        return ims_message_invalid_size;
    }
    
    float user_value = *(float*)message_addr;
    
    MyUnion u;
    u.vf = user_value * _scale_factor + _gain;

    _data = htonl( u.vi );
    
    _has_data = true;

    return ims_no_error;
}

//
// Read data
//
ims_return_code_t message_analogue::read_sampling(char*           message_addr,
                                                  uint32_t*       message_size,
                                                  ims_validity_t* message_validity)
throw(ims::exception)
{
    if (get_direction() == ims_output) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot read from an OUTPUT message !");
    }
    
    MyUnion u;
    
    if (_has_data)
    {
      u.vi = htonl(_data);
      *(float*)message_addr = (u.vf - _gain) / _scale_factor;
      *message_size = ANALOGUE_USER_SIZE;
      
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
      u.vi = htonl(_init_data);
      *(float*)message_addr = (u.vf - _gain) / _scale_factor;
      *message_validity = ims_never_received_but_initialized;
      *message_size = ANALOGUE_USER_SIZE;
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
ims_return_code_t message_analogue::reset()
throw(ims::exception)
{
    _has_data = false;
    _data = _init_data;
    _data_time_us = INVALID_DATE;
    return ims_no_error;
}

//
// Invalidate this message
//
ims_return_code_t message_analogue::invalidate()
throw(ims::exception)
{
    _data_time_us = INVALID_DATE;
    return ims_no_error;
}

//
// Check message
//
bool message_analogue::check(ims_protocol_t   protocol,
                             uint32_t         max_size,
                             uint32_t         depth,
                             ims_direction_t  direction)
{
    
    if (protocol != ims_analogue) {
        LOG_ERROR("Message " << get_name() << " is an analogue message, not a " <<
                  ims_protocol_string(protocol) << "!");
        return false;
    }

    if (max_size != ANALOGUE_USER_SIZE)
    {
        LOG_ERROR("Analogue message " << get_name() << " should have a max size of 4, not " << max_size << "!");
        return false;
    }

    if (depth != 1) {
        LOG_ERROR("Message " << get_name() << " is a analogue message, then it has a depth of 1, not " << depth << "!");
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
