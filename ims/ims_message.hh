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
 * Base message class
 * A message has no name since several application may consume the same message.
 */
#ifndef _IMS_MESSAGE_HH_
#define _IMS_MESSAGE_HH_

#include "shared_ptr.hh"
#include "ims.h"
#include "ims_log.hh"
#include <string>
#include <tr1/unordered_map>

// C API interface 
// Beacause of the use of namesapces, we can't directly use this type in C++ code
struct ims_internal_message_t {};

namespace ims
{
// Pointer to message
class message;
typedef shared_ptr<message> message_ptr;
typedef message*            weak_message_ptr;

typedef std::tr1::unordered_map<std::string, message_ptr> message_map_t;

// Message itself
class message : public ims_internal_message_t
{
    //***************************************************************************
    // Basic accessors
    //***************************************************************************
public:
    inline std::string get_name();
    inline ims_protocol_t get_protocol();
    inline ims_direction_t get_direction();
    inline ims_mode_t get_mode();
    inline std::string get_local_name();
    inline std::string get_bus_name();
    inline uint32_t get_period_us();
    inline ims_nad_type_t get_nad_type();
    inline uint32_t get_nad_dim1();
    inline uint32_t get_nad_dim2();
    
protected:
    std::string       _name;
    ims_protocol_t    _protocol;
    ims_direction_t   _direction;
    ims_mode_t        _mode;
    std::string       _local_name;
    std::string       _bus_name;
    uint32_t          _period_us;
    ims_nad_type_t    _nad_type;
    uint32_t          _nad_dim1;
    uint32_t          _nad_dim2;
    
    //***************************************************************************
    // IMS API interface
    // Theses method may throw an ims::exception according to IMS api (@see ims.h)
    // Most of theses method have to be implemented in derived classes.
    //***************************************************************************
public:
    
    // Send/receive (to be implemented in derived classes)
    // Default implementation just throw an ims::exception.
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
    
    virtual ims_return_code_t reset()
    throw(ims::exception) = 0;
    
    virtual ims_return_code_t invalidate()
    throw(ims::exception);
    
    virtual ims_return_code_t queuing_pending(uint32_t* count)
    throw(ims::exception);
    
    virtual ims_return_code_t get_depth(uint32_t* depth)
    throw(ims::exception);

	virtual ims_return_code_t set_id(const uint32_t pId)
	throw(ims::exception);

	virtual ims_return_code_t set_name(const std::string pName)
	throw(ims::exception);
    
    // Return true if message match given information
    // Error are logged with ims::log
    virtual bool check(ims_protocol_t   protocol,
                       uint32_t         max_size,
                       uint32_t         depth,
                       ims_direction_t  direction) = 0;
    
    //***************************************************************************
    // Init
    //***************************************************************************
public:
    inline virtual ~message();
    
    // if the message has an init value, this method is called to init the buffer
    virtual void init_data(const char* data,
                           uint32_t    size) throw(ims::exception) = 0;
    
protected:
    inline message(std::string     name,
                   ims_protocol_t  protocol,
                   ims_direction_t direction,
                   ims_mode_t      mode,
                   std::string     local_name,
                   std::string     bus_name,
                   uint32_t        period_us);
    
    
    //***************************************************************************
    // Debug
    //***************************************************************************
public:
    virtual void dump(std::string prefix = std::string());
};

//***************************************************************************
// Inlines
//***************************************************************************
message::message(std::string     name,
                 ims_protocol_t  protocol,
                 ims_direction_t direction,
                 ims_mode_t      mode,
                 std::string     local_name,
                 std::string     bus_name,
                 uint32_t        period_us) :
    _name(name),
    _protocol(protocol),
    _direction(direction),
    _mode(mode),
    _local_name(local_name),
    _bus_name(bus_name),
    _period_us(period_us),
    _nad_type(ims_nad_undefined),
    _nad_dim1(0),
    _nad_dim2(0)
{
}

message::~message()
{
}

std::string message::get_name()
{
    return _name;
}

ims_protocol_t message::get_protocol()
{
    return _protocol;
}

ims_direction_t message::get_direction()
{
    return _direction;
}

ims_mode_t message::get_mode()
{
    return _mode;
}

std::string message::get_local_name()
{
    return _local_name;
}

std::string message::get_bus_name()
{
    return _bus_name;
}

uint32_t message::get_period_us()
{
    return _period_us;
}

ims_nad_type_t message::get_nad_type()
{
    return _nad_type;
}

uint32_t message::get_nad_dim1()
{
    return _nad_dim1;
}

uint32_t message::get_nad_dim2()
{
    return _nad_dim2;
}
}

#endif
