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
// Simple message sampling wich handle number and sdi
//
#ifndef _VISTAS_MESSAGE_SAMPLING_A429_HH_
#define _VISTAS_MESSAGE_SAMPLING_A429_HH_
#include "vistas_message_sampling.hh"
#include "a429_tools.hh"


namespace vistas
{
  class message_sampling_a429;
  typedef shared_ptr<message_sampling_a429> message_sampling_a429_ptr;

  class message_sampling_a429 : public message_sampling
  {
  public:
    inline message_sampling_a429(context_weak_ptr     context, 
                                 std::string          name,
                                 a429::label_number_t number,
                                 a429::sdi_t          sdi,
                                 ims_direction_t      direction, 
                                 uint32_t             validity_duration_us,
                                 std::string     local_name,
                                 std::string     bus_name,
                                 uint32_t        period_us,
                                 port_weak_ptr        port);

    inline uint32_t port_read_data(char* data, uint32_t max_size);

  private:
    a429::label_number_t number;
    a429::sdi_t          sdi;
  };


  //***************************************************************************
  // Inlines
  //***************************************************************************
  message_sampling_a429::message_sampling_a429(context_weak_ptr     context, 
                                               std::string          name,
                                               a429::label_number_t number,
                                               a429::sdi_t          sdi,
                                               ims_direction_t      direction, 
                                               uint32_t             validity_duration_us,
                                               std::string     local_name,
                                               std::string     bus_name,
                                               uint32_t        period_us,
                                               port_weak_ptr        port) :
  message_sampling(context, name, direction, ims_a429, A429_LABEL_SIZE, A429_LABEL_SIZE, validity_duration_us, local_name, bus_name, period_us, port),
    number(number),
    sdi(sdi)
  {
  }


  uint32_t message_sampling_a429::port_read_data(char* data, __attribute__((__unused__)) uint32_t max_size)
  {
    memcpy(data, _data, 4);
    update_label((uint8_t*)data, number, sdi);      
    port_set_sent();
    return 4;
  }
  

}


#endif
