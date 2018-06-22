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
// CAN test - actor 1
//
#include "ims_test.h"

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

//
// Message data
//
#define BUS1_M1_SIZE 4
#define BUS1_M2_SIZE 8
#define BUS2_M1_SIZE 8
#define BUS2_M2_SIZE 4

#define INVALID_POINTER ((void*)42)

//
// Main
//
int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     bus1_message1;
    ims_message_t     bus1_message2;
    ims_message_t     bus2_message1;
    ims_message_t     bus2_message2;

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

    bus1_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_can, "bus1_m1", BUS1_M1_SIZE, 1, ims_output, &bus1_message1) == ims_no_error &&
                       bus1_message1 != (ims_message_t)INVALID_POINTER && bus1_message1 != NULL,
                       "We can get the bus1_message1.");

    bus1_message2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_can, "bus1_m2", BUS1_M2_SIZE, 1, ims_output, &bus1_message2) == ims_no_error &&
                       bus1_message2 != (ims_message_t)INVALID_POINTER && bus1_message2 != NULL,
                       "We can get the bus1_message2.");

    bus2_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_can, "bus2_m1", BUS2_M1_SIZE, 1, ims_output, &bus2_message1) == ims_no_error &&
                       bus2_message1 != (ims_message_t)INVALID_POINTER && bus2_message1 != NULL,
                       "We can get the bus2_message1.");

    bus2_message2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_can, "bus2_m2", BUS2_M2_SIZE, 1, ims_output, &bus2_message2) == ims_no_error &&
                       bus2_message2 != (ims_message_t)INVALID_POINTER && bus2_message2 != NULL,
                       "We can get the bus2_message2.");

    // Check send
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    // Check import
    TEST_ASSERT(actor, ims_import(ims_context, 10*1000) == ims_no_error, "ims_import return ims_no_error.");

    // Done
    ims_free_context(ims_context);
    return ims_test_end(actor);
}
