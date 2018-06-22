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
#define MESSAGE_SIZE        42
#define RECEIVE_MAX_SIZE  1000

static const char expected_payload1[MESSAGE_SIZE] = "hello, world!";
static const char expected_payload2[MESSAGE_SIZE] = "You too!";
static const char expected_payload3[MESSAGE_SIZE] = "Ok, perfect!";

#define INVALID_POINTER ((void*)42)

int main()
{
    ims_node_t        ims_context;
    ims_node_t        ims_equipment;
    ims_node_t        ims_application;
    ims_message_t     ims_message;
    ims_validity_t    validity;
    uint32_t          received_size;
    char              received_payload[RECEIVE_MAX_SIZE];

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

    ims_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalName", MESSAGE_SIZE, 1, ims_input, &ims_message) == ims_no_error &&
                       ims_message != (ims_message_t)INVALID_POINTER && ims_message != NULL,
                       "We can get the first AFDX message.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write
    TEST_WAIT(actor, 1);

    // Check we receive payload1
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
    TEST_ASSERT(actor, ims_read_sampling_message(ims_message, received_payload, &received_size, &validity) == ims_no_error, "Message1 read.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Message1 has the expected length.");
    TEST_ASSERT(actor, validity == ims_valid, "Message1 is valid.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_payload1, MESSAGE_SIZE) == 0, "Message1 has the expected content.");

    // Ask to stop
    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // Check we don't have receive anything after the stop (message invalid and we still have payload1 inside)
    TEST_ASSERT(actor, ims_progress(ims_context, 100*1000) == ims_no_error, "Progress of 100ms (message become invalid)");
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
    TEST_ASSERT(actor, ims_read_sampling_message(ims_message, received_payload, &received_size, &validity) == ims_no_error, "Message1 read.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Message1 has the expected length.");
    TEST_ASSERT(actor, validity == ims_invalid, "Message1 is invalid.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_payload1, MESSAGE_SIZE) == 0, "Message1 has the expected content.");

    // Ask to restart
    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // Check we receive payload1 again after the restart
    TEST_ASSERT(actor, ims_progress(ims_context, 100*1000) == ims_no_error, "Progress of 100ms (message become invalid)");
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
    TEST_ASSERT(actor, ims_read_sampling_message(ims_message, received_payload, &received_size, &validity) == ims_no_error, "Message1 read.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Message1 has the expected length.");
    TEST_ASSERT(actor, validity == ims_valid, "Message1 is valid.");
    TEST_ASSERT(actor, memcmp(received_payload, expected_payload3, MESSAGE_SIZE) == 0, "Message1 has the expected content.");

    ims_free_context(ims_context);

    TEST_SIGNAL(actor, 1);

    return ims_test_end(actor);
}
