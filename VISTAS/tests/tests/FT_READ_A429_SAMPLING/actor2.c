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

#define BUS1_LABEL3_SDI     4  //XX
#define BUS1_LABEL3_NUMBER  ims_test_a429_label_number_encode("024")

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
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     bus1_label1;
    ims_message_t     bus1_label2;
	ims_message_t     bus1_label3;
    ims_message_t     bus2_label1;
    ims_message_t     bus2_label2;
    ims_message_t     output_message;
    ims_message_t     queuing_message;

    // Internal data
    const uint32_t data_internal_max_size = 100;
    char data_internal[data_internal_max_size];
    uint32_t data_internal_size;

    ims_validity_t    validity;
    ims_return_code_t ims_retcode;
    uint32_t          received_size;
    char              received_payload[MESSAGE_SIZE * 4];

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
    ims_test_a429_fill_label(bus1_label1_payload2, BUS1_LABEL1_NUMBER, BUS1_LABEL1_SDI,  0x1F0F0, 2, 1);
    ims_test_a429_fill_label(bus1_label1_payload3, BUS1_LABEL1_NUMBER, BUS1_LABEL1_SDI,  0x12345, 3, 1);

    actor = ims_test_init(ACTOR_ID);

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
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l1", MESSAGE_SIZE, 1, ims_input, &bus1_label1) == ims_no_error &&
                       bus1_label1 != (ims_message_t)INVALID_POINTER && bus1_label1 != NULL,
                       "We can get the bus1_label1.");

    bus1_label2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l2", MESSAGE_SIZE, 1, ims_input, &bus1_label2) == ims_no_error &&
                       bus1_label2 != (ims_message_t)INVALID_POINTER && bus1_label2 != NULL,
                       "We can get the bus1_label2.");

	bus1_label3 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l3", MESSAGE_SIZE, 1, ims_input, &bus1_label3) == ims_no_error &&
                       bus1_label3 != (ims_message_t)INVALID_POINTER && bus1_label3 != NULL,
                       "We can get the bus1_label3.");

    bus2_label1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus2_l1", MESSAGE_SIZE, 1, ims_input, &bus2_label1) == ims_no_error &&
                       bus2_label1 != (ims_message_t)INVALID_POINTER && bus2_label1 != NULL,
                       "We can get the bus2_label1.");

    bus2_label2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus2_l2", MESSAGE_SIZE, 1, ims_input, &bus2_label2) == ims_no_error &&
                       bus2_label2 != (ims_message_t)INVALID_POINTER && bus2_label2 != NULL,
                       "We can get the bus2_label2.");

    output_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "output_message", MESSAGE_SIZE, 1, ims_output, &output_message) == ims_no_error &&
                       output_message != (ims_message_t)INVALID_POINTER && output_message != NULL,
                       "We can get the output_message.");

    queuing_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "queuing_message", MESSAGE_SIZE, 50, ims_input, &queuing_message) == ims_no_error &&
                       queuing_message != (ims_message_t)INVALID_POINTER && queuing_message != NULL,
                       "We can get the queuing_message.");

    // Check empty messages
    TEST_ASSERT(actor, ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == 0, "Message1 is empty.");
    TEST_ASSERT(actor, validity == ims_never_received, "Message1 has never been received.");

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message that has never be sent return no_error.");
    TEST_ASSERT(actor, received_size == 0, "Message1 is empty.");
    TEST_ASSERT(actor, validity == ims_never_received, "Message1 has never been received.");

    // Invalid configuration test
    TEST_ASSERT(actor, ims_read_sampling_message(output_message, received_payload, &received_size, &validity) == ims_invalid_configuration,
                "Reading output message return ims_invalid_configuration.");

    TEST_ASSERT(actor, ims_read_sampling_message(queuing_message, received_payload, &received_size, &validity) == ims_invalid_configuration,
                "Reading queuing message return ims_invalid_configuration.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write
    TEST_WAIT(actor, 1);

    // Without import, message still empty
    TEST_ASSERT(actor, ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without call of import return no_error.");
    TEST_ASSERT(actor, received_size == 0, "Message1 is empty.");
    TEST_ASSERT(actor, validity == ims_never_received, "Message1 has never been received.");

    // After import, all messages are filled in
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "bus1_label1 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "bus1_label1 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload, MESSAGE_SIZE) == 0, "bus1_label1 content is good.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus1_label1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus1_label1 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, received_payload, MESSAGE_SIZE) == 0, "Retrieved bus1_label1 internal data is good.");
#endif

    ims_retcode = ims_read_sampling_message(bus1_label2, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "bus1_label2 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "bus1_label2 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label2_payload, MESSAGE_SIZE) == 0, "bus1_label2 content is good.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus1_label2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus1_label2 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, received_payload, MESSAGE_SIZE) == 0, "Retrieved bus1_label2 internal data is good.");
#endif

	ims_retcode = ims_read_sampling_message(bus1_label3, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "bus1_label3 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "bus1_label3 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label2_payload, MESSAGE_SIZE) == 0, "bus1_label3 content is good and is equal to bus1_label2.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus1_label3, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus1_label3 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, received_payload, MESSAGE_SIZE) == 0, "Retrieved bus1_label3 internal data is good.");
#endif

    ims_retcode = ims_read_sampling_message(bus2_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "bus2_label1 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "bus2_label1 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus2_label1_payload, MESSAGE_SIZE) == 0, "bus2_label1 content is good.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus2_label1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus2_label1 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, received_payload, MESSAGE_SIZE) == 0, "Retrieved bus2_label1 internal data is good.");
#endif

    ims_retcode = ims_read_sampling_message(bus2_label2, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "bus2_label2 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "bus2_label2 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus2_label2_payload, MESSAGE_SIZE) == 0, "bus2_label2 content is good.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus2_label2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus2_label2 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, received_payload, MESSAGE_SIZE) == 0, "Retrieved bus2_label2 internal data is good.");
#endif

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write several times
    TEST_WAIT(actor, 1);

    // Actor 1 has send several times, import sould drop all values except the last one
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity) == ims_no_error, "Message1 read.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Message1 has the expected length.");
    TEST_ASSERT(actor, validity == ims_valid, "Message1 is valid.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload3, MESSAGE_SIZE) == 0, "Message1 has the expected content.");

    // Validity tests

    // The message validity is 50ms, so if the time progress of 25 ms, the message still valid
    TEST_ASSERT(actor, ims_progress(ims_context, 25 * 1000) == ims_no_error, "ims progress success. (total 25ms)");
    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "2nd read: A429 message still valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "2nd read: Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload3, MESSAGE_SIZE) == 0, "2nd read: A429 message content is good.");

    // The message validity is 50ms, so if the total progression is more than 50ms, the message become invalid
    TEST_ASSERT(actor, ims_progress(ims_context, 30 * 1000) == ims_no_error, "ims progress success. (total 55 ms)");
    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_invalid && ims_retcode == ims_no_error, "3rd read: A429 message become invalid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "3rd read: Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload3, MESSAGE_SIZE) == 0, "3rd read: A429 message content is good.");

    // The message2 validity is 60ms, so with a progress of 55 ms, the message2 still valid
    ims_retcode = ims_read_sampling_message(bus1_label2, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "2nd read: A429 message2 still valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "2nd read: Message2 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label2_payload, MESSAGE_SIZE) == 0, "2nd read: A429 message2 content is good.");

	// The message3 validity is 60ms, so with a progress of 55 ms, the message3 still valid
    ims_retcode = ims_read_sampling_message(bus1_label3, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "2nd read: A429 message3 still valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "2nd read: Message3 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload3, MESSAGE_SIZE) == 0, "2nd read: A429 message3 content is good.");

    // If we update the message validity more than the total progression, the message become valid again
    TEST_ASSERT(actor, ims_message_set_sampling_timeout(bus1_label1, 60 * 1000) == ims_no_error, "Message validity updated to 60ms.");
    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "4th read: A429 message become valid again.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "4th read: Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload3, MESSAGE_SIZE) == 0, "4th read: A429 message content is good.");

    // Message 2 become invalid too
    ims_retcode = ims_read_sampling_message(bus1_label2, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "4th read: A429 message2 become valid again.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "4th read: Message2 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label2_payload, MESSAGE_SIZE) == 0, "4th read: A429 message2 content is good.");

	// Message 3 become invalid too
    ims_retcode = ims_read_sampling_message(bus1_label3, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "4th read: A429 message3 become valid again.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "4th read: Message3 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload3, MESSAGE_SIZE) == 0, "4th read: A429 message3 content is good.");

    // If the total progression is more than the new validity, the message become invalid again.
    TEST_ASSERT(actor, ims_progress(ims_context, 10 * 1000) == ims_no_error, "ims progress success. (total 65 ms)");
    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_invalid && ims_retcode == ims_no_error, "5th read: A429 message become invalid again.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "5th read: Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload3, MESSAGE_SIZE) == 0, "5th read: A429 message content is good.");

    // If we set validity to 0, message will be always valid once received
    TEST_ASSERT(actor, ims_message_set_sampling_timeout(bus1_label1, 0) == ims_no_error, "Message validity set to 0.");
    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "6th read: A429 message become valid again.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "6th read: Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload3, MESSAGE_SIZE) == 0, "6th read: A429 message content is good.");

    // Reset validity to its original value
    ims_message_set_sampling_timeout(bus1_label1, 50 * 1000);

    // If we don't received data, validity is not changed
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_invalid && ims_retcode == ims_no_error, "Import without data: message still invalid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload3, MESSAGE_SIZE) == 0, "A429 message content is good.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write
    TEST_WAIT(actor, 1);

    // Message has been wrote by actor1, message should become valid again
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "Import with data: message become valid again.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload, MESSAGE_SIZE) == 0, "A429 message content is good.");

    // Message 2 has not be sent: it still invalid
    ims_retcode = ims_read_sampling_message(bus1_label2, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_invalid && ims_retcode == ims_no_error, "Message2 is invalid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label2_payload, MESSAGE_SIZE) == 0, "A429 message content is good.");

	// Message 3 has not be sent: it still invalid
    ims_retcode = ims_read_sampling_message(bus1_label3, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "Import with data: message1 is updated and so is message3, message3 become valid again.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload, MESSAGE_SIZE) == 0, "A429 message content is good.");

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
