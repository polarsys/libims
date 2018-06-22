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
#include "ims_test_signal.hh"
#include "ims_test_actor.hh"

#ifdef __linux
#include <arpa/inet.h>
#include <errno.h>
#endif
#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <mswsock.h>
#endif

#define WAIT_ACTOR_IP     "127.0.0.1"
#define BASE_PORT         65000

static const __attribute__((__unused__)) int zero = 0;
static const __attribute__((__unused__)) int one = 1;

namespace ims_test
{
// ==========================================================================
// signal_wait
// ==========================================================================

//
// Ctor: create server socket and listen it.
//
signal_wait::signal_wait(ims_test_actor_t actor_base) :
    _actor(static_cast<ims_test::actor*>(actor_base)),
    _server_socket(INVALID_SOCKET)
{

    for (int id = 0; id <= MAX_NUMBER_ACTOR; id++) {
        _client_socket[id] = INVALID_SOCKET;
    }

    _server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket == INVALID_SOCKET) {
        TEST_ABORT(_actor, "Failed to create new socket!");
    }
    
    if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, (const char*) &one, sizeof(one)) != 0) {
        TEST_ABORT(_actor, "Failed to set SO_REUSEADDR!");
    }

    int port = BASE_PORT + _actor->id;
    struct sockaddr_in bind_addr;
    memset(&bind_addr, 0, sizeof(sockaddr_in));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;
    bind_addr.sin_port = htons(port);
    if (bind(_server_socket, (struct sockaddr *) &bind_addr, sizeof(bind_addr)) != 0) {
        TEST_ABORT(_actor, "Failed to bin socket to port %d!", port);
    }

    if (listen(_server_socket, MAX_NUMBER_ACTOR) != 0) {
        TEST_ABORT(_actor, "Failed to listen on port %d!", port);
    }
}

//
// Accept a connection for the given actor
// Return false if the given actor don't connect in timeout_us
//
bool signal_wait::accept(uint32_t actor_id, uint32_t timeout_us)
{
    int ret = 0;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_us;

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(_server_socket, &rfds);

    // Wait for connection
    if ((ret = select(_server_socket + 1, &rfds, NULL, NULL, &tv)) <= 0) {
        TEST_LOG(_actor, "Select failed ! [%d]", ret);
        return false;
    }

    // Accept it
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(sockaddr_in);
    TEST_LOG(_actor,"[ACCEPT %d]",_actor->id);
    _client_socket[actor_id] = ::accept(_server_socket, (struct sockaddr *) &addr, &addr_len);
    if (_client_socket[actor_id] <= 0) {
        TEST_ABORT(_actor, "Failed to accept TCP connection !");
    }
    
    return true;
}

