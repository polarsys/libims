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
// NAD test - actor 1
//
#include "ims_test.h"

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE1_SIZE  4
#define MESSAGE2_SIZE  1
#define MESSAGE3_SIZE  8
#define MESSAGE4_SIZE 10

#define INVALID_POINTER ((void*)42)

//
// Main
//
int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     grp1_msg1;
    ims_message_t     grp1_msg2;
    ims_message_t     grp2_msg1;
    ims_message_t     grp2_msg2;
    ims_message_t     input_message;
    char              data[20];

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

    grp1_msg1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message1", MESSAGE1_SIZE, 1, ims_output, &grp1_msg1) == ims_no_error &&
                       grp1_msg1 != (ims_message_t)INVALID_POINTER && grp1_msg1 != NULL,
                       "We can get the grp1_msg1.");

    grp1_msg2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message2", MESSAGE2_SIZE, 1, ims_output, &grp1_msg2) == ims_no_error &&
                       grp1_msg2 != (ims_message_t)INVALID_POINTER && grp1_msg2 != NULL,
                       "We can get the grp1_msg2.");

    grp2_msg1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message3", MESSAGE3_SIZE, 1, ims_output, &grp2_msg1) == ims_no_error &&
                       grp2_msg1 != (ims_message_t)INVALID_POINTER && grp2_msg1 != NULL,
                       "We can get the grp2_msg1.");

    grp2_msg2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message4", MESSAGE4_SIZE, 1, ims_output, &grp2_msg2) == ims_no_error &&
                       grp2_msg2 != (ims_message_t)INVALID_POINTER && grp2_msg2 != NULL,
                       "We can get the grp2_msg2.");

    input_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "input_message", 10, 1, ims_input, &input_message) == ims_no_error &&
                       input_message != (ims_message_t)INVALID_POINTER && input_message != NULL,
                       "We can get the input_message.");

    // Check invalid writes
    memset(data, 5, 10);

    TEST_ASSERT(actor, ims_write_sampling_message(grp1_msg1, data, MESSAGE1_SIZE + 1) == ims_invalid_configuration,
                "Cannot call sampling functions for NAD.");

    TEST_ASSERT(actor, ims_write_nad_message(grp1_msg1, data, MESSAGE1_SIZE + 1) == ims_message_invalid_size,
                "Cannot write an invalid size.");

    TEST_ASSERT(actor, ims_write_nad_message(input_message, data, 10) == ims_invalid_configuration,
                "Cannot write an input message.");

    // Send default value
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Tell actor2 we have write
    TEST_WAIT(actor, 2);

    // Write messages but don't call send nor progress
    memset(data, 5, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp1_msg1, data, MESSAGE1_SIZE) == ims_no_error, "grp1_msg1 wrote.");

    memset(data, 6, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp1_msg2, data, MESSAGE2_SIZE) == ims_no_error, "grp1_msg2 wrote.");

    memset(data, 7, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp2_msg1, data, MESSAGE3_SIZE) == ims_no_error, "grp2_msg1 wrote.");

    memset(data, 8, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp2_msg2, data, MESSAGE4_SIZE) == ims_no_error, "grp2_msg2 wrote.");

    TEST_SIGNAL(actor, 2); // Tell actor2 we have write
    TEST_WAIT(actor, 2);

    // Call send without progress: nothing should be send, nad are send periodically
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Tell actor2 we have send
    TEST_WAIT(actor, 2);

    // Call progress enought for group1, but not for group 2
    TEST_ASSERT(actor, ims_progress(ims_context, 50 * 1000) == ims_no_error, "Progress sucess, total 50ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have progress + sent
    TEST_WAIT(actor, 2);

    // Call progress enought to reach group2 cycle, but not for group 1 again
    TEST_ASSERT(actor, ims_progress(ims_context, 20 * 1000) == ims_no_error, "Progress sucess, total 70ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have progress + sent
    TEST_WAIT(actor, 2);

    // Nad are sent periodically, we only progress+send (not wire) and message will be send again
    TEST_ASSERT(actor, ims_progress(ims_context, 50 * 1000) == ims_no_error, "Progress sucess, total 120ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have progress + sent
    TEST_WAIT(actor, 2);

    // Write several time grp1_msg1 and grp2_msg1
    memset(data, 12, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp1_msg1, data, MESSAGE1_SIZE) == ims_no_error, "grp1_msg1 wrote.");
    memset(data, 14, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp1_msg1, data, MESSAGE1_SIZE) == ims_no_error, "grp1_msg1 wrote.");
    memset(data, 16, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp1_msg1, data, MESSAGE1_SIZE) == ims_no_error, "grp1_msg1 wrote.");

    memset(data, 45, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp2_msg1, data, MESSAGE3_SIZE) == ims_no_error, "grp2_msg1 wrote.");
    memset(data, 4, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp2_msg1, data, MESSAGE3_SIZE) == ims_no_error, "grp2_msg1 wrote.");
    memset(data, 11, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp2_msg1, data, MESSAGE3_SIZE) == ims_no_error, "grp2_msg1 wrote.");

    TEST_ASSERT(actor, ims_progress(ims_context, 120 * 1000) == ims_no_error, "Progress sucess, total 240ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have write + progress + sent

    // Done
    ims_free_context(ims_context);

    return ims_test_end(actor);
}
