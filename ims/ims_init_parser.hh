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
#ifndef _IMS_INIT_PARSER_HH_
#define _IMS_INIT_PARSER_HH_

#include "xml_parser.hh"
#include <map>

//
// Parser itself
//

namespace ims
{

  class ims_init_parser : public xml_parser
  {
  public:
  
    ims_init_parser();
    virtual ~ims_init_parser();
    
    void load_and_validate(const char* xml_file_path) throw(ims::exception);
  
    bool hasAfdxSamplingMessageValue(const std::string & local_name );
    bool hasA429LabelMessageValue(const std::string & local_name );
    bool hasCanSamplingMessageValue(const std::string & local_name );
    bool hasAnalogueSignalValue(const std::string & local_name );
    bool hasDiscreteSignalValue(const std::string & local_name );
  
    std::string getAfdxSamplingMessageValue(const std::string & local_name );
    std::string getA429LabelMessageValue(const std::string & local_name );
    std::string getCanSamplingMessageValue(const std::string & local_name );
    float getAnalogueSignalValue(const std::string & local_name );
    std::string getDiscreteSignalValue(const std::string & local_name );
  
  private:
    // data is an array of chars with a size, so we store it in a string
    std::map<std::string, std::string> _afdx_map;
    std::map<std::string, std::string> _a429_map;
    std::map<std::string, std::string> _can_map;
    // data is a float
    std::map<std::string, float> _analogue_map;
    // data is really a string !
    std::map<std::string, std::string> _discrete_map;
    
    // convert a string "123456" to a string with {0x12, 0x34, 0x56}
    std::string convertHexaToBuffer(std::string text_value);
  };

}

#endif
