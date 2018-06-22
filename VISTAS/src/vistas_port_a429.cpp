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
 * A429 port
 */
#include "vistas_port_a429.hh"
#include "ims_log.hh"

namespace vistas
{
//
// Ctor / Dtor
//
port_a429::port_a429(context_weak_ptr context, socket_ptr socket, std::string bus_name, uint32_t fifo_size) :
    port_application<message_buffered_ptr>(context, socket),
    _bus_name(bus_name),
    _fifo_size(4*fifo_size+VISTAS_HEADER_SIZE),
    _fifo(new uint8_t[_fifo_size])
{
    memset(_fifo, 0, _fifo_size);
}

port_a429::~port_a429()
{
    delete[] _fifo;
}

//
// Receive
//
void port_a429::receive()
{
    uint32_t received_size = _socket->receive((char*)_fifo, _fifo_size);
    
    for (uint8_t* label = _fifo+VISTAS_HEADER_SIZE; label < _fifo + received_size; label += A429_LABEL_SIZE)
    {
        message_map_t::iterator isdi_message_map = _message_map.find(a429::label_number_get(label));
        if (isdi_message_map != _message_map.end()) {

            message_buffered_ptr message;
			message_buffered_ptr messageXX;
            if (isdi_message_map->second[a429::SDI_DD]) message = isdi_message_map->second[a429::SDI_DD];
            else 
			{
				message =  isdi_message_map->second[a429::sdi_get(label)]; 
				messageXX = isdi_message_map->second[a429::SDI_XX];
			}

            if (message) {
                message->port_set_data((char*)label, A429_LABEL_SIZE);
				if (messageXX) {
					messageXX->port_set_data((char*)label, A429_LABEL_SIZE);
				}
            }
            else {
                LOG_DEBUG("Label " << a429::label_number_get(label) << "/" << a429::sdi_get(label) << " not found in bus " << _bus_name);
            }
        } else {
            LOG_DEBUG("Label " << a429::label_number_get(label) << " not found in bus " << _bus_name);
        }
    }
}

//
// Send
//
void port_a429::send()
{
    uint32_t send_size = VISTAS_HEADER_SIZE;

    for (message_list_t::iterator imessage = _message_list.begin();
         imessage != _message_list.end();
         imessage++)
    {
        if ((*imessage)->get_data_size()) {
            uint32_t data_size = (*imessage)->port_read_data((char*)_fifo + send_size, _fifo_size - send_size);
            send_size += data_size;
        }
    }

    if (send_size > VISTAS_HEADER_SIZE) {
        prepare_header(_fifo);
        _socket->send((const char*)_fifo, send_size);
    }
}
}
