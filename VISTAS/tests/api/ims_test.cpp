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
 * IMS test framework. Main.
 */
#include "ims_test_actor.hh"
#include "ims_test_mc_input.hh"
#include "ims_test_tcp.hh"
#include "ims_test_vcom_client.hh"

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#ifdef _WIN32
# include <windows.h>
#endif

// The directive __USE_MINGW_ANSI_STDIO seem to don't work in C++
#ifdef _WIN32
# define sprintf __mingw_sprintf
# define vprintf __mingw_vprintf
# define printf __mingw_printf
# define vfprintf __mingw_vfprintf
# define fprintf __mingw_fprintf
#endif

void myexit(void)
{
    fprintf(stdout, __FILE__" [EXIT]\n");
}

//
// Initialize the API
//
ims_test_actor_t ims_test_init(uint32_t actor_id)
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    atexit(myexit);

    return new ims_test::actor(actor_id);
}

//
// Logging
//
void ims_test_log(ims_test_actor_t actor_base, const char* file, int line, 
                  const char* prefix, const char* message, ...)
{
    ims_test::actor* actor = static_cast<ims_test::actor*>(actor_base);

    fprintf(stderr, "[actor %d] %s:%d ", actor->id, file, line);
    if (prefix) fprintf(stderr, prefix);

    va_list vl;
    va_start(vl, message);
    vfprintf(stderr, message, vl);
    va_end(vl);

    fprintf(stderr, "\n");
#ifdef _WIN32
    fflush(stderr);
#endif
}

void ims_test_log_payload(ims_test_actor_t actor_base, const char* file, int line, 
                          const char* payload_title, const char* payload, uint32_t payload_len)
{
    ims_test::actor* actor = static_cast<ims_test::actor*>(actor_base);
    fprintf(stderr, "[actor %d] %s:%d %s: ", actor->id, file, line, payload_title);

    for (uint32_t i = 0; i < payload_len; i++) {
        fprintf(stderr, "%02hhX ", payload[i]);
    }

    fprintf(stderr, "\n");
#ifdef _WIN32
    fflush(stderr);
#endif  
}

//
// Mark the actor as failed
//
void ims_test_fail(ims_test_actor_t actor_base)
{
    ims_test::actor* actor = static_cast<ims_test::actor*>(actor_base);
    actor->success = false;
}

//
// End test (stop program)
//
int ims_test_end(ims_test_actor_t actor_base)
{
    ims_test::actor* actor = static_cast<ims_test::actor*>(actor_base);
    int result = (actor->success)? 0 : 1;
    delete actor;
    return result;
}

//
// get time
//
uint64_t ims_test_time_us()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * (uint64_t)1000000 + (uint64_t)tv.tv_usec;
}

//
// Sleep
//
void ims_test_sleep(uint32_t duration_us)
{
#ifdef _WIN32
    Sleep(duration_us / 1000);
#else
    struct timespec ts;
    ts.tv_sec = duration_us / (1000 * 1000);
    ts.tv_nsec = (duration_us % (1000 * 1000)) * 1000;
    nanosleep(&ts, NULL);
#endif
}

//
// Wait for a signal from the given actor.
// Return non-0 if the signal has been received or
// 0 if the timeout has been reached.
//
int ims_test_signal_wait(ims_test_actor_t actor_base, uint32_t actor_id, uint32_t timeout_us)
{
    ims_test::actor* actor = static_cast<ims_test::actor*>(actor_base);
    return (actor->sig_wait.wait(actor_id, timeout_us))? 1 : 0;
}

//
// Send a signal to actor_id.
// If send fail, retry until success or timeout reached.
// return non-0 if send success.
//
int ims_test_signal_send(ims_test_actor_t actor_base, uint32_t actor_id, uint32_t timeout_us)
{
    ims_test::actor* actor = static_cast<ims_test::actor*>(actor_base);
    return (actor->sig_send.send(actor_id, timeout_us))? 1 : 0;
}

// Create the socket
ims_test_mc_input_t ims_test_mc_input_create(ims_test_actor_t actor, const char* ip, uint32_t port)
{
    return new ims_test::mc_input(actor, ip, port);
}

// Read the socket
uint32_t ims_test_mc_input_receive(ims_test_mc_input_t socket_base, char* buffer, uint32_t buffer_size, uint32_t timeout_us)
{
    ims_test::mc_input* socket = static_cast<ims_test::mc_input*>(socket_base);
    return socket->receive(buffer, buffer_size, timeout_us);
}

// Free the socket
void ims_test_mc_input_free(ims_test_mc_input_t socket_base)
{
    ims_test::mc_input* socket = static_cast<ims_test::mc_input*>(socket_base);
    delete socket;
}

// Create the socket
ims_test_tcp_t ims_test_tcp_create(ims_test_actor_t actor, const char* ip, uint32_t port)
{
    return new ims_test::test_tcp(actor, ip, port);
}

// Read the socket
uint32_t ims_test_tcp_receive(ims_test_tcp_t socket_base, char* buffer, uint32_t buffer_size, uint32_t timeout_us)
{
    ims_test::test_tcp* socket = static_cast<ims_test::test_tcp*>(socket_base);
    return socket->receive(buffer, buffer_size, timeout_us);
}

// Write the socket
uint32_t ims_test_tcp_send(ims_test_tcp_t socket_base, char* buffer, uint32_t buffer_size)
{
    ims_test::test_tcp* socket = static_cast<ims_test::test_tcp*>(socket_base);
    return socket->send(buffer, buffer_size);
}

// Free the socket
void ims_test_tcp_free(ims_test_tcp_t socket_base)
{
    ims_test::test_tcp* socket = static_cast<ims_test::test_tcp*>(socket_base);
    delete socket;
}

//
// VCOM client part
// 

ims_test_vcom_client_t ims_test_vcom_client_create(ims_test_actor_t actor, const char* ip, uint32_t port)
{
    return new ims_test::vcom_client(actor, ip, port);
}

void ims_test_vcom_client_reset(ims_test_vcom_client_t vcom_client_base, uint32_t vcom_id)
{
    ims_test::vcom_client* vcom_client = static_cast<ims_test::vcom_client*>(vcom_client_base);
    vcom_client->reset(vcom_id);
}

void ims_test_vcom_client_push_int(ims_test_vcom_client_t vcom_client_base, uint32_t value)
{
    ims_test::vcom_client* vcom_client = static_cast<ims_test::vcom_client*>(vcom_client_base);
    vcom_client->push(value);
}

void ims_test_vcom_client_push_data(ims_test_vcom_client_t vcom_client_base, const char* buffer, uint32_t buffer_size)
{
    ims_test::vcom_client* vcom_client = static_cast<ims_test::vcom_client*>(vcom_client_base);
    vcom_client->push(buffer, buffer_size);
}

void ims_test_vcom_client_send(ims_test_vcom_client_t vcom_client_base)
{
    ims_test::vcom_client* vcom_client = static_cast<ims_test::vcom_client*>(vcom_client_base);
    vcom_client->send();
}

int ims_test_vcom_client_wait_answer(ims_test_vcom_client_t vcom_client_base, char* buffer, uint32_t buffer_size, uint32_t timeout_us)
{
    ims_test::vcom_client* vcom_client = static_cast<ims_test::vcom_client*>(vcom_client_base);
    return vcom_client->wait_answer(buffer, buffer_size, timeout_us);
}

void ims_test_vcom_client_free(ims_test_vcom_client_t vcom_client_base)
{
    ims_test::vcom_client* vcom_client = static_cast<ims_test::vcom_client*>(vcom_client_base);
    delete vcom_client;
}
