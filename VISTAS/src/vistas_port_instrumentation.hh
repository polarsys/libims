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
 * Instrumentation port
 */
#ifndef _VISTAS_PORT_INSTRUMENTATION_HH_
#define _VISTAS_PORT_INSTRUMENTATION_HH_
#include "vistas_port.hh"
#include "vistas_socket_tcp_message.hh"

namespace vistas
{
  class port_instrumentation : public port
  {
  public:
    // Ctor
    port_instrumentation(context_weak_ptr context, socket_tcp_message_ptr socket);
    
    // Receive and handle one instrumentation message.
    void receive();

    // Will always thow error::invalid_direction.
    void send();
    
    // Send the F_REGISTER_xxxx packet and waits for the R_REGISTER_xxxxx packet
    void register_instrum();
    void register_mode();
    void register_power();
    void register_sync();
    
    // Send the R_SYNCHRO packet
    void acknowledge_synchro();

    ~port_instrumentation();

  private:
    
    // Hidden implementation
    class request_parser;

    char*           _message;
    request_parser* _parser;
    
    bool _registered_instrum;
    bool _registered_mode;
    bool _registered_power;
    bool _registered_sync;
    
    void register_generic(void * register_packet, uint32_t register_packet_size, uint32_t expected_response_id);
    
    void fill_vc_name(uint32_t & string_size, char * vc_name);
  };
}
#endif
