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
// Simple message sampling wich handle can ID
//
#ifndef _VISTAS_MESSAGE_SAMPLING_CAN_HH_
#define _VISTAS_MESSAGE_SAMPLING_CAN_HH_
#include "vistas_message_sampling.hh"


namespace vistas
{
  class message_sampling_can;
  typedef shared_ptr<message_sampling_can> message_sampling_can_ptr;

  class message_sampling_can : public message_sampling
  {
  public:
    inline message_sampling_can(context_weak_ptr    context, 
                                std::string         name,
                                uint32_t            id,
                                uint32_t            size,
                                ims_direction_t     direction, 
                                uint32_t            validity_duration_us,
								std::string			local_name,
								std::string			bus_name,
								uint32_t			period_us,
                                port_weak_ptr       port);

	inline ims_return_code_t set_id(const uint32_t pId)
	throw(ims::exception);

    uint32_t id;
  };


  //***************************************************************************
  // Inlines
  //***************************************************************************
  message_sampling_can::message_sampling_can(context_weak_ptr     context, 
                                             std::string          name,
                                             uint32_t             id,
                                             uint32_t             size,
                                             ims_direction_t      direction, 
                                             uint32_t             validity_duration_us,
                                             std::string     local_name,
                                             std::string     bus_name,
                                             uint32_t        period_us,
                                             port_weak_ptr        port) :
    message_sampling(context, name, direction, ims_can, size, size, validity_duration_us, local_name, bus_name, period_us, port),
    id(id)
	{
	}

  ims_return_code_t message_sampling_can::set_id(const uint32_t pId)
	  throw(ims::exception)
  {
    // Convert the new id to hexa
	std::stringstream iss;
	iss << pId;

    uint32_t result;
    iss >> std::skipws >> std::hex >> result;

    if (!iss || iss.eof() == false) 
	{
        LOG_ERROR("Invalid integer value '" << pId << "' !");
		return ims_invalid_configuration;
    }

	// Convert the id to big endian 
	id = htonl(result);

	LOG_INFO("Set of ID for message " << get_name() << " with new value : " << "<" << id << ">."); 

	// Update the name with the new id 
	std::stringstream sname;
    sname <<  _bus_name << "_" << id;

	set_name(sname.str());

	return ims_no_error;
  }
}


#endif
