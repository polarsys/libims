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
 * A429 port
 */
#ifndef _VISTAS_PORT_A429_HH_
#define _VISTAS_PORT_A429_HH_
#include "vistas_port_application.hh"
#include "vistas_message_sampling_a429.hh"
#include "vistas_message_queuing_a429.hh"
#include "vistas_message_wrapper.hh"

namespace vistas
{
class port_a429;
typedef shared_ptr<port_a429> port_a429_ptr;

//
// The port class itself
//
class port_a429: public port_application<message_buffered_ptr>
{
public:
    port_a429(context_weak_ptr context, socket_ptr socket, std::string bus_name, uint32_t fifo_size);
    ~port_a429();

    // Lookup/initialize the message associated with this port
    template <ims_mode_t>
    ims::message_ptr get_label(std::string number, std::string sdi, uint32_t validity_duration_us, uint32_t depth, std::string local_name, uint32_t period_us);

    // Read/write data from socket
    void send();
    void receive();

    // Return port protocol
    virtual ims_protocol_t get_protocol() { return ims_a429; }

private:
    template <ims_mode_t mode>
    inline message_buffered_ptr create_message_a429(std::string          name,
                                                    a429::label_number_t number,
                                                    a429::sdi_t          sdi,
                                                    uint32_t             validity_duration_us,
                                                    uint32_t             depth,
                                                    std::string          local_name,
                                                    uint32_t             period_us);

    std::string _bus_name;
    uint32_t    _fifo_size;
    uint8_t*    _fifo;

    // Map SDI => messages
    typedef std::vector<message_buffered_ptr> sdi_message_map_t;

    // Map label => sdi_message_map_t (same content as _message_list in port_application)
    typedef std::tr1::unordered_map<a429::label_number_t, sdi_message_map_t> message_map_t;
    message_map_t _message_map;
};

//***************************************************************************
// Templates
//***************************************************************************
template <>
inline message_buffered_ptr port_a429::create_message_a429<ims_sampling>(std::string          name,
                                                                         a429::label_number_t number,
                                                                         a429::sdi_t          sdi,
                                                                         uint32_t             validity_duration_us,
                                                                         __attribute__((__unused__)) uint32_t             depth,
                                                                         std::string          local_name,
                                                                         uint32_t             period_us)
{
    return message_sampling_a429_ptr(new message_sampling_a429(_context,
                                                               name,
                                                               number,
                                                               sdi,
                                                               _socket->get_address()->get_direction(),
                                                               validity_duration_us,
                                                               local_name,
                                                               _bus_name,
                                                               period_us,
                                                               this));
}

template <>
inline message_buffered_ptr port_a429::create_message_a429<ims_queuing>(std::string          name,
                                                                        __attribute__((__unused__)) a429::label_number_t number,
                                                                        __attribute__((__unused__)) a429::sdi_t          sdi,
                                                                        __attribute__((__unused__)) uint32_t             validity_duration_us,
                                                                        uint32_t             depth,
                                                                        std::string     local_name,
                                                                        __attribute__((__unused__)) uint32_t        period_us)
{
    return message_queuing_a429_ptr(new message_queuing_a429(_context,
                                                             name,
                                                             _socket->get_address()->get_direction(),
                                                             depth,
                                                             local_name,
                                                             _bus_name,
                                                             this));
}

template <ims_mode_t mode>
ims::message_ptr port_a429::get_label(std::string number,
                                      std::string sdi,
                                      uint32_t    validity_duration_us,
                                      uint32_t    depth,
                                      std::string    local_name,
                                      uint32_t       period_us)
{
    message_buffered_ptr message;

    // Compute label and SDI
    a429::label_number_t label_number;
    a429::sdi_t          label_sdi;

    try {
        label_number = a429::label_number_encode(number.c_str());
        label_sdi = a429::sdi_encode(sdi);
    }
    catch (std::exception& e)
    {
        THROW_IMS_ERROR(ims_init_failure, e.what());
    }
    
    // Get or create sdi_message_map
    sdi_message_map_t& sdi_message_map = _message_map[label_number];
    sdi_message_map.resize(6);

    if (label_sdi > a429::SDI_XX) {
      // We want to add a label without SDI.
      if (sdi_message_map[a429::SDI_DD]) {
        // We already have this label
        // return a wrapper of the message with the correct local_name
        return message_wrapper_ptr(new message_wrapper(sdi_message_map[a429::SDI_DD], local_name) );
      } else {
        // This label doesn't exists
        if (sdi_message_map[a429::SDI_00] ||
            sdi_message_map[a429::SDI_01] ||
            sdi_message_map[a429::SDI_10] ||
            sdi_message_map[a429::SDI_11]) {
          // We already have a label with SDI in the map !
          THROW_IMS_ERROR(ims_init_failure, "Labels with SDI and without SDI on the same bus !");
        }
        // We don't have any label in the map, create it
        message = sdi_message_map[a429::SDI_DD] = 
          create_message_a429<mode>(_bus_name + "_" + number + "_" + sdi,
                                    label_number,
                                    a429::SDI_DD,
                                    validity_duration_us,
                                    depth,
                                    local_name,
                                    period_us);

            _message_list.push_back(message);
        }
    }
    else {
        // We want to add a label with SDI
        if (sdi_message_map[a429::SDI_DD]) {
            // We already have a label without SDI in the map !
            THROW_IMS_ERROR(ims_init_failure, "Labels with SDI and without SDI on the same bus !");
        }

      if (sdi_message_map[label_sdi]) {
        // This label already exists
        // return a wrapper of the message with the correct local_name
        return message_wrapper_ptr(new message_wrapper(sdi_message_map[label_sdi], local_name) );
      } else {
        // It doesn't exists, create it.
        message = sdi_message_map[label_sdi] =
          create_message_a429<mode>(_bus_name + "_" + number + "_" + sdi,
                                    label_number,
                                    label_sdi,
                                    validity_duration_us,
                                    depth,
                                    local_name,
                                    period_us);

            _message_list.push_back(message);
        }
    }

    if (_message_list.size() * A429_LABEL_SIZE > _fifo_size) {
        THROW_IMS_ERROR(ims_init_failure, "fifo size too small for A429 bus " << _bus_name);
    }

    return message;
}

}

#endif

