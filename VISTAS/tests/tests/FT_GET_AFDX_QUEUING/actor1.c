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
    TEST_ASSERT(actor,
                ims_get_message(ims_application, ims_a429, "AFDXLocalQ1", MESSAGE1_MAX_SIZE, MESSAGE1_DEPTH, ims_output, &ims_message1) == ims_invalid_configuration &&
                ims_message1 == NULL,
                "We can't get an A429 message from an AFDX one.");

    ims_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT(actor,
                ims_get_message(ims_application, ims_afdx, "AFDXDontExists", MESSAGE1_MAX_SIZE, MESSAGE1_DEPTH, ims_output, &ims_message1) == ims_invalid_configuration &&
                ims_message1 == NULL,
                "We can't get non-existant message.");

    ims_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT(actor,
                ims_get_message(ims_application, ims_afdx, "AFDXLocalQ1", MESSAGE1_MAX_SIZE + 1, MESSAGE1_DEPTH, ims_output, &ims_message1) == ims_invalid_configuration &&
                ims_message1 == NULL,
                "We can't get message with an invalid size.");

    ims_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT(actor,
                ims_get_message(ims_application, ims_afdx, "AFDXLocalQ1", MESSAGE1_MAX_SIZE, MESSAGE1_DEPTH + 1, ims_output, &ims_message1) == ims_invalid_configuration &&
                ims_message1 == NULL,
                "We can't get message with an invalid depth.");

    ims_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT(actor,
                ims_get_message(ims_application, ims_afdx, "AFDXLocalQ1", MESSAGE1_MAX_SIZE, MESSAGE1_DEPTH, ims_input, &ims_message1) == ims_invalid_configuration &&
                ims_message1 == NULL,
                "We can't get message with an invalid direction.");

    ims_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalQ1", MESSAGE1_MAX_SIZE, MESSAGE1_DEPTH, ims_output, &ims_message1) == ims_no_error &&
                ims_message1 != (ims_message_t)INVALID_POINTER && ims_message1 != NULL,
                "We can get AFDX message 1.");

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
