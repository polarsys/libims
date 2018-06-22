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
// Backend context
//
#ifndef _BACKEND_CONTEXT_HH_
#define _BACKEND_CONTEXT_HH_
#include "shared_ptr.hh"
#include "ims_node.hh"

namespace backend
{

//
// Return the name of this backend
//
const char* get_name();

//
// Context for the backend
//
class context
{
public:
    class factory;
    typedef shared_ptr<context::factory> factory_ptr;

    //
    // Running state
    //
    virtual ims_running_state_t get_running_state() = 0;
    virtual void set_running_state(ims_running_state_t state) = 0;

    virtual float get_time_ratio() = 0;
    virtual void set_time_ratio(float time_ratio) = 0;

    virtual bool is_powersupply_on() = 0;
    virtual void set_powersupply_on(bool powersupply) = 0;

    virtual const char *virtual_component_name() = 0;

    //
    // Synchronization
    //
    virtual bool is_autonomous_realtime() = 0;
    virtual void set_autonomous_realtime(bool autonomous_realtime) = 0;

    virtual uint32_t get_synchro_steps_request() = 0;

    //
    // Import incoming data into messages
    //
    virtual ims_return_code_t import(uint32_t timeout) = 0;

    //
    // Send All prepared and periodic messages
    //
    virtual ims_return_code_t send_all() = 0;

    //
    // Progress time
    //
    virtual ims_return_code_t progress(uint32_t duration_us) = 0;

    //
    // Reset all messages
    //
    virtual ims_return_code_t reset_all() = 0;

    //
    // Invalidate all messages
    //
    virtual ims_return_code_t invalidate_all() = 0;

    //
    // Free
    //
    virtual ~context() {}

};

typedef shared_ptr<context> context_ptr;

//
// Implemented by the backend to create its factory
//
context::factory_ptr create_factory(const char* backend_config_file_path);

//
// Backend factory
//
class context::factory
{
public:

    //
    // Init the context with some common info
    //
    virtual void init(std::string vc_name,
                      uint32_t prod_id,
                      uint32_t period_us,
                      bool step_by_step_enabled
                      ) = 0;

    //
    // Create an AFDX sampling
    //
    virtual ims::message_ptr create_afdx_sampling(ims::node_ptr        ims_parent_node,
                                                  std::string          message_name,
                                                  ims_direction_t      direction,
                                                  uint32_t             size,
                                                  uint32_t             validity_duration_us,
                                                  std::string          local_name,
                                                  uint32_t             period_us) = 0;
    //
    // Create an AFDX queuing
    //
    virtual ims::message_ptr create_afdx_queuing(ims::node_ptr        ims_parent_node,
                                                 std::string          message_name,
                                                 ims_direction_t      direction,
                                                 uint32_t             max_size,
                                                 uint32_t             queue_depth,
                                                 std::string          local_name) = 0;

    //
    // Create an A429 sampling
    //
    virtual ims::message_ptr create_a429_sampling(ims::node_ptr        ims_parent_node,
                                                  std::string          bus_name,
                                                  std::string          label_number,
                                                  std::string          label_sdi,
                                                  ims_direction_t      direction,
                                                  uint32_t             validity_duration_us,
                                                  std::string          local_name,
                                                  uint32_t             period_us) = 0;

    //
    // Create an CAN sampling
    //
    virtual ims::message_ptr create_can_sampling(ims::node_ptr        ims_parent_node,
                                                 std::string          bus_name,
                                                 uint32_t             message_id,
                                                 uint32_t             message_size,
                                                 ims_direction_t      direction,
                                                 uint32_t             validity_duration_us,
                                                 std::string          local_name,
                                                 uint32_t             period_us) = 0;

    //
    // Create an A429 sampling
    //
    virtual ims::message_ptr create_a429_queuing(ims::node_ptr        ims_parent_node,
                                                 std::string          bus_name,
                                                 std::string          label_number,
                                                 std::string          label_sdi,
                                                 ims_direction_t      direction,
                                                 uint32_t             queue_depth,
                                                 std::string          local_name) = 0;

    //
    // Create a discrete signal
    //
    virtual ims::message_ptr create_discrete_signal(ims::node_ptr         ims_parent_node,
                                                    std::string           signal_name,
                                                    ims_direction_t       direction,
                                                    uint32_t              validity_duration_us,
                                                    uint32_t              true_state,
                                                    uint32_t              false_state,
                                                    uint32_t              port_period_us,
                                                    std::string           local_name) = 0;

    //
    // Create a analogue signal
    //
    virtual ims::message_ptr create_analogue_signal(ims::node_ptr         ims_parent_node,
                                                    std::string           signal_name,
                                                    ims_direction_t       direction,
                                                    uint32_t              validity_duration_us,
                                                    float                 scale_factor,
                                                    float                 gain,
                                                    uint32_t              port_period_us,
                                                    std::string           local_name) = 0;

    //
    // Create a NAD message
    //
    virtual ims::message_ptr create_nad_message(ims::node_ptr         ims_parent_node,
                                                std::string           message_name,
                                                ims_direction_t       direction,
                                                uint32_t              size,
                                                uint32_t              port_period_us,
                                                std::string           local_name,
                                                ims_nad_type_t        nad_type,
                                                uint32_t              nad_dim1,
                                                uint32_t              nad_dim2) = 0;

    //
    // Create the backend context.
    // It's the last witch will be called on the factory
    //
    virtual context_ptr build() = 0;

    //
    // Free
    //
    virtual ~factory() {}
};
}

#endif
