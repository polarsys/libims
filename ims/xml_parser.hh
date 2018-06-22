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
// base parser for XML file
//
#ifndef _IMS_XML_PARSER_HH_
#define _IMS_XML_PARSER_HH_

#include "ims_log.hh"
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>
#include <libxml/xpath.h>

//
// Throw helpers
//

// If an libxml error has occured, log it.
// Then log the provided message.
// Then throw an ims::exception with ims_init_failure.
#define THROW_XML_ERROR(xml_parser_ptr, msg)   \
    do {                                         \
    xml_parser_ptr->log_xml_error();           \
    THROW_IMS_ERROR(ims_init_failure, msg);    \
    } while (0)

// Log an xmlNode error.
// if node is valid, use file:line from it.
// Else use file:line where the macro is expensed.
// Then throw an ims::exception with ims_init_failure.
#define THROW_NODE_ERROR(node, msg)                                                      \
    do {                                                                                   \
    if (node && node->doc && node->doc->URL) {                                           \
    LOG_PROCESS(ims::log::error, node->doc->URL << ':' << node->line << " " << msg);   \
    } else {                                                                             \
    LOG_ERROR(msg);                                                                    \
    }                                                                                    \
    throw ims::exception(ims_init_failure);                                              \
    } while (0)

//
// Parser itself
//

class xml_parser
{
public:

    xml_parser();
    virtual ~xml_parser();

    // Load an xml file and validate it
    // child classes will have access to _doc member
    // @param xml_file_path path to the xml file to load
    // @param schema_in_memory schemas as string
    void load_and_validate(const char* xml_file_path,
                           const char* schema_in_memory)
    throw(ims::exception);

    // If an libxml error has occured, log it.
    // Else, does nothing.
    void log_xml_error();

    // Return a property of a node as string
    // may throw if optional is false and property not found
    std::string xml_node_property(xmlNodePtr node,
                                  std::string name,
                                  bool optional = false)
    throw(ims::exception);

    // Return a property of a node as signed integer
    // may throw if property not found or not an integer
    int32_t xml_node_property_int(xmlNodePtr node, std::string name)
    throw(ims::exception);

    // Return a property of a node as signed integer
    // may throw if property not an integer
    int32_t xml_node_property_int(xmlNodePtr node, std::string name, int32_t default_value)
    throw(ims::exception);

    // Return a property of a node as unsigned integer
    // may throw if property not found or not an integer
    uint32_t xml_node_property_uint(xmlNodePtr node, std::string name)
    throw(ims::exception);

    // Return a property of a node as unsigned integer
    // may throw if property not an integer
    uint32_t xml_node_property_uint(xmlNodePtr node, std::string name, uint32_t default_value)
    throw(ims::exception);

    // Return a property of a node as float
    // may throw if property not found or not a floating point number
    float xml_node_property_float(xmlNodePtr node, std::string name)
    throw(ims::exception);

    // Return a property of a node as float
    // may throw if property not a floating point number
    float xml_node_property_float(xmlNodePtr node, std::string name, float default_value)
    throw(ims::exception);

    // Return a hex property of a node as unsigned integer
    // may throw if property not found or not an hexa integer
    uint32_t xml_node_property_hex(xmlNodePtr node, std::string name)
    throw(ims::exception);

    // Return the first child node (ignore text nodes)
    xmlNodePtr xml_node_first_child(xmlNodePtr node);

    // Return the next node (ignore text nodes)
    xmlNodePtr xml_node_next_sibling(xmlNodePtr node);


    // return all children which match a xpath query
    xmlNodeSetPtr xml_xpath_get_children(const xmlDocPtr   doc,
                                         const xmlNodePtr  node,
                                         const char* const xpath)
    throw(ims::exception);

    // Return the text content of a node element
    std::string xml_node_get_content(const xmlNodePtr node)
    throw(ims::exception);

    // Internal document
    xmlDocPtr _doc;

private:

    static uint32_t        _parsers_count;
    xmlParserCtxtPtr       _parser_ctxt;
    xmlDocPtr              _schema_doc;
    xmlSchemaParserCtxtPtr _schema_parser_ctxt;
    xmlSchemaPtr           _schema;
    xmlSchemaValidCtxtPtr  _validation_ctxt;
};

#endif
