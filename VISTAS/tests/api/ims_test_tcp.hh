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
 * IMS test tcp
 */
#ifndef _IMS_TEST_TCP_H_
#define _IMS_TEST_TCP_H_
#include "ims_test.h"
#include <string>

#ifdef __linux
#define IMS_SOCKET int
#define INVALID_SOCKET (-1)
#endif
#ifdef _WIN32
#include <winsock2.h>
#define IMS_SOCKET SOCKET
#endif

struct ims_test_tcp_internal {};

namespace ims_test
{
class test_tcp : public ims_test_tcp_internal
{
public:
    test_tcp(ims_test_actor_t actor_base, std::string ip, uint32_t port);
    ~test_tcp() { close(); }

    uint32_t receive(char* buffer, uint32_t buffer_size, uint32_t timeout_us);
    uint32_t send(char* buffer, uint32_t buffer_size);

private:
    void close();
    IMS_SOCKET _sock;
    IMS_SOCKET _clientsock;
};
}

#endif
