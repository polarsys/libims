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

//
// vistas root class
//
#ifndef _VISTAS_CONTEXT_HH_
#define _VISTAS_CONTEXT_HH_
#include "backend_context.hh"
#include "vistas_output_queue.hh"
#include "vistas_port_application.hh"
#include "vistas_port_instrumentation.hh"
#include "vistas_socket_pool.hh"
#include <list>

namespace vistas
{
class context;
typedef shared_ptr<context> context_ptr;
typedef context* context_weak_ptr;

class context : public backend::context
{
public:
    class factory;
    typedef shared_ptr<context::factory> factory_ptr;
    
    // Init common info
    inline void set_vc_name(std::string vc_name);
    inline std::string get_vc_name();
    
    inline void set_prod_id(uint32_t prod_id);
    inline uint32_t get_prod_id();
    
    inline void set_period_us(uint32_t period_us);
    inline uint32_t get_period_us();
    
    inline void set_step_by_step_enabled(bool step_by_step_enabled);
    inline bool is_step_by_step_enabled();
    
    // Running state
    inline ims_running_state_t get_running_state();
    inline void set_running_state(ims_running_state_t state);
    
    inline float get_time_ratio();
    inline void set_time_ratio(float time_ratio);

    inline bool is_powersupply_on();
    inline void set_powersupply_on(bool powersupply);

    inline const char* virtual_component_name();

    //
    // Synchronization
    //
    inline bool is_autonomous_realtime();
    inline void set_autonomous_realtime(bool autonomous);

    inline uint32_t get_synchro_steps_request();
    inline void set_synchro_steps_request(uint32_t steps, port_instrumentation * port_instrumentation);

    // Import incoming data into messages
    inline ims_return_code_t import(uint32_t timeout);

    // Send All prepared and periodic ports
    ims_return_code_t send_all();

    // Access to the list of prepared ports
    inline output_queue_ptr get_output_queue();

    // Access to the socket pool
    inline socket_pool_ptr get_socket_pool();

    // Reset all messages
    ims_return_code_t reset_all();

    // Invalidate all messages
    ims_return_code_t invalidate_all();

    // Time handling
    inline uint64_t get_time_us();
    inline uint64_t get_posix_timestamp();
    ims_return_code_t progress(uint32_t duration_us);

private:
    inline context();
    typedef std::list<port_application_ptr> port_list_t;
    typedef std::vector<port_application_ptr> port_vector_t;
    std::string              _vc_name;
    uint32_t                 _prod_id;
    uint32_t                 _period_us;
    uint64_t                 _time_us_before_notify;    // in xxx us, must respond R_SYNCHRO
    bool                     _step_by_step_enabled;
    ims_running_state_t      _running_state;
    bool                     _autonomous_realtime;
    uint32_t                 _steps_requested;
    port_instrumentation*    _port_instrumentation;
    bool                     _powersupply_on;
    float                    _time_ratio;
    uint64_t                 _time_us;                 // Current time
    output_queue_ptr         _output_queue;            // Messages to be send
    port_vector_t            _periodic_output_ports;   // Ports to be send periodicaly
    socket_pool_ptr          _socket_pool;             // All sockets
    port_list_t              _port_list;               // All defined ports
    uint64_t                 _posix_timestamp;         // POSIX timestamp
};

//***************************************************************************
// Inlines
//***************************************************************************
context::context() :
    _prod_id(0),
    _period_us(0),
    _time_us_before_notify(0),
    _step_by_step_enabled(true),
    _running_state(ims_running_state_run),
    _autonomous_realtime(true),
    _steps_requested(0),
    _port_instrumentation(NULL),
    _powersupply_on(false),
    _time_ratio(1.0f),
    _time_us(0),
    _output_queue(new output_queue())
{
}

void context::set_vc_name(std::string vc_name)
{
    _vc_name = vc_name;
}

std::string context::get_vc_name()
{
    return _vc_name;
}

const char * context::virtual_component_name()
{
    return _vc_name.c_str();
}

void context::set_prod_id(uint32_t prod_id)
{
    _prod_id = prod_id;
}

uint32_t context::get_prod_id()
{
    return _prod_id;
}

void context::set_period_us(uint32_t period_us)
{
    _period_us = period_us;
}

uint32_t context::get_period_us()
{
    return _period_us;
}

void context::set_step_by_step_enabled(bool step_by_step_enabled)
{
    _step_by_step_enabled = step_by_step_enabled;
}

bool context::is_step_by_step_enabled()
{
    return _step_by_step_enabled;
}

ims_running_state_t context::get_running_state()
{
    return _running_state;
}

void context::set_running_state(ims_running_state_t state)
{
    _running_state = state;
}

bool context::is_autonomous_realtime()
{
    return _autonomous_realtime;
}

void context::set_autonomous_realtime(bool autonomous)
{
    _autonomous_realtime = autonomous;
}

uint32_t context::get_synchro_steps_request()
{
    uint32_t temp = _steps_requested;
    _steps_requested = 0;
    return temp;
}

void context::set_synchro_steps_request(uint32_t steps, port_instrumentation * port_instrumentation)
{
    _steps_requested = steps;
    _port_instrumentation = port_instrumentation;
    _time_us_before_notify = steps * (uint64_t)_period_us;
}

bool context::is_powersupply_on()
{
    return _powersupply_on;
}

void context::set_powersupply_on(bool powersupply)
{
    _powersupply_on = powersupply;
}

float context::get_time_ratio()
{
    return _time_ratio;
}

void context::set_time_ratio(float time_ratio)
{
    _time_ratio = time_ratio;
}

ims_return_code_t context::import(uint32_t timeout)
{
    return _socket_pool->import(timeout);
}

output_queue_ptr context::get_output_queue()
{
    return _output_queue;
}

socket_pool_ptr context::get_socket_pool()
{
    return _socket_pool;
}

uint64_t context::get_time_us()
{
    return _time_us;
}

uint64_t context::get_posix_timestamp()
{
    return _posix_timestamp;
}

}

#endif
