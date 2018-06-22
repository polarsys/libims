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
// actor 1
//
#include "ims_test.h"

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE_SIZE 42
static const char payload1[MESSAGE_SIZE] = "hello, world!";
static const char payload2[MESSAGE_SIZE] = "You too!";

#define INVALID_POINTER ((void*)42)

//
// Main
//
int main()
{
    ims_node_t     ims_context;
    ims_node_t     ims_equipment;
    ims_node_t     ims_application;
    ims_message_t  ims_message;

    actor = ims_test_init(ACTOR_ID);

    TEST_WAIT(actor, 2); // Wait actor2 ready to read before starting our tests
    sleep(1); // actor2 is now waiting for a connection on its socket

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

    ims_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalName", MESSAGE_SIZE, 1, ims_output, &ims_message) == ims_no_error &&
                       ims_message != (ims_message_t)INVALID_POINTER && ims_message != NULL,
                       "We can get the first AFDX message.");

    TEST_SIGNAL(actor, 2); // Signal we have sent
    TEST_WAIT(actor, 2);

    TEST_ASSERT_SILENT(actor, ims_import(ims_context, 1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_write_sampling_message(ims_message, payload1, MESSAGE_SIZE) == ims_no_error,
                "Write message return no_error.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have sent
    TEST_WAIT(actor, 2);
    TEST_ASSERT_SILENT(actor, ims_import(ims_context, 1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_write_sampling_message(ims_message, payload2, MESSAGE_SIZE) == ims_no_error,
                "Write message return no_error.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have sent
    TEST_WAIT(actor, 2);
    TEST_ASSERT_SILENT(actor, ims_import(ims_context, 1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_write_sampling_message(ims_message, payload1, MESSAGE_SIZE) == ims_no_error,
                "Write message return no_error.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    ims_free_context(ims_context);

    TEST_SIGNAL(actor, 2); // Signal we have sent

    return ims_test_end(actor);
}
