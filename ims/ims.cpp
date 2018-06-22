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
// C interface implementation
//

/************
 * Includes *
 ************/

#include "ims.h"
#include "ims_log.hh"
#include "ims_context.hh"
#include "ims_list.hh"

/*****************
 * Generic catch *
 *****************/

#define CATCH(default_return_code, message)  \
    catch(ims::exception& error) {           \
    LOG_ERROR(message);                      \
    return error.get_ims_return_code();      \
    }                                        \
    catch(std::exception& error) {           \
    LOG_ERROR(message);                      \
    LOG_ERROR(error.what());                 \
    return default_return_code;              \
    }                                        \
    catch(...) {                             \
    LOG_ERROR(message << " (unkown error)"); \
    return default_return_code;              \
    }

/****************************
 * Library name and version *
 ****************************/

#ifdef LIBIMS_VERSION
# define QUOTE(x) #x
# define EXPAND(x) QUOTE(x)
const char* _ims_version = EXPAND(LIBIMS_VERSION);
#else
const char* _ims_version = "unversioned";
#endif

const char* ims_implementation_name()
{
    return backend::get_name();
}

const char* ims_implementation_version()
{
    return _ims_version;
}

const char* ims_virtual_component_name(ims_node_t ims_context)
{
    LOG_INFO("CALL ims_virtual_component_name()");
    return static_cast<ims::context*>(ims_context)->get_backend_context()->virtual_component_name();
}

/******************
 * Initialization *
 ******************/

ims_return_code_t ims_create_context(const char*  ims_config_file_path,
                                     const char*  backend_config_file_path,
                                     ims_create_context_parameter_t* create_context_parameter,
                                     ims_node_t*  ims_context)
{
    LOG_INFO("CALL ims_create_context('" << ims_config_file_path << "', '" << backend_config_file_path << "')");

    *ims_context = NULL;

    try {
        backend::context::factory_ptr backend_factory =
                backend::create_factory(backend_config_file_path);

        *ims_context = ims::context::create(backend_factory, ims_config_file_path, create_context_parameter);
    }

    CATCH(ims_init_failure, "Failed to initialize IMS API !");

    // Display context
    if (IS_LOG_INFO()) {
        static_cast<ims::context*>(*ims_context)->dump();
    }

    return ims_no_error;
}

void ims_free_context(ims_node_t ims_context)
{
    LOG_INFO("CALL ims_free_context()");
    delete static_cast<ims::context*>(ims_context);
}

ims_running_state_t ims_get_running_state(ims_node_t ims_context)
{
    LOG_INFO("CALL ims_get_running_state()");
    return static_cast<ims::context*>(ims_context)->get_backend_context()->get_running_state();
}

float ims_get_time_ratio(ims_node_t ims_context)
{
    LOG_INFO("CALL ims_get_time_ratio()");
    return static_cast<ims::context*>(ims_context)->get_backend_context()->get_time_ratio();
}

bool ims_is_powersupply_on(ims_node_t ims_context)
{
    LOG_INFO("CALL ims_is_powersupply_on()");
    return static_cast<ims::context*>(ims_context)->get_backend_context()->is_powersupply_on();
}

bool ims_is_autonomous_realtime(ims_node_t ims_context)
{
    LOG_INFO("CALL ims_is_autonomous_realtime()");
    return static_cast<ims::context*>(ims_context)->get_backend_context()->is_autonomous_realtime();
}

uint32_t ims_get_synchro_steps_request(ims_node_t ims_context)
{
    LOG_INFO("CALL ims_get_synchro_steps_request()");
    return static_cast<ims::context*>(ims_context)->get_backend_context()->get_synchro_steps_request();
}

/*************
 * Accessors *
 *************/

ims_return_code_t ims_node_get_child(ims_node_t  parent_base, 
                                     const char* name,
                                     ims_node_t* child_base)
{
    ims::weak_node_ptr parent = static_cast<ims::weak_node_ptr>(parent_base);

    LOG_INFO("CALL ims_node_get_child('" << parent->get_path() << "', '" << name << "')");

    try
    {
        ims::node_ptr child = parent->find_child(name);
        *child_base = child.get();
    }

    CATCH(ims_invalid_configuration, "Failed to find child '" << parent->get_path()  << '/' << name << "'!");

    return ims_no_error;
}

