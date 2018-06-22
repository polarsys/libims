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
#include "xml_parser.hh"
#include <sstream>
#include <string.h>

// LIBXML2 error callback
void libxml_structured_error_callback(void* userdata,
                                      xmlErrorPtr error);

// Static member
uint32_t xml_parser::_parsers_count = 0;

//
// Load an xml file and validate it
//
void xml_parser::load_and_validate(const char* xml_file_path, 
                                   const char* schema_in_memory)
throw(ims::exception)
{
    _parsers_count++;

    /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
    LIBXML_TEST_VERSION;

    // Create context and initialize error handling
    _parser_ctxt = xmlNewParserCtxt();
    if (_parser_ctxt == NULL) {
        THROW_IMS_ERROR(ims_init_failure, "Failed to allocate a parser context!");
    }
    xmlSetStructuredErrorFunc((void*)this, &libxml_structured_error_callback);

    // Parse docmument
    _doc = xmlCtxtReadFile(_parser_ctxt, xml_file_path, NULL, 0);
    if (_doc == NULL) {
        THROW_XML_ERROR(this, "Cannot parse config file '" << xml_file_path << "' !");
    }

    // Parse internal XSD
    _schema_doc = xmlCtxtReadMemory(_parser_ctxt,
                                    schema_in_memory,
                                    strlen(schema_in_memory),
                                    NULL, NULL, 0);
    if (_schema_doc == NULL) {
        THROW_XML_ERROR(this, "Cannot parse internal schema document!");
    }

    // Validate document
    _schema_parser_ctxt = xmlSchemaNewDocParserCtxt(_schema_doc);
    if (_schema_parser_ctxt == NULL) {
        THROW_XML_ERROR(this, "Cannot create context for schema.");
    }

    _schema = xmlSchemaParse(_schema_parser_ctxt);
    if (_schema == NULL) {
        THROW_XML_ERROR(this, "Invalid internal schema!");
    }

    _validation_ctxt = xmlSchemaNewValidCtxt(_schema);
    if (_validation_ctxt == NULL) {
        THROW_XML_ERROR(this, "Cannot create validation context!");
    }

    if (xmlSchemaValidateDoc(_validation_ctxt, _doc) != 0) {
        THROW_XML_ERROR(this, "IMS config file '" << xml_file_path << "' is invalid!");
    }
}

//
// Return a property of a node as string
// may throw if optional is false and property not found
//
std::string xml_parser::xml_node_property(xmlNodePtr node, 
                                          std::string name,
                                          bool optional)
throw(ims::exception)
{
    xmlChar* value = xmlGetProp(node, (xmlChar*) name.c_str());
    if (! value) {
        if (optional) return std::string();
        THROW_NODE_ERROR(node, "Property '" << name <<
                         "' not found in node '" << node->name << "'!");
    }

    std::string result((const char*) value);
    xmlFree(value);
    return result;
}

//
// Return a property of a node as signed integer
// may throw if property not found or not an integer
//
int32_t xml_parser::xml_node_property_int(xmlNodePtr node, std::string name)
throw(ims::exception)
{
    std::string value = xml_node_property(node, name);

    std::istringstream iss(value);
    int32_t result;
    iss >> std::skipws >> result;
    if (!iss || iss.eof() == false) {
        THROW_NODE_ERROR(node, "Invalid signed 32-bit integer value '" << value << "' !");
    }

    return result;
}

//
// Return a property of a node as integer
// may throw if property not an integer
//
int32_t xml_parser::xml_node_property_int(xmlNodePtr node, std::string name, int32_t default_value)
throw(ims::exception)
{
    std::string value = xml_node_property(node, name, true);
    if (value.empty()) return default_value;

    std::istringstream iss(value);
    int32_t result;
    iss >> std::skipws >> result;
    if (!iss || iss.eof() == false) {
        THROW_NODE_ERROR(node, "Invalid signed 32-bit integer value '" << value << "' !");
    }

    return result;
}

//
// Return a property of a node as unsigned integer
// may throw if property not found or not an integer
//
uint32_t xml_parser::xml_node_property_uint(xmlNodePtr node, std::string name)
throw(ims::exception)
{
    std::string value = xml_node_property(node, name);

    std::istringstream iss(value);
    uint32_t result;
    iss >> std::skipws >> result;
    if (!iss || iss.eof() == false) {
        THROW_NODE_ERROR(node, "Invalid unsigned 32-bit integer value '" << value << "' !");
    }

    return result;
}

//
// Return a property of a node as unsigned integer
// may throw if property not an integer
//
uint32_t xml_parser::xml_node_property_uint(xmlNodePtr node, std::string name, uint32_t default_value)
throw(ims::exception)
{
    std::string value = xml_node_property(node, name, true);
    if (value.empty()) return default_value;

    std::istringstream iss(value);
    uint32_t result;
    iss >> std::skipws >> result;
    if (!iss || iss.eof() == false) {
        THROW_NODE_ERROR(node, "Invalid unsigned 32-bit integer value '" << value << "' !");
    }

    return result;
}

//
// Return a property of a node as float
//
float xml_parser::xml_node_property_float(xmlNodePtr node, std::string name)
throw(ims::exception)
{
    std::string value = xml_node_property(node, name);

    std::istringstream iss(value);
    float result;
    iss >> std::skipws >> result;
    if (!iss || iss.eof() == false) {
        THROW_NODE_ERROR(node, "Invalid float value '" << value << "' !");
    }

    return result;
}

