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
// A429 test - actor 1
//
#include "ims_test.h"
#include "a429_tools.h"

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE_SIZE 4

#define BUS1_LABEL1_SDI     1  //01
#define BUS1_LABEL1_NUMBER  ims_test_a429_label_number_encode("024")

#define BUS1_LABEL2_SDI     3  //11
#define BUS1_LABEL2_NUMBER  ims_test_a429_label_number_encode("024")
#define BUS1_LABEL2_DEPTH   4

#define BUS2_LABEL1_SDI     1  //01
#define BUS2_LABEL1_NUMBER  ims_test_a429_label_number_encode("024")

#define BUS2_LABEL2_SDI     1 //01
#define BUS2_LABEL2_NUMBER  ims_test_a429_label_number_encode("123")
#define BUS2_LABEL2_DEPTH   5

#define MAX_DEPTH    5

#define FILLED_BY_IMS 0

#define INVALID_POINTER ((void*)42)

#define FILL_LABEL(label, value) \
    ims_test_a429_fill_label(label, FILLED_BY_IMS, FILLED_BY_IMS,  value, 1, FILLED_BY_IMS)

void fill_q_label(char* labels, uint8_t number, uint8_t sdi, uint32_t first_value, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i++)
    {
        ims_test_a429_fill_label(labels + i * 4, number, sdi, first_value + i, 1, 0);
    }
}

