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
 * Analogue message class
 */
#ifndef _VISTAS_MESSAGE_ANALOGUE_HH_
#define _VISTAS_MESSAGE_ANALOGUE_HH_
#include "vistas_message.hh"

#define ANALOGUE_USER_SIZE (sizeof(float))

union MyUnion {
  float    vf;
  uint32_t vi;
};

namespace vistas
{
// reference to the port
class port_analogue;

// Pointer to this class
class message_analogue;
typedef shared_ptr<message_analogue> message_analogue_ptr;


class message_analogue: public message
{
public:
    message_analogue(context_weak_ptr context,
                     std::string      name,
                     ims_direction_t  direction,
                     port_weak_ptr    port,
                     uint32_t         validity_duration,
                     float            scale_factor,
                     float            gain,
                     std::string     local_name,
                     std::string     bus_name,
                     uint32_t        period_us);

    virtual void init_data(const char* data,
                           uint32_t    size) throw(ims::exception);

    // API implementation
    virtual ims_return_code_t write_sampling(const char* message_addr,
                                             uint32_t    message_size)
    throw(ims::exception);
    
    virtual ims_return_code_t read_sampling(char*           message_addr,
                                            uint32_t*       message_size,
                                            ims_validity_t* message_validity)
    throw(ims::exception);

    inline virtual uint32_t get_data(char      *data,
                              uint32_t  max_size,
                              uint32_t  queue_index = 0)
    throw(ims::exception);

    virtual ims_return_code_t reset()
    throw(ims::exception);

    virtual ims_return_code_t invalidate()
    throw(ims::exception);

    virtual bool check(ims_protocol_t   protocol,
                       uint32_t         max_size,
                       uint32_t         depth,
                       ims_direction_t  direction);

    virtual ims_return_code_t set_validity_duration(uint32_t validity_duration_us)
    throw(ims::exception);

    virtual ims_return_code_t get_validity_duration(uint32_t* validity_duration_us)
    throw(ims::exception);

    inline ims_return_code_t get_max_size(uint32_t* max_size) throw(ims::exception);

    inline void port_read_data(uint8_t * data);
    inline void port_set_data(const uint8_t * data);

private:
    friend class port_analogue;   // analogue port can directly fill data

    uint32_t  _data;
    bool      _has_data;
    float     _scale_factor;
    float     _gain;
    uint32_t  _init_data;
    bool      _has_init_data;
    uint32_t  _validity_duration_us; // Data validity duration
    uint64_t  _data_time_us;         // Last data receive date (valid only if _data_size > 0)
};

//***************************************************************************
// Inlines
//***************************************************************************

void message_analogue::port_read_data(uint8_t * data)
{
  if (_has_data)
  {
    memcpy( data, &_data, 4 );
  }
  else
  {
    memcpy( data, &_init_data, 4 );
  }
}

void message_analogue::port_set_data(const uint8_t * data)
{
  _has_data = true;
  _data_time_us = _context->get_time_us();
  memcpy( &_data, data, 4 );
}

ims_return_code_t message_analogue::get_max_size(uint32_t* max_size)
throw(ims::exception)
{
    *max_size = ANALOGUE_USER_SIZE;
    return ims_no_error;
}

uint32_t message_analogue::get_data(char      *data,
                                    uint32_t  max_size,
                                    uint32_t  queue_index)
throw(ims::exception)
{
    if(queue_index != 0){
        THROW_IMS_ERROR(ims_message_invalid_size, "Message is not a queuing one. Queue index should be nul.");
    }
    if(max_size < ANALOGUE_USER_SIZE){
        THROW_IMS_ERROR(ims_message_invalid_size, "Buffer size " << max_size << " is too small to contain " << ANALOGUE_USER_SIZE << " bytes !");
    }

    memset(data, 0, max_size);
	
	MyUnion u;
	u.vi = htonl(_data);
    *(float*)data = (u.vf - _gain) / _scale_factor;

    return ANALOGUE_USER_SIZE;
}

}

#endif
