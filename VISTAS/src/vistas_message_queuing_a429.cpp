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
#include "vistas_message_queuing_a429.hh"
#include "ims_log.hh"
#include <string.h>
#include "vistas_port.hh"

namespace vistas
{

//
// Ctor
//
message_queuing_a429::message_queuing_a429(context_weak_ptr context,
                                           std::string      name,
                                           ims_direction_t  direction,
                                           uint32_t         depth,
                                           std::string     local_name,
                                           std::string     bus_name,
                                           port_weak_ptr    port):
    message_buffered(context, name, ims_a429, direction, ims_queuing, depth * 4 * 4, local_name, bus_name, 0, port),   // The internal buffer can store 4x the depth (only needed on input)
    _begin(0),
    _depth(depth),
    _user_max_size(0)
{
}

//
// Fill data and mark port as to be send
//
ims_return_code_t message_queuing_a429::push_queuing(const char* message_addr,
                                                     uint32_t    message_size)
throw(ims::exception)
{
    if (get_direction() == ims_input) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot write to an INPUT message !");
    }

    if (get_mode() == ims_sampling) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot push to a SAMPLING message ! Use write() method instead.");
    }

    if (message_size > _depth * 4) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Not enough space in internal buffer of A429 " << get_name() << "! (max : " << _depth << " labels)");
    }

    if (message_size % 4 != 0) {
        THROW_IMS_ERROR(ims_message_invalid_size, "An A429 queuing should have a size multiple of 4.");
    }

    if (_data_size + message_size > _depth * 4) {
        THROW_IMS_ERROR(ims_message_queue_full,
                        "Not enough space in internal buffer of A429 " << get_name() << "! (max : " << _depth << " labels)");
    }

    if (message_size % 4 != 0) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Data for A429 " << get_name() << " is not a multiple of 4!");
    }

    push(message_addr, message_size);
    _context->get_output_queue()->push(_port);

    return ims_no_error;
}

//
// Read data
//
ims_return_code_t message_queuing_a429::pop_queuing(char*     message_addr,
                                                    uint32_t* message_size)
throw(ims::exception)
{
    if (get_direction() == ims_output) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot read from an OUTPUT message !");
    }
    
    if (get_mode() == ims_sampling) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot pop from a SAMPLING message ! Use read() method instead.");
    }

    *message_size = pop(message_addr, _user_max_size);

    return ims_no_error;
}

//
// Data added by port
//
void message_queuing_a429::port_set_data(const char* data, uint32_t size)
{
    push(data, size);
}

//
// Data readed by port (for sending)
//
uint32_t message_queuing_a429::port_read_data(char* data, uint32_t max_size)
{
    return pop(data, max_size);
}

//
// Add data to cycle buffer
//
void message_queuing_a429::push(const char* message, uint32_t message_size)
{
    if (_data_size + message_size > _max_size) {
        LOG_ERROR("Lost data on A429 " << get_name() << "! Amount: " << _data_size + message_size - _max_size << " bytes.");
        message_size = _max_size - _data_size;
        if (message_size == 0) return; // buffer full !
    }

    uint32_t first_byte = (_begin + _data_size) % _max_size;

    if (message_size < _max_size - first_byte) {
        memcpy(_data + first_byte, message, message_size);
    } else {
        uint32_t part1_size = _max_size - first_byte;
        memcpy(_data + first_byte, message, part1_size);
        memcpy(_data, message + part1_size, message_size - part1_size);
    }

    _data_size += message_size;
}

//
// Read and remove data from cycle buffer
//
uint32_t message_queuing_a429::pop(char* message, uint32_t message_max_size)
{
    uint32_t message_size = std::min(_data_size, message_max_size);

    if (message_size < _max_size - _begin) {
        memcpy(message, _data + _begin, message_size);
        _begin += message_size;
    } else {
        uint32_t part1_size = _max_size - _begin;
        memcpy(message, _data + _begin, part1_size);
        memcpy(message + part1_size, _data, message_size - part1_size);
        _begin = message_size - part1_size;
    }

    _data_size -= message_size;
    return message_size;
}

//
// Reset this message
//
ims_return_code_t message_queuing_a429::reset()
throw(ims::exception)
{
    memset(_data, 0, _max_size);
    _data_size = 0;
    _begin = 0;
    return ims_no_error;
}

//
// Pending count
//
ims_return_code_t message_queuing_a429::queuing_pending(uint32_t* count)
throw(ims::exception)
{
    *count = _data_size / 4;
    return ims_no_error;
}

//
// Check message
//
bool message_queuing_a429::check(ims_protocol_t   protocol,
                                 uint32_t         max_size,
                                 uint32_t         depth,
                                 ims_direction_t  direction)
{
    if (protocol != get_protocol()) {
        LOG_ERROR("Message " << get_name() << " is an " << ims_protocol_string(get_protocol()) << " message, not a " <<
                  ims_protocol_string(protocol) << "!");
        return false;
    }

    if (max_size != 4) {
        LOG_ERROR("Message " << get_name() << " has a max size of 4, not " << max_size << "!");
        return false;
    }

    if (depth > _depth) {
        LOG_ERROR("Message " << get_name() << " has a depth of " << _depth << ", witch is too small for given depth: " << depth << "!");
        return false;
    }
    _user_max_size = depth * 4;

    if (direction != get_direction()) {
        LOG_ERROR("Message " << get_name() << " is an " << ims_direction_string(get_direction()) << " message, not an " <<
                  ims_direction_string(direction) << " one!");
        return false;
    }

    return true;
}

}
