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
 * Simple message with associated buffer
 */
#ifndef _VISTAS_MESSAGE_BUFFERED_HH_
#define _VISTAS_MESSAGE_BUFFERED_HH_
#include "vistas_message.hh"

namespace vistas
{

  // Pointer to this class
  class message_buffered;
  typedef shared_ptr<message_buffered> message_buffered_ptr;


  class message_buffered: public message
  {
  public:

    inline message_buffered(context_weak_ptr context, 
                            std::string      name, 
                            ims_protocol_t   protocol,
                            ims_direction_t  direction, 
                            ims_mode_t       mode, 
                            uint32_t         max_size,
                            std::string     local_name,
                            std::string     bus_name,
                            uint32_t        period_us,
                            port_weak_ptr    port);


    inline ~message_buffered();

    inline void init_data(const char* data, uint32_t size) throw(ims::exception);

    // Direct access
    inline char*    get_data()                   { return _data;      }
    inline uint32_t get_data_size()              { return _data_size; }
    inline void     set_data_size(uint32_t size) { _data_size = size; }
    inline uint32_t get_max_size()               { return _max_size;  }

    // API implementation
    inline ims_return_code_t get_max_size(uint32_t* max_size)
      throw(ims::exception);
    inline virtual uint32_t get_data(char       *data,
                                     uint32_t   max_size,
                                     uint32_t   queue_index = 0)
      throw(ims::exception);


    //*********************************************************************
    // Function below are for port classes.
    // Message classes should use direct access
    //*********************************************************************

    // Set new data. 
    // Default implementation just update internal buffer and size
    // and call port_set_received() function below
    inline virtual void port_set_data(const char* data, uint32_t size);

    // Read data and return readed size
    // Default implementation just copy internal buffer
    // and call port_set_sent() function below
    inline virtual uint32_t port_read_data(char* data, uint32_t max_size);

    // Mark data as received.
    // Default implementation does nothing
    inline virtual void port_set_received() {}

    // Mark data as readed.
    // Default implementation does nothing
    inline virtual void port_set_sent() {}



  protected:
    char*     _data;
    uint32_t  _max_size;   // Allocated memory
    uint32_t  _data_size;  // Size of content
    char*     _init_data;  // a copy of the init value if this message has an init value, else NULL
    uint32_t  _init_size;  // Size of the init value if this message has an init value, else 0
  };




  //***************************************************************************
  // Inlines
  //***************************************************************************
  message_buffered::message_buffered(context_weak_ptr context, 
                                     std::string      name, 
                                     ims_protocol_t   protocol,
                                     ims_direction_t  direction, 
                                     ims_mode_t       mode, 
                                     uint32_t         max_size,
                                     std::string     local_name,
                                     std::string     bus_name,
                                     uint32_t        period_us,
                                     port_weak_ptr    port) :
    message(context, name, protocol, direction, mode, local_name, bus_name, period_us, port),
    _data(new char[max_size]),
    _max_size(max_size),
    _data_size(0),
    _init_data(NULL),
    _init_size(0)
  {
    memset(_data, 0, max_size);
  }

  inline message_buffered::~message_buffered()
  {
    delete[] _data;
    if (_init_data != NULL)
    {
      delete[] _init_data;
  }
  }


  void message_buffered::init_data(const char* data, uint32_t size) throw(ims::exception)
  {
    _init_data = new char[size];
    _init_size = size;
    memcpy(_init_data, data, size);
  }

  void message_buffered::port_set_data(const char* data, uint32_t size)
  {
    memcpy(_data, data, size);
    _data_size = size;
    port_set_received();
  }

  uint32_t message_buffered::port_read_data(char* data, uint32_t max_size)
  {
    uint32_t size = std::min(_data_size, max_size);
    memcpy(data, _data, size);
    port_set_sent();
    return size;
  }

  ims_return_code_t message_buffered::get_max_size(uint32_t* max_size) 
    throw(ims::exception)
  {
    *max_size = _max_size;
    return ims_no_error;
  }

  inline uint32_t message_buffered::get_data(char       *data,
                                             uint32_t   max_size,
                                             uint32_t   queue_index)
    throw(ims::exception)
  {
      if(queue_index != 0){
          THROW_IMS_ERROR(ims_message_invalid_size, "Message is not a queuing one. Queue index should be nul.");
      }
      if(max_size < _data_size){
          THROW_IMS_ERROR(ims_message_invalid_size, "Buffer size " << max_size << " is too small to contain " << _data_size <<" bytes !");
      }

      memcpy(data, _data, _data_size);
      return _data_size;
  }

}

#endif