//
// Main
//
int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     bus1_label1;
    ims_message_t     bus1_label2;
    ims_message_t     bus2_label1;
    ims_message_t     bus2_label2;
    ims_message_t     input_queuing;
    char              payload[MAX_DEPTH * 4];
    uint32_t          count;

    actor = ims_test_init(ACTOR_ID);

    TEST_WAIT(actor, 2); // Wait actor2 ready to read before starting our tests

    // Init
    ims_context = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE, NULL, &ims_context) == ims_no_error &&
                       ims_context != (ims_node_t)INVALID_POINTER && ims_context != NULL,
                       "We can create a valid context.");

    ims_equipment = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_equipment(ims_context, "firstEquipment", &ims_equipment) == ims_no_error &&
                       ims_equipment != (ims_node_t)INVALID_POINTER && ims_equipment != NULL,
                       "We can get the first equipment.");

    ims_application = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_application(ims_equipment, "firstApplication", &ims_application) == ims_no_error &&
                       ims_application != (ims_node_t)INVALID_POINTER && ims_application != NULL,
                       "We can get the first application.");

    bus1_label1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l1", MESSAGE_SIZE, 1, ims_output, &bus1_label1) == ims_no_error &&
                       bus1_label1 != (ims_message_t)INVALID_POINTER && bus1_label1 != NULL,
                       "We can get the bus1_label1.");

    bus1_label2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l2", MESSAGE_SIZE, BUS1_LABEL2_DEPTH, ims_output, &bus1_label2) == ims_no_error &&
                       bus1_label2 != (ims_message_t)INVALID_POINTER && bus1_label2 != NULL,
                       "We can get the bus1_label2.");

    bus2_label1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus2_l1", MESSAGE_SIZE, 1, ims_output, &bus2_label1) == ims_no_error &&
                       bus2_label1 != (ims_message_t)INVALID_POINTER && bus2_label1 != NULL,
                       "We can get the bus2_label1.");

    bus2_label2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus2_l2", MESSAGE_SIZE, BUS2_LABEL2_DEPTH, ims_output, &bus2_label2) == ims_no_error &&
                       bus2_label2 != (ims_message_t)INVALID_POINTER && bus2_label2 != NULL,
                       "We can get the bus2_label2.");

    input_queuing = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "input_queuing", MESSAGE_SIZE, 5, ims_input, &input_queuing) == ims_no_error &&
                       input_queuing != (ims_message_t)INVALID_POINTER && input_queuing != NULL,
                       "We can get the input_queuing.");

    // Check invalid push
    TEST_ASSERT(actor, ims_push_queuing_message(bus1_label2, payload, 5) == ims_message_invalid_size,
                "Cannot write an invalid size.");

    TEST_ASSERT(actor, ims_push_queuing_message(input_queuing, payload, MESSAGE_SIZE) == ims_invalid_configuration,
                "Cannot push an input message.");

    TEST_ASSERT(actor, ims_push_queuing_message(bus2_label1, payload, 4) == ims_invalid_configuration,
                "Cannot push a sampling message.");

    // Check push without send
    FILL_LABEL(payload, 0x42);
    TEST_ASSERT(actor, ims_write_sampling_message(bus1_label1, payload, MESSAGE_SIZE) == ims_no_error, "bus1_label1 write.");

    fill_q_label(payload, BUS1_LABEL2_NUMBER, BUS1_LABEL2_SDI, 1, 3);
    TEST_ASSERT(actor, ims_push_queuing_message(bus1_label2, payload, 3 * MESSAGE_SIZE) == ims_no_error, "bus1_label2 write.");

    FILL_LABEL(payload, 0x22);
    TEST_ASSERT(actor, ims_write_sampling_message(bus2_label1, payload, MESSAGE_SIZE) == ims_no_error, "bus2_label1 write.");

    fill_q_label(payload, BUS2_LABEL2_NUMBER, BUS2_LABEL2_SDI, 12, 2);
    TEST_ASSERT(actor, ims_push_queuing_message(bus2_label2, payload, 2 * MESSAGE_SIZE) == ims_no_error, "bus2_label2 write.");

    TEST_SIGNAL(actor, 2);  // Ask to actor2 to check it has not received anything
    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error, "queuing_message_pending return no_error");
    TEST_ASSERT(actor, count == 3, "3 messages pending for bus1_label2.");

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error, "queuing_message_pending return no_error");
    TEST_ASSERT(actor, count == 0, "No more messages pending for bus1_label2.");

    TEST_SIGNAL(actor, 2); // Signal we have sent
    TEST_WAIT(actor, 2);

    // Severals push on same queuing label
    fill_q_label(payload, BUS1_LABEL2_NUMBER, BUS1_LABEL2_SDI, 3, 5);
    TEST_ASSERT(actor, ims_push_queuing_message(bus1_label2, payload +  0, MESSAGE_SIZE) == ims_no_error, "bus1_label2 write.");
    TEST_ASSERT(actor, ims_push_queuing_message(bus1_label2, payload +  4, MESSAGE_SIZE) == ims_no_error, "bus1_label2 write.");
    TEST_ASSERT(actor, ims_push_queuing_message(bus1_label2, payload +  8, MESSAGE_SIZE) == ims_no_error, "bus1_label2 write.");
    TEST_ASSERT(actor, ims_push_queuing_message(bus1_label2, payload + 12, MESSAGE_SIZE) == ims_no_error, "bus1_label2 write.");

    // We have reach the queue depth. We can't push more labels
    TEST_ASSERT(actor, ims_push_queuing_message(bus1_label2, payload + 16, MESSAGE_SIZE) == ims_message_queue_full, "We can't push more labels.");
    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error, "queuing_message_pending return no_error");
    TEST_ASSERT(actor, count == 4, "4 messages pending for bus1_label2.");

    // Send all
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    // After send, we can push messages again
    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error, "queuing_message_pending return no_error");
    TEST_ASSERT(actor, count == 0, "No more messages pending for bus1_label2.");

    TEST_ASSERT(actor, ims_push_queuing_message(bus1_label2, payload + 16, MESSAGE_SIZE) == ims_no_error, "bus1_label2 write again.");

    // too much data in one call
    TEST_ASSERT(actor, ims_push_queuing_message(bus1_label2, payload, MESSAGE_SIZE * 5) == ims_invalid_configuration, "Too much message in one call.");

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");
    TEST_SIGNAL(actor, 2); // Signal we have sent

    // Done
    ims_free_context(ims_context);
    return ims_test_end(actor);
}
