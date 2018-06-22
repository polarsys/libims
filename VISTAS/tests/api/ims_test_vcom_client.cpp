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

#include "ims_test_vcom_client.hh"
#include "ims_test_actor.hh"

#define MAX_MSG_SIZE 200

namespace ims_test
{

vcom_client::vcom_client(ims_test_actor_t actor_base, const char* ip, uint32_t port) :
    _message(new char[MAX_MSG_SIZE])
{
    ims_test::actor* actor = static_cast<ims_test::actor*>(actor_base);

    _sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (_sock == INVALID_SOCKET) {
        TEST_ABORT(actor, "Error creating test socket.");
    }

    memset(&_saddr, 0, sizeof(sockaddr_in));
    _saddr.sin_family = PF_INET;
    _saddr.sin_addr.s_addr = inet_addr(ip);
    _saddr.sin_port = htons(port);

    _pos = _message;
}

void vcom_client::reset(uint32_t vcom_id)
{
    *(uint32_t*)_message = htonl(vcom_id);
    _pos = _message + 2 * sizeof(uint32_t);  // VcomID + Vcom len
}

void vcom_client::push(uint32_t value)
{
    *(uint32_t*)_pos = htonl(value);
    _pos += sizeof(uint32_t);
}

void vcom_client::push(const char* buffer, uint32_t buffer_size)
{
    memcpy(_pos, buffer, buffer_size);
    _pos += buffer_size;
}

void vcom_client::send()
{
    *(uint32_t*)(_message + sizeof(uint32_t)) = htonl(_pos - _message); // vcom len
    sendto(_sock, _message, _pos - _message, 0, (struct sockaddr *) &_saddr, sizeof(sockaddr_in));
}

int vcom_client::wait_answer(char* buffer, uint32_t buffer_size, uint32_t timeout_us)
{
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_us;
    
    fd_set select_set;
    FD_ZERO(&select_set);
    FD_SET(_sock, &select_set);

    int result = select(_sock + 1, &select_set, NULL, NULL, &tv);
    if (result > 0) {
        return recvfrom(_sock, buffer, buffer_size, 0, NULL, NULL);
    } else {
        return 0;
    }
}

vcom_client::~vcom_client()
{
    delete[] _message;

    if (_sock != INVALID_SOCKET) {
        shutdown(_sock, 2);
#ifdef _WIN32
        closesocket(_sock);
#else
        ::close(_sock);
#endif
        _sock = INVALID_SOCKET;
    }
}

}
