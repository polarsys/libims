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
 * Discrete message class
 */
#ifndef _VISTAS_MESSAGE_DISCRETE_HH_
#define _VISTAS_MESSAGE_DISCRETE_HH_
#include "vistas_message.hh"

namespace vistas
{
namespace discrete
{
enum engineering_value_t {
    value_false = 0,
    value_true = 1
};
}

// reference to the port
class port_discrete;

// Pointer to this class
class message_discrete;
typedef shared_ptr<message_discrete> message_discrete_ptr;

class message_discrete: public message
{
public:
    message_discrete(context_weak_ptr              context,
                     std::string                   name,
                     ims_direction_t               direction,
                     port_weak_ptr                 port,
                     uint32_t                      validity_duration,
                     uint32_t                      true_state,
                     uint32_t                      false_state,
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

    inline virtual uint32_t get_data(char       *data,
                                     uint32_t   max_size,
                                     uint32_t   queue_index = 0)
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
    friend class port_discrete;   // discrete port can directly fill data

    uint8_t                        _data;
    bool                           _has_data;
    uint32_t                       _user_payload_size;
    uint32_t                       _true_state;
    uint32_t                       _false_state;
    uint8_t                        _init_data;
    bool                           _has_init_data;
    uint32_t                       _validity_duration_us; // Data validity duration
    uint64_t                       _data_time_us;         // Last data receive date (valid only if _data_size > 0)
};

//***************************************************************************
// Inlines
//***************************************************************************
ims_return_code_t message_discrete::get_max_size(uint32_t* max_size) throw(ims::exception)
{
    *max_size = _user_payload_size;
    return ims_no_error;
}

void message_discrete::port_read_data(uint8_t * data)
{
  if (_has_data)
  {
    *data = _data;
  }
  else
  {
    *data = _init_data;
  }
}

void message_discrete::port_set_data(const uint8_t * data)
{
  _has_data = true;
  _data_time_us = _context->get_time_us();
  _data = *data;
}

uint32_t message_discrete::get_data(char      *data,
                                    uint32_t  max_size,
                                    uint32_t  queue_index)
throw(ims::exception)
{
    if(queue_index != 0){
        THROW_IMS_ERROR(ims_message_invalid_size, "Message is not a queuing one. Queue index should be nul.");
    }
    if(max_size < _user_payload_size){
        THROW_IMS_ERROR(ims_message_invalid_size, "Buffer size " << max_size << " is too small to contain " << _user_payload_size << " byte !");
    }
	
	memset(data, 0, max_size);
	if (_has_data)
    {
      if ( _data == _true_state) {
        *(data + _user_payload_size - 1) = discrete::value_true;
	  }
    }
    else if (_has_init_data)
    {
      if ( _init_data == _true_state) {
        *(data + _user_payload_size - 1) = discrete::value_true;
      }
    }

    return _user_payload_size;
}

}

#endif