ims_return_code_t ims_get_equipment(ims_node_t  ims_context,
                                    const char* name,
                                    ims_node_t* equipment)
{
    return ims_node_get_child(ims_context, name, equipment);
}

ims_return_code_t ims_get_application(ims_node_t equipment,
                                      const char*  name,
                                      ims_node_t*  application)
{
    return ims_node_get_child(equipment, name, application);
}

ims_return_code_t ims_node_get_children(ims_node_t       parent_base, 
                                        ims_node_list_t* node_list)
{
    ims::weak_node_ptr parent = static_cast<ims::weak_node_ptr>(parent_base);
    *node_list = new ims::node_children_list(parent);
    return ims_no_error;
}

ims_return_code_t ims_node_list_next(ims_node_list_t list_base,
                                     ims_node_t*     next_node)
{
    ims::node_children_list* list = static_cast<ims::node_children_list*>(list_base);
    *next_node = list->next();
    return ims_no_error;
}

ims_return_code_t ims_node_list_free(ims_node_list_t list_base)
{
    ims::node_children_list* list = static_cast<ims::node_children_list*>(list_base);
    delete list;
    return ims_no_error;
}

const char* ims_node_get_name(ims_node_t node_base)
{
    ims::weak_node_ptr node = static_cast<ims::weak_node_ptr>(node_base);
    return node->get_name().c_str();
}

ims_return_code_t ims_get_message(ims_node_t        node_base,
                                  ims_protocol_t    message_protocol,
                                  const char*       message_key,
                                  uint32_t          message_max_size,
                                  uint32_t          message_depth,
                                  ims_direction_t   message_direction,
                                  ims_message_t*    message)
{
    LOG_INFO("CALL ims_get_message( "<< ims_node_get_name(node_base) << " , " <<ims_protocol_string(message_protocol)<<" , " << message_key << " , " << message_max_size <<" , " << message_depth << " , " << ims_direction_string(message_direction) << " )");
    *message = NULL;

    try
    {
        ims::message_ptr ims_message =
                static_cast<ims::weak_node_ptr>(node_base)->find_message(message_key);

        if (ims_message->check(message_protocol,
                               message_max_size,
                               message_depth,
                               message_direction) == false) {
            return ims_invalid_configuration;
        }

        *message = ims_message.get();
    }
    CATCH(ims_invalid_configuration, "Failed to find message '" << message_key << "'!");

    return ims_no_error;
}

ims_return_code_t ims_node_get_messages(ims_node_t           parent_base, 
                                        ims_messages_list_t* messages_list)
{
    ims::weak_node_ptr parent = static_cast<ims::weak_node_ptr>(parent_base);
    *messages_list = new ims::node_messages_list(parent);
    return ims_no_error;
}

ims_return_code_t ims_messages_list_next(ims_messages_list_t list_base,
                                         ims_message_t*      next_message)
{
    ims::node_messages_list* list = static_cast<ims::node_messages_list*>(list_base);
    *next_message = list->next();
    return ims_no_error;
}

ims_return_code_t ims_messages_list_free(ims_messages_list_t list_base)
{
    ims::node_messages_list* list = static_cast<ims::node_messages_list*>(list_base);
    delete list;
    return ims_no_error;
}

const char* ims_message_get_name(ims_message_t message_base)
{
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    return message->get_name().c_str();
}

const char* ims_message_get_local_name(ims_message_t message_base)
{
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    return message->get_local_name().c_str();
}

const char* ims_message_get_bus_name(ims_message_t message_base)
{
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    return message->get_bus_name().c_str();
}

ims_protocol_t ims_message_get_protocol(ims_message_t message_base)
{
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    return message->get_protocol();
}

ims_direction_t ims_message_get_direction(ims_message_t message_base)
{
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    return message->get_direction();
}

ims_mode_t ims_message_get_mode(ims_message_t message_base)
{
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    return message->get_mode();
}

