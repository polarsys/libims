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
#include "ims_init_parser.hh"
#include "ims_init_xsd.h"

#include <iostream>
#include <sstream>

#define NODE_AFDX               "AfdxSamplingMessage"
#define NODE_A429               "A429SamplingLabel"
#define NODE_CAN                "CanSamplingMessage"
#define NODE_DISCRETE           "DiscreteSignal"
#define NODE_ANALOGUE           "AnalogueSignal"

#define ATTRIBUTE_LOCAL_NAME    "LocalName"
#define ATTRIBUTE_VALUE         "Value"

namespace ims
{
ims_init_parser::ims_init_parser()
{

}

ims_init_parser::~ims_init_parser()
{

}

// convert a string "123456" to a string with {0x12, 0x34, 0x56}
std::string ims_init_parser::convertHexaToBuffer(std::string text_value)
{
    size_t str_size = text_value.size();
    size_t index= 0;
    
    std::string hexa_string;
    
    // if the text is "123", treat it as "0123"
    if (str_size & 1)
    {
        char c =  (char)strtol( text_value.substr(0, 1).c_str(), NULL, 16 );
        hexa_string.push_back(c);

        index = 1;
    }
    
    while( index < str_size)
    {
        char c =  (char)strtol( text_value.substr(index, 2).c_str(), NULL, 16 );
        hexa_string.push_back(c);

        index += 2;
    }
    
    return hexa_string;
}

void ims_init_parser::load_and_validate(const char* xml_file_path) throw(ims::exception)
{


    xml_parser::load_and_validate(xml_file_path, ims_init_xsd);
    
    xmlNodePtr root_node = xmlDocGetRootElement(_doc);
    
    // Load AFDX
    xmlNodeSetPtr afdx_node_set = xml_xpath_get_children(_doc, root_node, NODE_AFDX);
    if (afdx_node_set != NULL)
    {
        for (int32_t afdx_node_id = 0;
             afdx_node_id < afdx_node_set->nodeNr;
             afdx_node_id++)
        {
            xmlNodePtr afdx_node = afdx_node_set->nodeTab[afdx_node_id];

            std::string local_name = xml_node_property(afdx_node, ATTRIBUTE_LOCAL_NAME);
            std::string text_value = xml_node_property(afdx_node, ATTRIBUTE_VALUE);

            _afdx_map[local_name] = convertHexaToBuffer(text_value);
        }

        xmlXPathFreeNodeSet(afdx_node_set);
    }
    
    // Load A429
    xmlNodeSetPtr a429_node_set = xml_xpath_get_children(_doc, root_node, NODE_A429);
    if (a429_node_set != NULL)
    {
        for (int32_t a429_node_id = 0;
             a429_node_id < a429_node_set->nodeNr;
             a429_node_id++)
        {
            xmlNodePtr a429_node = a429_node_set->nodeTab[a429_node_id];

            std::string local_name = xml_node_property(a429_node, ATTRIBUTE_LOCAL_NAME);
            std::string text_value = xml_node_property(a429_node, ATTRIBUTE_VALUE);

            _a429_map[local_name] = convertHexaToBuffer(text_value);
        }

        xmlXPathFreeNodeSet(a429_node_set);
    }
    
    // Load CAN
    xmlNodeSetPtr can_node_set = xml_xpath_get_children(_doc, root_node, NODE_CAN);
    if (can_node_set != NULL)
    {
        for (int32_t can_node_id = 0;
             can_node_id < can_node_set->nodeNr;
             can_node_id++)
        {
            xmlNodePtr can_node = can_node_set->nodeTab[can_node_id];

            std::string local_name = xml_node_property(can_node, ATTRIBUTE_LOCAL_NAME);
            std::string text_value = xml_node_property(can_node, ATTRIBUTE_VALUE);

            _can_map[local_name] = convertHexaToBuffer(text_value);
        }

        xmlXPathFreeNodeSet(can_node_set);
    }
    
    // Load ANALOGUE
    xmlNodeSetPtr ana_node_set = xml_xpath_get_children(_doc, root_node, NODE_ANALOGUE);
    if (ana_node_set != NULL)
    {
        for (int32_t ana_node_id = 0;
             ana_node_id < ana_node_set->nodeNr;
             ana_node_id++)
        {
            xmlNodePtr ana_node = ana_node_set->nodeTab[ana_node_id];

            std::string local_name = xml_node_property(ana_node, ATTRIBUTE_LOCAL_NAME);
            std::string text_value = xml_node_property(ana_node, ATTRIBUTE_VALUE);

            float f = atof( text_value.c_str() );

            _analogue_map[local_name] = f;
        }

        xmlXPathFreeNodeSet(ana_node_set);
    }
    
    // Load DISCRETE
    xmlNodeSetPtr disc_node_set = xml_xpath_get_children(_doc, root_node, NODE_DISCRETE);
    if (disc_node_set != NULL)
    {
        for (int32_t disc_node_id = 0;
             disc_node_id < disc_node_set->nodeNr;
             disc_node_id++)
        {
            xmlNodePtr disc_node = disc_node_set->nodeTab[disc_node_id];

            std::string local_name = xml_node_property(disc_node, ATTRIBUTE_LOCAL_NAME);
            std::string text_value = xml_node_property(disc_node, ATTRIBUTE_VALUE);

            _discrete_map[local_name] = text_value;
        }

        xmlXPathFreeNodeSet(disc_node_set);
    }
}

bool ims_init_parser::hasAfdxSamplingMessageValue(const std::string & local_name )
{
    return _afdx_map.find(local_name) != _afdx_map.end();
}

bool ims_init_parser::hasA429LabelMessageValue(const std::string & local_name )
{
    return _a429_map.find(local_name) != _a429_map.end();
}

bool ims_init_parser::hasCanSamplingMessageValue(const std::string & local_name )
{
    return _can_map.find(local_name) != _can_map.end();
}

bool ims_init_parser::hasAnalogueSignalValue(const std::string & local_name )
{
    return _analogue_map.find(local_name) != _analogue_map.end();
}

bool ims_init_parser::hasDiscreteSignalValue(const std::string & local_name )
{
    return _discrete_map.find(local_name) != _discrete_map.end();
}

std::string ims_init_parser::getAfdxSamplingMessageValue(const std::string & local_name )
{
    return _afdx_map[local_name];
}

std::string ims_init_parser::getA429LabelMessageValue(const std::string & local_name )
{
    return _a429_map[local_name];
}

std::string ims_init_parser::getCanSamplingMessageValue(const std::string & local_name )
{
    return _can_map[local_name];
}

float ims_init_parser::getAnalogueSignalValue(const std::string & local_name )
{
    return _analogue_map[local_name];
}

std::string ims_init_parser::getDiscreteSignalValue(const std::string & local_name )
{
    return _discrete_map[local_name];
}
}
