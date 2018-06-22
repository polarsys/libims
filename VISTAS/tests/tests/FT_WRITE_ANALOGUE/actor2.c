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
// Analogue test - actor 2
//
#include "ims_test.h"
#include "math.h"
#include "htonl.h"

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"
#define IMS_INIT_FILE        "config/actor2/init.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE_SIZE (sizeof(float))

static const float GRP1_SIG1_ACTOR1_DEFAULT = 11.2;
static const float GRP1_SIG2_ACTOR1_DEFAULT = 12.0;
static const float GRP2_SIG1_ACTOR1_DEFAULT = 0;
static const float GRP2_SIG2_ACTOR1_DEFAULT = 0;

#define GRP1_SIG1_CONVERT(value) (value *  1.5 +   0.0)
#define GRP1_SIG2_CONVERT(value) (value *  2.0 +   3.5)
#define GRP2_SIG1_CONVERT(value) (value *  0.5 -   4.2)
#define GRP2_SIG2_CONVERT(value) (value * -1.5 + 200.0)

#define PAYLOAD_CONVERT(p) htonf(*(float*)(p))

static const float EPSILON = 0.00001;
static int fequalto(float a, float b)
{
    return fabs(a - b) < EPSILON;
}

#define GROUP1_SIZE           2 * MESSAGE_SIZE
#define GROUP1_IP             "226.23.12.4"
#define GROUP1_PORT           5078

#define GROUP2_SIZE           2 * MESSAGE_SIZE
#define GROUP2_IP             "226.23.12.4"
#define GROUP2_PORT           5079

#define INVALID_POINTER ((void*)42)

int main()
{
    char* received_payload = (char*)malloc(50); // I cannot alias float* to char[4] ????

    actor = ims_test_init(ACTOR_ID);

    ims_test_mc_input_t socket_group1 = ims_test_mc_input_create(actor, GROUP1_IP, GROUP1_PORT);
    ims_test_mc_input_t socket_group2 = ims_test_mc_input_create(actor, GROUP2_IP, GROUP2_PORT);

    TEST_SIGNAL(actor, 1); // We are ready
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == GROUP1_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 1.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE), GRP1_SIG1_CONVERT(GRP1_SIG1_ACTOR1_DEFAULT)), "Grp1 Analogue1 has the expected value.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE + 4), GRP1_SIG2_CONVERT(GRP1_SIG2_ACTOR1_DEFAULT)), "Grp1 Analogue2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == GROUP2_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 2.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE), GRP2_SIG1_CONVERT(GRP2_SIG1_ACTOR1_DEFAULT)), "Grp2 Analogue1 has the expected value.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE + 4), GRP2_SIG2_CONVERT(GRP2_SIG2_ACTOR1_DEFAULT)), "Grp2 Analogue2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 2.");

    TEST_SIGNAL(actor, 1);
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
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE), GRP1_SIG1_CONVERT(5)), "Grp1 Analogue1 has the expected value.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE + 4), GRP1_SIG2_CONVERT(6)), "Grp1 Analogue2 has the expected value.");

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
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE), GRP2_SIG1_CONVERT(7)), "Grp2 Analogue1 has the expected value.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE + 4), GRP2_SIG2_CONVERT(8)), "Grp2 Analogue2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 2.");

    TEST_SIGNAL(actor, 1); // Signal to progress + send
    TEST_WAIT(actor, 1);

    // Analogue are send periodically, actor 1 has just called progress + send, not write, and wil will receive data
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == GROUP1_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 1.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE), GRP1_SIG1_CONVERT(5)), "Grp1 Analogue1 has the expected value.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE + 4), GRP1_SIG2_CONVERT(6)), "Grp1 Analogue2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == GROUP2_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 2.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE), GRP2_SIG1_CONVERT(7)), "Grp2 Analogue1 has the expected value.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE + 4), GRP2_SIG2_CONVERT(8)), "Grp2 Analogue2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 2.");

    TEST_SIGNAL(actor, 1); // signal to write several times + progress + send
    TEST_WAIT(actor, 1);

    // Only the last value sent will be received
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == GROUP1_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 1.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE), GRP1_SIG1_CONVERT(16)), "Grp1 Analogue1 has the expected value.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE + 4), GRP1_SIG2_CONVERT(6)), "Grp1 Analogue2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group1, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == GROUP2_SIZE + VISTAS_HEADER_SIZE,
                "We have received the group 2.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE), GRP2_SIG1_CONVERT(11.9)), "Grp2 Analogue1 has the expected value.");
    TEST_ASSERT(actor, fequalto(PAYLOAD_CONVERT(received_payload + VISTAS_HEADER_SIZE + 4), GRP2_SIG2_CONVERT(8)), "Grp2 Analogue2 has the expected value.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_group2, received_payload, 50, 1000 * 100) == 0,
                "No more message on group 2.");

    ims_test_mc_input_free(socket_group1);
    ims_test_mc_input_free(socket_group2);
    free(received_payload);

    return ims_test_end(actor);
}
