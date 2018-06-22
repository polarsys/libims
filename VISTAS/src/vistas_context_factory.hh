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
// Load VISTAS implementation
//
#ifndef _VISTAS_FACTORY_HH_
#define _VISTAS_FACTORY_HH_
#include "ims_log.hh"
#include "shared_ptr.hh"
#include "backend_context.hh"
#include "vistas_context.hh"

namespace vistas
{

class context::factory : public backend::context::factory
{
public:

    factory(const char* vistas_config_file_path) throw(ims::exception);

    ~factory();

    //
    // Init the context with some common info
    //
    void init(std::string vc_name,
              uint32_t prod_id,
              uint32_t period_us,
              bool step_by_step_enabled
              );

    // Create an AFDX sampling
    // @param name : joining key in vistas config file
    ims::message_ptr create_afdx_sampling(ims::node_ptr        ims_parent_node,
                                          std::string          message_name,
                                          ims_direction_t      direction,
                                          uint32_t             size,
                                          uint32_t             validity_duration_us,
                                          std::string          local_name,
                                          uint32_t             period_us);

    // Create an AFDX queuing
    // @param name : joining key in vistas config file
    ims::message_ptr create_afdx_queuing(ims::node_ptr        ims_parent_node,
                                         std::string          message_name,
                                         ims_direction_t      direction,
                                         uint32_t             max_size,
                                         uint32_t             queue_depth,
                                         std::string          local_name);

    // Create an A429 sampling
    // @param bus_name : joining key in vistas config file
    ims::message_ptr create_a429_sampling(ims::node_ptr        ims_parent_node,
                                          std::string          bus_name,
                                          std::string          label_number,
                                          std::string          label_sdi,
                                          ims_direction_t      direction,
                                          uint32_t             validity_duration_us,
                                          std::string          local_name,
                                          uint32_t             period_us);

    // Create an CAN sampling
    // @param bus_name : joining key in vistas config file
    ims::message_ptr create_can_sampling(ims::node_ptr        ims_parent_node,
                                         std::string          bus_name,
                                         uint32_t             message_id,
                                         uint32_t             message_size,
                                         ims_direction_t      direction,
                                         uint32_t             validity_duration_us,
                                         std::string          local_name,
                                         uint32_t             period_us);

    // Create an A429 sampling
    // @param bus_name : joining key in vistas config file
    ims::message_ptr create_a429_queuing(ims::node_ptr        ims_parent_node,
                                         std::string          bus_name,
                                         std::string          label_number,
                                         std::string          label_sdi,
                                         ims_direction_t      direction,
                                         uint32_t             queue_depth,
                                         std::string          local_name);

    // Create a discrete signal
    // @param name : joining key in vistas config file
    ims::message_ptr create_discrete_signal(ims::node_ptr         ims_parent_node,
                                            std::string           signal_name,
                                            ims_direction_t       direction,
                                            uint32_t              validity_duration_us,
                                            uint32_t              true_state,
                                            uint32_t              false_state,
                                            uint32_t              port_period_us,
                                            std::string           local_name);

    // Create a analogue signal
    // @param name : joining key in vistas config file
    ims::message_ptr create_analogue_signal(ims::node_ptr         ims_parent_node,
                                            std::string           signal_name,
                                            ims_direction_t       direction,
                                            uint32_t              validity_duration_us,
                                            float                 scale_factor,
                                            float                 gain,
                                            uint32_t              port_period_us,
                                            std::string           local_name);
    
    // Create a NAD message
    // @param name : joining key in vistas config file
    ims::message_ptr create_nad_message(ims::node_ptr         ims_parent_node,
                                        std::string           message_name,
                                        ims_direction_t       direction,
                                        uint32_t              size,
                                        uint32_t              port_period_us,
                                        std::string           local_name,
                                        ims_nad_type_t        nad_type,
                                        uint32_t              nad_dim1,
                                        uint32_t              nad_dim2);

    // Create the vistas context.
    // It's the last function to call on the factory !
    backend::context_ptr build();

private:
    template<typename port_factory, typename port_info_ptr>
    typename port_factory::port_ptr_t get_or_create_port(port_factory factory,
                                                         port_info_ptr port_info);

    void context_register_port(port_application_ptr port);
    
    class parser;

    parser*              _parser;
    socket_pool::factory _socket_pool_factory;
    context_ptr          _context;
};
}

#endif
