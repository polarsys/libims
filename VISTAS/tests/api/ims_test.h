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
 * IMS test framework. Main header.
 */
#ifndef _IMS_TEST_H_
#define _IMS_TEST_H_
#include "ims.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef ENABLE_INSTRUMENTATION
#include "ims_instrumentation.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define VISTAS_HEADER_SIZE 20

// Time constants
#define TEST_MILISECOND 1000
#define TEST_SECOND     (TEST_MILISECOND * 1000)

// =======================================================================================
// Global actor part
// =======================================================================================

// Internal actor
typedef struct ims_test_actor_internal* ims_test_actor_t;

// Initialize the API
ims_test_actor_t ims_test_init(uint32_t actor_id);

// End test and return result
int ims_test_end(ims_test_actor_t actor);

// Mark the actor test as failed
void ims_test_fail(ims_test_actor_t actor);

// =======================================================================================
// Tools
// =======================================================================================

// get time
uint64_t ims_test_time_us();

// Sleep
void ims_test_sleep(uint32_t duration_us);

// =======================================================================================
// Logging
// =======================================================================================

// Log a message.
// args are printf one.
#define TEST_LOG(actor, ...) ims_test_log(actor, __FILE__, __LINE__, NULL, __VA_ARGS__);

// Log + set test as failed
#define TEST_FAIL(actor, ...)                                           \
    do {                                                                  \
    ims_test_log(actor, __FILE__, __LINE__, "[Test fail !] ", __VA_ARGS__); \
    ims_test_fail(actor);                                               \
} while (0)

// TEST_FAIL + end test
#define TEST_ABORT(actor, ...)                                                         \
    do {                                                                                 \
    ims_test_log(actor, __FILE__, __LINE__, NULL, __VA_ARGS__);                        \
    ims_test_log(actor, __FILE__, __LINE__, NULL, "[Test fail !] "); \
    ims_test_fail(actor);                                                              \
    exit(ims_test_end(actor));                                                         \
} while (0)

// Display a payload in hexa
#define TEST_LOG_PAYLOAD(actor, payload_title, payload, payload_len)                   \
    ims_test_log_payload(actor, __FILE__, __LINE__, payload_title, payload, payload_len)

// Check
#define TEST_ASSERT(actor, test, ...)                              \
    do {                                                             \
    if (!(test)) {                                                 \
    TEST_FAIL(actor, __VA_ARGS__);                               \
} else {                                                       \
    ims_test_log(actor, __FILE__, __LINE__, "ok ", __VA_ARGS__); \
}                                                              \
} while (0)

// Check with message only on error
#define TEST_ASSERT_SILENT(actor, test, ...)                       \
    do {                                                             \
    if (!(test)) {                                                 \
    TEST_FAIL(actor, __VA_ARGS__);                               \
}                                                              \
} while (0)

// Wait for given actor up to 5 seconds
#define TEST_WAIT(actor, target_actor_id)                                      \
    do {                                                                         \
    TEST_LOG(actor, "Wait for actor %d...", target_actor_id);                  \
    TEST_ASSERT(actor,                                                         \
    ims_test_signal_wait(actor, target_actor_id, 10 * TEST_SECOND), \
    "Signal received from actor %d.", target_actor_id);            \
} while (0)

// Signal actor to target one
#define TEST_SIGNAL(actor, target_actor_id)                                    \
    do {                                                                         \
    TEST_LOG(actor, "Send signal to actor %d...", target_actor_id);            \
    TEST_ASSERT(actor,                                                         \
    ims_test_signal_send(actor, target_actor_id, 10 * TEST_SECOND), \
    "Signal sent to actor %d.", target_actor_id);                  \
} while (0)

// low level log function (prefer use macros upside)
void ims_test_log(ims_test_actor_t actor, const char* file, int line,
                  const char* prefix, const char* message, ...);

void ims_test_log_payload(ims_test_actor_t actor, const char* file, int line,
                          const char* payload_title, const char* payload, uint32_t payload_len);

// =======================================================================================
// Synchronize actors
// =======================================================================================

// Wait for a signal from the given actor.
// Return non-0 if the signal has been received or
// 0 if the timeout has been reached.
int ims_test_signal_wait(ims_test_actor_t actor, uint32_t actor_id, uint32_t timeout_us);

// Send a signal to actor_id.
// If send fail, retry until success or timeout reached.
// return non-0 if send success.
int ims_test_signal_send(ims_test_actor_t actor, uint32_t actor_id, uint32_t timeout_us);

// =======================================================================================
// Receive UDP multicast messages
// =======================================================================================

// Internal multicast input socket
typedef struct ims_test_mc_input_internal* ims_test_mc_input_t;

// Create the socket
ims_test_mc_input_t ims_test_mc_input_create(ims_test_actor_t actor, const char* ip, uint32_t port);

// Read the socket
uint32_t ims_test_mc_input_receive(ims_test_mc_input_t socket, char* buffer, uint32_t buffer_size, uint32_t timeout_us);

// Free the socket
void ims_test_mc_input_free(ims_test_mc_input_t socket);

// =======================================================================================
// Exchange TCP messages
// =======================================================================================

// Internal TCP socket
typedef struct ims_test_tcp_internal* ims_test_tcp_t;

// Create the socket
ims_test_tcp_t ims_test_tcp_create(ims_test_actor_t actor, const char* ip, uint32_t port);

// Read the socket
uint32_t ims_test_tcp_receive(ims_test_tcp_t socket, char* buffer, uint32_t buffer_size, uint32_t timeout_us);

// Write the socket
uint32_t ims_test_tcp_send(ims_test_tcp_t socket, char* buffer, uint32_t buffer_size);

// Free the socket
void ims_test_tcp_free(ims_test_tcp_t socket);

// =======================================================================================
// Sending VCOM-like messages and their ansers
// =======================================================================================
typedef struct ims_test_vcom_client_internal* ims_test_vcom_client_t;

ims_test_vcom_client_t ims_test_vcom_client_create(ims_test_actor_t actor, const char* ip, uint32_t port);

void ims_test_vcom_client_reset(ims_test_vcom_client_t vcom_client, uint32_t vcom_id);

void ims_test_vcom_client_push_int(ims_test_vcom_client_t vcom_client, uint32_t value); // Value in host encoding

void ims_test_vcom_client_push_data(ims_test_vcom_client_t vcom_client, const char* buffer, uint32_t buffer_size);

void ims_test_vcom_client_send(ims_test_vcom_client_t vcom_client);

int ims_test_vcom_client_wait_answer(ims_test_vcom_client_t vcom_client, char* buffer, uint32_t buffer_size, uint32_t timeout_us);

void ims_test_vcom_client_free(ims_test_vcom_client_t vcom_client);

#ifdef __cplusplus
}
#endif

#endif
