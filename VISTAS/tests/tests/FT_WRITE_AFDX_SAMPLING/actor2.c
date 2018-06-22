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
#define MESSAGE1_SIZE 42
#define MESSAGE2_SIZE 21
#define MESSAGE_SIZE_MAX 42
static const char expected_payload1[MESSAGE1_SIZE] = "hello, world!";
static const char expected_payload2[MESSAGE2_SIZE] = "You too!";
static const char expected_last_payload1[MESSAGE1_SIZE] = "Ok, see you!";

#define MESSAGE1_IP    "226.23.12.4"
#define MESSAGE1_PORT  5078

#define MESSAGE2_IP    "226.23.12.4"
#define MESSAGE2_PORT  5077

#define INVALID_POINTER ((void*)42)

int main()
{
    //  uint32_t received_size = 0;
    char received_payload[100];

    actor = ims_test_init(ACTOR_ID);

    ims_test_mc_input_t socket1 = ims_test_mc_input_create(actor, MESSAGE1_IP, MESSAGE1_PORT);
    ims_test_mc_input_t socket2 = ims_test_mc_input_create(actor, MESSAGE2_IP, MESSAGE2_PORT);

    TEST_SIGNAL(actor, 1);  // We are ready
    TEST_WAIT(actor, 1);    // Wait write

    // Actor 1 has write but not sent: we will receive nothing
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket1, received_payload, MESSAGE1_SIZE, 100 * 1000) == 0,
                "No message1 received");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket2, received_payload, MESSAGE2_SIZE, 100 * 1000) == 0,
                "No message2 received");

    TEST_SIGNAL(actor, 1);  // Ask to send
    TEST_WAIT(actor, 1);    // Send done

    // Actor 1 has sent, we should receive both messages
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket1, received_payload, MESSAGE1_SIZE + VISTAS_HEADER_SIZE, 100 * 1000) == MESSAGE1_SIZE + VISTAS_HEADER_SIZE,
                "Message1 received and has the good length.");
    TEST_ASSERT(actor, strcmp(received_payload + VISTAS_HEADER_SIZE, expected_payload1) == 0, "Message1 has the expected content.");


    TEST_ASSERT(actor, ims_test_mc_input_receive(socket2, received_payload, MESSAGE2_SIZE + VISTAS_HEADER_SIZE, 100 * 1000) == MESSAGE2_SIZE + VISTAS_HEADER_SIZE,
                "Message2 received and has the good length.");
    TEST_ASSERT(actor, strcmp(received_payload + VISTAS_HEADER_SIZE, expected_payload2) == 0, "Message2 has the expected content.");

    TEST_SIGNAL(actor, 1);  // Ask to send severals time
    TEST_WAIT(actor, 1);    // Send done

    // We should receive only the last messages writed by actor1
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket1, received_payload, MESSAGE1_SIZE + VISTAS_HEADER_SIZE, 100 * 1000) == MESSAGE1_SIZE + VISTAS_HEADER_SIZE,
                "Message1 received and has the good length.");
    TEST_ASSERT(actor, strcmp(received_payload + VISTAS_HEADER_SIZE, expected_last_payload1) == 0, "Message1 has the expected content.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket1, received_payload, MESSAGE1_SIZE, 100 * 1000) == 0,
                "No more message1 available.");

    // No message2 has been sent
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket2, received_payload, MESSAGE2_SIZE, 100 * 1000) == 0,
                "No message2 received");

    ims_test_mc_input_free(socket1);
    ims_test_mc_input_free(socket2);

    return ims_test_end(actor);
}
