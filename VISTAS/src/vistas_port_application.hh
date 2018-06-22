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
 * Application Port. (i.e. associated with IMS message)
 */
#ifndef _VISTAS_PORT_APPLICATION_HH_
#define _VISTAS_PORT_APPLICATION_HH_
#include "vistas_port.hh"
#include <vector>

namespace vistas
{

class port_application_base;
typedef shared_ptr<port_application_base> port_application_ptr;

class port_application_base : public port
{
public:
    port_application_base(context_weak_ptr context, socket_ptr socket);

    // Reset all messages of this port
    virtual ims_return_code_t reset_messages()
    throw(ims::exception) = 0;

    // Invalidate all messages of this port
    virtual ims_return_code_t invalidate_messages()
    throw(ims::exception) = 0;

    // Return port protocol
    virtual ims_protocol_t get_protocol() = 0;
    
    // Return true if this port is a periodic output one
    virtual bool is_periodic_output() { return false; }
    
    // Init header flags, call after constructor
    void init_header_flags(uint16_t prod_id, bool seq_num_enabled, bool qos_timestamp_enabled, bool data_timestamp_enabled);
    
    // getters
    inline uint16_t get_prod_id() {return _prod_id;}
    inline bool is_seq_num_enabled() {return _seq_num_enabled;}
    inline bool is_qos_timestamp_enabled() {return _qos_timestamp_enabled;}
    inline bool is_data_timestamp_enabled() {return _data_timestamp_enabled;}

protected:
    // size to reserve when sending/receiving, "prepare_header" will prepare the header in these 20 bytes
    static const uint32_t VISTAS_HEADER_SIZE;
    // call before sending to fill the header
    void prepare_header(void* buffer);

    uint16_t _prod_id;
    uint16_t _seq_num;
    bool _seq_num_enabled;
    bool _qos_timestamp_enabled;
    bool _data_timestamp_enabled;
};

template <typename message_t>
class port_application : public port_application_base
{
public:

    //Ctor
    inline port_application(context_weak_ptr context, socket_ptr socket);

    // Reset all messages of this port
    virtual ims_return_code_t reset_messages()
    throw(ims::exception);

    // Invalidate all messages of this port
    ims_return_code_t invalidate_messages()
    throw(ims::exception);


protected:
    typedef std::vector<message_t> message_list_t;
    message_list_t _message_list;

};

//***************************************************************************
// Templates/inlines
//***************************************************************************
template <typename message_t>
port_application<message_t>::port_application(context_weak_ptr context, socket_ptr socket) :
    port_application_base(context, socket)
{
}

template <typename message_t>
ims_return_code_t port_application<message_t>::reset_messages()
throw(ims::exception)
{
    ims_return_code_t result = ims_no_error;
    
    _seq_num = 0;

    for (typename message_list_t::iterator imessage = _message_list.begin();
         imessage != _message_list.end();
         imessage++)
    {
        result = std::max((*imessage)->reset(), result);
    }

    return result;
}

template <typename message_t>
ims_return_code_t port_application<message_t>::invalidate_messages()
throw(ims::exception)
{
    ims_return_code_t result = ims_no_error;

    for (typename message_list_t::iterator imessage = _message_list.begin();
         imessage != _message_list.end();
         imessage++)
    {
        result = std::max((*imessage)->invalidate(), result);
    }

    return result;
}

}
#endif
