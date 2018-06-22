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
 * Sampling message class
 */
#ifndef _VISTAS_MESSAGE_SAMPLING_HH_
#define _VISTAS_MESSAGE_SAMPLING_HH_
#include "vistas_message_buffered.hh"

namespace vistas
{
// Pointer to this class
class message_sampling;
typedef shared_ptr<message_sampling> message_sampling_ptr;


class message_sampling: public message_buffered
{
public:
    message_sampling(context_weak_ptr context,
                     std::string      name,
                     ims_direction_t  direction,
                     ims_protocol_t   protocol,
                     uint32_t         size,
                     uint32_t         expected_size,
                     uint32_t         validity_duration_us,
                     std::string     local_name,
                     std::string     bus_name,
                     uint32_t        period_us,
                     port_weak_ptr    port);

    // API implementation
    virtual ims_return_code_t write_sampling(const char* message_addr,
                                             uint32_t    message_size)
    throw(ims::exception);

    virtual ims_return_code_t read_sampling(char*           message_addr,
                                            uint32_t*       message_size,
                                            ims_validity_t* message_validity)
    throw(ims::exception);

    virtual ims_return_code_t set_validity_duration(uint32_t validity_duration_us)
    throw(ims::exception);

    virtual ims_return_code_t get_validity_duration(uint32_t* validity_duration_us)
    throw(ims::exception);

    virtual ims_return_code_t reset()
    throw(ims::exception);

    virtual ims_return_code_t invalidate()
    throw(ims::exception);

    virtual bool check(ims_protocol_t   protocol,
                       uint32_t         max_size,
                       uint32_t         depth,
                       ims_direction_t  direction);

    // message_buffered implementation
    void port_set_received();
    void port_set_sent();

private:
    uint32_t        _validity_duration_us; // Data validity duration
    uint64_t        _data_time_us;         // Last data receive date (valid only if _data_size > 0)
    uint32_t        _expected_size;        // Size expected from "ims_get_message" / "check"
};

}

#endif
