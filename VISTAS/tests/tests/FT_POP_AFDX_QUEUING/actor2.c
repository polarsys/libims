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
#define MESSAGE1_MAX_SIZE 42
#define MESSAGE1_DEPTH    2

#define MESSAGE2_MAX_SIZE 21
#define MESSAGE2_DEPTH    4

#define MESSAGE_SIZE_MAX 42

#define INVALID_POINTER ((void*)42)

int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     ims_message1;
    ims_message_t     ims_message2;
    ims_message_t     output_message;
    ims_message_t     sampling_message;
    uint32_t          count;

    uint32_t          received_size;
    char              received_payload[MESSAGE_SIZE_MAX];
    const char*       expected_payload;
    const char*       payload[10];
    uint32_t          payloadMemLength[10];

    // Internal data
    uint32_t data_internal_max_size = 100;
    char data_internal[data_internal_max_size];
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
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalQ1", MESSAGE1_MAX_SIZE, MESSAGE1_DEPTH, ims_input, &ims_message1) == ims_no_error &&
                       ims_message1 != (ims_message_t)INVALID_POINTER && ims_message1 != NULL,
                       "We can get the first AFDX message.");

    ims_message2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalQ2", MESSAGE2_MAX_SIZE, MESSAGE2_DEPTH, ims_input, &ims_message2) == ims_no_error &&
                       ims_message2 != (ims_message_t)INVALID_POINTER && ims_message2 != NULL,
                       "We can get the second AFDX message.");

    sampling_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXsampling", 21, 1, ims_input, &sampling_message) == ims_no_error &&
                       sampling_message != (ims_message_t)INVALID_POINTER && sampling_message != NULL,
                       "We can get the sampling AFDX message.");

    output_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXoutput", 42, 2, ims_output, &output_message) == ims_no_error &&
                       output_message != (ims_message_t)INVALID_POINTER && output_message != NULL,
                       "We can get the output AFDX message.");

    // Invalid configuration test
    TEST_ASSERT(actor, ims_pop_queuing_message(sampling_message, received_payload, &received_size) == ims_invalid_configuration,
                "Cannot pop a sampling message.");

    TEST_ASSERT(actor, ims_pop_queuing_message(output_message, received_payload, &received_size) == ims_invalid_configuration,
                "Cannot pop an output message.");

    TEST_ASSERT(actor, ims_queuing_message_pending(sampling_message, &count) == ims_invalid_configuration,
                "cannot call ims_queuing_message_pending on a sampling message.");

    // Check empty messages
    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message1, received_payload, &received_size) == ims_no_error,
                "pop ims_message1 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "ims_message1 is empty.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message2, received_payload, &received_size) == ims_no_error,
                "pop ims_message2 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "ims_message2 is empty.");

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message1, received_payload, &received_size) == ims_no_error,
                "pop ims_message1 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "ims_message1 is empty.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message2, received_payload, &received_size) == ims_no_error,
                "pop ims_message2 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "ims_message2 is empty.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write
    TEST_WAIT(actor, 1);

    // Without import, message still empty
    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message1, received_payload, &received_size) == ims_no_error,
                "pop ims_message1 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "ims_message1 is empty.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message2, received_payload, &received_size) == ims_no_error,
                "pop ims_message2 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "ims_message2 is empty.");

    // After import, all messages are filled in
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error && count == 2,
                "2 message1 pending");

    payload[0] = "Hello, world!";
    payloadMemLength[0] = strlen(payload[0]);
    payload[1] = "Me again!";
    payloadMemLength[1] = strlen(payload[1]);

