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
// ANALOGUE test - actor 1
//
#include "ims_test.h"

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"
#define IMS_INIT_FILE        "config/actor1/init.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE_SIZE 4

#define INVALID_POINTER ((void*)42)

//
// Main
//
int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     grp1_sig1;
    ims_message_t     grp1_sig2;
    ims_message_t     grp2_sig1;
    ims_message_t     grp2_sig2;
    ims_message_t     input_signal;
    float             data = 0;

    actor = ims_test_init(ACTOR_ID);

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

    grp1_sig1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_analogue, "signal1", MESSAGE_SIZE, 1, ims_output, &grp1_sig1) == ims_no_error &&
                       grp1_sig1 != (ims_message_t)INVALID_POINTER && grp1_sig1 != NULL,
                       "We can get the grp1_sig1.");

    grp1_sig2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_analogue, "signal2", MESSAGE_SIZE, 1, ims_output, &grp1_sig2) == ims_no_error &&
                       grp1_sig2 != (ims_message_t)INVALID_POINTER && grp1_sig2 != NULL,
                       "We can get the grp1_sig2.");

    grp2_sig1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_analogue, "signal3", MESSAGE_SIZE, 1, ims_output, &grp2_sig1) == ims_no_error &&
                       grp2_sig1 != (ims_message_t)INVALID_POINTER && grp2_sig1 != NULL,
                       "We can get the grp2_sig1.");

    grp2_sig2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_analogue, "signal4", MESSAGE_SIZE, 1, ims_output, &grp2_sig2) == ims_no_error &&
                       grp2_sig2 != (ims_message_t)INVALID_POINTER && grp2_sig2 != NULL,
                       "We can get the grp2_sig2.");

    input_signal = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_analogue, "input_signal", MESSAGE_SIZE, 1, ims_input, &input_signal) == ims_no_error &&
                       input_signal != (ims_message_t)INVALID_POINTER && input_signal != NULL,
                       "We can get the input_signal.");

    // Check invalid writes
    TEST_ASSERT(actor, ims_write_sampling_message(grp1_sig1, (char*)&data, MESSAGE_SIZE + 1) == ims_message_invalid_size,
                "Cannot write an invalid size.");

    TEST_ASSERT(actor, ims_write_sampling_message(input_signal, (char*)&data, MESSAGE_SIZE) == ims_invalid_configuration,
                "Cannot write an input signal.");

    // The first send will send all default values
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Tell actor2 we have sent
    TEST_WAIT(actor, 2);

    // Write messages but don't call send nor progress
    data = 5;
    TEST_ASSERT(actor, ims_write_sampling_message(grp1_sig1, (char*)&data, MESSAGE_SIZE) == ims_no_error, "grp1_sig1 wrote.");

    data = 6;
    TEST_ASSERT(actor, ims_write_sampling_message(grp1_sig2, (char*)&data, MESSAGE_SIZE) == ims_no_error, "grp1_sig2 wrote.");

    data = 7;
    TEST_ASSERT(actor, ims_write_sampling_message(grp2_sig1, (char*)&data, MESSAGE_SIZE) == ims_no_error, "grp2_sig1 wrote.");

    data = 8;
    TEST_ASSERT(actor, ims_write_sampling_message(grp2_sig2, (char*)&data, MESSAGE_SIZE) == ims_no_error, "grp2_sig2 wrote.");

    TEST_SIGNAL(actor, 2); // Tell actor2 we have write
    TEST_WAIT(actor, 2);

    // Call send without progress: nothing should be send, analogue are send periodically
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

    // Analogue are sent periodically, we only progress+send (not wire) and message will be send again
    TEST_ASSERT(actor, ims_progress(ims_context, 50 * 1000) == ims_no_error, "Progress sucess, total 120ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have progress + sent
    TEST_WAIT(actor, 2);

    // Write several time grp1_sig1 and grp2_sig1
    data = 12;
    TEST_ASSERT(actor, ims_write_sampling_message(grp1_sig1, (char*)&data, MESSAGE_SIZE) == ims_no_error, "grp1_sig1 wrote.");
    data = 14;
    TEST_ASSERT(actor, ims_write_sampling_message(grp1_sig1, (char*)&data, MESSAGE_SIZE) == ims_no_error, "grp1_sig1 wrote.");
    data = 16;
    TEST_ASSERT(actor, ims_write_sampling_message(grp1_sig1, (char*)&data, MESSAGE_SIZE) == ims_no_error, "grp1_sig1 wrote.");

    data = 45;
    TEST_ASSERT(actor, ims_write_sampling_message(grp2_sig1, (char*)&data, MESSAGE_SIZE) == ims_no_error, "grp2_sig1 wrote.");
    data = 4.5;
    TEST_ASSERT(actor, ims_write_sampling_message(grp2_sig1, (char*)&data, MESSAGE_SIZE) == ims_no_error, "grp2_sig1 wrote.");
    data = 11.9;
    TEST_ASSERT(actor, ims_write_sampling_message(grp2_sig1, (char*)&data, MESSAGE_SIZE) == ims_no_error, "grp2_sig1 wrote.");

    TEST_ASSERT(actor, ims_progress(ims_context, 120 * 1000) == ims_no_error, "Progress sucess, total 240ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have write + progress + sent

    // Done
    ims_free_context(ims_context);
    return ims_test_end(actor);
}
