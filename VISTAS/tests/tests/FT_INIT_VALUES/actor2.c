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
#include "a429_tools.h"

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"
#define IMS_INIT_FILE        "config/actor2/init.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE_AFDX_SIZE        42
#define MESSAGE_SMALL_AFDX_SIZE  10
#define MESSAGE_AFDX_INIT_SIZE    4
#define RECEIVE_MAX_SIZE  10000

#define MESSAGE_A429_SIZE 4
#define BUS1_LABEL1_SDI     1  //01
#define BUS1_LABEL1_NUMBER  ims_test_a429_label_number_encode("024")

#define BUS1_LABEL2_SDI     3  //11
#define BUS1_LABEL2_NUMBER  ims_test_a429_label_number_encode("024")

#define BUS1_M1_SIZE 4
#define BUS1_M2_SIZE 8

static const char expected_payload1[MESSAGE_AFDX_SIZE] = "hello, world!";
static const char expected_init_payload1[MESSAGE_AFDX_INIT_SIZE] = {0xDE, 0xAD, 0xBE, 0xEF};
static const char expected_init_payload_small[MESSAGE_SMALL_AFDX_SIZE] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99};

#define INVALID_POINTER ((void*)42)

int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     ims_message_afdx;
    ims_message_t     ims_message_afdx_small;
    ims_message_t     bus1_label1;
    ims_message_t     bus1_label2;
    ims_message_t     bus1_message1;
    ims_message_t     bus1_message2;
    ims_validity_t    validity;
    uint32_t          received_size;
    ims_return_code_t ims_retcode;
    char              received_payload[RECEIVE_MAX_SIZE];
    char              bus1_label1_payload[4];
    char              bus1_init_label1_payload[4];
    char              bus1_init_label2_payload[4];
    char              bus1_message1_payload[BUS1_M1_SIZE] = { 0x12, 0x23, 0x36, 0x77 };
    char              bus1_init_message1_payload[BUS1_M1_SIZE] = { 0xAA, 0xBB, 0xCC, 0xDD };
    char              bus1_init_message2_payload[BUS1_M2_SIZE] = { 0x00, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };

    ims_test_a429_fill_label(bus1_label1_payload, BUS1_LABEL1_NUMBER, BUS1_LABEL1_SDI,  0x7FFFF, 1, 0);
    ims_test_a429_fill_label(bus1_init_label1_payload, BUS1_LABEL1_NUMBER, BUS1_LABEL1_SDI, 42, 0, 1);
    ims_test_a429_fill_label(bus1_init_label2_payload, BUS1_LABEL2_NUMBER, BUS1_LABEL2_SDI, 43, 0, 1);

    actor = ims_test_init(ACTOR_ID);

    ims_create_context_parameter_t create_parameter = IMS_CREATE_CONTEXT_INITIALIZER;
    create_parameter.init_file_path = IMS_INIT_FILE;

    ims_context = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE, &create_parameter, &ims_context) == ims_no_error &&
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

    ims_message_afdx = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalName", MESSAGE_AFDX_SIZE, 1, ims_input, &ims_message_afdx) == ims_no_error &&
                       ims_message_afdx != (ims_message_t)INVALID_POINTER && ims_message_afdx != NULL,
                       "We can get the first AFDX message.");

    ims_message_afdx_small = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "smallAFDXLocalName", MESSAGE_SMALL_AFDX_SIZE, 1, ims_input, &ims_message_afdx_small) == ims_no_error &&
                       ims_message_afdx_small != (ims_message_t)INVALID_POINTER && ims_message_afdx_small != NULL,
                       "We can get the small AFDX message.");

    bus1_label1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l1", MESSAGE_A429_SIZE, 1, ims_input, &bus1_label1) == ims_no_error &&
                       bus1_label1 != (ims_message_t)INVALID_POINTER && bus1_label1 != NULL,
                       "We can get the bus1_label1.");

    bus1_label2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l2", MESSAGE_A429_SIZE, 1, ims_input, &bus1_label2) == ims_no_error &&
                       bus1_label2 != (ims_message_t)INVALID_POINTER && bus1_label2 != NULL,
                       "We can get the bus1_label2.");

    bus1_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_can, "bus1_m1", BUS1_M1_SIZE, 1, ims_input, &bus1_message1) == ims_no_error &&
                       bus1_message1 != (ims_message_t)INVALID_POINTER && bus1_message1 != NULL,
                       "We can get the bus1_message1.");

    bus1_message2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_can, "bus1_m2", BUS1_M2_SIZE, 1, ims_input, &bus1_message2) == ims_no_error &&
                       bus1_message2 != (ims_message_t)INVALID_POINTER && bus1_message2 != NULL,
                       "We can get the bus1_message2.");

    // Check init value
    TEST_ASSERT(actor, ims_read_sampling_message(ims_message_afdx, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_AFDX_INIT_SIZE, "Message1 has good size.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Message1 has never been received.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_init_payload1, MESSAGE_AFDX_INIT_SIZE) == 0, "Message1 has the expected content.");


    TEST_ASSERT(actor, ims_read_sampling_message(ims_message_afdx_small, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SMALL_AFDX_SIZE, "Small AFDX has good size.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Message1 has never been received.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_init_payload_small, MESSAGE_SMALL_AFDX_SIZE) == 0, "Message1 has the expected content.");

    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized && ims_retcode == ims_no_error, "bus1_label1 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_A429_SIZE, "bus1_label1 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_init_label1_payload, MESSAGE_A429_SIZE) == 0, "bus1_label1 content is good.");

    ims_retcode = ims_read_sampling_message(bus1_label2, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized && ims_retcode == ims_no_error, "bus1_label2 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_A429_SIZE, "bus1_label2 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_init_label2_payload, MESSAGE_A429_SIZE) == 0, "bus1_label2 content is good.");

    TEST_ASSERT(actor, ims_read_sampling_message(bus1_message1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message that has never be sent return no_error.");
    TEST_ASSERT(actor, received_size == BUS1_M1_SIZE, "Message1 is empty.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Message1 has never been received.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_init_message1_payload, BUS1_M1_SIZE) == 0, "bus1_message1 content is good.");

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_read_sampling_message(ims_message_afdx, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message that has never be sent return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_AFDX_INIT_SIZE, "Message1 has good size.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Message1 has never been received.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_init_payload1, MESSAGE_AFDX_INIT_SIZE) == 0, "Message1 has the expected content.");

    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized && ims_retcode == ims_no_error, "bus1_label1 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_A429_SIZE, "bus1_label1 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_init_label1_payload, MESSAGE_A429_SIZE) == 0, "bus1_label1 content is good.");

    TEST_ASSERT(actor, ims_read_sampling_message(bus1_message1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message that has never be sent return no_error.");
    TEST_ASSERT(actor, received_size == BUS1_M1_SIZE, "Message1 is empty.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Message1 has never been received.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_init_message1_payload, BUS1_M1_SIZE) == 0, "bus1_message1 content is good.");

    TEST_ASSERT(actor, ims_read_sampling_message(bus1_message2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message that has never be sent return no_error.");
    TEST_ASSERT(actor, received_size == BUS1_M2_SIZE, "Message2 is empty.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Message2 has never been received.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_init_message2_payload, BUS1_M2_SIZE) == 0, "bus1_message2 content is good.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write
    TEST_WAIT(actor, 1);

    // Without import, message still has init value
    TEST_ASSERT(actor, ims_read_sampling_message(ims_message_afdx, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without call of import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_AFDX_INIT_SIZE, "Message1 has good size.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Message1 has never been received.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_init_payload1, MESSAGE_AFDX_INIT_SIZE) == 0, "Message1 has the expected content.");

    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized && ims_retcode == ims_no_error, "bus1_label1 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_A429_SIZE, "bus1_label1 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_init_label1_payload, MESSAGE_A429_SIZE) == 0, "bus1_label1 content is good.");

    TEST_ASSERT(actor, ims_read_sampling_message(bus1_message1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message that has never be sent return no_error.");
    TEST_ASSERT(actor, received_size == BUS1_M1_SIZE, "Message1 is empty.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Message1 has never been received.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_init_message1_payload, BUS1_M1_SIZE) == 0, "bus1_message1 content is good.");

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    // After import, the message has the received value
    TEST_ASSERT(actor, ims_read_sampling_message(ims_message_afdx, received_payload, &received_size, &validity) == ims_no_error, "Message1 read.");
    TEST_ASSERT(actor, received_size == MESSAGE_AFDX_SIZE, "Message1 has the expected length.");
    TEST_ASSERT(actor, validity == ims_valid, "Message1 is valid.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_payload1, MESSAGE_AFDX_SIZE) == 0, "Message1 has the expected content.");

    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "bus1_label1 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_A429_SIZE, "bus1_label1 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload, MESSAGE_A429_SIZE) == 0, "bus1_label1 content is good.");

    ims_retcode = ims_read_sampling_message(bus1_message1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "bus1_message1 is valid.");
    TEST_ASSERT(actor, received_size == BUS1_M1_SIZE, "bus1_message1 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_message1_payload, BUS1_M1_SIZE) == 0, "bus1_message1 content is good.");

    // After reset, the message should have its init value again

    ims_reset_all(ims_context);

    TEST_ASSERT(actor, ims_read_sampling_message(ims_message_afdx, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_AFDX_INIT_SIZE, "Message1 has good size.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Message1 has never been received.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_init_payload1, MESSAGE_AFDX_INIT_SIZE) == 0, "Message1 has the expected content.");

    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized && ims_retcode == ims_no_error, "bus1_label1 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_A429_SIZE, "bus1_label1 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_init_label1_payload, MESSAGE_A429_SIZE) == 0, "bus1_label1 content is good.");

    TEST_ASSERT(actor, ims_read_sampling_message(bus1_message1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message that has never be sent return no_error.");
    TEST_ASSERT(actor, received_size == BUS1_M1_SIZE, "Message1 is empty.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Message1 has never been received.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_init_message1_payload, BUS1_M1_SIZE) == 0, "bus1_message1 content is good.");

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
