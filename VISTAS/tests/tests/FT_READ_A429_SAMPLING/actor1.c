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

#define BUS1_LABEL3_SDI     4  //XX
#define BUS1_LABEL3_NUMBER  ims_test_a429_label_number_encode("024")

#define BUS2_LABEL1_SDI     1  //01
#define BUS2_LABEL1_NUMBER  ims_test_a429_label_number_encode("024")

#define BUS2_LABEL2_SDI     SDI_IS_PAYLOAD
#define BUS2_LABEL2_NUMBER  ims_test_a429_label_number_encode("123")

#define FILLED_BY_IMS 0

#define INVALID_POINTER ((void*)42)

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
	ims_message_t     bus1_label3;
    ims_message_t     bus2_label1;
    ims_message_t     bus2_label2;

    // Internal data
    const uint32_t data_internal_max_size = 100;
    char data_internal[data_internal_max_size];
    uint32_t data_internal_size;

    char              bus1_label1_payload[4];
    char              bus2_label1_payload[4];
    char              bus1_label2_payload[4];
    char              bus2_label2_payload[4];

    // Label Number, SDI and parity are filled by IMS
    ims_test_a429_fill_label(bus1_label1_payload, FILLED_BY_IMS, FILLED_BY_IMS,  0x7FFFF, 1, FILLED_BY_IMS);
    ims_test_a429_fill_label(bus2_label1_payload, FILLED_BY_IMS, FILLED_BY_IMS,  0x00F00, 2, FILLED_BY_IMS);
    ims_test_a429_fill_label(bus1_label2_payload, FILLED_BY_IMS, FILLED_BY_IMS,  0x7F005, 3, FILLED_BY_IMS);
    ims_test_a429_fill_label(bus2_label2_payload, FILLED_BY_IMS, SDI_IS_PAYLOAD, 0x10F0F1, 1, FILLED_BY_IMS);  // no SDI for this label

    // Some other payload for bus1 label1
    char              bus1_label1_payload2[4];
    char              bus1_label1_payload3[4];
    ims_test_a429_fill_label(bus1_label1_payload2, FILLED_BY_IMS, FILLED_BY_IMS,  0x1F0F0, 2, FILLED_BY_IMS);
    ims_test_a429_fill_label(bus1_label1_payload3, FILLED_BY_IMS, FILLED_BY_IMS,  0x12345, 3, FILLED_BY_IMS);

    actor = ims_test_init(ACTOR_ID);

    TEST_WAIT(actor, 2); // Wait actor2 ready to read before starting our tests

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
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l2", MESSAGE_SIZE, 1, ims_output, &bus1_label2) == ims_no_error &&
                       bus1_label2 != (ims_message_t)INVALID_POINTER && bus1_label2 != NULL,
                       "We can get the bus1_label2.");

	bus1_label3 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l3", MESSAGE_SIZE, 1, ims_output, &bus1_label3) == ims_no_error &&
                       bus1_label3 != (ims_message_t)INVALID_POINTER && bus1_label3 != NULL,
                       "We can get the bus1_label3.");

    bus2_label1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus2_l1", MESSAGE_SIZE, 1, ims_output, &bus2_label1) == ims_no_error &&
                       bus2_label1 != (ims_message_t)INVALID_POINTER && bus2_label1 != NULL,
                       "We can get the bus2_label1.");

    bus2_label2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus2_l2", MESSAGE_SIZE, 1, ims_output, &bus2_label2) == ims_no_error &&
                       bus2_label2 != (ims_message_t)INVALID_POINTER && bus2_label2 != NULL,
                       "We can get the bus2_label2.");

    // Fill all messages
    TEST_ASSERT(actor, ims_write_sampling_message(bus1_label1, bus1_label1_payload, MESSAGE_SIZE) == ims_no_error,
                "bus1_label1 wrote.");
    TEST_ASSERT(actor, ims_write_sampling_message(bus1_label2, bus1_label2_payload, MESSAGE_SIZE) == ims_no_error,
                "bus1_label2 wrote.");
    TEST_ASSERT(actor, ims_write_sampling_message(bus2_label1, bus2_label1_payload, MESSAGE_SIZE) == ims_no_error,
                "bus2_label1 wrote.");
    TEST_ASSERT(actor, ims_write_sampling_message(bus2_label2, bus2_label2_payload, MESSAGE_SIZE) == ims_no_error,
                "bus2_label2 wrote.");

    // Check internal data
#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal,0,data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus1_label1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus1_label1 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved bus1_label1 internal data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, bus1_label1_payload, MESSAGE_SIZE) == 0, "Retrieved bus1_label1 internal data is good.");
    memset(data_internal,0,data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus1_label2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus1_label2 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved bus1_label2 internal data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, bus1_label2_payload, MESSAGE_SIZE) == 0, "Retrieved bus1_label2 internal data is good.");
	memset(data_internal,0,data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus1_label3, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus1_label3 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved bus1_label3 internal data has the right size.");
    memset(data_internal,0,data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus2_label1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus2_label1 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved bus2_label1 internal data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, bus2_label1_payload, MESSAGE_SIZE) == 0, "Retrieved bus2_label1 internal data is good.");
    memset(data_internal,0,data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus2_label2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus2_label2 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved bus2_label2 internal data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, bus2_label2_payload, MESSAGE_SIZE) == 0, "Retrieved bus2_label2 internal data is good.");
#endif

    // Send
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal messages filled, reset and sent
    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_write_sampling_message(bus1_label1, bus1_label1_payload, MESSAGE_SIZE) == ims_no_error,
                "bus1_label1 wrote.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_ASSERT(actor, ims_write_sampling_message(bus1_label1, bus1_label1_payload2, MESSAGE_SIZE) == ims_no_error,
                "bus1_label1 wrote.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_ASSERT(actor, ims_write_sampling_message(bus1_label1, bus1_label1_payload3, MESSAGE_SIZE) == ims_no_error,
                "bus1_label1 wrote.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal messages filled, reset and sent
    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_write_sampling_message(bus1_label1, bus1_label1_payload, MESSAGE_SIZE) == ims_no_error,
                "bus1_label1 wrote.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal messages filled, reset and sent

    // Done
    ims_free_context(ims_context);
    return ims_test_end(actor);
}
