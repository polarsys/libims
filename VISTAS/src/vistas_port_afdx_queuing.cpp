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
 * Afdx queuing port
 */
#include "vistas_port_afdx_queuing.hh"
#include "ims_log.hh"
#include "vistas_message_wrapper.hh"

namespace vistas
{
//
// Ctor
//
port_afdx_queuing::port_afdx_queuing(context_weak_ptr context, socket_ptr socket, uint32_t max_size, uint32_t expected_max_size, uint32_t queue_depth) :
    port_application<message_queuing_afdx_ptr>(context, socket),
    _max_size(max_size),
    _expected_max_size(expected_max_size),
    _message_queue_depth(queue_depth),
    _queue_depth(queue_depth * 4),      // The internal queue depth is bigger to prevent lost of data on input
    _nb_messages(0),
    _begin(0),
    _fifo_size(max_size+VISTAS_HEADER_SIZE),
    _fifo(new char[_fifo_size])
{
    _queue = new queued_message_t[_queue_depth];
    for (uint32_t id = 0; id < _queue_depth; id++) {
        _queue[id].data = new char[_max_size];
        memset(_queue[id].data, 0, _max_size);
    }
}

port_afdx_queuing::~port_afdx_queuing()
{
    for (uint32_t id = 0; id < _queue_depth; id++) {
        delete[] _queue[id].data;
    }
    delete[] _queue;
    delete[] _fifo;
}

  //
  // Find and check associated message
  //
  ims::message_ptr port_afdx_queuing::get_message(std::string name,
                                                  std::string local_name)
  {
    if (_message_list.empty() == true) {
      message_queuing_afdx_ptr original_msg(new message_queuing_afdx(_context, 
                                                                                name,
                                                                                _socket->get_address()->get_direction(), 
                                                                                _max_size,
                                                                                _expected_max_size,
                                                                                _message_queue_depth, 
                                                                                local_name,
                                                                                this));
      _message_list.push_back(original_msg);
      return original_msg;
    }
    
    // return a wrapper of the message with the correct local_name
    return message_wrapper_ptr(new message_wrapper(_message_list.front(), local_name) );
}

//
// Receive
//
void port_afdx_queuing::receive()
{
    if (_nb_messages >= _queue_depth) {
        LOG_WARN("Lost data on AFDX !");
        char trash;
        _socket->receive(&trash, 1);
        return;
    }

    uint32_t id = (_begin + _nb_messages) % _queue_depth;
    uint32_t data_size = _socket->receive(_fifo, _fifo_size);
    if (data_size > VISTAS_HEADER_SIZE)
    {
        _queue[id].size = data_size-VISTAS_HEADER_SIZE;
        memcpy(_queue[id].data, _fifo+VISTAS_HEADER_SIZE, _queue[id].size);
    }
    
    _nb_messages++;
}

//
// Send
//
void port_afdx_queuing::send()
{
    while (_nb_messages > 0) {
        prepare_header(_fifo);
        memcpy(_fifo+VISTAS_HEADER_SIZE, _queue[_begin].data, _queue[_begin].size);
        _socket->send(_fifo, _queue[_begin].size + VISTAS_HEADER_SIZE);
        _begin = (_begin + 1) % _queue_depth;
        _nb_messages--;
    }
    _nb_messages = 0;
    _begin = 0;
}

//
// Add data to cycle buffer
//
void port_afdx_queuing::push(const char* message, uint32_t message_size)
throw(ims::exception)
{
    if (message_size > _max_size) {
        THROW_IMS_ERROR(ims_message_invalid_size, "Message size " << message_size << " is too big !");
    }

    if (_nb_messages >= _message_queue_depth) {
        THROW_IMS_ERROR(ims_message_queue_full, "Message queue is full!");
    }

    uint32_t id = (_begin + _nb_messages) % _queue_depth;
    memcpy(_queue[id].data, message, message_size);
    _queue[id].size = message_size;

    _nb_messages++;
}

//
// Read and remove data from cycle buffer
//
uint32_t port_afdx_queuing::pop(char* message, uint32_t message_max_size)
{
    if (_nb_messages == 0) return 0;

    uint32_t size = std::min(message_max_size, _queue[_begin].size);
    memcpy(message, _queue[_begin].data, size);

    _begin = (_begin + 1) % _queue_depth;
    _nb_messages--;

    return size;
}

//
// Get queue data (read only)
//
uint32_t port_afdx_queuing::get_queue_data(char       *data,
                                           uint32_t   max_size,
                                           uint32_t   queue_index)
throw(ims::exception)
{
    if(queue_index >= _nb_messages){
        THROW_IMS_ERROR(ims_message_invalid_size, "Message queue index " << queue_index <<
                        " is higher than the number of messages " << _nb_messages << "!");
    }

    uint32_t id = (_begin + queue_index) % _queue_depth;

    if(max_size < _queue[id].size){
        THROW_IMS_ERROR(ims_message_invalid_size, "Buffer size " << _queue[id].size <<
                        " is lower than the message size of " << max_size << "!");
    }

    memcpy(data, _queue[id].data, _queue[id].size);

    return _queue[id].size;
}

//
// Empty the queue
//
void port_afdx_queuing::reset()
{
    _begin = 0;
    _nb_messages = 0;
}

}
