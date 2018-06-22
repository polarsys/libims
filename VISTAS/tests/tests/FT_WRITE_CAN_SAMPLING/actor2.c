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
// CAN test - actor 2
//
#include "ims_test.h"

#ifdef __linux
#include <arpa/inet.h>
#else
#include <winsock2.h>
#endif

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define CAN_FRAME_SIZE   14

#define BUS1_M1_SIZE 4
#define BUS1_M2_SIZE 8
#define BUS2_M1_SIZE 8
#define BUS2_M2_SIZE 4

#define BUS1_M1_ID   0x1F000000
#define BUS1_M2_ID   0x20000000
#define BUS2_M1_ID   0x10000000
#define BUS2_M2_ID   0x30000000

#define BUS1_IP             "226.23.12.4"
#define BUS1_PORT           5078

#define BUS2_IP             "226.23.12.4"
#define BUS2_PORT           5079

#define INVALID_POINTER ((void*)42)

#define CHECK_CAN(id, length, expected_content, received_payload, message_name)                                                      \
    do {                                                                                                                             \
    TEST_ASSERT(actor, *(uint32_t*)(received_payload + 10) == (id), message_name " has the good ID.");                               \
    TEST_ASSERT(actor, *(uint16_t*)(received_payload + 8) == htons(length), message_name " has the good length.");                   \
    TEST_ASSERT(actor, memcmp(received_payload + 8 - length, expected_content, length) == 0, message_name " has the good content."); \
    } while (0)

int main()
{
    char* received_payload = (char*)malloc(100); // cannot cast char[] to uint32_t* ????

    char bus1_message1_payload[BUS1_M1_SIZE] = { 0x12, 0x23, 0x36, 0x77 };
    char bus1_message2_payload[BUS1_M2_SIZE] = { 0x26, 0x78, 0x02, 0x44, 0x23, 0xAD, 0x65, 0x12 };
    char bus2_message1_payload[BUS2_M1_SIZE] = { 0x52, 0x45, 0x56, 0x32, 0xAE, 0x32, 0x74, 0x11 };

    char bus1_message1_payload2[BUS1_M1_SIZE] = { 0x44, 0x12, 0xAF, 0x44 };

    actor = ims_test_init(ACTOR_ID);

    ims_test_mc_input_t socket_bus1 = ims_test_mc_input_create(actor, BUS1_IP, BUS1_PORT);
    ims_test_mc_input_t socket_bus2 = ims_test_mc_input_create(actor, BUS2_IP, BUS2_PORT);

    TEST_SIGNAL(actor, 1); // We are ready
    TEST_WAIT(actor, 1);

    // Send has not been called, nothing will be received
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, 100, 100 * 1000) == 0,
                "No messages on bus 1");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus2, received_payload, 100, 100 * 1000) == 0,
                "No messages on bus 2");

    TEST_SIGNAL(actor, 1); // Ask actor1 to send
    TEST_WAIT(actor, 1);

    // Send has been called, we should receive messages
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, 100, 100 * 1000) == 2 * CAN_FRAME_SIZE + VISTAS_HEADER_SIZE,
                "1 message on bus 1");
    CHECK_CAN(BUS1_M1_ID, BUS1_M1_SIZE, bus1_message1_payload, received_payload + VISTAS_HEADER_SIZE, "Message1 on Bus1");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus2, received_payload, 100, 100 * 1000) == 2 * CAN_FRAME_SIZE + VISTAS_HEADER_SIZE,
                "1 message on bus 2");
    CHECK_CAN(BUS2_M1_ID, BUS2_M1_SIZE, bus2_message1_payload, received_payload + VISTAS_HEADER_SIZE, "Message1 on Bus2");

    TEST_SIGNAL(actor, 1); // Ask actor1 to write several times
    TEST_WAIT(actor, 1);

    // message1 has been wrote severals times but only the last one should have been sent
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, 100, 100 * 1000) == 2 * CAN_FRAME_SIZE + VISTAS_HEADER_SIZE,
                "2 message on bus 1");
    CHECK_CAN(BUS1_M1_ID, BUS1_M1_SIZE, bus1_message1_payload2, received_payload + VISTAS_HEADER_SIZE, "Message1 on Bus1");
    CHECK_CAN(BUS1_M2_ID, BUS1_M2_SIZE, bus1_message2_payload, received_payload + VISTAS_HEADER_SIZE + CAN_FRAME_SIZE, "Message2 on Bus1");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, 100, 100 * 1000) == 0,
                "No more messages on bus 1");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus2, received_payload, 100, 100 * 1000) == 0,
                "No messages on bus 2");

    ims_test_mc_input_free(socket_bus1);
    ims_test_mc_input_free(socket_bus2);
    free(received_payload);

    return ims_test_end(actor);
}
