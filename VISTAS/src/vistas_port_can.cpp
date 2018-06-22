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
 * CAN port
 */
#include "vistas_port_can.hh"
#include "ims_log.hh"
#include "vistas_message_wrapper.hh"
#include <sstream>
#include "htonl.h"

#define CAN_FRAME_SIZE   14
#define CAN_LENGTH_INDEX 8
#define CAN_ID_INDEX     10

namespace vistas
{
//
// Ctor / Dtor
//
port_can::port_can(context_weak_ptr context, socket_ptr socket, std::string bus_name, uint32_t fifo_size) :
    port_application<message_sampling_can_ptr>(context, socket),
    _bus_name(bus_name),
    _fifo_size(fifo_size+VISTAS_HEADER_SIZE),
    _fifo(new char[_fifo_size]),
    _total_messages_size(0)
{
    memset(_fifo, 0, _fifo_size);
}

port_can::~port_can()
{
    delete[] _fifo;
}

//
// Get message
//
ims::message_ptr port_can::get_message(uint32_t message_id,
                                       uint32_t message_size,
                                       uint32_t validity_duration_us,
                                       std::string local_name,
                                       uint32_t  period_us)
{

    // Store message id directly in big-endian
    message_id = htonl(message_id);

    if (_message_map[message_id]) {
      // This message already exists
      // return a wrapper of the message with the correct local_name
      return message_wrapper_ptr(new message_wrapper(_message_map[message_id], local_name) );
    }
    
    std::stringstream sname;
    sname <<  _bus_name << "_" << message_id;

    // It doesn't exists, create it.
    message_sampling_can_ptr message = _message_map[message_id] =
      message_sampling_can_ptr(new message_sampling_can(_context, 
                                                        sname.str(),
                                                        message_id,
                                                        message_size,
                                                        _socket->get_address()->get_direction(), 
                                                        validity_duration_us, 
                                                        local_name,
                                                        _bus_name,
                                                        period_us,
                                                        this));
    
    
      _message_list.push_back(message);
      
      _total_messages_size += CAN_FRAME_SIZE;
      if (_total_messages_size > _fifo_size) {
        THROW_IMS_ERROR(ims_init_failure, "fifo size too small for CAN bus " << _bus_name);
      }

    return message;
}

//
// Receive
//
// CAN frame :
//   - 8 byes for data
//   - 2 byte for length
//   - 4 bytes for ID
//
void port_can::receive()
{
    uint32_t received_size = _socket->receive((char*)_fifo, _fifo_size);
    
    
    char* end = _fifo + received_size;

    for (char* first = _fifo+VISTAS_HEADER_SIZE; first < end; first += CAN_FRAME_SIZE)
    {
        if (first + CAN_FRAME_SIZE > end) {
            LOG_ERROR(_bus_name << ": Received payload too small!");
            break;
        }

        uint32_t data_size = ntohs(*(uint16_t*)(first + CAN_LENGTH_INDEX));

        if (data_size > 8) {
            LOG_ERROR(_bus_name << ": Invalid data length code!");
            break;
        }

        uint32_t message_id = *(uint32_t*)(first + CAN_ID_INDEX);

        message_map_t::iterator imessage = _message_map.find(message_id);
        if (imessage == _message_map.end()) {
            LOG_DEBUG(_bus_name << ": Unkown ID " << message_id);
            continue;
        }

        if (imessage->second->get_max_size() != data_size) {
            LOG_DEBUG(_bus_name << ": Incompatible received length for can ID " <<
                      message_id << " (" << data_size << " != " << imessage->second->get_max_size() << ")");
            continue;
        }

        imessage->second->port_set_data(first + 8 - data_size, data_size);
    }
}

//
// Send
//
void port_can::send()
{
    char* frame = _fifo+VISTAS_HEADER_SIZE;

    for (message_list_t::iterator imessage = _message_list.begin();
         imessage != _message_list.end();
         imessage++)
    {
        uint32_t data_size = (*imessage)->get_data_size();
        // if CAN is less than 8 bytes, data must be padded with 0
        memset(frame, 0, 8);
        (*imessage)->port_read_data(frame + 8 - data_size, data_size);
        *(uint16_t*)(frame + CAN_LENGTH_INDEX) = htons(data_size);
        *(uint32_t*)(frame + CAN_ID_INDEX) = (*imessage)->id;

        frame += CAN_FRAME_SIZE;
    }

    if (frame - _fifo > (int)VISTAS_HEADER_SIZE) {
        prepare_header(_fifo);
        _socket->send((const char*)_fifo, frame - _fifo);
    }
}
}
