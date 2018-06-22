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
 * Wrapper of an existing message allowing to have a different local name
 * without allocating a new buffer or message
 */
#include "vistas_message_wrapper.hh"

namespace vistas
{
  message_wrapper::message_wrapper(ims::message_ptr  original,
                   std::string local_name) :
    ims::message(original->get_name(), original->get_protocol(), original->get_direction(), original->get_mode(), local_name, original->get_bus_name(), original->get_period_us()), _original(original)
  {
  }

  //
  // Call original message for all action
  //
  ims_return_code_t message_wrapper::reset()
      throw(ims::exception)
  {
    return _original->reset();
  }

  bool message_wrapper::check(ims_protocol_t   protocol,
                               uint32_t         max_size,
                               uint32_t         depth,
                               ims_direction_t  direction)
  {
    return _original->check(protocol, max_size, depth, direction);
  }

  void message_wrapper::init_data(const char* data,  uint32_t    size) throw(ims::exception)
  {
    return _original->init_data(data, size);
  }
  

  ims_return_code_t message_wrapper::write_sampling(const char* message_addr, 
                                           uint32_t    message_size)
    throw(ims::exception)
  {
    return _original->write_sampling(message_addr, message_size);
  }

  ims_return_code_t message_wrapper::read_sampling(char*           message_addr,
                                          uint32_t*       message_size,
                                          ims_validity_t* message_validity)
    throw(ims::exception)
  {
    return _original->read_sampling(message_addr, message_size, message_validity);
  }

  ims_return_code_t message_wrapper::push_queuing(const char* message_addr, 
                                         uint32_t    message_size)
    throw(ims::exception)
  {
    return _original->push_queuing(message_addr, message_size);
  }

  ims_return_code_t message_wrapper::pop_queuing(char*     message_addr, 
                                        uint32_t* message_size)
    throw(ims::exception)
  {
    return _original->pop_queuing(message_addr, message_size);
  }


  ims_return_code_t message_wrapper::write_nad(const char* message_addr, 
                                      uint32_t    message_size)
    throw(ims::exception)
  {
    return _original->write_nad(message_addr, message_size);
  }
  
  ims_return_code_t message_wrapper::read_nad(char*           message_addr,
                                     uint32_t*       message_size,
                                     ims_validity_t* message_validity)
    throw(ims::exception)
  {
    return _original->read_nad(message_addr, message_size, message_validity);
  }


  ims_return_code_t message_wrapper::get_max_size(uint32_t* max_size)
    throw(ims::exception)
  {
    return _original->get_max_size(max_size);
  }


  ims_return_code_t message_wrapper::set_validity_duration(uint32_t validity_duration_us)
    throw(ims::exception)
  {
    return _original->set_validity_duration(validity_duration_us);
  }

  ims_return_code_t message_wrapper::get_validity_duration(uint32_t* validity_duration_us)
    throw(ims::exception)
  {
    return _original->get_validity_duration(validity_duration_us);
  }

  uint32_t message_wrapper::get_data(char      *data,
                            uint32_t  max_size,
                            uint32_t  queue_index )
    throw(ims::exception)
  {
    return _original->get_data(data, max_size, queue_index);
  }


  ims_return_code_t message_wrapper::invalidate()
    throw(ims::exception)
  {
    return _original->invalidate();
  }

  ims_return_code_t message_wrapper::queuing_pending(uint32_t* count)
    throw(ims::exception)
  {
    return _original->queuing_pending(count);
  }
    
  ims_return_code_t message_wrapper::get_depth(uint32_t* depth)
    throw(ims::exception)
  {
    return _original->get_depth(depth);
  }

}