uint32_t ims_message_get_validity_duration_us(ims_message_t message_base)
{
    uint32_t validity_duration_us = 0;
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    try {
        message->get_validity_duration(&validity_duration_us);
    }
    CATCH(ims_invalid_configuration, "Cannot get validity duration of message " << message->get_name() << "!");

    return validity_duration_us;
}

uint32_t ims_message_get_period_us(ims_message_t message_base)
{
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    return message->get_period_us();
}

ims_nad_type_t ims_message_get_nad_type(ims_message_t message_base)
{
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    return message->get_nad_type();
}

uint32_t ims_message_get_nad_dim1(ims_message_t message_base)
{
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    return message->get_nad_dim1();
}

uint32_t ims_message_get_nad_dim2(ims_message_t message_base)
{
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    return message->get_nad_dim2();
}

uint32_t ims_message_get_max_size(ims_message_t message_base)
{
    uint32_t max_size = 0;
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    try {
        message->get_max_size(&max_size);
    }
    CATCH(ims_invalid_configuration, "Cannot get max size of message " << message->get_name() << "!");

    return max_size;
}

uint32_t ims_message_get_depth(ims_message_t message_base)
{
    uint32_t depth = 0;
    ims::weak_message_ptr message = static_cast<ims::weak_message_ptr>(message_base);
    try {
        message->get_depth(&depth);
    }
    CATCH(ims_invalid_configuration, "Cannot get depth of message " << message->get_name() << "!");

    return depth;
}

ims_return_code_t ims_set_id(ims_message_t message_base,
						     uint32_t message_id)
{
	ims::message* message = static_cast<ims::message*>(message_base);
    try {
        return message->set_id(message_id);
    }
    CATCH(ims_invalid_configuration, "Cannot set id of message " << message->get_name() << "!");
}

ims_return_code_t ims_message_set_sampling_timeout(ims_message_t message_base,
                                                   uint32_t      validity_duration_us)
{
    try {
        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_set_sampling_message_timeout(" << message->get_name() << ", " << validity_duration_us << ")");

        return message->set_validity_duration(validity_duration_us);
    }
    CATCH(ims_implementation_specific_error, "Failed to update message validity duration!");
}

/**************
 * Read/write *
 **************/

ims_return_code_t ims_write_sampling_message(ims_message_t message_base,
                                             const char*   message_addr,
                                             uint32_t      message_size)
{

    try {
        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_write_sampling_message(" << message->get_name() << ")");

        return message->write_sampling(message_addr, message_size);
    }
    CATCH(ims_implementation_specific_error, "Failed to write message!");
}

ims_return_code_t ims_read_sampling_message(ims_message_t   message_base,
                                            char*           message_addr,
                                            uint32_t*       message_size,
                                            ims_validity_t* message_validity)
{
    try {
        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_read_sampling_message(" << message->get_name() << ")");

        return message->read_sampling(message_addr, message_size, message_validity);

    }
    CATCH(ims_implementation_specific_error, "Failed to read message!");
}

ims_return_code_t ims_push_queuing_message(ims_message_t message_base,
                                           const char*   message_addr,
                                           uint32_t      message_size)
{

    try {
        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_push_queuing_message(" << message->get_name() << ")");

        return message->push_queuing(message_addr, message_size);
    }
    CATCH(ims_implementation_specific_error, "Failed to push message!");
}

ims_return_code_t ims_pop_queuing_message(ims_message_t message_base, 
                                          char*         message_addr,
                                          uint32_t*     message_size)
{
    try {
        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_pop_queuing_message(" << message->get_name() << ")");

        return message->pop_queuing(message_addr, message_size);
    }
    CATCH(ims_implementation_specific_error, "Failed to pop message!");
}

ims_return_code_t ims_write_nad_message(ims_message_t message_base,
                                        const char*   message_addr,
                                        uint32_t      message_size)
{

    try {
        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_write_nad_message(" << message->get_name() << ")");

        return message->write_nad(message_addr, message_size);
    }
    CATCH(ims_implementation_specific_error, "Failed to write message!");
}

ims_return_code_t ims_read_nad_message(ims_message_t   message_base,
                                       char*           message_addr,
                                       uint32_t*       message_size,
                                       ims_validity_t* message_validity)
{
    try {
        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_read_nad_message(" << message->get_name() << ")");

        return message->read_nad(message_addr, message_size, message_validity);

    }
    CATCH(ims_implementation_specific_error, "Failed to read message!");
}

