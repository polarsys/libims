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
#include "vistas_context_factory.hh"
#include "vistas_config_xsd.h"
#include "xml_parser.hh"
#include "vistas_message_sampling.hh"
#include "vistas_port_afdx_sampling.hh"
#include "vistas_port_afdx_queuing.hh"
#include "vistas_port_a429.hh"
#include "vistas_port_can.hh"
#include "vistas_port_nad.hh"
#include "vistas_port_discrete.hh"
#include "vistas_port_analogue.hh"
#include "vistas_port_instrumentation.hh"
#include "vistas_socket_multicast_input.hh"
#include "vistas_socket_multicast_output.hh"
#include "vistas_socket_unicast_input.hh"
#include "vistas_socket_unicast_output.hh"
#include "ims_context.hh"

namespace vistas
{

#define NODE_PROTO_AFDX_NAME     "A664_Channel"
#define NODE_PROTO_A429_NAME     "A429_Channel"
#define NODE_PROTO_CAN_NAME      "A825_Channel"
#define NODE_PROTO_DISCRETE_NAME "Discrete_Channel"
#define NODE_PROTO_ANALOGUE_NAME "Analog_Channel"
#define NODE_PROTO_NAD_NAME      "NAD_Channel"

//
// Store information on a port
//
struct port_info_t
{
    std::string        channel_name;
    socket_address_ptr addr;
    uint32_t           fifo_size;
    uint32_t           message_max_size;
    uint16_t           prod_id;
    bool               seq_num_enabled;
    bool               qos_timestamp_enabled;
    bool               data_timestamp_enabled;
};
typedef shared_ptr<port_info_t>  port_info_ptr;

//
// Safe port cast (with IMS error)
//
template <typename port_typename>
inline shared_ptr<port_typename> dynamic_port_cast(port_ptr port, port_info_ptr port_info)
{
    shared_ptr<port_typename> typed_port = dynamic_pointer_cast<port_typename>(port);
    if (typed_port == NULL) {
        THROW_IMS_ERROR(ims_init_failure, "Different kind of messages on same " << port_info->addr->to_string() << " !");
    }

    return typed_port;
}

//
// Port factory
//
template <typename port_ptr_typename>
struct port_factory
{
    inline port_ptr_typename create_port(context_ptr context, socket_ptr socket, port_info_ptr port_info);
};

//
// Specialization of port factory for AFDX sampling
//
template<>
struct port_factory<port_afdx_sampling_ptr>
{
    typedef port_afdx_sampling     port_t;
    typedef port_afdx_sampling_ptr port_ptr_t;

    inline port_afdx_sampling_ptr create_port(context_ptr context,
                                              socket_ptr socket,
                                              __attribute__((__unused__)) port_info_ptr port_info)
    {
        return port_afdx_sampling_ptr(new vistas::port_afdx_sampling(context.get(), socket));
    }
};

//
// Specialization of port factory for AFDX queuing
//
template<>
struct port_factory<port_afdx_queuing_ptr>
{
    typedef port_afdx_queuing     port_t;
    typedef port_afdx_queuing_ptr port_ptr_t;

    inline port_factory(uint32_t max_size, uint32_t expected_max_size, uint32_t queue_depth) :
        max_size(max_size),
        expected_max_size(expected_max_size),
        queue_depth(queue_depth)
    {
    }

    inline port_afdx_queuing_ptr create_port(context_ptr context,
                                             socket_ptr socket,
                                             __attribute__((__unused__)) port_info_ptr port_info)
    {
        return port_afdx_queuing_ptr(new vistas::port_afdx_queuing(context.get(), socket, max_size, expected_max_size, queue_depth));
    }

    uint32_t max_size;
    uint32_t expected_max_size;
    uint32_t queue_depth;
};

//
// Specialization of port factory for A429
//
template<>
struct port_factory<port_a429_ptr>
{
    typedef port_a429     port_t;
    typedef port_a429_ptr port_ptr_t;

    inline port_factory(std::string bus_name) :
        bus_name(bus_name)
    {
    }

    inline port_a429_ptr create_port(context_ptr context, socket_ptr socket, port_info_ptr port_info)
    {
        return port_a429_ptr(new vistas::port_a429(context.get(), socket, bus_name, port_info->message_max_size));
    }

    std::string bus_name;
};

//
// Specialization of port factory for CAN
//
template<>
struct port_factory<port_can_ptr>
{
    typedef port_can     port_t;
    typedef port_can_ptr port_ptr_t;

