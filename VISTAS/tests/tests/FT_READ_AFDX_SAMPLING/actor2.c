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

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE1_SIZE        42
#define MESSAGE2_SIZE        21
#define BIG_MESSAGE_SIZE   7600
#define RECEIVE_MAX_SIZE  10000

static const char expected_payload1[MESSAGE1_SIZE] = "hello, world!";
static const char expected_payload2[MESSAGE2_SIZE] = "You too!";
static const char expected_last_payload1[MESSAGE1_SIZE] = "Ok, see you!";

#define INVALID_POINTER ((void*)42)

int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     ims_message1;
    ims_message_t     ims_message2;
    ims_message_t     big_message;
    ims_message_t     output_message;
    ims_message_t     queuing_message;
    ims_return_code_t ims_retcode;
    ims_validity_t    validity;
    uint32_t          received_size;
    char              received_payload[RECEIVE_MAX_SIZE];

    // Internal data
    uint32_t data_internal_max_size = 100;
    char *data_internal;
    uint32_t data_internal_size;

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

    ims_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalName", MESSAGE1_SIZE, 1, ims_input, &ims_message1) == ims_no_error &&
                       ims_message1 != (ims_message_t)INVALID_POINTER && ims_message1 != NULL,
                       "We can get the first AFDX message.");

    ims_message2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "OtherAFDXLocalName", MESSAGE2_SIZE, 1, ims_input, &ims_message2) == ims_no_error &&
                       ims_message2 != (ims_message_t)INVALID_POINTER && ims_message2 != NULL,
                       "We can get the first AFDX message.");

    big_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "bigAFDX", BIG_MESSAGE_SIZE, 1, ims_input, &big_message) == ims_no_error &&
                       big_message != (ims_message_t)INVALID_POINTER && big_message != NULL,
                       "We can get the big AFDX message.");

    output_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "output_message", 42, 1, ims_output, &output_message) == ims_no_error &&
                       output_message != (ims_message_t)INVALID_POINTER && output_message != NULL,
                       "We can get the output AFDX message.");

    queuing_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "queuing_message", 21, 4, ims_input, &queuing_message) == ims_no_error &&
                       queuing_message != (ims_message_t)INVALID_POINTER && queuing_message != NULL,
                       "We can get the queuing AFDX message.");

    // Check empty messages
    TEST_ASSERT(actor, ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == 0, "Message1 is empty.");
    TEST_ASSERT(actor, validity == ims_never_received, "Message1 has never been received.");

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity) == ims_no_error,
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
    TEST_ASSERT(actor, ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without call of import return no_error.");
    TEST_ASSERT(actor, received_size == 0, "Message1 is empty.");
    TEST_ASSERT(actor, validity == ims_never_received, "Message1 has never been received.");

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    // After import, both messages are filed
    TEST_ASSERT(actor, ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity) == ims_no_error, "Message1 read.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "Message1 has the expected length.");
    TEST_ASSERT(actor, validity == ims_valid, "Message1 is valid.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_payload1, MESSAGE1_SIZE) == 0, "Message1 has the expected content.");

    // Check retrieved data and internal data consistency
#ifdef ENABLE_INSTRUMENTATION
    data_internal = (char*)malloc(data_internal_max_size);
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(ims_message1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "ims_message1 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE1_SIZE, "Retrieved ims_message1 has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, received_payload, MESSAGE1_SIZE) == 0, "Retrieved ims_message1 internal data is good.");
    free(data_internal);
#endif

    TEST_ASSERT(actor, ims_read_sampling_message(ims_message2, received_payload, &received_size, &validity) == ims_no_error, "Message2 read.");
    TEST_ASSERT(actor, received_size == MESSAGE2_SIZE, "Message2 has the expected length.");
    TEST_ASSERT(actor, validity == ims_valid, "Message2 is valid.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_payload2, MESSAGE2_SIZE) == 0, "Message2 has the expected content.");

    // Check retrieved data and internal data consistency
#ifdef ENABLE_INSTRUMENTATION
    data_internal = (char*)malloc(data_internal_max_size);
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(ims_message2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "ims_message2 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE2_SIZE, "Retrieved ims_message2 has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, received_payload, MESSAGE2_SIZE) == 0, "Retrieved ims_message2 internal data is good.");
    free(data_internal);
#endif

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write several times
    TEST_WAIT(actor, 1);

    // Actor 1 has send several times, import sould drop all values except the last one
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity) == ims_no_error, "Message1 read.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "Message1 has the expected length.");
    TEST_ASSERT(actor, validity == ims_valid, "Message1 is valid.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_last_payload1, MESSAGE1_SIZE) == 0, "Message1 has the expected content.");

    // Validity tests

    // The message validity is 50ms, so if the time progress of 25 ms, the message still valid
    TEST_ASSERT(actor, ims_progress(ims_context, 25 * 1000) == ims_no_error, "ims progress success. (total 25ms)");
    ims_retcode = ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "2nd read: AFDX message still valid.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "2nd read: Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_last_payload1, MESSAGE1_SIZE) == 0, "2nd read: AFDX message content is good.");

    // The message validity is 50ms, so if the total progression is more than 50ms, the message become invalid
    TEST_ASSERT(actor, ims_progress(ims_context, 30 * 1000) == ims_no_error, "ims progress success. (total 55 ms)");
    ims_retcode = ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_invalid && ims_retcode == ims_no_error, "3rd read: AFDX message become invalid.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "3rd read: Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_last_payload1, MESSAGE1_SIZE) == 0, "3rd read: AFDX message content is good.");

    // If we update the message validity more than the total progression, the message become valid again
    TEST_ASSERT(actor, ims_message_set_sampling_timeout(ims_message1, 60 * 1000) == ims_no_error, "Message validity updated to 60ms.");
    ims_retcode = ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "4th read: AFDX message become valid again.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "4th read: Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_last_payload1, MESSAGE1_SIZE) == 0, "4th read: AFDX message content is good.");

    // If the total progression is more than the new validity, the message become invalid again.
    TEST_ASSERT(actor, ims_progress(ims_context, 10 * 1000) == ims_no_error, "ims progress success. (total 65 ms)");
    ims_retcode = ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_invalid && ims_retcode == ims_no_error, "5th read: AFDX message become invalid again.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "5th read: Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_last_payload1, MESSAGE1_SIZE) == 0, "5th read: AFDX message content is good.");

    // If we set validity to 0, message will be always valid once received
    TEST_ASSERT(actor, ims_message_set_sampling_timeout(ims_message1, 0) == ims_no_error, "Message validity set to 0.");
    ims_retcode = ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "6th read: AFDX message become valid again.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "6th read: Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_last_payload1, MESSAGE1_SIZE) == 0, "6th read: AFDX message content is good.");

    // Reset validity to its original value
    ims_message_set_sampling_timeout(ims_message1, 50 * 1000);

    // If we don't received data, validity is not changed
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    ims_retcode = ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_invalid && ims_retcode == ims_no_error, "Import without data: message still invalid.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_last_payload1, MESSAGE1_SIZE) == 0, "AFDX message content is good.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write
    TEST_WAIT(actor, 1);

    // Message has been wrote by actor1, message should become valid again
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    ims_retcode = ims_read_sampling_message(ims_message1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "Import with data: message become valid again.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_payload1, MESSAGE1_SIZE) == 0, "AFDX message content is good.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write
    TEST_WAIT(actor, 1);

    char expected_big_payload[BIG_MESSAGE_SIZE];
    int i;
    for (i = 0; i < BIG_MESSAGE_SIZE; i++) expected_big_payload[i] = i % 256;

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
    ims_retcode = ims_read_sampling_message(big_message, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "Receiv big message is valid.");
    TEST_ASSERT(actor, received_size == BIG_MESSAGE_SIZE, "Message has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_big_payload, BIG_MESSAGE_SIZE) == 0, "AFDX message content is good.");

    // Check received data and internal data consistency
#ifdef ENABLE_INSTRUMENTATION
    data_internal = (char*)malloc(data_internal_max_size);
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(big_message, data_internal, &data_internal_size, data_internal_max_size) != ims_no_error,
                "Cannot retrieve internal message, buffer too small.");
    data_internal_max_size = BIG_MESSAGE_SIZE;
    data_internal = (char*)realloc(data_internal,data_internal_max_size);
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(big_message, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "big_message internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == BIG_MESSAGE_SIZE, "Retrieved big_message internal data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, received_payload, BIG_MESSAGE_SIZE) == 0, "Retrieved big_message internal data is good.");
#endif

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
