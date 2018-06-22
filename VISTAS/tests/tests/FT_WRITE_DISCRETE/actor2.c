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
// Discrete test - actor 2
//
#include "ims_test.h"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define GRP1_SIG1_TRUE     1
#define GRP1_SIG1_FALSE    0

#define GRP1_SIG2_TRUE     2
#define GRP1_SIG2_FALSE    0

#define GRP2_SIG1_TRUE     0
#define GRP2_SIG1_FALSE    3

#define GRP2_SIG2_TRUE     4
#define GRP2_SIG2_FALSE    0

#define GRP1_SIG1_DEFAULT  GRP1_SIG1_FALSE
#define GRP1_SIG2_DEFAULT  GRP1_SIG2_TRUE
#define GRP2_SIG1_DEFAULT  GRP2_SIG1_TRUE
#define GRP2_SIG2_DEFAULT  GRP2_SIG2_FALSE

#define GROUP1_IP             "226.23.12.4"
#define GROUP1_PORT           5078
#define GROUP1_SIZE           2

#define GROUP2_IP             "226.23.12.4"
#define GROUP2_PORT           5079
#define GROUP2_SIZE           2

#define INVALID_POINTER ((void*)42)

int main()
{
    char      received_payload[50];

    actor = ims_test_init(ACTOR_ID);

    ims_test_mc_input_t socket_group1 = ims_test_mc_input_create(actor, GROUP1_IP, GROUP1_PORT);
    ims_test_mc_input_t socket_group2 = ims_test_mc_input_create(actor, GROUP2_IP, GROUP2_PORT);

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // We should have receive the default value
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == VISTAS_HEADER_SIZE + GROUP1_SIZE,
                "We have received the group 1.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 0] == GRP1_SIG1_DEFAULT, "GRP1_SIG1 has the expected value.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 1] == GRP1_SIG2_DEFAULT, "GRP1_SIG2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == VISTAS_HEADER_SIZE + GROUP2_SIZE,
                "We have received the group 2.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 0] == GRP2_SIG1_DEFAULT, "GRP2_SIG1 has the expected value.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 1] == GRP2_SIG2_DEFAULT, "GRP2_SIG2 has the expected value.");

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // Actor 1 has write but don't have called send, we should d'ont receive anything
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "Group 1 is empty.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "Group 2 is empty.");

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    //Actor 1 has sent, only group 1 has change, so we should receive only group 1
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == VISTAS_HEADER_SIZE + GROUP1_SIZE,
                "We have received the group 1.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 0] == GRP1_SIG1_TRUE, "GRP1_SIG1 has the expected value.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 1] == GRP1_SIG2_TRUE, "GRP1_SIG2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "Group 2 is empty.");

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // Actor1 call progress enought for group1, but not for group 2
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == VISTAS_HEADER_SIZE + GROUP1_SIZE,
                "We have received the group 1.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 0] == GRP1_SIG1_TRUE, "GRP1_SIG1 has the expected value.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 1] == GRP1_SIG2_TRUE, "GRP1_SIG2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "Group 2 is empty.");

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // Actor1 call progress enought for group2, but not for an other group1 cycle
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "Group 1 is empty.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == VISTAS_HEADER_SIZE + GROUP2_SIZE,
                "We have received the group 2.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 0] == GRP2_SIG1_DEFAULT, "GRP2_SIG1 has the expected value.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 1] == GRP2_SIG2_DEFAULT, "GRP2_SIG2 has the expected value.");

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // Actor1 write several times but we should receive only the last one
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == VISTAS_HEADER_SIZE + GROUP1_SIZE,
                "We have received the group 1.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 0] == GRP1_SIG1_TRUE, "GRP1_SIG1 has the expected value.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 1] == GRP1_SIG2_TRUE, "GRP1_SIG2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "Group 1 is empty.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == VISTAS_HEADER_SIZE + GROUP2_SIZE,
                "We have received the group 2.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 0] == GRP2_SIG1_FALSE, "GRP2_SIG1 has the expected value.");
    TEST_ASSERT(actor, received_payload[VISTAS_HEADER_SIZE + 1] == GRP2_SIG2_DEFAULT, "GRP2_SIG2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "Group 2 is empty.");

    ims_test_mc_input_free(socket_group1);
    ims_test_mc_input_free(socket_group2);

    return ims_test_end(actor);
}
