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

/************
 * Includes *
 ************/

#include "ims_instrumentation.h"
#include "ims_message.hh"

/*****************
 * Generic catch *
 *****************/

#define CATCH(default_return_code, message)  \
    catch(ims::exception& error) {           \
    LOG_ERROR(message);                      \
    return error.get_ims_return_code();      \
    }                                        \
    catch(std::exception& error) {           \
    LOG_ERROR(message);                      \
    LOG_ERROR(error.what());                 \
    return default_return_code;              \
    }                                        \
    catch(...) {                             \
    LOG_ERROR(message << " (unkown error)"); \
    return default_return_code;              \
    }

/*************
 * Variables *
 *************/

ims_socket_handler_t ims_socket_handler_recv = 0;
ims_socket_handler_t ims_socket_handler_send = 0;
ims_socket_handler_t ims_socket_handler_reply = 0;

/*************
 * Functions *
 *************/

ims_return_code_t ims_instrumentation_set_socket_handler_recv(ims_socket_handler_t handler)
{
    ims_socket_handler_recv = handler;
    return ims_no_error;
}

ims_return_code_t ims_instrumentation_set_socket_handler_send(ims_socket_handler_t handler)
{
    ims_socket_handler_send = handler;
    return ims_no_error;
}

ims_return_code_t ims_instrumentation_set_socket_handler_reply(ims_socket_handler_t handler)
{
    ims_socket_handler_reply = handler;
    return ims_no_error;
}

ims_return_code_t ims_instrumentation_message_get_sampling_data(ims_message_t   message_base,
                                                                char            *data,
                                                                uint32_t        *data_size,
                                                                uint32_t        max_size)
{
    try {
        *data_size = 0;

        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_message_get_data(" << message->get_name() << ")");

        *data_size = message->get_data(data,max_size);

        return ims_no_error;
    }
    CATCH(ims_implementation_specific_error, "Failed to get message data!");
}

ims_return_code_t ims_instrumentation_message_get_queued_data(ims_message_t message_base,
                                                              char          *data,
                                                              uint32_t      *data_size,
                                                              uint32_t      max_size,
                                                              uint32_t      queue_index)
{
    try {
        *data_size = 0;

        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_message_get_queue_data(" << message->get_name() << ")");

        *data_size = message->get_data(data,max_size,queue_index);

        return ims_no_error;
    }
    CATCH(ims_implementation_specific_error, "Failed to get queue message data!");
}

