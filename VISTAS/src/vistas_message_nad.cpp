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
 * Nad message class
 */
#include "vistas_message_nad.hh"
#include "ims_log.hh"
#include <string.h>
#include "vistas_port.hh"

namespace vistas
{
//
// Ctor
//
message_nad::message_nad(context_weak_ptr context,
                         std::string      name,
                         ims_direction_t  direction,
                         port_weak_ptr    port,
                         uint8_t*         data,                // 8 bytes allocated by port
                         uint32_t         size,
                         std::string     local_name,
                         std::string     bus_name,
                         uint32_t        period_us,
                         ims_nad_type_t   nad_type,
                         uint32_t         nad_dim1,
                         uint32_t         nad_dim2) :
    message(context, name, ims_nad, direction, ims_sampling, local_name, bus_name, period_us, port),
    _data(data),
    _size(size),
    _validity(ims_never_received)
{
    _nad_type = nad_type;
    _nad_dim1 = nad_dim1;
    _nad_dim2 = nad_dim2;
    reset();
}

void message_nad::init_data(__attribute__((__unused__)) const char* data,
                            __attribute__((__unused__)) uint32_t size) throw(ims::exception)
{
    THROW_IMS_ERROR(ims_implementation_specific_error, "Init data not supported for NAD");
}

//
// Fill data
//
ims_return_code_t message_nad::write_nad(const char* message_addr,
                                         uint32_t    message_size)
throw(ims::exception)
{
    if (get_direction() == ims_input) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot write to an INPUT message !");
    }

    if (message_size != _size) {
        return ims_message_invalid_size;
    }

    memcpy(_data, message_addr, _size);

    return ims_no_error;
}

//
// Read data
//
ims_return_code_t message_nad::read_nad(char*           message_addr,
                                        uint32_t*       message_size,
                                        ims_validity_t* message_validity)
throw(ims::exception)
{
    if (get_direction() == ims_output) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot read from an OUTPUT message !");
    }

    if (_validity == ims_valid) {
        memcpy(message_addr, _data, _size);
        *message_size = _size;
    } else {
        *message_size = 0;
    }

    *message_validity = _validity;
    return ims_no_error;
}

//
// Reset this message
//
ims_return_code_t message_nad::reset()
throw(ims::exception)
{
    _validity = ims_never_received;
    memset(_data, 0, _size);
    return ims_no_error;
}

//
// Check message
//
bool message_nad::check(ims_protocol_t   protocol,
                        uint32_t         max_size,
                        uint32_t         depth,
                        ims_direction_t  direction)
{
    
    if (protocol != ims_nad) {
        LOG_ERROR("Message " << get_name() << " is an nad message, not a " <<
                  ims_protocol_string(protocol) << "!");
        return false;
    }

    if (max_size != _size)
    {
        LOG_ERROR("Nad message " << get_name() << " should have a max size of " << _size << ", not " << max_size << "!");
        return false;
    }

    if (depth != 1) {
        LOG_ERROR("Message " << get_name() << " is a nad message, then it has a depth of 1, not " << depth << "!");
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
