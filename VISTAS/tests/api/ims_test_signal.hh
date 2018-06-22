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
 * signals between actor
 */
#ifndef _IMS_TEST_SIGNAL_H_
#define _IMS_TEST_SIGNAL_H_
#include "ims_test.h"

#define MAX_NUMBER_ACTOR  10

#ifdef __linux
#define IMS_SOCKET int
#define INVALID_SOCKET (-1)
#endif
#ifdef _WIN32
#include <winsock2.h>
#define IMS_SOCKET SOCKET
#endif

namespace ims_test
{
class actor;

struct signal_wait
{
    signal_wait(ims_test_actor_t actor);
    ~signal_wait();

    bool wait(uint32_t actor_id, uint32_t timeout_us);

private:
    bool accept(uint32_t actor_id, uint32_t timeout_us);
    void close(IMS_SOCKET& socket);

    actor* _actor;
    IMS_SOCKET    _server_socket;
    IMS_SOCKET    _client_socket[MAX_NUMBER_ACTOR + 1];
};

struct signal_send
{
    signal_send(ims_test_actor_t actor);
    ~signal_send();

    bool send(uint32_t actor_id, uint32_t timeout_us);

private:
    bool connect(uint32_t actor_id, uint32_t timeout_us);
    void close(IMS_SOCKET& socket);

    actor* _actor;
    IMS_SOCKET    _socket[MAX_NUMBER_ACTOR + 1];
};

}

#endif
