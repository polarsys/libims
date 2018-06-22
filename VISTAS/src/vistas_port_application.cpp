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
#include "vistas_port_application.hh"
#include "vistas_context.hh"

#ifdef __linux
#include <byteswap.h>
#else
#define bswap_16(x) __builtin_bswap16(x)
#define bswap_32(x) __builtin_bswap32(x)
#define bswap_64(x) __builtin_bswap64(x)
#endif

namespace vistas
{
const uint32_t port_application_base::VISTAS_HEADER_SIZE = 20;

port_application_base::port_application_base(context_weak_ptr context, socket_ptr socket) :
    port(context, socket),
    _prod_id(0),
    _seq_num(0),
    _seq_num_enabled(false),
    _qos_timestamp_enabled(false),
    _data_timestamp_enabled(false)
{
}

void port_application_base::init_header_flags(uint16_t prod_id, bool seq_num_enabled, bool qos_timestamp_enabled, bool data_timestamp_enabled)
{
    _prod_id = prod_id;
    _seq_num =0;
    _seq_num_enabled = seq_num_enabled;
    _qos_timestamp_enabled = qos_timestamp_enabled;
    _data_timestamp_enabled = data_timestamp_enabled;
}

#pragma pack (push, 1)

struct vistas_header {
    uint16_t prod_id;
    uint16_t seq_num;
    uint64_t qos_timestamp;
    uint64_t data_timestamp;
};

#pragma pack (pop)

void port_application_base::prepare_header(void* buffer)
{
    vistas_header * header = (vistas_header*)buffer;
    header->prod_id = bswap_16(_prod_id);
    if (_seq_num_enabled)
    {
        _seq_num++;
        if (_seq_num == 0)
        {
            // the spec says "When the maximum value (65535) is reached, the sequence number is set to 1 for the corresponding channel at the next emission."
            _seq_num = 1;
        }
        header->seq_num = bswap_16(_seq_num);
    }
    else
    {
        header->seq_num = 0;
    }
    
    if (_qos_timestamp_enabled)
    {
        header->qos_timestamp = bswap_64(_context->get_posix_timestamp());
    }
    else
    {
        header->qos_timestamp = 0;
    }
    
    if (_data_timestamp_enabled)
    {
        header->data_timestamp = bswap_64(_context->get_time_us());
    }
    else
    {
        header->data_timestamp = 0;
    }
}

}
