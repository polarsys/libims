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

/*
 * IMS context
 */
#ifndef _IMS_VIRTUAL_COMPONENT_HH_
#define _IMS_VIRTUAL_COMPONENT_HH_
#include "ims_node.hh"
#include "backend_context.hh"

namespace ims
{
  class context : public node
  {
  public:
    static context* create(backend::context::factory_ptr backend_factory,
                           const char*                   ims_config_file_path,
                           ims_create_context_parameter_t* create_context_parameter)
      throw(ims::exception);

    inline backend::context_ptr get_backend_context() { return _backend_context; }


  protected:
    inline context(std::string vc_name) : node(vc_name) {}
    class factory;

    backend::context_ptr _backend_context;
  };
}

#endif