//
// Return a property of a node as float
//
float xml_parser::xml_node_property_float(xmlNodePtr node, std::string name, float default_value)
throw(ims::exception)
{
    std::string value = xml_node_property(node, name, true);
    if (value.empty()) return default_value;

    std::istringstream iss(value);
    float result;
    iss >> std::skipws >> result;
    if (!iss || iss.eof() == false) {
        THROW_NODE_ERROR(node, "Invalid float value '" << value << "' !");
    }

    return result;
}

//
// Return a hex property of a node as integer
//
uint32_t xml_parser::xml_node_property_hex(xmlNodePtr node, std::string name)
throw(ims::exception)
{
    std::string value = xml_node_property(node, name);

    std::istringstream iss(value);
    uint32_t result;
    iss >> std::skipws >> std::hex >> result;
    if (!iss || iss.eof() == false) {
        THROW_NODE_ERROR(node, "Invalid integer value '" << value << "' !");
    }

    return result;
}

//
// Return the first child node 
//
xmlNodePtr xml_parser::xml_node_first_child(xmlNodePtr node)
{
    xmlNodePtr child_node = node->children;
    while (child_node && child_node->type != XML_ELEMENT_NODE) child_node = child_node->next;
    return child_node;
}

//
// Return the next node 
//
xmlNodePtr xml_parser::xml_node_next_sibling(xmlNodePtr node)
{
    node = node->next;
    while (node && node->type != XML_ELEMENT_NODE) node = node->next;
    return node;
}

//
// return all children which match a xpath query
//
xmlNodeSetPtr xml_parser::xml_xpath_get_children(const xmlDocPtr   doc, 
                                                 const xmlNodePtr  node,
                                                 const char* const xpath)
throw(ims::exception)
{
    xmlXPathContextPtr context;
    xmlXPathObjectPtr  res;
    xmlNodeSetPtr      children;

    if ((doc == NULL) || (xpath == NULL)) {
        return NULL;
    }

    context = xmlXPathNewContext(doc);
    context->node = node;

    res = xmlXPathEvalExpression((const xmlChar*)xpath, context);

    xmlXPathFreeContext(context);

    if (res == NULL) {
        xmlXPathFreeNodeSetList(res);
        THROW_XML_ERROR(this, "Cannot run XPATH query '" << xpath << "' !");
        return NULL;
    }

    if (xmlXPathNodeSetIsEmpty(res->nodesetval)) {
        xmlXPathFreeNodeSet(res->nodesetval);
        xmlXPathFreeNodeSetList(res);
        return NULL;
    }

    children = res->nodesetval;

    xmlXPathFreeNodeSetList(res);

    return children;
}

//
// Return the text content of a node element
//
std::string xml_parser::xml_node_get_content(const xmlNodePtr node)
throw(ims::exception)
{
    if (node->children == NULL) THROW_NODE_ERROR(node, "Node " << node->name << " is empty !");
    if (node->children->next) THROW_NODE_ERROR(node, "Node " << node->name << " should contain only one text node !");
    if (node->children->type != XML_TEXT_NODE) THROW_NODE_ERROR(node, "Node " << node->name << " should contain only one text node !");

    return (const char*) node->children->content;
}

//
// Error handling
//

// Due to the implementation of libxml2, this object must be shared between xml_parser.
// (xmlSetStructuredErrorFunc doesn't have context parameter, it's just a "gobal" func)
static xmlError libxml_error;  
void libxml_structured_error_callback(__attribute__((__unused__)) void* userdata,
                                      xmlErrorPtr error)
{
    xmlResetError(&libxml_error);
    xmlCopyError(error, &libxml_error);
}


void xml_parser::log_xml_error()
{
    if (libxml_error.code != XML_ERR_OK)
    {
        if (libxml_error.file != NULL)
        {
            if (libxml_error.message != NULL)
            {
                LOG_PROCESS(ims::log::error,
                            libxml_error.file << ":" << libxml_error.line << " " <<
                            libxml_error.message);
            } else {
                LOG_PROCESS(ims::log::error,
                            libxml_error.file << ":" << libxml_error.line << " No LIBXML2 error message.");
            }
        } else if (libxml_error.message != NULL) {
            LOG_PROCESS(ims::log::error, "<unknown XML file>: " << libxml_error.message);
        }
    }
}

//
// Ctor
//
xml_parser::xml_parser() :
    _doc(NULL),
    _parser_ctxt(NULL),
    _schema_doc(NULL),
    _schema_parser_ctxt(NULL),
    _schema(NULL),
    _validation_ctxt(NULL)
{
    memset(&libxml_error, 0, sizeof(xmlError));
    libxml_error.code = XML_ERR_OK;
}

//
// Dtor
//
xml_parser::~xml_parser()
{
    _parsers_count--;
    if (_validation_ctxt) xmlSchemaFreeValidCtxt(_validation_ctxt);
    if (_schema) xmlSchemaFree(_schema);
    if (_schema_parser_ctxt) xmlSchemaFreeParserCtxt(_schema_parser_ctxt);
    if (_schema_doc) xmlFreeDoc(_schema_doc);
    if (_doc) xmlFreeDoc(_doc);
    if (_parser_ctxt) xmlFreeParserCtxt(_parser_ctxt);
    if (_parsers_count <= 0) {
        xmlResetError(&libxml_error);
        xmlCleanupParser();
    }
}

