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
#ifndef _VISTAS_MESSAGE_QUEUING_AFDX_HH_
#define _VISTAS_MESSAGE_QUEUING_AFDX_HH_
#include "vistas_message.hh"

namespace vistas
{

// Pointer to this class
class message_queuing_afdx;
typedef shared_ptr<message_queuing_afdx> message_queuing_afdx_ptr;

class message_queuing_afdx: public message
{
public:

    message_queuing_afdx(vistas::context_weak_ptr context,
                         std::string              name,
                         ims_direction_t          direction,
                         uint32_t                 max_size,
                         uint32_t                 expected_max_size,
                         uint32_t                 queue_depth,
                         std::string              local_name,
                         port_weak_ptr            port);

    virtual void init_data(const char* data,
                           uint32_t    size) throw(ims::exception);

    // API implementation
    virtual ims_return_code_t push_queuing(const char* message_addr,
                                           uint32_t    message_size)
    throw(ims::exception);


    virtual ims_return_code_t pop_queuing(char*     message_addr,
                                          uint32_t* message_size)
    throw(ims::exception);

    virtual ims_return_code_t queuing_pending(uint32_t* count)
    throw(ims::exception);

    virtual ims_return_code_t reset()
    throw(ims::exception);

    virtual bool check(ims_protocol_t   protocol,
                       uint32_t         max_size,
                       uint32_t         depth,
                       ims_direction_t  direction);

    uint32_t get_data(char      *data,
                      uint32_t  max_size,
                      uint32_t  queue_index=0)
    throw(ims::exception);

    inline ims_return_code_t get_max_size(uint32_t* max_size) throw(ims::exception);
    inline ims_return_code_t get_depth(uint32_t* depth) throw(ims::exception);

    // Accessors
    inline uint32_t get_max_size() { return _max_size; }
    inline uint32_t get_queue_depth() { return _queue_depth; }

private:
    uint32_t _max_size;
    uint32_t _expected_max_size;
    uint32_t _queue_depth;
};

//***************************************************************************
// Inlines
//***************************************************************************
ims_return_code_t message_queuing_afdx::get_max_size(uint32_t* max_size) throw(ims::exception)
{
    *max_size = _max_size;
    return ims_no_error;
}

ims_return_code_t message_queuing_afdx::get_depth(uint32_t* depth) throw(ims::exception)
{
    *depth = _queue_depth;
    return ims_no_error;
}

}


#endif
