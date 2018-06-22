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

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE_SIZE 4

#define MAX_DEPTH           5

#define BUS1_IP             "226.23.12.4"
#define BUS1_PORT           5078

#define BUS2_IP             "226.23.12.4"
#define BUS2_PORT           5079

#define INVALID_POINTER ((void*)42)

#define CHECK_LABEL_PAYLOAD(label, name, value)                                      \
    do {                                                                             \
    ims_test_a429_exploded_label exploded;                                           \
    ims_test_a429_explode_label(label, &exploded, 0);                                \
    TEST_ASSERT(actor, exploded.payload == value, name " has the expected value.");  \
    } while (0)

int main()
{
    char received_payload[VISTAS_HEADER_SIZE + MESSAGE_SIZE * MAX_DEPTH];

    actor = ims_test_init(ACTOR_ID);

    ims_test_mc_input_t socket_bus1 = ims_test_mc_input_create(actor, BUS1_IP, BUS1_PORT);
    ims_test_mc_input_t socket_bus2 = ims_test_mc_input_create(actor, BUS2_IP, BUS2_PORT);

    TEST_SIGNAL(actor, 1); // We are ready
    TEST_WAIT(actor, 1);

    // Actor1 doesn't has send anything
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, MESSAGE_SIZE * 4, 100 * 1000) == 0,
                "No message available on bus1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus2, received_payload, MESSAGE_SIZE * 4, 100 * 1000) == 0,
                "No message available on bus2.");

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // Both buses filled and sent
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, VISTAS_HEADER_SIZE + MESSAGE_SIZE * 5, 100 * 1000) == MESSAGE_SIZE * 4 + VISTAS_HEADER_SIZE,
                "Expected len on bus1.");
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE, "bus1 label1", 0x42);
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE +  4, "bus1 queuing1", 1);
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE +  8, "bus1 queuing2", 2);
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE + 12, "bus1 queuing3", 3);
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload + VISTAS_HEADER_SIZE, MESSAGE_SIZE * 5, 100 * 1000) == 0,
                "No more data on bus1.");

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus2, received_payload, VISTAS_HEADER_SIZE + MESSAGE_SIZE * 5, 100 * 1000) == MESSAGE_SIZE * 3 + VISTAS_HEADER_SIZE,
                "Expected len on bus2.");
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE, "bus2 label1", 0x22);
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE +  4, "bus2 queuing1", 12);
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE +  8, "bus2 queuing2", 13);
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus2, received_payload, MESSAGE_SIZE * 5, 100 * 1000) == 0,
                "No more data on bus2.");

    TEST_SIGNAL(actor, 1); // we have read
    TEST_WAIT(actor, 1);

    // First message
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, VISTAS_HEADER_SIZE + MESSAGE_SIZE * 5, 100 * 1000) == MESSAGE_SIZE * 4 + VISTAS_HEADER_SIZE,
                "Expected len on bus1.");
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE +  0, "bus1 queuing1", 3);
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE +  4, "bus1 queuing2", 4);
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE +  8, "bus1 queuing3", 5);
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE + 12, "bus1 queuing3", 6);

    // Second message
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, VISTAS_HEADER_SIZE + MESSAGE_SIZE * 5, 100 * 1000) == MESSAGE_SIZE + VISTAS_HEADER_SIZE,
                "Expected len on bus1.");
    CHECK_LABEL_PAYLOAD(received_payload + VISTAS_HEADER_SIZE, "bus1 queuing1", 7);

    // No more messages
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_bus1, received_payload, VISTAS_HEADER_SIZE + MESSAGE_SIZE * 5, 100 * 1000) == 0,
                "No more data on bus1.");

    ims_test_mc_input_free(socket_bus1);
    ims_test_mc_input_free(socket_bus2);

    return ims_test_end(actor);
}
