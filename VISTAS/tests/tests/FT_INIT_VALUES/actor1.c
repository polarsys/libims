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
#include "a429_tools.h"

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE_AFDX_SIZE     42

static const char message1_payload[MESSAGE_AFDX_SIZE] = "hello, world!";

#define MESSAGE_A429_SIZE 4

#define BUS1_LABEL1_SDI     1  //01
#define BUS1_LABEL1_NUMBER  ims_test_a429_label_number_encode("024")

#define FILLED_BY_IMS 0

#define BUS1_M1_SIZE 4

#define INVALID_POINTER ((void*)42)

//
// Main
//
int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     ims_message_afdx;
    ims_message_t     bus1_label1;
    ims_message_t     bus1_message1;

    char              bus1_label1_payload[4];
    ims_test_a429_fill_label(bus1_label1_payload, FILLED_BY_IMS, FILLED_BY_IMS,  0x7FFFF, 1, FILLED_BY_IMS);

    char bus1_message1_payload[BUS1_M1_SIZE] = { 0x12, 0x23, 0x36, 0x77 };

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

    ims_message_afdx = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalName", MESSAGE_AFDX_SIZE, 1, ims_output, &ims_message_afdx) == ims_no_error &&
                       ims_message_afdx != (ims_message_t)INVALID_POINTER && ims_message_afdx != NULL,
                       "We can get the first AFDX message.");

    bus1_label1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l1", MESSAGE_A429_SIZE, 1, ims_output, &bus1_label1) == ims_no_error &&
                       bus1_label1 != (ims_message_t)INVALID_POINTER && bus1_label1 != NULL,
                       "We can get the bus1_label1.");

    bus1_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_can, "bus1_m1", BUS1_M1_SIZE, 1, ims_output, &bus1_message1) == ims_no_error &&
                       bus1_message1 != (ims_message_t)INVALID_POINTER && bus1_message1 != NULL,
                       "We can get the bus1_message1.");

    // Write both messages and send them
    TEST_ASSERT(actor, ims_write_sampling_message(ims_message_afdx, message1_payload, MESSAGE_AFDX_SIZE) == ims_no_error,
                "Message 1 wrote.");

    TEST_ASSERT(actor, ims_write_sampling_message(bus1_label1, bus1_label1_payload, MESSAGE_A429_SIZE) == ims_no_error,
                "bus1_label1 wrote.");

    TEST_ASSERT(actor, ims_write_sampling_message(bus1_message1, bus1_message1_payload, BUS1_M1_SIZE) == ims_no_error, "bus1_message1 wrote.");

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "Messages sent.");

    TEST_SIGNAL(actor, 2); // We have sent

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
