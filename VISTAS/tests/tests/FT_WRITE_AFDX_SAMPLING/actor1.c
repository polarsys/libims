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
#define MESSAGE1_SIZE 42
#define MESSAGE2_SIZE 21
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
    ims_message_t     ims_input_message;
    ims_message_t     ims_queuing_message;

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

    ims_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalName", MESSAGE1_SIZE, 1, ims_output, &ims_message1) == ims_no_error &&
                       ims_message1 != (ims_message_t)INVALID_POINTER && ims_message1 != NULL,
                       "We can get the first AFDX message.");

    ims_message2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "OtherAFDXLocalName", MESSAGE2_SIZE, 1, ims_output, &ims_message2) == ims_no_error &&
                       ims_message2 != (ims_message_t)INVALID_POINTER && ims_message2 != NULL,
                       "We can get the second AFDX message.");

    ims_input_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "inputMessage", 42, 1, ims_input, &ims_input_message) == ims_no_error &&
                       ims_input_message != (ims_message_t)INVALID_POINTER && ims_input_message != NULL,
                       "We can get input AFDX message.");

    ims_queuing_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalQ", 21, 4, ims_output, &ims_queuing_message) == ims_no_error &&
                       ims_queuing_message != (ims_message_t)INVALID_POINTER && ims_queuing_message != NULL,
                       "We can get queuing AFDX message.");

    // Check invalid writes
    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, message1_payload, MESSAGE1_SIZE + 1) == ims_message_invalid_size,
                "Cannot write an invalid size.");

    TEST_ASSERT(actor, ims_write_sampling_message(ims_input_message, message1_payload, 42) == ims_invalid_configuration,
                "Cannot write an input message.");

    TEST_ASSERT(actor, ims_write_sampling_message(ims_queuing_message, message1_payload, 4) == ims_invalid_configuration,
                "Cannot write a queuing message.");

    // Check write without send
    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, message1_payload, MESSAGE1_SIZE) == ims_no_error,
                "Write message return no_error.");

    TEST_ASSERT(actor, ims_write_sampling_message(ims_message2, message2_payload, MESSAGE2_SIZE) == ims_no_error,
                "Write message return no_error.");

    TEST_SIGNAL(actor, 2); // Ask to actor2 to check it has not received anything
    TEST_WAIT(actor, 2);

    // Check send
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have sent
    TEST_WAIT(actor, 2);

    // Write severals times the first message but not the message 2
    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, message1_payload, MESSAGE1_SIZE) == ims_no_error,
                "Write message1 return no_error.");

    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, message1_last_payload, MESSAGE1_SIZE) == ims_no_error,
                "Write message1 again return no_error.");

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have sent

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