/****************
 * Send/receive *
 ****************/

ims_return_code_t ims_send_all(ims_node_t ims_context)
{
    LOG_INFO("CALL ims_send_all()");

    try {
        ims::context* context = static_cast<ims::context*>(ims_context);
        return context->get_backend_context()->send_all();
    }
    CATCH(ims_implementation_specific_error, "Failed to send messages!");
}

ims_return_code_t ims_import(ims_node_t ims_context, uint32_t timeout_us)
{
    try {
        ims::context* context = static_cast<ims::context*>(ims_context);
        return context->get_backend_context()->import(timeout_us);
    }
    CATCH(ims_implementation_specific_error, "Failed to import messages!");
}

/*****************
 * Time handling *
 *****************/

ims_return_code_t ims_progress(ims_node_t ims_context, uint32_t us)
{
    LOG_INFO("CALL ims_progress(" << us << ")");

    try {
        ims::context* context = static_cast<ims::context*>(ims_context);
        return context->get_backend_context()->progress(us);
    }
    CATCH(ims_implementation_specific_error, "Failed to import messages!");
}

/*****************************
 * Reset/invalidate messages *
 *****************************/

ims_return_code_t ims_reset_message(ims_message_t message_base)
{
    try {
        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_reset_message(" << message->get_name() << ")");

        return message->reset();
    }
    CATCH(ims_implementation_specific_error, "Failed to reset message!");
}

ims_return_code_t ims_reset_all(ims_node_t ims_context)
{
    LOG_INFO("CALL ims_reset_all()");

    try {
        ims::context* context = static_cast<ims::context*>(ims_context);
        return context->get_backend_context()->reset_all();
    }
    CATCH(ims_implementation_specific_error, "Failed to reset all !");


    return ims_no_error;
}

ims_return_code_t ims_invalidate_sampling_message(ims_message_t message_base)
{
    try {
        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_invalidate_sampling_message(" << message->get_name() << ")");

        return message->invalidate();
    }
    CATCH(ims_implementation_specific_error, "Failed to invalidate message!");
}


ims_return_code_t ims_invalidate_all_sampling_messages(ims_node_t ims_context)
{
    LOG_INFO("CALL ims_invalidate_all_messages()");

    try {
        ims::context* context = static_cast<ims::context*>(ims_context);
        return context->get_backend_context()->invalidate_all();
    }
    CATCH(ims_implementation_specific_error, "Failed to invalidate all messages!");


    return ims_no_error;
}

/********************
 * Messages pending *
 ********************/

ims_return_code_t ims_queuing_message_pending(ims_message_t message_base,
                                              uint32_t*     messages_count)
{
    try {
        ims::message* message = static_cast<ims::message*>(message_base);
        LOG_INFO("CALL ims_queuing_message_pending(" << message->get_name() << ")");

        return message->queuing_pending(messages_count);
    }
    CATCH(ims_implementation_specific_error, "Failed to get pending count!");
}

/***********
 * Helpers *
 ***********/

const char* ims_direction_string(ims_direction_t direction)
{
    switch (direction) {
    case ims_input:  return "INPUT";
    case ims_output: return "OUTPUT";
    }

    LOG_ERROR("invalid direction ID " << direction);
    return "invalid direction ID";
}


const char* ims_mode_string(ims_mode_t mode)
{
    switch (mode) {
    case ims_sampling: return "sampling";
    case ims_queuing:  return "queuing";
    }

    LOG_ERROR("invalid mode ID " << mode);
    return "invalid mode ID";
}

const char* ims_protocol_string(ims_protocol_t protocol)
{
    switch (protocol) {
    case ims_afdx:     return "AFDX";
    case ims_a429:     return "A429";
    case ims_can:      return "CAN";
    case ims_analogue: return "analogue";
    case ims_discrete: return "discrete";
    case ims_nad:      return "NAD";
    }

    LOG_ERROR("invalid protocol ID " << protocol);
    return "invalid protocol ID";
}
