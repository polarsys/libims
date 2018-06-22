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
// Create IMS context
//
#include "ims_context.hh"
#include "ims_config_xsd.h"
#include "xml_parser.hh"
#include "ims_init_parser.hh"
#include "a429_tools.hh"
#include <string.h>

#define ATTRIBUTE_NAME               "Name"
#define ATTRIBUTE_PROD_ID            "ProdId"
#define ATTRIBUTE_RUNNING_STATE      "StartupRunningState"
#define ATTRIBUTE_POWER_STATE        "PowerState"
#define ATTRIBUTE_LOCAL_NAME         "LocalName"
#define ATTRIBUTE_PERIOD             "PeriodUs"
#define ATTRIBUTE_SIZE               "MessageSizeBytes"
#define ATTRIBUTE_MAX_SIZE           "MaxSizeBytes"
#define ATTRIBUTE_VALIDITY_DURATION  "ValidityDurationUs"
#define ATTRIBUTE_LABEL_NUMBER       "Number"
#define ATTRIBUTE_LABEL_SDI          "Sdi"
#define ATTRIBUTE_TRUE_STATE         "TrueState"
#define ATTRIBUTE_FALSE_STATE        "FalseState"
#define ATTRIBUTE_SCALE_FACTOR       "ScaleFactor"
#define ATTRIBUTE_OFFSET             "Offset"
#define ATTRIBUTE_QUEUE_DEPTH        "QueueDepth"
#define ATTRIBUTE_MESSAGE_ID         "Id"
#define ATTRIBUTE_MESSAGE_DEFAULT_ID "DefaultId"
#define ATTRIBUTE_MESSAGE_TYPE       "MessageType"
#define ATTRIBUTE_MESSAGE_DIM1       "MessageDimension1"
#define ATTRIBUTE_MESSAGE_DIM2       "MessageDimension2"
#define NODE_DATA_EXCHANGE           "DataExchange"
#define NODE_EQUIPMENT               "Equipment"
#define NODE_APPLICATION             "Application"
#define NODE_INPUT_DATA              "ConsumedData"
#define NODE_OUTPUT_DATA             "ProducedData"
#define NODE_PROTO_AFDX              "AFDX"
#define NODE_PROTO_A429              "A429"
#define NODE_PROTO_CAN               "CAN"
#define NODE_PROTO_NAD               "NAD"
#define NODE_PROTO_DISCRETE          "DISCRETE"
#define NODE_PROTO_ANALOGUE          "ANALOGUE"
#define NODE_AFDX_SAMPLING           "SamplingMessage"
#define NODE_AFDX_QUEUING            "QueuingMessage"
#define NODE_A429_SAMPLING           "SamplingLabel"
#define NODE_A429_QUEUING            "QueuingLabel"
#define NODE_CAN_SAMPLING            "SamplingMessage"
#define NODE_CAN_QUEUING             "QueuingMessage"

namespace ims
{

//
// A deleter that does nothing
//
struct noop_deleter {
    void operator ()(void *) {}
};

//
// Internal factory class
//
class context::factory : public xml_parser
{
public:
    context* create_context(backend::context::factory_ptr backend_factory, const char* config_file_path, ims_create_context_parameter_t* create_context_parameter);

private:
    // Load DataExchange
    void load_data_exchange(node_ptr ims_node, xmlNodePtr xml_node);

    // Load all direction nodes in the given parent node and given protocol
    template <ims_protocol_t>
    void load_directions(node_ptr ims_parent_node, xmlNodePtr xml_parent_node);

    // Load all bus nodes in the given parent node and given protocol
    template <ims_protocol_t>
    void load_buses(node_ptr ims_parent_node, xmlNodePtr xml_parent_node, ims_direction_t direction);
    
    // Load A429 labels from given bus
    void load_a429_bus(node_ptr ims_parent_node, xmlNodePtr bus_node,
                       ims_direction_t direction, std::string bus_name);

    // Load CAN messages from given bus
    void load_can_bus(node_ptr ims_parent_node, xmlNodePtr bus_node,
                      ims_direction_t direction, std::string bus_name);

