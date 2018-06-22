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
 * Base message class
 */
#include "ims_message.hh"
#include "ims_log.hh"

namespace ims
{

/*********************
 * IMS API interface *
 *********************/
ims_return_code_t message::write_sampling(__attribute__((__unused__)) const char* message_addr,
                                          __attribute__((__unused__)) uint32_t    message_size)
throw(ims::exception)
{
    if (_direction == ims_input) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot write to an INPUT message !");
    }

    if (_mode == ims_queuing) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot write to a QUEUING message ! Use push() method instead.");
    }

    if (_protocol == ims_nad) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot use sampling functions for NAD protocol !");
    }


    THROW_IMS_ERROR(ims_implementation_specific_error,
                    "Cannot write to this message ! (internal error).");
}

ims_return_code_t message::read_sampling(__attribute__((__unused__)) char*           message_addr,
                                         __attribute__((__unused__)) uint32_t*       message_size,
                                         __attribute__((__unused__)) ims_validity_t* message_validity)
throw(ims::exception)
{
    if (_direction == ims_output) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot read from an OUTPUT message !");
    }

    if (_mode == ims_queuing) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot read from a QUEUING message ! Use pop() method instead.");
    }

    if (_protocol == ims_nad) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot use sampling functions for NAD protocol !");
    }

    THROW_IMS_ERROR(ims_implementation_specific_error,
                    "Cannot read from this message ! (internal error).");
}

ims_return_code_t message::push_queuing(__attribute__((__unused__)) const char* message_addr,
                                        __attribute__((__unused__)) uint32_t    message_size)
throw(ims::exception)
{
    if (_direction == ims_input) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot push to an INPUT message !");
    }

    if (_protocol == ims_nad) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot use queuing functions for NAD protocol !");
    }

    if (_mode == ims_sampling) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot push to a SAMPLING message ! Use write() method instead.");
    }

    THROW_IMS_ERROR(ims_implementation_specific_error,
                    "Cannot push to this message ! (internal error).");
}

ims_return_code_t message::pop_queuing(__attribute__((__unused__)) char*     message_addr,
                                       __attribute__((__unused__)) uint32_t* message_size)
throw(ims::exception)
{
    if (_direction == ims_output) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot pop from an OUTPUT message !");
    }

    if (_protocol == ims_nad) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot use queuing functions for NAD protocol !");
    }

    if (_mode == ims_sampling) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot pop from a SAMPLING message ! Use read() method instead.");
    }

    THROW_IMS_ERROR(ims_implementation_specific_error,
                    "Cannot pop from this message ! (internal error).");
}

ims_return_code_t message::get_max_size(__attribute__((__unused__)) uint32_t* max_size)
throw(ims::exception)
{
    THROW_IMS_ERROR(ims_implementation_specific_error,
                    "Cannot get max size on message " << get_name() << " (internal error).");
}

ims_return_code_t message::set_validity_duration(__attribute__((__unused__)) uint32_t validity_duration_us)
throw(ims::exception)
{
    THROW_IMS_ERROR(ims_invalid_configuration,
                    "This message has no validity duration (it never get invalid).");
}

ims_return_code_t message::get_validity_duration(__attribute__((__unused__)) uint32_t* validity_duration_us)
throw(ims::exception)
{
    THROW_IMS_ERROR(ims_invalid_configuration,
                    "This message has no validity duration (it never get invalid).");
}

uint32_t message::get_data(__attribute__((__unused__)) char       *data,
                           __attribute__((__unused__)) uint32_t   max_size,
                           __attribute__((__unused__)) uint32_t   queue_index)
throw(ims::exception)
{
    THROW_IMS_ERROR(ims_invalid_configuration,
                    "Cannot retrieve message data.");
}

ims_return_code_t message::invalidate()
throw(ims::exception)
{
    if (_mode == ims_queuing) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot invalidate queuing message !");
    }

    THROW_IMS_ERROR(ims_invalid_configuration,
                    "Cannot invalidate this message !");
}

ims_return_code_t message::queuing_pending(__attribute__((__unused__)) uint32_t* count)
throw(ims::exception)
{
    if (_mode == ims_sampling) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Message " << get_name() << " is not a queuing message !");
    }

    THROW_IMS_ERROR(ims_implementation_specific_error,
                    "Cannot get pending count on message " << get_name() << " (internal error).");


}

ims_return_code_t message::get_depth(uint32_t* depth)
throw(ims::exception)
{
    *depth = 1;
    return ims_no_error;
}

ims_return_code_t message::set_id(__attribute__((__unused__)) const uint32_t pId)
throw(ims::exception)
{
	THROW_IMS_ERROR(ims_implementation_specific_error,
                        "Cannot set id for the message " << get_name() << " !");
}

ims_return_code_t message::set_name(const std::string pName)
throw(ims::exception)
{
	this->_name = pName; 
	return ims_no_error; 
}

ims_return_code_t message::write_nad(__attribute__((__unused__)) const char* message_addr,
                                     __attribute__((__unused__)) uint32_t    message_size)
throw(ims::exception)
{
    if (_direction == ims_input) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot write to an INPUT message !");
    }
    
    if (_mode == ims_queuing) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot write to a QUEUING message ! Use push() method instead.");
    }

    if (_protocol != ims_nad) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot use NAD functions for " << ims_protocol_string(_protocol) << " protocol !");
    }


    THROW_IMS_ERROR(ims_implementation_specific_error,
                    "Cannot write to this message ! (internal error).");
}

ims_return_code_t message::read_nad(__attribute__((__unused__)) char*           message_addr,
                                    __attribute__((__unused__)) uint32_t*       message_size,
                                    __attribute__((__unused__)) ims_validity_t* message_validity)
throw(ims::exception)
{
    if (_direction == ims_output) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot read from an OUTPUT message !");
    }
    
    if (_mode == ims_queuing) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot read from a QUEUING message ! Use pop() method instead.");
    }

    if (_protocol != ims_nad) {
        THROW_IMS_ERROR(ims_invalid_configuration,
                        "Cannot use NAD functions for " << ims_protocol_string(_protocol) << " protocol !");
    }

    THROW_IMS_ERROR(ims_implementation_specific_error,
                    "Cannot read from this message ! (internal error).");
}

/***************
 * Debug stuff *
 ***************/
void message::dump(std::string prefix)
{
    LOG_SAY(prefix << "Message " << ims_protocol_string(get_protocol()) << " " <<
            ims_direction_string(get_direction()) << " " <<
            ims_mode_string(get_mode()) << " '" << get_name() << "'");
}

}
