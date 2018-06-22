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
#include "stdio.h"

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

#define INVALID_POINTER ((void*)42)

//
// Main
//
int main()
{

    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     ims_message[NUMBER_OF_PORTS];
    char              msg_name[20];
    char              payload[100];
    uint32_t          received_size;
    ims_validity_t    validity;

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

    TEST_LOG(actor, "Create %d input messages...", NUMBER_OF_PORTS);

    int id;
    for (id = 0; id < NUMBER_OF_PORTS; id++) {
        sprintf(msg_name, "AFDX_%d", id);
        TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, msg_name, 10, 1, ims_input, &ims_message[id]) == ims_no_error &&
                           ims_message[id] != (ims_message_t)INVALID_POINTER && ims_message[id] != NULL,
                           "message %d created.", id);
    }

    TEST_SIGNAL(actor, 1);  // We are ready
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_LOG(actor, "Checking %d messages...", NUMBER_OF_PORTS);

    for (id = 0; id < NUMBER_OF_PORTS; id++) {
        TEST_ASSERT_SILENT(actor, ims_read_sampling_message(ims_message[id], payload, &received_size, &validity) == ims_no_error, "Message %d read.", id);
        TEST_ASSERT_SILENT(actor, received_size == 10, "Message %d has the expected length.", id);
        TEST_ASSERT_SILENT(actor, validity == ims_valid, "Message %d is valid.", id);
        TEST_ASSERT_SILENT(actor, memcmp(payload, &id, 4) == 0, "Message %d has the expected content.", id);
    }

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
