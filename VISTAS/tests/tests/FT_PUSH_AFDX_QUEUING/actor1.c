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
#define MESSAGE1_DEPTH    2

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
    ims_message_t     input_queuing;
    ims_message_t     sampling_message;
    uint32_t          count;
    const char*       payload = "never sent";

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
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalQ1", MESSAGE1_MAX_SIZE, MESSAGE1_DEPTH, ims_output, &ims_message1) == ims_no_error &&
                       ims_message1 != (ims_message_t)INVALID_POINTER && ims_message1 != NULL,
                       "We can get AFDX message 1.");

    ims_message2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalQ2", MESSAGE2_MAX_SIZE, MESSAGE2_DEPTH, ims_output, &ims_message2) == ims_no_error &&
                       ims_message2 != (ims_message_t)INVALID_POINTER && ims_message2 != NULL,
                       "We can get AFDX message 2.");

    input_queuing = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "input_queuing", 42, 2, ims_input, &input_queuing) == ims_no_error &&
                       input_queuing != (ims_message_t)INVALID_POINTER && input_queuing != NULL,
                       "We can get AFDX input message.");

    sampling_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "sampling_message", 42, 1, ims_output, &sampling_message) == ims_no_error &&
                       sampling_message != (ims_message_t)INVALID_POINTER && sampling_message != NULL,
                       "We can get AFDX sampling message.");

    // Check invalid push
    char fake[100] = "";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, fake, MESSAGE1_MAX_SIZE + 1) == ims_message_invalid_size,
                "Cannot write an invalid size.");

    TEST_ASSERT(actor, ims_push_queuing_message(input_queuing, payload, 2) == ims_invalid_configuration,
                "Cannot push an input message.");

    TEST_ASSERT(actor, ims_push_queuing_message(sampling_message, payload, 2) == ims_invalid_configuration,
                "Cannot push a sampling message.");

    // Check push without send
    payload = "Hello, world!";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload, strlen(payload)) == ims_no_error, "Message1 wrote.");

    payload = "Hello, 2!";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message2, payload, strlen(payload)) == ims_no_error, "Message2 wrote.");

    TEST_SIGNAL(actor, 2);  // Ask to actor2 to check it has not received anything
    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error, "queuing_message_pending return no_error");
    TEST_ASSERT(actor, count == 1, "1 messages pending for ims_message1.");

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error, "queuing_message_pending return no_error");
    TEST_ASSERT(actor, count == 0, "No more messages pending for ims_message1.");

    TEST_SIGNAL(actor, 2); // Signal we have sent
    TEST_WAIT(actor, 2);

    // Severals push on same queuing label
    payload = "first data";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload, strlen(payload)) == ims_no_error, "Message1 wrote.");

    payload = "Have fun with this data!";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload, strlen(payload)) == ims_no_error, "Message1 wrote.");

    // We have reach the queue depth. We can't push more labels
    payload = "No, no, no!";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload, strlen(payload)) == ims_message_queue_full, "We can't push more messages.");
    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error, "queuing_message_pending return no_error");
    TEST_ASSERT(actor, count == 2, "2 messages pending for ims_message1.");

    // Send all
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    // After send, we can push messages again
    TEST_ASSERT(actor, ims_queuing_message_pending(ims_message1, &count) == ims_no_error, "queuing_message_pending return no_error");
    TEST_ASSERT(actor, count == 0, "No message pending for ims_message1.");

    payload = "Ok for this one.";
    TEST_ASSERT(actor, ims_push_queuing_message(ims_message1, payload, strlen(payload)) == ims_no_error, "Message1 wrote.");

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");
    TEST_SIGNAL(actor, 2); // Signal we have sent

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
