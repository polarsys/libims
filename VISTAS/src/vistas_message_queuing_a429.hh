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
 * A429 queuing message class
 */
#ifndef _VISTAS_MESSAGE_QUEUING_A429_HH_
#define _VISTAS_MESSAGE_QUEUING_A429_HH_
#include "vistas_message_buffered.hh"
#include "a429_tools.hh"

namespace vistas
{
// Pointer to this class
class message_queuing_a429;
typedef shared_ptr<message_queuing_a429> message_queuing_a429_ptr;

class message_queuing_a429: public message_buffered
{
public:
    message_queuing_a429(context_weak_ptr context,
                         std::string      name,
                         ims_direction_t  direction,
                         uint32_t         depth,
                         std::string     local_name,
                         std::string     bus_name,
                         port_weak_ptr    port);


    // API implementation
    virtual ims_return_code_t push_queuing(const char* message_addr,
                                           uint32_t    message_size)
    throw(ims::exception);

    virtual ims_return_code_t pop_queuing(char*     message_addr,
                                          uint32_t* message_size)
    throw(ims::exception);

    virtual ims_return_code_t reset()
    throw(ims::exception);

    virtual bool check(ims_protocol_t   protocol,
                       uint32_t         max_size,
                       uint32_t         depth,
                       ims_direction_t  direction);

    inline virtual uint32_t get_data(char       *data,
                                     uint32_t   max_size,
                                     uint32_t   queue_index = 0)
    throw(ims::exception);

    virtual ims_return_code_t queuing_pending(uint32_t* count)
    throw(ims::exception);

    // Message buffer implementation
    void port_set_data(const char* data, uint32_t size);
    uint32_t port_read_data(char* data, uint32_t max_size);

private:
    // Cycle buffer implementation
    void push(const char* message, uint32_t message_size);
    uint32_t pop(char* message, uint32_t max_size);

    uint32_t _begin;
    uint32_t _depth;
    uint32_t _user_max_size;
};

//***************************************************************************
// Inlines
//***************************************************************************
inline uint32_t message_queuing_a429::get_data(char       *data,
                                               uint32_t   max_size,
                                               uint32_t   queue_index)
throw(ims::exception)
{
    if(max_size < A429_LABEL_SIZE){
        THROW_IMS_ERROR(ims_message_invalid_size, "Buffer size " << max_size << " is too small to contain " << A429_LABEL_SIZE << " bytes !");
    }

    uint32_t id = (_begin + queue_index * A429_LABEL_SIZE) % _max_size;

    if (A429_LABEL_SIZE < _max_size - id) {
        memcpy(data, _data + id, A429_LABEL_SIZE);
    } else {
        uint32_t part1_size = _max_size - id;
        memcpy(data, _data + id, part1_size);
        memcpy(data + part1_size, _data, A429_LABEL_SIZE - part1_size);
    }

    return A429_LABEL_SIZE;
}

}

#endif
