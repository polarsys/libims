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
#ifndef _VISTAS_MESSAGE_NAD_HH_
#define _VISTAS_MESSAGE_NAD_HH_
#include "vistas_message.hh"

namespace vistas
{
// reference to the port
class port_nad;

// Pointer to this class
class message_nad;
typedef shared_ptr<message_nad> message_nad_ptr;

class message_nad: public message
{
public:
    message_nad(context_weak_ptr context,
                std::string      name,
                ims_direction_t  direction,
                port_weak_ptr    port,
                uint8_t*         data,                // payload allocated by port
                uint32_t         size,
                std::string     local_name,
                std::string     bus_name,
                uint32_t        period_us,
                ims_nad_type_t        nad_type,
                uint32_t              nad_dim1,
                uint32_t              nad_dim2);

    virtual void init_data(const char* data,
                           uint32_t    size) throw(ims::exception);

    // API implementation
    virtual ims_return_code_t write_nad(const char* message_addr,
                                        uint32_t    message_size)
    throw(ims::exception);

    virtual ims_return_code_t read_nad(char*           message_addr,
                                       uint32_t*       message_size,
                                       ims_validity_t* message_validity)
    throw(ims::exception);

    inline virtual uint32_t get_data(char       *data,
                                     uint32_t   max_size,
                                     uint32_t   queue_index = 0)
    throw(ims::exception);

    virtual ims_return_code_t reset()
    throw(ims::exception);

    virtual bool check(ims_protocol_t   protocol,
                       uint32_t         max_size,
                       uint32_t         depth,
                       ims_direction_t  direction);

    inline ims_return_code_t get_max_size(uint32_t* max_size) throw(ims::exception);
    inline ims_return_code_t get_data(char     *message_addr,
                                      uint32_t *message_size)
    throw(ims::exception);

private:
    friend class port_nad;   // nad port can directly fill data

    uint8_t*       _data;
    uint32_t       _size;
    ims_validity_t _validity;
};

//***************************************************************************
// Inlines
//***************************************************************************
ims_return_code_t message_nad::get_max_size(uint32_t* max_size) throw(ims::exception)
{
    *max_size = _size;
    return ims_no_error;
}

uint32_t message_nad::get_data(char       *data,
                               uint32_t   max_size,
                               uint32_t   queue_index)
throw(ims::exception)
{
    if(queue_index != 0){
        THROW_IMS_ERROR(ims_message_invalid_size, "Message is not a queuing one. Queue index should be nul.");
    }
    if(max_size < _size){
        THROW_IMS_ERROR(ims_message_invalid_size, "Buffer size " << max_size << " is too small to contain " << _size << " bytes !");
    }

    memcpy(data, _data, _size);

    return _size;
}

}

#endif