    inline port_factory(std::string bus_name) :
        bus_name(bus_name)
    {
    }

    inline port_can_ptr create_port(context_ptr context, socket_ptr socket, port_info_ptr port_info)
    {
        return port_can_ptr(new vistas::port_can(context.get(), socket, bus_name, port_info->message_max_size));
    }

    std::string bus_name;
};

//
// Specialization of port factory for Discrete
//
template<>
struct port_factory<port_discrete_ptr>
{
    typedef port_discrete     port_t;
    typedef port_discrete_ptr port_ptr_t;

    inline port_factory(uint32_t port_period_us) :
        port_period_us(port_period_us)
    {
    }

    inline port_discrete_ptr create_port(context_ptr context, socket_ptr socket, port_info_ptr port_info)
    {
        return port_discrete_ptr(new vistas::port_discrete(context.get(), socket, port_info->message_max_size, port_period_us));
    }

    uint32_t port_period_us;
};

//
// Specialization of port factory for Analogue
//
template<>
struct port_factory<port_analogue_ptr>
{
    typedef port_analogue     port_t;
    typedef port_analogue_ptr port_ptr_t;

    inline port_factory(uint32_t port_period_us) :
        port_period_us(port_period_us)
    {
    }

    inline port_analogue_ptr create_port(context_ptr context, socket_ptr socket, port_info_ptr port_info)
    {
        return port_analogue_ptr(new vistas::port_analogue(context.get(), socket, port_info->message_max_size, port_period_us));
    }

    uint32_t port_period_us;
};

//
// Specialization of port factory for Nad
//
template<>
struct port_factory<port_nad_ptr>
{
    typedef port_nad     port_t;
    typedef port_nad_ptr port_ptr_t;

    inline port_factory(uint32_t port_period_us) :
        port_period_us(port_period_us)
    {
    }

    inline port_nad_ptr create_port(context_ptr context, socket_ptr socket, port_info_ptr port_info)
    {
        return port_nad_ptr(new vistas::port_nad(context.get(), socket, port_info->message_max_size, port_period_us));
    }

    uint32_t port_period_us;
};

//
// Internal xml parser
//
class context::factory::parser : public xml_parser
{
public:
    parser(context_ptr context) : _context(context)
    {

    }
    
    void load(const char* config_file_path)
    {
        load_and_validate(config_file_path, vistas_config_xsd);
        _root_node = xmlDocGetRootElement(_doc);
    }

    xmlNodeSetPtr xpath_query(std::string query) {
        return xml_xpath_get_children(_doc,
                                      _root_node,
                                      query.c_str());
    }

    // Return the instrumentation address or NULL
    socket_address_ptr get_power_supply_address();
    socket_address_ptr get_signal_overriding_address();
    socket_address_ptr get_synchronization_address();
    socket_address_ptr get_modes_address();

    // Generate the XPATHof the given ims node
    std::string build_node_xpath(ims::weak_node_ptr ims_node);

    // Look for the given port
    xmlNodePtr find_port(ims::node_ptr        ims_parent_node,
                         std::string          protocol,
                         std::string          port_name,
                         ims_direction_t      direction);


    // Look for the given signal
    xmlNodePtr find_group_element(ims::node_ptr        ims_parent_node,
                                  std::string          protocol,
                                  std::string          port_name,
                                  ims_direction_t      direction);


    // Parse port informations
    port_info_ptr parse_port(xmlNodePtr port_node,
                             ims_direction_t direction,
                             ims::node_ptr   ims_parent_node);