//
// Wait for the given actor
// Return false if no anser in timeout_us.
// Return false if the wrong actor connect.
//
bool signal_wait::wait(uint32_t expected_actor_id,
                       uint32_t timeout_us)
{
    uint64_t begin_us = ims_test_time_us();

    // If client is not connected, accept it
    if (_client_socket[expected_actor_id] == INVALID_SOCKET) {
        if (accept(expected_actor_id, timeout_us) == false) {
            TEST_LOG(_actor, "Wait failed !");
            return false;
        }
    }

    // Wait for incomming data
    uint64_t elapsed_us = ims_test_time_us() - begin_us;
    if (elapsed_us > timeout_us){
        TEST_LOG(_actor, "Time elapsed !");
        return false;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeout_us - elapsed_us;

    fd_set rfds;
    FD_ZERO(&rfds);
    FD_SET(_client_socket[expected_actor_id], &rfds);

    if (select(_client_socket[expected_actor_id] + 1, &rfds, NULL, NULL, &tv) <= 0) {
        TEST_LOG(_actor, "Select failed !");
        return false;
    }

    // Read data and check actor ID
    uint32_t actor_id = 0;
    int ret = 0;
    TEST_LOG(_actor,"[RECV %d]",_actor->id);
    if ((ret = recv(_client_socket[expected_actor_id], (char*)&actor_id, 4, 0)) != 4) {
        TEST_ABORT(_actor, "Wrong test signal protocol ! [%d] %s", ret, strerror(errno));
    }

    if (actor_id != expected_actor_id) {
        TEST_FAIL(_actor, "Actor %d signal itself be we expect actor %d!",
                  actor_id, expected_actor_id);
        close(_client_socket[expected_actor_id]); // This socket is not associated with the good client
        return false;
    }

    return true;
}

//
// Close provided socket
//
void signal_wait::close(IMS_SOCKET& socket)
{
    TEST_LOG(_actor, "[WAIT CLOSE %d]",_actor->id);
    if (socket != INVALID_SOCKET) {
        shutdown(socket, 2);
#ifdef _WIN32
        closesocket(socket);
#else
        ::close(socket);
#endif
        socket = INVALID_SOCKET;
    }
}

//
// Destroy all sockets
//
signal_wait::~signal_wait()
{
    for (int id = 0; id <= MAX_NUMBER_ACTOR; id++) {
        close(_client_socket[id]);
    }
    close(_server_socket);
}

// ==========================================================================
// signal_send
// ==========================================================================

//
// Ctor
//
signal_send::signal_send(ims_test_actor_t actor_base) :
    _actor(static_cast<ims_test::actor*>(actor_base))
{
    for (int id = 0; id <= MAX_NUMBER_ACTOR; id++) {
        _socket[id] = INVALID_SOCKET;
    }
}

//
// Connect to the target actor
//
bool signal_send::connect(uint32_t actor_id, uint32_t timeout_us)
{
    uint64_t begin_us = ims_test_time_us();

    _socket[actor_id] = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket[actor_id] == INVALID_SOCKET) {
        TEST_ABORT(_actor, "Failed to create new socket!");
    }

    int port = BASE_PORT + actor_id;
    struct sockaddr_in wait_addr;
    memset(&wait_addr, 0, sizeof(sockaddr_in));
    wait_addr.sin_family = AF_INET;
    wait_addr.sin_addr.s_addr = inet_addr(WAIT_ACTOR_IP);
    wait_addr.sin_port = htons(port);

    // Loop on connection because target actor may not have call accept() yey
    int connect_status;
    do {
        TEST_LOG(_actor,"[CONNECT %d]",_actor->id);
        connect_status = ::connect(_socket[actor_id], (struct sockaddr *) &wait_addr, sizeof(sockaddr_in));
        if (connect_status != 0) {
            ims_test_sleep(1000);
        }
    } while (connect_status != 0 && timeout_us > ims_test_time_us() - begin_us);
    

    if (connect_status != 0) {
        close(_socket[actor_id]);
        return false;
    }
    
    return true;
}

//
// Send message to target actor
//
bool signal_send::send(uint32_t actor_id,
                       uint32_t timeout_us)
{
    if (_socket[actor_id] == INVALID_SOCKET) {
        if (connect(actor_id, timeout_us) == false) {
            return false;
        }
    }

    TEST_LOG(_actor,"[SEND %d]",_actor->id);
    if (::send(_socket[actor_id], (const char*)&_actor->id, 4, 0) != 4) {
        TEST_ABORT(_actor, "Failed to write to actor %d at addr %s:%d, %s",
                   actor_id, WAIT_ACTOR_IP, BASE_PORT + actor_id, strerror(errno));
    }

    return true;
}

//
// Close provided socket
//
void signal_send::close(IMS_SOCKET& socket)
{
    TEST_LOG(_actor, "[SEND CLOSE %d]",_actor->id);
    if (socket != INVALID_SOCKET) {
        shutdown(socket, 2);
#ifdef _WIN32
        closesocket(socket);
#else
        ::close(socket);
#endif
        socket = INVALID_SOCKET;
    }
}

//
// Dtor: close all sockets
//
signal_send::~signal_send()
{
    for (int id = 0; id <= MAX_NUMBER_ACTOR; id++) {
        close(_socket[id]);
    }
}

}
