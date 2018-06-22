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
// VISTAS header test - actor 1
//
#include "ims_test.h"
#include "a429_tools.h"

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"
#define IMS_INIT_FILE        "config/actor1/init.xml"

#define ACTOR_ID 1
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
    char              data[100];

    actor = ims_test_init(ACTOR_ID);

    memset(data, 0, sizeof(data) );

    TEST_WAIT(actor, 2); // Wait actor2 ready to read before starting our tests

    ims_create_context_parameter_t create_parameter = IMS_CREATE_CONTEXT_INITIALIZER;
    create_parameter.init_file_path = IMS_INIT_FILE;

    ims_context = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE, &create_parameter, &ims_context) == ims_no_error &&
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

    // test a NAD message with everything
    ims_message_t NAD_Message1_O = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_context, ims_nad, "NAD_Message1_O", 1, 1, ims_output, &NAD_Message1_O) == ims_no_error &&
                       NAD_Message1_O != (ims_message_t)INVALID_POINTER && NAD_Message1_O != NULL,
                       "We can get the NAD_Message1_O.");

    TEST_ASSERT(actor, ims_write_nad_message(NAD_Message1_O, data, 1) == ims_no_error, "NAD_Message1_O wrote.");

    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "Progress sucess, total 100ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    // test a A429 with only the prod_id
    ims_message_t bus1_l1_O = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l1_O", 4, 1, ims_output, &bus1_l1_O) == ims_no_error &&
                       bus1_l1_O != (ims_message_t)INVALID_POINTER && bus1_l1_O != NULL,
                       "We can get the bus1_l1_O.");

    TEST_ASSERT(actor, ims_write_sampling_message(bus1_l1_O, data, 4) == ims_no_error, "bus1_l1_O wrote.");

    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "Progress sucess, total 200ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    // test a CAN with only the SN

    ims_message_t bus1_m1_O = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_can, "bus1_m1_O", 4, 1, ims_output, &bus1_m1_O) == ims_no_error &&
                       bus1_m1_O != (ims_message_t)INVALID_POINTER && bus1_m1_O != NULL,
                       "We can get the bus1_m1_O.");

    TEST_ASSERT(actor, ims_write_sampling_message(bus1_m1_O, data, 4) == ims_no_error, "bus1_m1_O wrote.");

    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "Progress sucess, total 300ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_write_sampling_message(bus1_m1_O, data, 4) == ims_no_error, "bus1_m1_O wrote.");

    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "Progress sucess, total 400ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_reset_all(ims_context) == ims_no_error, "All messages reset.");
    TEST_ASSERT(actor, ims_write_sampling_message(bus1_m1_O, data, 4) == ims_no_error, "bus1_m1_O wrote.");

    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "Progress sucess, total 400ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    // test an AFDX with only the POSIX timestamp
    ims_message_t AFDXLocalName_O = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalName_O", 42, 1, ims_output, &AFDXLocalName_O) == ims_no_error &&
                       AFDXLocalName_O != (ims_message_t)INVALID_POINTER && AFDXLocalName_O != NULL,
                       "We can get the AFDXLocalName_O.");

    TEST_ASSERT(actor, ims_write_sampling_message(AFDXLocalName_O, data, 42) == ims_no_error, "AFDXLocalName_O wrote.");


    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "Progress sucess, total 500ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    // test a discrete with only the data timestamp
    ims_message_t DISC_signal1_O = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_discrete, "DISC_signal1_O", 1, 1, ims_output, &DISC_signal1_O) == ims_no_error &&
                       DISC_signal1_O != (ims_message_t)INVALID_POINTER && DISC_signal1_O != NULL,
                       "We can get the DISC_signal1_O.");

    TEST_ASSERT(actor, ims_write_sampling_message(DISC_signal1_O, data, 1) == ims_no_error, "DISC_signal1_O wrote.");


    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "Progress sucess, total 600ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    // Done
    TEST_SIGNAL(actor, 2);
    ims_free_context(ims_context);

    return ims_test_end(actor);
}
