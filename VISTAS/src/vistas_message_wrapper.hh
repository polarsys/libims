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
#ifndef _VISTAS_MESSAGE_WRAPPER_HH_
#define _VISTAS_MESSAGE_WRAPPER_HH_
#include "ims_message.hh"

namespace vistas 
{
  // Pointer to message
  class message_wrapper;
  typedef shared_ptr<message_wrapper> message_wrapper_ptr;
  typedef message_wrapper*            weak_message_wrapper_ptr;
  
  class message_wrapper : public ims::message
  {
  public:
    message_wrapper(ims::message_ptr  original,
                   std::string local_name);

    // needed because they are pure virtual
    virtual void init_data(const char* data, 
                           uint32_t    size) throw(ims::exception);


    virtual bool check(ims_protocol_t   protocol,
                       uint32_t         max_size,
                       uint32_t         depth,
                       ims_direction_t  direction);

    virtual ims_return_code_t reset()
      throw(ims::exception);


    // inherited from ims::message

    virtual ims_return_code_t write_sampling(const char* message_addr, 
                                             uint32_t    message_size)
      throw(ims::exception);

    virtual ims_return_code_t read_sampling(char*           message_addr,
                                            uint32_t*       message_size,
                                            ims_validity_t* message_validity)
      throw(ims::exception);

    virtual ims_return_code_t push_queuing(const char* message_addr, 
                                           uint32_t    message_size)
      throw(ims::exception);

    virtual ims_return_code_t pop_queuing(char*     message_addr, 
                                          uint32_t* message_size)
      throw(ims::exception);


    virtual ims_return_code_t write_nad(const char* message_addr, 
                                        uint32_t    message_size)
      throw(ims::exception);
    
    virtual ims_return_code_t read_nad(char*           message_addr,
                                       uint32_t*       message_size,
                                       ims_validity_t* message_validity)
      throw(ims::exception);


    virtual ims_return_code_t get_max_size(uint32_t* max_size)
      throw(ims::exception);


    virtual ims_return_code_t set_validity_duration(uint32_t validity_duration_us)
      throw(ims::exception);

    virtual ims_return_code_t get_validity_duration(uint32_t* validity_duration_us)
      throw(ims::exception);

    virtual uint32_t get_data(char      *data,
                              uint32_t  max_size,
                              uint32_t  queue_index = 0)
      throw(ims::exception);


    virtual ims_return_code_t invalidate()
      throw(ims::exception);

    virtual ims_return_code_t queuing_pending(uint32_t* count)
      throw(ims::exception);
      
    virtual ims_return_code_t get_depth(uint32_t* depth)
      throw(ims::exception);

  protected:
    ims::message_ptr _original;




  };

}

#endif