    backend::context::factory_ptr _backend_factory;
    ims_init_parser _init_parser;
};

//
// Load AFDX buses/messages
//
template <>
void context::factory::load_buses<ims_afdx>(node_ptr        ims_parent_node,
                                            xmlNodePtr      xml_parent_node,
                                            ims_direction_t direction)
{
    for(xmlNodePtr message_node = xml_node_first_child(xml_parent_node);
        message_node != NULL;
        message_node = xml_node_next_sibling(message_node))
    {
        std::string mode_name((const char*)message_node->name);
        std::string message_local_name(xml_node_property(message_node, ATTRIBUTE_LOCAL_NAME));
        std::string message_name(xml_node_property(message_node, ATTRIBUTE_NAME));

        if (mode_name == NODE_AFDX_SAMPLING) {
            uint32_t size = xml_node_property_uint(message_node, ATTRIBUTE_SIZE);
            uint32_t validity_duration_us = xml_node_property_uint(message_node, ATTRIBUTE_VALIDITY_DURATION);
            uint32_t period_us = xml_node_property_uint(message_node, ATTRIBUTE_PERIOD, 0);

            message_ptr new_message = _backend_factory->create_afdx_sampling(ims_parent_node, message_name,
                                                                             direction, size, validity_duration_us, message_local_name, period_us);
            if ( _init_parser.hasAfdxSamplingMessageValue(message_local_name) )
            {
                std::string value = _init_parser.getAfdxSamplingMessageValue(message_local_name);
                if ( value.length() > size )
                {
                    LOG_WARN("The AFDX message " << message_name << " has a size of " << size << " but its init value has a size of " << value.length() << " : the init value has been truncated." );
                    new_message->init_data( value.data(), size );
                }
                else
                {
                    new_message->init_data( value.data(), value.length() );
                }

            }
            ims_parent_node->add_message(message_local_name, new_message);
        }

        else {
            uint32_t max_size = xml_node_property_uint(message_node, ATTRIBUTE_MAX_SIZE);
            uint32_t queue_depth = xml_node_property_uint(message_node, ATTRIBUTE_QUEUE_DEPTH);

            message_ptr new_message = _backend_factory->create_afdx_queuing(ims_parent_node, message_name,
                                                                            direction, max_size, queue_depth, message_local_name);
            ims_parent_node->add_message(message_local_name, new_message);
        }
    }
}

//
// Load A429 buses
//
template <>
void context::factory::load_buses<ims_a429>(node_ptr        ims_parent_node,
                                            xmlNodePtr      xml_parent_node,
                                            ims_direction_t direction)
{
    for(xmlNodePtr bus_node = xml_node_first_child(xml_parent_node);
        bus_node != NULL;
        bus_node = xml_node_next_sibling(bus_node))
    {
        std::string bus_name(xml_node_property(bus_node, ATTRIBUTE_NAME));
        load_a429_bus(ims_parent_node, bus_node, direction, bus_name);
    }
}

//
// Load CAN buses
//
template <>
void context::factory::load_buses<ims_can>(node_ptr        ims_parent_node,
                                           xmlNodePtr      xml_parent_node,
                                           ims_direction_t direction)
{
    for(xmlNodePtr bus_node = xml_node_first_child(xml_parent_node);
        bus_node != NULL;
        bus_node = xml_node_next_sibling(bus_node))
    {
        std::string bus_name(xml_node_property(bus_node, ATTRIBUTE_NAME));
        load_can_bus(ims_parent_node, bus_node, direction, bus_name);
    }
}

//
// Load Discrete signals
//
template <>
void context::factory::load_buses<ims_discrete>(node_ptr        ims_parent_node,
                                                xmlNodePtr      xml_parent_node,
                                                ims_direction_t direction)
{
    for(xmlNodePtr signal_node = xml_node_first_child(xml_parent_node);
        signal_node != NULL;
        signal_node = xml_node_next_sibling(signal_node))
    {
        std::string signal_name(xml_node_property(signal_node, ATTRIBUTE_NAME));
        uint32_t signal_true_state(xml_node_property_uint(signal_node, ATTRIBUTE_TRUE_STATE, 1));
        uint32_t signal_false_state(xml_node_property_uint(signal_node, ATTRIBUTE_FALSE_STATE, 0));
        std::string signal_local_name(xml_node_property(signal_node, ATTRIBUTE_LOCAL_NAME));
        
        uint32_t validity_duration_us = xml_node_property_uint(signal_node, ATTRIBUTE_VALIDITY_DURATION, 0);

        uint32_t port_period_us(xml_node_property_uint(signal_node, ATTRIBUTE_PERIOD, 0));


        message_ptr new_message = _backend_factory->create_discrete_signal(ims_parent_node, signal_name,
                                                                           direction, validity_duration_us,
                                                                           signal_true_state,
                                                                           signal_false_state,
                                                                           port_period_us,
                                                                           signal_local_name);

        if ( _init_parser.hasDiscreteSignalValue(signal_name) )
        {
            char default_value;
            std::string default_value_str = _init_parser.getDiscreteSignalValue(signal_name);
            if (default_value_str == "1"    ||
                    default_value_str == "true" ||
                    default_value_str == "TRUE")
            {
                default_value = 1;
            }
            else
            {
                default_value = 0;
            }
            
            new_message->init_data( &default_value, 1 );
        }

        ims_parent_node->add_message(signal_local_name, new_message);
    }
}

//
// Load Analogue signals
//
template <>
void context::factory::load_buses<ims_analogue>(node_ptr        ims_parent_node,
                                                xmlNodePtr      xml_parent_node,
                                                ims_direction_t direction)
{
    for(xmlNodePtr signal_node = xml_node_first_child(xml_parent_node);
        signal_node != NULL;
        signal_node = xml_node_next_sibling(signal_node))
    {
        std::string signal_name(xml_node_property(signal_node, ATTRIBUTE_NAME));
        std::string signal_local_name(xml_node_property(signal_node, ATTRIBUTE_LOCAL_NAME));
        float signal_scale_factor(xml_node_property_float(signal_node, ATTRIBUTE_SCALE_FACTOR));
        float signal_offset(xml_node_property_float(signal_node, ATTRIBUTE_OFFSET));
        uint32_t port_period_us(xml_node_property_uint(signal_node, ATTRIBUTE_PERIOD, 0));
        
        uint32_t validity_duration_us = xml_node_property_uint(signal_node, ATTRIBUTE_VALIDITY_DURATION, 0);


        message_ptr new_message = _backend_factory->create_analogue_signal(ims_parent_node, signal_name,
                                                                           direction, validity_duration_us, signal_scale_factor, signal_offset,
                                                                           port_period_us,
                                                                           signal_local_name);

        if ( _init_parser.hasAnalogueSignalValue(signal_name) )
        {
            float signal_default_value = _init_parser.getAnalogueSignalValue(signal_name);
            new_message->init_data((const char*) &signal_default_value, sizeof(float) );
        }

        ims_parent_node->add_message(signal_local_name, new_message);
    }
}

//
// Load NAD messages
//
template <>
void context::factory::load_buses<ims_nad>(node_ptr        ims_parent_node,
                                           xmlNodePtr      xml_parent_node,
                                           ims_direction_t direction)
{
    for(xmlNodePtr message_node = xml_node_first_child(xml_parent_node);
        message_node != NULL;
        message_node = xml_node_next_sibling(message_node))
    {
        std::string message_name(xml_node_property(message_node, ATTRIBUTE_NAME));
        std::string message_local_name(xml_node_property(message_node, ATTRIBUTE_LOCAL_NAME));
        uint32_t message_size(xml_node_property_uint(message_node, ATTRIBUTE_SIZE));
        uint32_t port_period_us(xml_node_property_uint(message_node, ATTRIBUTE_PERIOD, 0));

        std::string type_string(xml_node_property(message_node, ATTRIBUTE_MESSAGE_TYPE, "undefined"));
        ims_nad_type_t nad_type = ims_nad_undefined;
        if (type_string == "BOOLEAN") {
            nad_type = ims_nad_boolean;
        } else if (type_string == "C08") {
            nad_type = ims_nad_c08;
        } else if (type_string == "F32") {
            nad_type = ims_nad_f32;
        } else if (type_string == "F64") {
            nad_type = ims_nad_f64;
        } else if (type_string == "I16") {
            nad_type = ims_nad_i16;
        } else if (type_string == "I32") {
            nad_type = ims_nad_i32;
        } else if (type_string == "I64") {
            nad_type = ims_nad_i64;
        }

        uint32_t nad_dim1(xml_node_property_uint(message_node, ATTRIBUTE_MESSAGE_DIM1, 0));
        uint32_t nad_dim2(xml_node_property_uint(message_node, ATTRIBUTE_MESSAGE_DIM2, 0));

        message_ptr new_message = _backend_factory->create_nad_message(ims_parent_node, message_name,
                                                                       direction, message_size, port_period_us, message_local_name, nad_type, nad_dim1, nad_dim2);

        ims_parent_node->add_message(message_local_name, new_message);
    }
}

//
// Load A429 labels from given bus
//
void context::factory::load_a429_bus(node_ptr ims_parent_node, xmlNodePtr bus_node,
                                     ims_direction_t direction, std::string bus_name)
{
    for(xmlNodePtr label_node = xml_node_first_child(bus_node);
        label_node != NULL;
        label_node = xml_node_next_sibling(label_node))
    {
        std::string mode_name((const char*)label_node->name);
        std::string label_local_name(xml_node_property(label_node, ATTRIBUTE_LOCAL_NAME));

        if (mode_name == NODE_A429_SAMPLING) {
            std::string label_number(xml_node_property(label_node, ATTRIBUTE_LABEL_NUMBER));
            std::string label_sdi(xml_node_property(label_node, ATTRIBUTE_LABEL_SDI));
            uint32_t validity_duration_us(xml_node_property_uint(label_node, ATTRIBUTE_VALIDITY_DURATION));
            uint32_t period_us(xml_node_property_uint(label_node, ATTRIBUTE_PERIOD, 0));


            message_ptr new_message = _backend_factory->create_a429_sampling(ims_parent_node, bus_name, label_number, label_sdi,
                                                                             direction, validity_duration_us, label_local_name, period_us);
            if ( _init_parser.hasA429LabelMessageValue(label_local_name) )
            {
                std::string value = _init_parser.getA429LabelMessageValue(label_local_name);

                a429::label_t init_value = {0,0,0,0};
                memcpy( init_value + 4 - value.length(), value.data(), value.length() );

                a429::label_number_t number = a429::label_number_encode(label_number);
                a429:: sdi_t sdi = a429::sdi_encode(label_sdi);

                a429::update_label(init_value, number, sdi);

                new_message->init_data( (const char*) init_value, 4 );
            }
            ims_parent_node->add_message(label_local_name, new_message);
        }

        else {
            std::string label_number(xml_node_property(label_node, ATTRIBUTE_LABEL_NUMBER));
            std::string label_sdi(xml_node_property(label_node, ATTRIBUTE_LABEL_SDI));
            uint32_t label_queue_depth(xml_node_property_uint(label_node, ATTRIBUTE_QUEUE_DEPTH));


            message_ptr new_message = _backend_factory->create_a429_queuing(ims_parent_node, bus_name, label_number, label_sdi,
                                                                            direction, label_queue_depth, label_local_name);
            ims_parent_node->add_message(label_local_name, new_message);
        }
    }
}

//
// Load CAN messages from given bus
//
void context::factory::load_can_bus(node_ptr ims_parent_node, xmlNodePtr bus_node,
                                    ims_direction_t direction, std::string bus_name)
{
    for(xmlNodePtr message_node = xml_node_first_child(bus_node);
        message_node != NULL;
        message_node = xml_node_next_sibling(message_node))
    {
        std::string mode_name((const char*)message_node->name);
        std::string message_local_name(xml_node_property(message_node, ATTRIBUTE_LOCAL_NAME));

        if (mode_name == NODE_CAN_SAMPLING) {
			uint32_t message_id;
			try 
			{
				message_id = xml_node_property_hex(message_node, ATTRIBUTE_MESSAGE_ID);
			}catch( ims::exception )
			{
				message_id = xml_node_property_hex(message_node, ATTRIBUTE_MESSAGE_DEFAULT_ID);
			}

            uint32_t message_size(xml_node_property_uint(message_node, ATTRIBUTE_SIZE));
            uint32_t validity_duration_us(xml_node_property_uint(message_node, ATTRIBUTE_VALIDITY_DURATION));
            uint32_t period_us = xml_node_property_uint(message_node, ATTRIBUTE_PERIOD, 0);

            message_ptr new_message = _backend_factory->create_can_sampling(ims_parent_node, bus_name, message_id, message_size,
                                                                            direction, validity_duration_us, message_local_name, period_us);
            if ( _init_parser.hasCanSamplingMessageValue(message_local_name) )
            {
                std::string value = _init_parser.getCanSamplingMessageValue(message_local_name);

                uint8_t maxcan[8] = {0,0,0,0,0,0,0,0};

                if ( value.length() > message_size )
                {
                    LOG_WARN("The CAN message " << message_id << " has a size of " << message_size << " but its init value has a size of " << value.length() << " : the init value has been truncated." );
                    memcpy( maxcan, value.data() + value.length() - message_size , message_size  );
                    new_message->init_data( (const char *) maxcan, message_size );
                }
                else
                {
                    memcpy( maxcan + message_size - value.length(), value.data(), value.length()  );
                    new_message->init_data( (const char *) maxcan, message_size );
                }
            }
            ims_parent_node->add_message(message_local_name, new_message);
        }

        else {
            LOG_ERROR("CAN QUEUING not supported yet !");
        }
    }
}

//
// Load directions node
//
template <ims_protocol_t protocol>
void context::factory::load_directions(node_ptr        ims_parent_node,
                                       xmlNodePtr      xml_parent_node)
{
    for(xmlNodePtr direction_node = xml_node_first_child(xml_parent_node);
        direction_node != NULL;
        direction_node = xml_node_next_sibling(direction_node))
    {
        ims_direction_t direction = (strcmp((const char*) direction_node->name, NODE_INPUT_DATA) == 0)? ims_input : ims_output;
        load_buses<protocol>(ims_parent_node, direction_node, direction);
    }
}

//
// Load DataExchange
//
void context::factory::load_data_exchange(node_ptr   ims_node,
                                          xmlNodePtr xml_node)
{
    xmlNodeSetPtr node_set = xml_xpath_get_children(_doc, xml_node, "DataExchange");
    if (node_set == NULL) return;

    try {
        if (node_set->nodeNr != 1) {
            THROW_XML_ERROR(this, "Severals DataExchange in node '" << ims_node->get_path() << "'!");
        }

        xmlNodePtr data_exchange_node = node_set->nodeTab[0];

        for(xmlNodePtr protocol_node = xml_node_first_child(data_exchange_node);
            protocol_node != NULL;
            protocol_node = xml_node_next_sibling(protocol_node))
        {
            std::string protocol_name((const char*) protocol_node->name);
            if (protocol_name == NODE_PROTO_AFDX) {
                load_directions<ims_afdx>(ims_node, protocol_node);
            }
            else if (protocol_name == NODE_PROTO_A429) {
                load_directions<ims_a429>(ims_node, protocol_node);
            }
            else if (protocol_name == NODE_PROTO_CAN) {
                load_directions<ims_can>(ims_node, protocol_node);
            }
            else if (protocol_name == NODE_PROTO_DISCRETE) {
                load_directions<ims_discrete>(ims_node, protocol_node);
            }
            else if (protocol_name == NODE_PROTO_ANALOGUE) {
                load_directions<ims_analogue>(ims_node, protocol_node);
            }
            else if (protocol_name == NODE_PROTO_NAD) {
                load_directions<ims_nad>(ims_node, protocol_node);
            }
            else {
                THROW_NODE_ERROR(protocol_node, "Unkown protocol '" << protocol_name << "'!");
            }
        }

        xmlXPathFreeNodeSet(node_set);

    } catch(...) {
        xmlXPathFreeNodeSet(node_set);
        throw;
    }
}

//
// Main create function
//
context* context::factory::create_context(backend::context::factory_ptr backend_factory, const char* config_file_path, ims_create_context_parameter_t* create_context_parameter)
{
    context* context = NULL;

    try {
        _backend_factory = backend_factory;
        load_and_validate(config_file_path, ims_config_xsd);
        xmlNodePtr vc_node = xmlDocGetRootElement(_doc);

        // The context cannot be hold by a shared_ptr because its unique
        // reference will be returned throw the C interface.
        // So, to create the associated node_ptr we provide a deleter that does nothing.
        std::string vc_name = xml_node_property(vc_node, ATTRIBUTE_NAME);
        uint32_t prod_id = xml_node_property_uint(vc_node, ATTRIBUTE_PROD_ID, 0);
        uint32_t period_us = 0;
        bool step_by_step = false;
        const char * init_file_path = NULL;
        if (create_context_parameter != NULL)
        {
            if (create_context_parameter->struct_size >= 8)
            {
                period_us = create_context_parameter->period_us;
            }
            if (create_context_parameter->struct_size >= 12)
            {
                // step by step is only possible if the period is defined
                step_by_step = create_context_parameter->step_by_step_supported != 0 && period_us != 0;
            }
            if (create_context_parameter->struct_size >= 12 + sizeof(char*) )
            {
                init_file_path = create_context_parameter->init_file_path;
            }
        }

        if (init_file_path != NULL)
        {
            LOG_INFO("Reading init XML file : " << init_file_path);
            _init_parser.load_and_validate(init_file_path);
        }

        _backend_factory->init(vc_name, prod_id, period_us, step_by_step);
        context = new ims::context(vc_name);

        // Get the running state
        std::string running_state_value = xml_node_property(vc_node, ATTRIBUTE_RUNNING_STATE, "run");
        ims_running_state_t running_state = ims_running_state_run;
        if ( running_state_value == "step" )
        {
            running_state = ims_running_state_step;
        }
        else if ( running_state_value == "hold" )
        {
            running_state = ims_running_state_hold;
        }
        else if ( running_state_value == "reset" )
        {
            running_state = ims_running_state_reset;
        }

        // Get the power state
        std::string power_state_value = xml_node_property(vc_node, ATTRIBUTE_POWER_STATE, "off");
        bool power_state = false;
        if ( power_state_value == "on" )
        {
            power_state = true;
        }

        load_data_exchange(node_ptr(context, noop_deleter()), vc_node);

        // Load equipments
        xmlNodeSetPtr equipment_node_set = xml_xpath_get_children(_doc, vc_node, NODE_EQUIPMENT);
        if (equipment_node_set != NULL) {
            for (int32_t equipment_node_id = 0;
                 equipment_node_id < equipment_node_set->nodeNr;
                 equipment_node_id++)
            {
                xmlNodePtr equipment_node = equipment_node_set->nodeTab[equipment_node_id];
                node_ptr equipment;
                try {
                    equipment = node::create(xml_node_property(equipment_node, ATTRIBUTE_NAME), context);
                    context->add_child(equipment);
                } catch (ims::exception& e) {
                    LOG_ERROR("Failed to add equipment !");
                    throw;
                }

                load_data_exchange(equipment, equipment_node);


                // Load applications
                xmlNodeSetPtr application_node_set = xml_xpath_get_children(_doc, equipment_node, NODE_APPLICATION);
                if (application_node_set) {
                    for (int32_t application_node_id = 0;
                         application_node_id < application_node_set->nodeNr;
                         application_node_id++)
                    {
                        xmlNodePtr application_node = application_node_set->nodeTab[application_node_id];
                        node_ptr application;
                        try {
                            application = node::create(xml_node_property(application_node, ATTRIBUTE_NAME), equipment.get());
                            equipment->add_child(application);
                        } catch (ims::exception& e) {
                            LOG_ERROR("Failed to add application !");
                            throw;
                        }
                        load_data_exchange(application, application_node);
                    }

                    xmlXPathFreeNodeSet(application_node_set);
                }
            }

            xmlXPathFreeNodeSet(equipment_node_set);
        }


        // Initialize the backend context and finalize context initialization
        context->_backend_context = backend_factory->build();
        context->_backend_context->set_running_state(running_state);
        context->_backend_context->set_powersupply_on(power_state);
        return context;

    } catch(...) {
        delete context;
        context = NULL;
        throw;
    }
}

//
// Interface implementation
//
context* context::create(backend::context::factory_ptr backend_factory, const char* config_file_path, ims_create_context_parameter_t* create_context_parameter)
throw(ims::exception)
{
    context::factory factory;
    return factory.create_context(backend_factory, config_file_path, create_context_parameter);
}

}
