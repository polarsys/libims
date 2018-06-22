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
 * IMS test vcom client
 */
#ifndef _IMS_TEST_VCOM_CLIENT_H_
#define _IMS_TEST_VCOM_CLIENT_H_
#include "ims_test.h"

#ifdef __linux
#include <arpa/inet.h>
#define IMS_SOCKET int
#define INVALID_SOCKET (-1)
#endif
#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#define IMS_SOCKET SOCKET
#endif

struct ims_test_vcom_client_internal {};

namespace ims_test
{
  class vcom_client : public ims_test_vcom_client_internal
  {
  public:
    vcom_client(ims_test_actor_t actor_base, const char* ip, uint32_t port);
    ~vcom_client();

    void reset(uint32_t vcom_id);

    void push(uint32_t value);

    void push(const char* buffer, uint32_t buffer_size);

    void send();

    int wait_answer(char* buffer, uint32_t buffer_size, uint32_t timeout_us);

  private:
    IMS_SOCKET         _sock;
    struct sockaddr_in _saddr;
    char*              _message;
    char*              _pos;
  };
}
#endif