    xmlNodePtr _root_node;
    context_ptr _context;
};

//
// Return the instrumentation address or NULL
//
socket_address_ptr context::factory::parser::get_power_supply_address()
{
    std::string instrumentation_ip = "";
    uint32_t instrumentation_port = 0;
    
    xmlNodeSetPtr node_set = xpath_query("/Network/ControlServers/PowerSupply");
    if (node_set != NULL)
    {
        xmlNodePtr node = node_set->nodeTab[0];

        instrumentation_ip = xml_node_property(node, "IPAddress", true);
        instrumentation_port = xml_node_property_uint(node, "Port", 0);

        xmlXPathFreeNodeSet(node_set);
    }

    if (instrumentation_ip.empty() == false &&
            instrumentation_port != 0) {
        LOG_INFO("Enable power supply on " << instrumentation_ip << ":" << instrumentation_port);
        return socket_address_ptr(new socket_address_t(ims_input, instrumentation_ip.c_str(), instrumentation_port));
    } else {
        LOG_INFO("Power supply disabled.");
        return socket_address_ptr((socket_address_t*)NULL);
    }
}

socket_address_ptr context::factory::parser::get_signal_overriding_address()
{
    std::string instrumentation_ip = "";
    uint32_t instrumentation_port = 0;
    
    xmlNodeSetPtr node_set = xpath_query("/Network/ControlServers/SignalOverriding");
    if (node_set != NULL)
    {
        xmlNodePtr node = node_set->nodeTab[0];

        instrumentation_ip = xml_node_property(node, "IPAddress", true);
        instrumentation_port = xml_node_property_uint(node, "Port", 0);

        xmlXPathFreeNodeSet(node_set);
    }

    if (instrumentation_ip.empty() == false &&
            instrumentation_port != 0) {
        LOG_INFO("Enable signal overriding on " << instrumentation_ip << ":" << instrumentation_port);
        return socket_address_ptr(new socket_address_t(ims_input, instrumentation_ip.c_str(), instrumentation_port));
    } else {
        LOG_INFO("Signal overriding disabled.");
        return socket_address_ptr((socket_address_t*)NULL);
    }
}

socket_address_ptr context::factory::parser::get_synchronization_address()
{
    std::string instrumentation_ip = "";
    uint32_t instrumentation_port = 0;
    
    xmlNodeSetPtr node_set = xpath_query("/Network/ControlServers/Synchronization");
    if (node_set != NULL)
    {
        xmlNodePtr node = node_set->nodeTab[0];

        instrumentation_ip = xml_node_property(node, "IPAddress", true);
        instrumentation_port = xml_node_property_uint(node, "Port", 0);

        xmlXPathFreeNodeSet(node_set);
    }

    if (instrumentation_ip.empty() == false &&
            instrumentation_port != 0) {
        LOG_INFO("Enable synchronization on " << instrumentation_ip << ":" << instrumentation_port);
        return socket_address_ptr(new socket_address_t(ims_input, instrumentation_ip.c_str(), instrumentation_port));
    } else {
        LOG_INFO("Synchronization disabled.");
        return socket_address_ptr((socket_address_t*)NULL);
    }
}

socket_address_ptr context::factory::parser::get_modes_address()
{
    std::string instrumentation_ip = "";
    uint32_t instrumentation_port = 0;
    
    xmlNodeSetPtr node_set = xpath_query("/Network/ControlServers/Modes");
    if (node_set != NULL)
    {
        xmlNodePtr node = node_set->nodeTab[0];

        instrumentation_ip = xml_node_property(node, "IPAddress", true);
        instrumentation_port = xml_node_property_uint(node, "Port", 0);

        xmlXPathFreeNodeSet(node_set);
    }

    if (instrumentation_ip.empty() == false &&
            instrumentation_port != 0) {
        LOG_INFO("Enable modes on " << instrumentation_ip << ":" << instrumentation_port);
        return socket_address_ptr(new socket_address_t(ims_input, instrumentation_ip.c_str(), instrumentation_port));
    } else {
        LOG_INFO("Modes disabled.");
        return socket_address_ptr((socket_address_t*)NULL);
    }
}

//
// Generate the XPATH of the given ims node
//
std::string context::factory::parser::build_node_xpath(ims::weak_node_ptr ims_node)
{
    if (ims_node == NULL) return "";
    
    return "/Network/VirtualComponent[@Name=\"" + _context->get_vc_name() + "\"]";
}

//
// Lookup for the given port
//
xmlNodePtr context::factory::parser::find_port(ims::node_ptr        ims_parent_node,
                                               std::string          protocol_tag_name,
                                               std::string          port_name,
                                               ims_direction_t      direction)
{
    std::string query = build_node_xpath(ims_parent_node.get()) + "/" + protocol_tag_name +
            "[@Name=\"" + port_name + "\"][@Direction=\""  +  ((direction == ims_input)? "In" : "Out") +"\"]" ;

    xmlNodeSetPtr node_set = xpath_query(query);
    xmlNodePtr node = NULL;

    try {
        if (node_set == NULL) {
            LOG_ERROR("Cannot find " << protocol_tag_name << " port '" << port_name << "'!");
            THROW_XML_ERROR(this, "Query was: " << query);
        }

        if (node_set->nodeNr != 1) {
            LOG_ERROR("Severals " << protocol_tag_name << " ports with same name '" << port_name << "'!");
            THROW_XML_ERROR(this, "Query was: " << query);
        }

        node = node_set->nodeTab[0];
        xmlXPathFreeNodeSet(node_set);

    } catch(...) {
        xmlXPathFreeNodeSet(node_set);
        throw;
    }

    return node;
}

//
// Look for the given element
//
xmlNodePtr context::factory::parser::find_group_element(ims::node_ptr        ims_parent_node,
                                                        std::string          protocol_tag_name,
                                                        std::string          element_name,
                                                        ims_direction_t      direction)
{
    std::string query = build_node_xpath(ims_parent_node.get()) + "/" + protocol_tag_name
            + "[@Direction=\""  +  ((direction == ims_input)? "In" : "Out") +"\"]/Signals/Signal[@Name=\"" + element_name + "\"]" ;

    xmlNodeSetPtr element_node_set = xpath_query(query);
    xmlNodePtr element_node = NULL;

    try {
        if (element_node_set == NULL) {
            LOG_ERROR("Cannot find " << protocol_tag_name << " element '" << element_name << "'!");
            THROW_XML_ERROR(this, "Query was: " << query);
        }

        if (element_node_set->nodeNr != 1) {
            LOG_ERROR("Severals " << protocol_tag_name << " element with same name '" << element_name << "'!");
            THROW_XML_ERROR(this, "Query was: " << query);
        }

        element_node = element_node_set->nodeTab[0];
        xmlXPathFreeNodeSet(element_node_set);

    } catch(...) {
        xmlXPathFreeNodeSet(element_node_set);
        throw;
    }

    return element_node;
}

//
// Parse given port node
//
port_info_ptr context::factory::parser::parse_port(xmlNodePtr      port_node,
                                                   ims_direction_t direction,
                                                   __attribute__((__unused__)) ims::node_ptr   ims_parent_node)
{
    port_info_ptr info(new port_info_t);

    info->fifo_size = xml_node_property_uint(port_node, "FifoSize");
    info->message_max_size = xml_node_property_uint(port_node, "MessageMaxSize");
    info->channel_name = xml_node_property(port_node, "Name");
    
    info->prod_id = 0;
    info->seq_num_enabled = false;
    info->qos_timestamp_enabled = false;
    info->data_timestamp_enabled = false;
    xmlNodeSetPtr header_node_set = xml_xpath_get_children(_doc, port_node, "Header");
    if (header_node_set != NULL)
    {
        xmlNodePtr header_node = header_node_set->nodeTab[0];

        std::string src_id = xml_node_property(header_node, "Src_Id");
        std::string sn = xml_node_property(header_node, "SN");
        std::string qos_timestamp = xml_node_property(header_node, "QoS_Timestamp");
        std::string data_timestamp = xml_node_property(header_node, "Data_Timestamp");

        if (src_id == "Yes")
        {
            info->prod_id = _context->get_prod_id();
        }
        if (sn == "Yes")
        {
            info->seq_num_enabled = true;
        }
        if (qos_timestamp == "Yes")
        {
            info->qos_timestamp_enabled = true;
        }
        if (data_timestamp == "Yes")
        {
            info->data_timestamp_enabled = true;
        }


        xmlXPathFreeNodeSet(header_node_set);
    }
    
    xmlNodeSetPtr socket_node_set = xml_xpath_get_children(_doc, port_node, "Socket");
    if (socket_node_set == NULL)
    {
        LOG_ERROR("No socket found in channel");
        THROW_XML_ERROR(this, "No socket found in channel");
    }
    
    // TODO handle multi sockets ?
    if (socket_node_set->nodeNr != 1)
    {
        xmlXPathFreeNodeSet(socket_node_set);
        LOG_ERROR("Only one socket by channel is supported");
        THROW_XML_ERROR(this, "Only one socket by channel is supported");
    }
    
    for (int32_t node_id = 0; node_id < socket_node_set->nodeNr; node_id++)
    {
        xmlNodePtr socket_node = socket_node_set->nodeTab[node_id];

        std::string address_ip = xml_node_property(socket_node, "DstIP");
        uint32_t address_port = xml_node_property_uint(socket_node, "DstPort");
        std::string interface_ip = xml_node_property(socket_node, "SrcIP", true);
        uint32_t output_port = xml_node_property_uint(socket_node, "SrcPort", 0);
        uint32_t output_TTL = xml_node_property_uint(socket_node, "TTL", 1);

        info->addr = socket_address_ptr(new socket_address_t(direction,
                                                             address_ip,
                                                             address_port,
                                                             interface_ip,
                                                             output_TTL,
                                                             output_port));

    }
    
    xmlXPathFreeNodeSet(socket_node_set);
    
    return info;
}

// If port described by port_info doesn't exists, create it using factory.
// Else, just check its kind.
template<typename port_factory, typename port_info_ptr>
typename port_factory::port_ptr_t context::factory::get_or_create_port(port_factory  factory,
                                                                       port_info_ptr port_info)
{
    port_ptr port = _socket_pool_factory.find(port_info->addr);
    if (port == NULL) {
        // Port doesn't exists, create a new one and its socket
        socket_ptr socket;
        bool is_multicast = port_info->addr->is_multicast();
        if (port_info->addr->get_direction() == ims_input)
        {
            if (is_multicast)
            {
                socket = socket_ptr(new socket_multicast_input(port_info->addr));
            }
            else
            {
                socket = socket_ptr(new socket_unicast_input(port_info->addr));
            }
        }
        else
        {
            if (is_multicast)
            {
                socket = socket_ptr(new socket_multicast_output(port_info->addr));
            }
            else
            {
                socket = socket_ptr(new socket_unicast_output(port_info->addr));
            }
        }

        typename port_factory::port_ptr_t typed_port = factory.create_port(_context, socket, port_info);
        typed_port->init_header_flags(port_info->prod_id, port_info->seq_num_enabled, port_info->qos_timestamp_enabled, port_info->data_timestamp_enabled);
        context_register_port(typed_port);
        _socket_pool_factory.add(socket, typed_port, port_info->channel_name);
        return typed_port;
    }
    else
    {
        // Port already exists
        if (port_info->addr->get_interface_ip() != port->get_socket()->get_address()->get_interface_ip()) {
            THROW_IMS_ERROR(ims_invalid_configuration, "Same address (" <<
                            port_info->addr->to_string() << ") with different interface!");
        }

        if (port_info->addr->get_TTL() != port->get_socket()->get_address()->get_TTL()) {
            THROW_IMS_ERROR(ims_invalid_configuration, "Same address (" <<
                            port_info->addr->to_string() << ") with different output TTL!");
        }

        if (port_info->addr->get_outgoing_port() != port->get_socket()->get_address()->get_outgoing_port()) {
            THROW_IMS_ERROR(ims_invalid_configuration, "Same address (" <<
                            port_info->addr->to_string() << ") with different outgoing port!");
        }

        port_application_base * weak_port_application =  dynamic_cast<port_application_base *>(port.get());
        if (weak_port_application != NULL)
        {
            if (port_info->prod_id != weak_port_application->get_prod_id()) {
                THROW_IMS_ERROR(ims_invalid_configuration, "Same address (" <<
                                port_info->addr->to_string() << ") with different prod ID!");
            }

            if (port_info->seq_num_enabled != weak_port_application->is_seq_num_enabled()) {
                THROW_IMS_ERROR(ims_invalid_configuration, "Same address (" <<
                                port_info->addr->to_string() << ") with different SN!");
            }

            if (port_info->qos_timestamp_enabled != weak_port_application->is_qos_timestamp_enabled()) {
                THROW_IMS_ERROR(ims_invalid_configuration, "Same address (" <<
                                port_info->addr->to_string() << ") with different QoS_Timestamp!");
            }

            if (port_info->data_timestamp_enabled != weak_port_application->is_data_timestamp_enabled()) {
                THROW_IMS_ERROR(ims_invalid_configuration, "Same address (" <<
                                port_info->addr->to_string() << ") with different Data_Timestamp!");
            }
        }

        return dynamic_port_cast<typename port_factory::port_t>(port, port_info);
    }
}

//
// Register a port into the context
//
void context::factory::context_register_port(port_application_ptr port)
{
    _context->_port_list.push_back(port);
    if (port->is_periodic_output()) {
        _context->_periodic_output_ports.push_back(port);
    }
}

//
// Init the context with some common info
//
void context::factory::init(std::string vc_name,
                            uint32_t prod_id,
                            uint32_t period_us,
                            bool step_by_step_enabled
                            )
{
    _context->set_vc_name(vc_name);
    _context->set_prod_id(prod_id);
    _context->set_period_us(period_us);
    _context->set_step_by_step_enabled(step_by_step_enabled);
}

//
// Create an AFDX sampling
//
ims::message_ptr context::factory::create_afdx_sampling(ims::node_ptr        ims_parent_node,
                                                        std::string          message_name,
                                                        ims_direction_t      direction,
                                                        uint32_t             size,
                                                        uint32_t             validity_duration_us,
                                                        std::string          local_name,
                                                        uint32_t             period_us)
{
    xmlNodePtr port_node = _parser->find_port(ims_parent_node, NODE_PROTO_AFDX_NAME, message_name, direction);
    port_info_ptr port_info = _parser->parse_port(port_node, direction, ims_parent_node);
    
    uint32_t buffer_size = std::min(port_info->message_max_size, size);

    if (port_info->message_max_size != size) {
        LOG_WARN( "Incompatible size between IMS config and VISTAS config on port '" <<
                  message_name << "' : " << size << " in IMS config and " <<
                  port_info->message_max_size << " in VISTAS config. IMS will use the smallest value : " << buffer_size);
    }

    port_afdx_sampling_ptr port = get_or_create_port(port_factory<port_afdx_sampling_ptr>(), port_info);

    return port->get_message(message_name, buffer_size, size, validity_duration_us, local_name, period_us);
}

//
// Create an AFDX queuing
//
ims::message_ptr context::factory::create_afdx_queuing(ims::node_ptr        ims_parent_node,
                                                       std::string          message_name,
                                                       ims_direction_t      direction,
                                                       uint32_t             max_size,
                                                       uint32_t             queue_depth,
                                                       std::string          local_name)
{
    xmlNodePtr port_node = _parser->find_port(ims_parent_node, NODE_PROTO_AFDX_NAME, message_name, direction);
    port_info_ptr port_info = _parser->parse_port(port_node, direction, ims_parent_node);

    if (port_info->fifo_size != queue_depth) {
        THROW_IMS_ERROR(ims_init_failure,
                        "Incompatible queue depth between IMS config and VISTAS config on port '" <<
                        message_name << "' : " << queue_depth << " in IMS config and " <<
                        port_info->fifo_size << " in VISTAS config.");
    }
    
    uint32_t buffer_size = std::min(port_info->message_max_size, max_size);
    
    if (port_info->message_max_size != max_size) {
        LOG_WARN( "Incompatible message max size between IMS config and VISTAS config on port '" <<
                  message_name << "' : " << max_size << " in IMS config and " <<
                  port_info->message_max_size << " in VISTAS config. IMS will use the smallest value : " << buffer_size);
    }

    port_afdx_queuing_ptr port = get_or_create_port(port_factory<port_afdx_queuing_ptr>(buffer_size, max_size, queue_depth), port_info);

    return port->get_message(message_name, local_name);

    return ims::message_ptr((ims::message*)NULL);
}

//
// Create an A429 sampling
//
ims::message_ptr context::factory::create_a429_sampling(ims::node_ptr        ims_parent_node,
                                                        std::string          bus_name,
                                                        std::string          label_number,
                                                        std::string          label_sdi,
                                                        ims_direction_t      direction,
                                                        uint32_t             validity_duration_us,
                                                        std::string          local_name,
                                                        uint32_t             period_us)
{
    xmlNodePtr port_node = _parser->find_port(ims_parent_node, NODE_PROTO_A429_NAME, bus_name, direction);
    port_info_ptr port_info = _parser->parse_port(port_node, direction, ims_parent_node);

    port_a429_ptr port = get_or_create_port(port_factory<port_a429_ptr>(bus_name), port_info);

    return port->get_label<ims_sampling>(label_number, label_sdi, validity_duration_us, 1, local_name, period_us);
}

//
// Create an CAN sampling
//
ims::message_ptr context::factory::create_can_sampling(ims::node_ptr        ims_parent_node,
                                                       std::string          bus_name,
                                                       uint32_t             message_id,
                                                       uint32_t             message_size,
                                                       ims_direction_t      direction,
                                                       uint32_t             validity_duration_us,
                                                       std::string          local_name,
                                                       uint32_t             period_us)
{
    xmlNodePtr port_node = _parser->find_port(ims_parent_node, NODE_PROTO_CAN_NAME, bus_name, direction);
    port_info_ptr port_info = _parser->parse_port(port_node, direction, ims_parent_node);

    port_can_ptr port = get_or_create_port(port_factory<port_can_ptr>(bus_name), port_info);

    return port->get_message(message_id, message_size, validity_duration_us, local_name, period_us);
}

//
// Create an A429 queuing
//
ims::message_ptr context::factory::create_a429_queuing(ims::node_ptr        ims_parent_node,
                                                       std::string          bus_name,
                                                       std::string          label_number,
                                                       std::string          label_sdi,
                                                       ims_direction_t      direction,
                                                       uint32_t             queue_depth,
                                                       std::string          local_name)
{
    xmlNodePtr port_node = _parser->find_port(ims_parent_node, NODE_PROTO_A429_NAME, bus_name, direction);
    port_info_ptr port_info = _parser->parse_port(port_node, direction, ims_parent_node);

    port_a429_ptr port = get_or_create_port(port_factory<port_a429_ptr>(bus_name), port_info);

    return port->get_label<ims_queuing>(label_number, label_sdi, 0, queue_depth, local_name, 0);
}

//
// Create discrete signal
//
ims::message_ptr context::factory::create_discrete_signal(ims::node_ptr         ims_parent_node,
                                                          std::string           signal_name,
                                                          ims_direction_t       direction,
                                                          uint32_t              validity_duration_us,
                                                          uint32_t              true_state,
                                                          uint32_t              false_state,
                                                          uint32_t              port_period_us,
                                                          std::string           local_name)
{
    xmlNodePtr signal_node = _parser->find_group_element(ims_parent_node, NODE_PROTO_DISCRETE_NAME, signal_name, direction);
    
    port_info_ptr port_info = _parser->parse_port(signal_node->parent->parent, direction, ims_parent_node);
    uint32_t signal_offset =  _parser->xml_node_property_uint(signal_node, "ByteOffset");
    std::string bus_name =  _parser->xml_node_property(signal_node->parent->parent, "Name");
    
    port_discrete_ptr port = get_or_create_port(port_factory<port_discrete_ptr>(port_period_us), port_info);
    
    return port->get_discrete_signal(signal_name, signal_offset, validity_duration_us, true_state, false_state, local_name, bus_name, port_period_us);
}

//
// Create a analogue signal
//
ims::message_ptr context::factory::create_analogue_signal(ims::node_ptr         ims_parent_node,
                                                          std::string           signal_name,
                                                          ims_direction_t       direction,
                                                          uint32_t              validity_duration_us,
                                                          float                 scale_factor,
                                                          float                 gain,
                                                          uint32_t              port_period_us,
                                                          std::string           local_name)
{
    xmlNodePtr signal_node = _parser->find_group_element(ims_parent_node, NODE_PROTO_ANALOGUE_NAME, signal_name, direction);

    port_info_ptr port_info = _parser->parse_port(signal_node->parent->parent, direction, ims_parent_node);
    uint32_t signal_offset =  _parser->xml_node_property_uint(signal_node, "ByteOffset");
    std::string bus_name =  _parser->xml_node_property(signal_node->parent->parent, "Name");

    port_analogue_ptr port = get_or_create_port(port_factory<port_analogue_ptr>(port_period_us), port_info);

    return port->get_analogue_signal(signal_name, signal_offset, validity_duration_us, scale_factor, gain, local_name, bus_name, port_period_us);
}

//
// Create a NAD message
//
ims::message_ptr context::factory::create_nad_message(ims::node_ptr         ims_parent_node,
                                                      std::string           message_name,
                                                      ims_direction_t       direction,
                                                      uint32_t              size,
                                                      uint32_t              port_period_us,
                                                      std::string           local_name,
                                                      ims_nad_type_t        nad_type,
                                                      uint32_t              nad_dim1,
                                                      uint32_t              nad_dim2)
{
    xmlNodePtr message_node = _parser->find_group_element(ims_parent_node, NODE_PROTO_NAD_NAME, message_name, direction);

    port_info_ptr port_info = _parser->parse_port(message_node->parent->parent, direction, ims_parent_node);
    uint32_t message_size =  _parser->xml_node_property_uint(message_node, "Size");
    uint32_t message_offset =  _parser->xml_node_property_uint(message_node, "ByteOffset");
    std::string bus_name =  _parser->xml_node_property(message_node->parent->parent, "Name");

    if (message_size != size) {
        THROW_IMS_ERROR(ims_init_failure,
                        "Incompatible size between IMS config and VISTAS config on NAD '" <<
                        message_name << "' : " << size << " in IMS config and " <<
                        message_size << " in VISTAS config.");
    }


    port_nad_ptr port = get_or_create_port(port_factory<port_nad_ptr>(port_period_us), port_info);

    return port->get_nad_message(message_name, message_size, message_offset, local_name, bus_name, port_period_us, nad_type, nad_dim1, nad_dim2);
}

//
// Finalize
//
backend::context_ptr context::factory::build()
{
    socket_address_ptr power_supply_address = _parser->get_power_supply_address();
    socket_address_ptr signal_overriding_address = _parser->get_signal_overriding_address();
    socket_address_ptr synchronization_address = _parser->get_synchronization_address();
    socket_address_ptr modes_address = _parser->get_modes_address();
    
    if (power_supply_address)
    {
        port_ptr port = _socket_pool_factory.find(power_supply_address);
        if (port == NULL)
        {
            LOG_INFO("Connecting to power supply controller");
            socket_tcp_message_ptr socket = socket_tcp_message_ptr(new socket_tcp_message(power_supply_address));
            port = port_ptr(new port_instrumentation(_context.get(), socket));
            _socket_pool_factory.add(socket, port, "");
        }
        LOG_INFO("Registering to power supply controller");
        port_instrumentation * port_instru = dynamic_cast<port_instrumentation *>(port.get());
        port_instru->register_power();
        LOG_INFO("Registered to power supply controller");
    }

    if (signal_overriding_address)
    {
        port_ptr port = _socket_pool_factory.find(signal_overriding_address);
        if (port == NULL)
        {
            LOG_INFO("Connecting to signal overriding controller");
            socket_tcp_message_ptr socket = socket_tcp_message_ptr(new socket_tcp_message(signal_overriding_address));
            port = port_ptr(new port_instrumentation(_context.get(), socket));
            _socket_pool_factory.add(socket, port, "");
        }
        LOG_INFO("Registering to signal overriding controller");
        port_instrumentation * port_instru = dynamic_cast<port_instrumentation *>(port.get());
        port_instru->register_instrum();
        LOG_INFO("Registered to signal overriding controller");
    }

    if (synchronization_address)
    {
        port_ptr port = _socket_pool_factory.find(synchronization_address);
        if (port == NULL)
        {
            LOG_INFO("Connecting to synchronization controller");
            socket_tcp_message_ptr socket = socket_tcp_message_ptr(new socket_tcp_message(synchronization_address));
            port = port_ptr(new port_instrumentation(_context.get(), socket));
            _socket_pool_factory.add(socket, port, "");
        }
        LOG_INFO("Registering to synchronization controller");
        port_instrumentation * port_instru = dynamic_cast<port_instrumentation *>(port.get());
        port_instru->register_sync();
        LOG_INFO("Registered to synchronization controller");
    }

    if (modes_address)
    {
        port_ptr port = _socket_pool_factory.find(modes_address);
        if (port == NULL)
        {
            LOG_INFO("Connecting to modes controller");
            socket_tcp_message_ptr socket = socket_tcp_message_ptr(new socket_tcp_message(modes_address));
            port = port_ptr(new port_instrumentation(_context.get(), socket));
            _socket_pool_factory.add(socket, port, "");
        }
        LOG_INFO("Registering to modes controller");
        port_instrumentation * port_instru = dynamic_cast<port_instrumentation *>(port.get());
        port_instru->register_mode();
        LOG_INFO("Registered to modes controller");
    }

    _context->_socket_pool = _socket_pool_factory.create_pool();
    return _context;
}

//
// Main Ctor
//
context::factory::factory(const char* vistas_config_file_path) throw(ims::exception) :
    _context(new context())
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    
    _parser = new parser(_context);
    try {
        _parser->load(vistas_config_file_path);
    } catch (...) {
        delete _parser;
        throw;
    }
}

context::factory::~factory()
{
    LOG_DEBUG("Delete vistas::factory");
    delete _parser;
}

}

// 
// backend implementation
// 
namespace backend
{
context::factory_ptr create_factory(const char* backend_config_file_path)
{
    return context::factory_ptr(new vistas::context::factory(backend_config_file_path));
}

const char* get_name()
{
    return "Vistas";
}
}
