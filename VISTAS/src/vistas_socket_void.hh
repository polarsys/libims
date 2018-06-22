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
// A socket that does nothing
// 
#include "vistas_socket.hh"

namespace vistas
{

class socket_void : public socket
{
    inline uint32_t receive(__attribute__((__unused__)) char* buffer,
                            __attribute__((__unused__)) uint32_t buffer_size,
                            __attribute__((__unused__)) client* client = NULL)
    throw(ims::exception) { return 0; }

    inline void send(__attribute__((__unused__)) const char* buffer,
                     __attribute__((__unused__)) uint32_t size)
    throw(ims::exception) { }

    inline void reply(__attribute__((__unused__)) client& client,
                      __attribute__((__unused__)) const char* buffer,
                      __attribute__((__unused__)) uint32_t size)
    throw(ims::exception) { }

};

}
