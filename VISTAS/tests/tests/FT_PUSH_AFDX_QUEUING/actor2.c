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
// AFDX test - actor 2
//
#include "ims_test.h"

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE1_IP    "226.23.12.4"
#define MESSAGE1_PORT  5078

#define MESSAGE2_IP    "226.23.12.4"
#define MESSAGE2_PORT  5077

#define INVALID_POINTER ((void*)42)

int main()
{
    char received_payload[100];
    const char* payload;

    actor = ims_test_init(ACTOR_ID);

    ims_test_mc_input_t socket_message1 = ims_test_mc_input_create(actor, MESSAGE1_IP, MESSAGE1_PORT);
    ims_test_mc_input_t socket_message2 = ims_test_mc_input_create(actor, MESSAGE2_IP, MESSAGE2_PORT);

    TEST_SIGNAL(actor, 1); // We are ready
    TEST_WAIT(actor, 1);

    // Actor1 doesn't has send anything
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_message1, received_payload, 100, 100 * 1000) == 0,
                "No message available on message1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_message2, received_payload, 100, 100 * 1000) == 0,
                "No message available on message2.");

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    payload = "Hello, world!";
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_message1, received_payload, 100, 100 * 1000) == strlen(payload) + VISTAS_HEADER_SIZE,
                "Expected len received for message1.");
    TEST_ASSERT(actor, strncmp(payload, received_payload + VISTAS_HEADER_SIZE, strlen(payload)) == 0, "We have received the expected payload.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_message1, received_payload, 100, 100 * 1000) == 0,
                "No more message available on message1.");

    payload = "Hello, 2!";
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_message2, received_payload, 100, 100 * 1000) == strlen(payload) + VISTAS_HEADER_SIZE,
                "Expected len received for message2.");
    TEST_ASSERT(actor, strncmp(payload, received_payload + VISTAS_HEADER_SIZE, strlen(payload)) == 0, "We have received the expected payload.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_message2, received_payload, 100, 100 * 1000) == 0,
                "No more message available on message2.");

    TEST_SIGNAL(actor, 1); // We are ready
    TEST_WAIT(actor, 1);

    // Actor 1 have sent severals messages
    payload = "first data";
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_message1, received_payload, 100, 100 * 1000) == strlen(payload) + VISTAS_HEADER_SIZE,
                "Expected len received for message1.");
    TEST_ASSERT(actor, strncmp(payload, received_payload + VISTAS_HEADER_SIZE, strlen(payload)) == 0, "We have received the expected payload.");

    payload = "Have fun with this data!";
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_message1, received_payload, 100, 100 * 1000) == strlen(payload) + VISTAS_HEADER_SIZE,
                "Expected len received for message1.");
    TEST_ASSERT(actor, strncmp(payload, received_payload + VISTAS_HEADER_SIZE, strlen(payload)) == 0, "We have received the expected payload.");

    payload = "Ok for this one.";
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_message1, received_payload, 100, 100 * 1000) == strlen(payload) + VISTAS_HEADER_SIZE,
                "Expected len received for message1.");
    TEST_ASSERT(actor, strncmp(payload, received_payload + VISTAS_HEADER_SIZE, strlen(payload)) == 0, "We have received the expected payload.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_message1, received_payload, 100, 100 * 1000) == 0,
                "No more message available on message1.");

    ims_test_mc_input_free(socket_message1);
    ims_test_mc_input_free(socket_message2);

    return ims_test_end(actor);
}
