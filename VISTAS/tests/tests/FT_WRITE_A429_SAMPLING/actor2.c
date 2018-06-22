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
// A429 test - actor 2
//
#include "ims_test.h"
#include "a429_tools.h"

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE_SIZE 4

#define BUS1_LABEL1_SDI     1  //01
#define BUS1_LABEL1_NUMBER  ims_test_a429_label_number_encode("024")

#define BUS1_LABEL2_SDI     3  //11
#define BUS1_LABEL2_NUMBER  ims_test_a429_label_number_encode("024")

#define BUS2_LABEL1_SDI     1  //01
#define BUS2_LABEL1_NUMBER  ims_test_a429_label_number_encode("024")

#define BUS2_LABEL2_SDI     SDI_IS_PAYLOAD
#define BUS2_LABEL2_NUMBER  ims_test_a429_label_number_encode("123")

#define BUS1_IP             "226.23.12.4"
#define BUS1_PORT           5078

#define BUS2_IP             "226.23.12.4"
#define BUS2_PORT           5079

#define INVALID_POINTER ((void*)42)

int main()
{
    char              received_payload[VISTAS_HEADER_SIZE + MESSAGE_SIZE * 4];

    char              bus1_label1_payload[4];
    char              bus2_label1_payload[4];
    char              bus1_label2_payload[4];
    char              bus2_label2_payload[4];
    ims_test_a429_fill_label(bus1_label1_payload, BUS1_LABEL1_NUMBER, BUS1_LABEL1_SDI,  0x7FFFF, 1, 0);
    ims_test_a429_fill_label(bus2_label1_payload, BUS2_LABEL1_NUMBER, BUS2_LABEL1_SDI,  0x00F00, 2, 1);
    ims_test_a429_fill_label(bus1_label2_payload, BUS1_LABEL2_NUMBER, BUS1_LABEL2_SDI,  0x7F005, 3, 0);
    ims_test_a429_fill_label(bus2_label2_payload, BUS2_LABEL2_NUMBER, BUS2_LABEL2_SDI, 0x10F0F1, 1, 0);  // no SDI for this label

    // Some other payload for bus1 label1
    char              bus1_label1_payload2[4];
    char              bus1_label1_payload3[4];
    ims_test_a429_fill_label(bus1_label1_payload2, BUS1_LABEL1_NUMBER, BUS1_LABEL1_SDI,  0x1F0F0, 2, 0);
    ims_test_a429_fill_label(bus1_label1_payload3, BUS1_LABEL1_NUMBER, BUS1_LABEL1_SDI,  0x12345, 3, 1);

    actor = ims_test_init(ACTOR_ID);

    ims_test_mc_input_t socket_bus1 = ims_test_mc_input_create(actor, BUS1_IP, BUS1_PORT);
    ims_test_mc_input_t socket_bus2 = ims_test_mc_input_create(actor, BUS2_IP, BUS2_PORT);

    TEST_SIGNAL(actor, 1); // We are ready
    TEST_WAIT(actor, 1);

    // Send has not been called, nothing will be received
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, MESSAGE_SIZE * 4, 100 * 1000) == 0,
                "No message available on bus1");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus2, received_payload, MESSAGE_SIZE * 4, 100 * 1000) == 0,
                "No message available on bus2");

    TEST_SIGNAL(actor, 1); // Ask actor1 to send
    TEST_WAIT(actor, 1);

    // Send has been called, we should receive messages
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, VISTAS_HEADER_SIZE + MESSAGE_SIZE * 4, 100 * 1000) == MESSAGE_SIZE + VISTAS_HEADER_SIZE,
                "1 message on bus 1");
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE, bus1_label1_payload, MESSAGE_SIZE) == 0, "Bus 1 content is good.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus2, received_payload, VISTAS_HEADER_SIZE + MESSAGE_SIZE * 4, 100 * 1000) == MESSAGE_SIZE + VISTAS_HEADER_SIZE,
                "1 message on bus 2");
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE, bus2_label1_payload, MESSAGE_SIZE) == 0, "Bus 2 content is good.");

    TEST_SIGNAL(actor, 1); // Ask actor1 to write several times
    TEST_WAIT(actor, 1);

    // label1 has been write severals times but only the last one should have been sent
    char bus1_payload[2 * MESSAGE_SIZE];
    memcpy(bus1_payload, bus1_label1_payload2, MESSAGE_SIZE);
    memcpy(bus1_payload + MESSAGE_SIZE, bus1_label2_payload, MESSAGE_SIZE);
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, VISTAS_HEADER_SIZE + MESSAGE_SIZE * 4, 100 * 1000) == 2 * MESSAGE_SIZE + VISTAS_HEADER_SIZE,
                "2 message on bus 1");
    TEST_ASSERT(actor, memcmp(received_payload + VISTAS_HEADER_SIZE, bus1_payload, 2 * MESSAGE_SIZE) == 0, "Bus 1 content is good.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, VISTAS_HEADER_SIZE + MESSAGE_SIZE * 4, 100 * 1000) == 0,
                "No more messages on bus 1");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus2, received_payload, VISTAS_HEADER_SIZE + MESSAGE_SIZE * 4, 100 * 1000) == 0,
                "No message on bus 2");

    ims_test_mc_input_free(socket_bus1);
    ims_test_mc_input_free(socket_bus2);

    return ims_test_end(actor);
}