#ifdef ENABLE_INSTRUMENTATION
    unsigned short i;
    for(i = 0 ; i < 2 ; i++){
        memset(data_internal, 0, data_internal_max_size);
        TEST_ASSERT(actor, ims_instrumentation_message_get_queued_data(ims_message1, data_internal, &data_internal_size, data_internal_max_size, i) == ims_no_error,
                    "ims_message1 internal queued data [%hu] retrieved.", i);
        TEST_ASSERT(actor, data_internal_size == payloadMemLength[i], "Retrieved ims_message1 [%hu] internal data has the right size.", i);
        TEST_ASSERT(actor, memcmp(data_internal, payload[i], data_internal_size) == 0, "Retrieved ims_message1 [%hu] internal data is good.");
    }
#endif

    expected_payload = "Hello, world!";
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message1, received_payload, &received_size) == ims_no_error &&
                received_size == strlen(expected_payload) &&
                strncmp(expected_payload, received_payload, received_size) == 0,
                "first message1 is good.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error && count == 1,
                "1 message1 pending");

    expected_payload = "Me again!";
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message1, received_payload, &received_size) == ims_no_error &&
                received_size == strlen(expected_payload) &&
                strncmp(expected_payload, received_payload, received_size) == 0,
                "second message1 is good.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error && count == 0,
                "0 message1 pending");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message2, &count) == ims_no_error && count == 1,
                "1 message2 pending");

    payload[0] = "My name is Robert.";
    payloadMemLength[0] = strlen(payload[0]);

#ifdef ENABLE_INSTRUMENTATION
    for(i = 0 ; i < 1 ; i++){
        memset(data_internal, 0, data_internal_max_size);
        TEST_ASSERT(actor, ims_instrumentation_message_get_queued_data(ims_message2, data_internal, &data_internal_size, data_internal_max_size, i) == ims_no_error,
                    "ims_message2 internal queued data [%hu] retrieved.", i);
        TEST_ASSERT(actor, data_internal_size == payloadMemLength[i], "Retrieved ims_message2 [%hu] internal data has the right size.", i);
        TEST_ASSERT(actor, memcmp(data_internal, payload[i], data_internal_size) == 0, "Retrieved ims_message2 [%hu] internal data is good.");
    }
#endif

    expected_payload = "My name is Robert.";
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message2, received_payload, &received_size) == ims_no_error &&
                received_size == strlen(expected_payload) &&
                strncmp(expected_payload, received_payload, received_size) == 0,
                "first message2 is good.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message2, &count) == ims_no_error && count == 0,
                "0 message2 pending");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write several times
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    // We should have 4 messages pending
    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error && count == 4,
                "4 message1 pending");

    expected_payload = "first payload";
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message1, received_payload, &received_size) == ims_no_error &&
                received_size == strlen(expected_payload) &&
                strncmp(expected_payload, received_payload, received_size) == 0,
                "message1 is good.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error && count == 3,
                "3 message1 pending");

    expected_payload = "second payload";
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message1, received_payload, &received_size) == ims_no_error &&
                received_size == strlen(expected_payload) &&
                strncmp(expected_payload, received_payload, received_size) == 0,
                "message1 is good.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error && count == 2,
                "2 message1 pending");

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error && count == 2,
                "2 message1 pending");

    expected_payload = "third payload which make the difference";
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message1, received_payload, &received_size) == ims_no_error &&
                received_size == strlen(expected_payload) &&
                strncmp(expected_payload, received_payload, received_size) == 0,
                "message1 is good.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error && count == 1,
                "1 message1 pending");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write again
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error && count == 2,
                "2 message1 pending");

    expected_payload = "An other payload";
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message1, received_payload, &received_size) == ims_no_error &&
                received_size == strlen(expected_payload) &&
                strncmp(expected_payload, received_payload, received_size) == 0,
                "message1 is good.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error && count == 1,
                "1 message1 pending");

    expected_payload = "And, the last one.";
    TEST_ASSERT(actor, ims_pop_queuing_message(ims_message1, received_payload, &received_size) == ims_no_error &&
                received_size == strlen(expected_payload) &&
                strncmp(expected_payload, received_payload, received_size) == 0,
                "message1 is good.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error && count == 0,
                "0 message1 pending");

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
