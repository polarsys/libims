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
// AFDX test - actor 1
//
#include "ims_test.h"

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE1_SIZE     42
#define MESSAGE2_SIZE     21
#define BIG_MESSAGE_SIZE  7600

static const char message1_payload[MESSAGE1_SIZE] = "hello, world!";
static const char message2_payload[MESSAGE2_SIZE] = "You too!";
static const char message1_last_payload[MESSAGE1_SIZE] = "Ok, see you!";

#define INVALID_POINTER ((void*)42)

//
// Main
//
int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     ims_message1;
    ims_message_t     ims_message2;
    ims_message_t     big_message;

    // Internal data
    uint32_t data_internal_max_size = 100;
    char *data_internal;
    uint32_t data_internal_size;

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

    ims_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalName", MESSAGE1_SIZE, 1, ims_output, &ims_message1) == ims_no_error &&
                       ims_message1 != (ims_message_t)INVALID_POINTER && ims_message1 != NULL,
                       "We can get the first AFDX message.");

    ims_message2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "OtherAFDXLocalName", MESSAGE2_SIZE, 1, ims_output, &ims_message2) == ims_no_error &&
                       ims_message2 != (ims_message_t)INVALID_POINTER && ims_message2 != NULL,
                       "We can get the second AFDX message.");

    big_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "bigAFDX", BIG_MESSAGE_SIZE, 1, ims_output, &big_message) == ims_no_error &&
                       big_message != (ims_message_t)INVALID_POINTER && big_message != NULL,
                       "We can get the big AFDX message.");

    // Write both messages and send them
    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, message1_payload, MESSAGE1_SIZE) == ims_no_error,
                "Message 1 wrote.");
#ifdef ENABLE_INSTRUMENTATION
    data_internal = (char*)malloc(data_internal_max_size);
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(ims_message1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "ims_message1 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE1_SIZE, "Retrieved ims_message1 has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, message1_payload, MESSAGE1_SIZE) == 0, "Retrieved ims_message1 internal data is good.");
    free(data_internal);
#endif

    TEST_ASSERT(actor, ims_write_sampling_message(ims_message2, message2_payload, MESSAGE2_SIZE) == ims_no_error,
                "Message 2 wrote.");
#ifdef ENABLE_INSTRUMENTATION
    data_internal = (char*)malloc(data_internal_max_size);
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(ims_message2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "ims_message2 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE2_SIZE, "Retrieved ims_message2 has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, message2_payload, MESSAGE2_SIZE) == 0, "Retrieved ims_message2 internal data is good.");
    free(data_internal);
#endif

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "Messages sent.");

    TEST_SIGNAL(actor, 2); // We have sent
    TEST_WAIT(actor, 2);

    // Write and send several times message1
    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, message1_payload, MESSAGE1_SIZE) == ims_no_error,
                "Message 1 wrote 1 time.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "Messages sent.");

    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, message1_payload, MESSAGE1_SIZE) == ims_no_error,
                "Message 1 wrote 2 times.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "Messages sent.");

    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, message1_last_payload, MESSAGE1_SIZE) == ims_no_error,
                "Message 1 wrote 3 times.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "Messages sent.");

    TEST_SIGNAL(actor, 2); // We have sent
    TEST_WAIT(actor, 2);

    // Write & send message1 again
    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, message1_payload, MESSAGE1_SIZE) == ims_no_error,
                "Message 1 wrote.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "Messages sent.");

    TEST_SIGNAL(actor, 2); // We have sent
    TEST_WAIT(actor, 2);

    char big_message_payload[BIG_MESSAGE_SIZE];
    int i;
    for (i = 0; i < BIG_MESSAGE_SIZE; i++) big_message_payload[i] = i % 256;
    TEST_ASSERT(actor, ims_write_sampling_message(big_message, big_message_payload, BIG_MESSAGE_SIZE) == ims_no_error,
                "big message wrote.");

    data_internal = (char*)malloc(data_internal_max_size);
    memset(data_internal, 0, data_internal_max_size);
#ifdef ENABLE_INSTRUMENTATION
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(big_message, data_internal, &data_internal_size, data_internal_max_size) != ims_no_error,
                "Cannot retrieve internal message, buffer too small.");
    data_internal_max_size = BIG_MESSAGE_SIZE;
    data_internal = (char*)realloc(data_internal,data_internal_max_size);
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(big_message, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "big_message internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == BIG_MESSAGE_SIZE, "Retrieved big_message internal data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, big_message_payload, BIG_MESSAGE_SIZE) == 0, "Retrieved big_message internal data is good.");
#endif

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "Messages sent.");

    TEST_SIGNAL(actor, 2); // We have sent

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
