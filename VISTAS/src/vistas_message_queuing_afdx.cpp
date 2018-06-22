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
 * Message afdx queuing
 */
#include "vistas_message_queuing_afdx.hh"
#include "vistas_port_afdx_queuing.hh"

namespace vistas
{

//
// Ctor
//
message_queuing_afdx::message_queuing_afdx(vistas::context_weak_ptr context,
                                           std::string              name,
                                           ims_direction_t          direction,
                                           uint32_t                 max_size,
                                           uint32_t                 expected_max_size,
                                           uint32_t                 queue_depth,
                                           std::string              local_name,
                                           port_weak_ptr            port) :
    message(context, name, ims_afdx, direction, ims_queuing, local_name, name, 0, port),
    _max_size(max_size),
    _expected_max_size(expected_max_size),
    _queue_depth(queue_depth)
{
}

void message_queuing_afdx::init_data(__attribute__((__unused__)) const char* data,
                                     __attribute__((__unused__)) uint32_t size) throw(ims::exception)
{
    THROW_IMS_ERROR(ims_implementation_specific_error, "Init data not supported for queuing");
}

ims_return_code_t message_queuing_afdx::push_queuing(const char* message_addr,
                                                     uint32_t    message_size)
throw(ims::exception)
{
    if (get_direction() == ims_input) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot push to an INPUT message !");
    }

    static_cast<port_afdx_queuing*>(_port)->push(message_addr, message_size);
    _context->get_output_queue()->push(_port);
    return ims_no_error;
}

ims_return_code_t message_queuing_afdx::pop_queuing(char*     message_addr,
                                                    uint32_t* message_size)
throw(ims::exception)
{
    if (get_direction() == ims_output) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot pop from an OUTPUT message !");
    }


    *message_size = static_cast<port_afdx_queuing*>(_port)->pop(message_addr, _max_size);
    return ims_no_error;
}

ims_return_code_t message_queuing_afdx::queuing_pending(uint32_t* count)
throw(ims::exception)
{
    *count = static_cast<port_afdx_queuing*>(_port)->pending_count();
    return ims_no_error;
}

ims_return_code_t message_queuing_afdx::reset()
throw(ims::exception)
{
    static_cast<port_afdx_queuing*>(_port)->reset();
    return ims_no_error;
}

bool message_queuing_afdx::check(ims_protocol_t   protocol,
                                 uint32_t         max_size,
                                 uint32_t         depth,
                                 ims_direction_t  direction)
{
    if (protocol != get_protocol()) {
        LOG_ERROR("Message " << get_name() << " is an " << ims_protocol_string(get_protocol()) << " message, not a " <<
                  ims_protocol_string(protocol) << "!");
        return false;
    }

    if (max_size != _expected_max_size && max_size != _max_size) {
        if (_expected_max_size == _max_size)
        {
            // simple error message for common case
            LOG_ERROR("Message " << get_name() << " has a max size of " << _max_size << ", not " << max_size << "!");
        }
        else
        {
            // longer message
            LOG_ERROR("Message " << get_name() << " has a max size of " << _max_size << " and an expected max size of " << _expected_max_size << " , " << max_size << " is neither !");
        }
        return false;
    }

    if (depth != _queue_depth) {
        LOG_ERROR("Message " << get_name() << " has a depth of " << _queue_depth << ", not " << depth << "!");
        return false;
    }
    
    if (direction != get_direction()) {
        LOG_ERROR("Message " << get_name() << " is an " << ims_direction_string(get_direction()) << " message, not an " <<
                  ims_direction_string(direction) << " one!");
        return false;
    }
    
    return true;
}

uint32_t message_queuing_afdx::get_data(char      *data,
                                        uint32_t  max_size,
                                        uint32_t  queue_index)
throw(ims::exception)
{
    return static_cast<port_afdx_queuing*>(_port)->get_queue_data(data,max_size,queue_index);
}

}

