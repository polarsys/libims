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
 * Sampling message class
 */
#include "vistas_message_sampling.hh"
#include "ims_log.hh"
#include <string.h>
#include "vistas_port.hh"

#define INVALID_DATE ((uint64_t)-1)

namespace vistas
{

//
// Ctor
//
message_sampling::message_sampling(context_weak_ptr context,
                                   std::string      name,
                                   ims_direction_t  direction,
                                   ims_protocol_t   protocol,
                                   uint32_t         size,
                                   uint32_t         expected_size,
                                   uint32_t         validity_duration_us,
                                   std::string     local_name,
                                   std::string     bus_name,
                                   uint32_t        period_us,
                                   port_weak_ptr    port):
    message_buffered(context, name, protocol, direction, ims_sampling, size, local_name, bus_name, period_us, port),
    _validity_duration_us(validity_duration_us),
    _data_time_us(INVALID_DATE),
    _expected_size(expected_size)
{
}

//
// Fill data and mark port as to be send
//
ims_return_code_t message_sampling::write_sampling(const char* message_addr,
                                                   uint32_t    message_size)
throw(ims::exception)
{
    LOG_DEBUG("Write " << message_size << " bytes on sampling message " << get_name());

    if (get_direction() == ims_input) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot write to an INPUT message !");
    }

    if (get_mode() == ims_queuing) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot write to a QUEUING message ! Use push() method instead.");
    }

    ims_return_code_t result = ims_no_error;
    if (message_size > _max_size) {
        result = ims_message_invalid_size;
        message_size = _max_size;
    }
    if (message_size < _max_size) {
        result = ims_message_invalid_size;
    }

    memcpy(_data, message_addr, message_size);
    _data_size = message_size;
    
    _context->get_output_queue()->push(_port);
    return result;
}

//
// Read data
//
ims_return_code_t message_sampling::read_sampling(char*           message_addr,
                                                  uint32_t*       message_size,
                                                  ims_validity_t* message_validity)
throw(ims::exception)
{
    LOG_DEBUG("Read sampling message " << get_name() << " ...");

    if (get_direction() == ims_output) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot read from an OUTPUT message !");
    }

    if (get_mode() == ims_queuing) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot read from a QUEUING message ! Use pop() method instead.");
    }

    if (_data_size == 0) {
        *message_size = _init_size;
        if (_init_data != NULL)
        {
            memcpy(message_addr, _init_data, _init_size);
            *message_validity = ims_never_received_but_initialized;
            LOG_DEBUG("Message never received but initialized.");
            return ims_no_error;
        }
        *message_validity = ims_never_received;
        LOG_DEBUG("Message never received.");
        return ims_no_error;
    }

    memcpy(message_addr, _data, _data_size);
    *message_size = _data_size;

    if (_validity_duration_us == 0 ||
            (_data_time_us != INVALID_DATE &&
             _context->get_time_us() - _data_time_us < _validity_duration_us)) {
        *message_validity = ims_valid;
    } else {
        *message_validity = ims_invalid;
    }

    LOG_DEBUG("Message size: " << _data_size << " validity: " << (*message_validity == ims_valid));

    if (_data_size != _max_size) {
        return ims_message_invalid_size;
    } else {
        return ims_no_error;
    }
}

//
// Time/validity handling
//
ims_return_code_t message_sampling::set_validity_duration(uint32_t validity_duration_us)
throw(ims::exception)
{
    _validity_duration_us = validity_duration_us;
    return ims_no_error;
}

ims_return_code_t message_sampling::get_validity_duration(uint32_t* validity_duration_us)
throw(ims::exception)
{
    *validity_duration_us = _validity_duration_us;
    return ims_no_error;
}

//
// Reset this message
//
ims_return_code_t message_sampling::reset()
throw(ims::exception)
{
    memset(_data, 0, _max_size);
    _data_size = 0;
    _data_time_us = INVALID_DATE;
    return ims_no_error;
}

//
// Invalidate this message
//
ims_return_code_t message_sampling::invalidate()
throw(ims::exception)
{
    _data_time_us = INVALID_DATE;
    return ims_no_error;
}

//
// Check message
//
bool message_sampling::check(ims_protocol_t   protocol,
                             uint32_t         max_size,
                             uint32_t         depth,
                             ims_direction_t  direction)
{
    if (protocol != get_protocol()) {
        LOG_ERROR("Message " << get_name() << " is an " << ims_protocol_string(get_protocol()) << " message, not a " <<
                  ims_protocol_string(protocol) << "!");
        return false;
    }

    if (max_size != _expected_size && max_size != _max_size) {
        if (_expected_size == _max_size)
        {
            // simple error message for common case
            LOG_ERROR("Message " << get_name() << " has a max size of " << _max_size << ", not " << max_size << "!");
        }
        else
        {
            // longer message
            LOG_ERROR("Message " << get_name() << " has a max size of " << _max_size << " and an expected size of " << _expected_size << " , " << max_size << " is neither !");
        }
        return false;
    }

    if (depth != 1) {
        LOG_ERROR("Message " << get_name() << " is a sampling message, then it has a depth of 1, not " << depth << "!");
        return false;
    }

    if (direction != get_direction()) {
        LOG_ERROR("Message " << get_name() << " is an " << ims_direction_string(get_direction()) << " message, not an " <<
                  ims_direction_string(direction) << " one!");
        return false;
    }

    return true;
}

//
// message_buffered implementation
//
void message_sampling::port_set_received()
{
    _data_time_us = _context->get_time_us();
}

void message_sampling::port_set_sent()
{
    _data_size = 0;
}

}
