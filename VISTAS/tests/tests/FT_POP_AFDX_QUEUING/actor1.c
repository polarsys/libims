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
#define MESSAGE1_MAX_SIZE 42
#define MESSAGE1_DEPTH    3

#define MESSAGE2_MAX_SIZE 21
#define MESSAGE2_DEPTH    4

#define MESSAGE_SIZE_MAX 42

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
    const char*       payload[10];
    uint32_t          payloadMemLength[10];

    // Internal data
    uint32_t data_internal_max_size = 100;
    char data_internal[data_internal_max_size];
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
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalQ1", MESSAGE1_MAX_SIZE, MESSAGE1_DEPTH, ims_output, &ims_message1) == ims_no_error &&
                       ims_message1 != (ims_message_t)INVALID_POINTER && ims_message1 != NULL,
                       "We can get AFDX message 1.");

    ims_message2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalQ2", MESSAGE2_MAX_SIZE, MESSAGE2_DEPTH, ims_output, &ims_message2) == ims_no_error &&
                       ims_message2 != (ims_message_t)INVALID_POINTER && ims_message2 != NULL,
                       "We can get AFDX message 2.");

    payload[0] = "Hello, world!";
    payloadMemLength[0] = strlen(payload[0]);
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload[0], strlen(payload[0])) == ims_no_error, "message 1 wrote");
    payload[1] = "Me again!";
    payloadMemLength[1] = strlen(payload[1]);
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload[1], strlen(payload[1])) == ims_no_error, "message 1 wrote");

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

    payload[0] = "My name is Robert.";
    payloadMemLength[0] = strlen(payload[0]);
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message2, payload[0], strlen(payload[0])) == ims_no_error, "message 2 wrote");

#ifdef ENABLE_INSTRUMENTATION
    for(i = 0 ; i < 1 ; i++){
        memset(data_internal, 0, data_internal_max_size);
        TEST_ASSERT(actor, ims_instrumentation_message_get_queued_data(ims_message2, data_internal, &data_internal_size, data_internal_max_size, i) == ims_no_error,
                    "ims_message2 internal queued data [%hu] retrieved.", i);
        TEST_ASSERT(actor, data_internal_size == payloadMemLength[i], "Retrieved ims_message2 [%hu] internal data has the right size.", i);
        TEST_ASSERT(actor, memcmp(data_internal, payload[i], data_internal_size) == 0, "Retrieved ims_message2 [%hu] internal data is good.");
    }
#endif

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // We have wrote
    TEST_WAIT(actor, 2);

    // Send severals times
    payload[0] = "first payload";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload[0], strlen(payload[0])) == ims_no_error, "message 1 wrote");
    payload[1] = "second payload";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload[1], strlen(payload[1])) == ims_no_error, "message 1 wrote");
    payload[2] = "third payload which make the difference";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload[2], strlen(payload[2])) == ims_no_error, "message 1 wrote");

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    payload[0] = "An other payload";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload[0], strlen(payload[0])) == ims_no_error, "message 1 wrote");

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // We have wrote
    TEST_WAIT(actor, 2);

    payload[0] = "And, the last one.";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload[0], strlen(payload[0])) == ims_no_error, "message 1 wrote");

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // We have wrote

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
