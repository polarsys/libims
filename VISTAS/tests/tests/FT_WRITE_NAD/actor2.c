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
// Nad test - actor 2
//
#include "ims_test.h"
#include "math.h"
#include "htonl.h"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define GRP1_MSG1_SIZE  4
#define GRP1_MSG2_SIZE  1
#define GRP2_MSG1_SIZE  8
#define GRP2_MSG2_SIZE  10

#define GRP1_MSG1_OFFSET 0
#define GRP1_MSG2_OFFSET 6
#define GRP2_MSG1_OFFSET 0
#define GRP2_MSG2_OFFSET 8

#define GROUP1_SIZE           10
#define GROUP1_IP             "226.23.14.9"
#define GROUP1_PORT           5078

#define GROUP2_SIZE           18
#define GROUP2_IP             "226.23.14.9"
#define GROUP2_PORT           5079

#define INVALID_POINTER ((void*)42)

int main()
{
    char expected_payload[50];
    char received_payload[50];

    actor = ims_test_init(ACTOR_ID);

    ims_test_mc_input_t socket_group1 = ims_test_mc_input_create(actor, GROUP1_IP, GROUP1_PORT);
    ims_test_mc_input_t socket_group2 = ims_test_mc_input_create(actor, GROUP2_IP, GROUP2_PORT);

    TEST_SIGNAL(actor, 1); // We are ready
    TEST_WAIT(actor, 1);

    // Default value has been sent
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == GROUP1_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 1.");
    memset(expected_payload, 0, GROUP1_SIZE);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE, expected_payload, GROUP1_SIZE) == 0, "group1 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == GROUP2_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 2.");
    memset(expected_payload, 0, GROUP2_SIZE);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE, expected_payload, GROUP2_SIZE) == 0, "group2 has the expected value.");

    TEST_LOG_PAYLOAD(actor, "Payload :", received_payload, GROUP2_SIZE);

    TEST_SIGNAL(actor, 1); // Signal to write
    TEST_WAIT(actor, 1);

    // Send has not been called, nothing will be received
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "No message on group 1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "No message on group 2.");

    TEST_SIGNAL(actor, 1); // Signal to send
    TEST_WAIT(actor, 1);

    // Send has been called, but not progress, nothing will be received
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "No message on group 1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "No message on group 2.");

    TEST_SIGNAL(actor, 1); // Signal to progress + send
    TEST_WAIT(actor, 1);

    // Pogress and send have been called, but just enough for cycle of group 1
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == GROUP1_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 1.");

    memset(expected_payload, 5, 10);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE + GRP1_MSG1_OFFSET, expected_payload, GRP1_MSG1_SIZE) == 0, "Grp1 Nad1 has the expected value.");

    memset(expected_payload, 6, 10);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE + GRP1_MSG2_OFFSET, expected_payload, GRP1_MSG2_SIZE) == 0, "Grp1 Nad2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "No message on group 2.");

    TEST_SIGNAL(actor, 1); // Signal to progress + send
    TEST_WAIT(actor, 1);

    // Pogress and send have been called, but just enough for cycle of group 2
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "No message on group 1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == GROUP2_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 2.");
    memset(expected_payload, 7, 10);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE + GRP2_MSG1_OFFSET, expected_payload, GRP2_MSG1_SIZE) == 0, "Grp2 Nad1 has the expected value.");

    memset(expected_payload, 8, 10);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE + GRP2_MSG2_OFFSET, expected_payload, GRP2_MSG2_SIZE) == 0, "Grp2 Nad2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 2.");

    TEST_SIGNAL(actor, 1); // Signal to progress + send
    TEST_WAIT(actor, 1);

    // Nad are send periodically, actor 1 has just called progress + send, not write, and wil will receive data
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == GROUP1_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 1.");

    memset(expected_payload, 5, 10);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE + GRP1_MSG1_OFFSET, expected_payload, GRP1_MSG1_SIZE) == 0, "Grp1 Nad1 has the expected value.");

    memset(expected_payload, 6, 10);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE + GRP1_MSG2_OFFSET, expected_payload, GRP1_MSG2_SIZE) == 0, "Grp1 Nad2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == GROUP2_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 2.");

    memset(expected_payload, 7, 10);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE + GRP2_MSG1_OFFSET, expected_payload, GRP2_MSG1_SIZE) == 0, "Grp2 Nad1 has the expected value.");

    memset(expected_payload, 8, 10);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE + GRP2_MSG2_OFFSET, expected_payload, GRP2_MSG2_SIZE) == 0, "Grp2 Nad2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 2.");

    TEST_SIGNAL(actor, 1); // signal to write several times + progress + send
    TEST_WAIT(actor, 1);

    // Only the last value sent will be received
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == GROUP1_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 1.");

    memset(expected_payload, 16, 10);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE + GRP1_MSG1_OFFSET, expected_payload, GRP1_MSG1_SIZE) == 0, "Grp1 Nad1 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == GROUP2_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 2.");

    memset(expected_payload, 11, 10);
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE + GRP2_MSG1_OFFSET, expected_payload, GRP2_MSG1_SIZE) == 0, "Grp2 Nad1 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 2.");

    ims_test_mc_input_free(socket_group1);
    ims_test_mc_input_free(socket_group2);

    return ims_test_end(actor);
}
