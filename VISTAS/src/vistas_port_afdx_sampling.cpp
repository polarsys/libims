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
 * Afdx sampling port
 */
#include "vistas_port_afdx_sampling.hh"
#include "ims_log.hh"
#include "vistas_message_wrapper.hh"

namespace vistas
{
//
// Ctor
//
port_afdx_sampling::port_afdx_sampling(context_weak_ptr context, socket_ptr socket) :
    port_application<message_sampling_ptr>(context, socket), _fifo_size(0), _fifo(NULL)
{
}

port_afdx_sampling::~port_afdx_sampling()
{
    if (_fifo != NULL)
    {
        delete[] _fifo;
    }
}

//
// Find and check associated message
//
ims::message_ptr port_afdx_sampling::get_message(std::string name, uint32_t size, uint32_t expected_size, uint32_t validity_duration_us, std::string local_name, uint32_t period_us)
{
    if (_message_list.empty() == true) {
      message_sampling_ptr original_msg(new message_sampling(_context, 
                                                                        name,
                                                                        _socket->get_address()->get_direction(), 
                                                                        ims_afdx,
                                                                        size,
                                                                        expected_size,
                                                                        validity_duration_us, 
                                                                        local_name,
                                                                        name,
                                                                        period_us,
                                                                        this));
      _message_list.push_back(original_msg);
      _fifo_size = size + VISTAS_HEADER_SIZE;
      _fifo = new char[_fifo_size];
      return original_msg;
    }
    
    if (size != _message_list.front()->get_max_size()) {
        THROW_IMS_ERROR(ims_init_failure, "Multiples fifo size defined for same " << _socket->get_address()->to_string() << " !");
    }

    // return a wrapper of the message with the correct local_name
    return message_wrapper_ptr(new message_wrapper(_message_list.front(), local_name) );
}

//
// Receive: afdx port is just a forwarder
//
void port_afdx_sampling::receive()
{
    if (_message_list.empty() == false)
    {
        uint32_t data_size = _socket->receive(_fifo, _fifo_size);

        if (data_size > VISTAS_HEADER_SIZE)
        {
            _message_list.front()->port_set_data(_fifo + VISTAS_HEADER_SIZE, data_size - VISTAS_HEADER_SIZE);
        }

        LOG_DEBUG(_message_list.front()->get_data_size() << " bytes received for AFDX message " << _message_list.front()->get_name());
    }
}

//
// Send: afdx port is just a forwarder
//
void port_afdx_sampling::send()
{
    if (_message_list.empty() == false && _message_list.front()->get_data_size() > 0) {
        LOG_DEBUG("Send " << _message_list.front()->get_data_size() << " bytes for AFDX message " << _message_list.front()->get_name());

        prepare_header(_fifo);
        uint32_t data_read = _message_list.front()->port_read_data(_fifo + VISTAS_HEADER_SIZE, _fifo_size - VISTAS_HEADER_SIZE);

        _socket->send(_fifo, data_read + VISTAS_HEADER_SIZE);
    }
}
}
