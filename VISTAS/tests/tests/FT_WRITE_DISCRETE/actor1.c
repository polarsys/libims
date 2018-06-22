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
// DISCRETE test - actor 1
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
#define DISC_TRUE  42
#define DISC_FALSE  0

#define GRP1_SIG1_DEFAULT  DISC_FALSE
#define GRP1_SIG2_DEFAULT  DISC_TRUE
#define GRP2_SIG1_DEFAULT  DISC_TRUE
#define GRP2_SIG2_DEFAULT  DISC_FALSE

#define GRP1_SIG1_SIZE  1
#define GRP1_SIG2_SIZE  4
#define GRP2_SIG1_SIZE  1
#define GRP2_SIG2_SIZE  4

#define SET_VALUE(grp, sig, value)                                  \
    do {                                                            \
    memset(payload, 0, 4);                                          \
    *(payload + GRP ## grp ## _SIG ## sig ## _SIZE - 1) = value;    \
    } while (0)

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
    char              payload[20];

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
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_discrete, "signal1", GRP1_SIG1_SIZE, 1, ims_output, &grp1_sig1) == ims_no_error &&
                       grp1_sig1 != (ims_message_t)INVALID_POINTER && grp1_sig1 != NULL,
                       "We can get the grp1_sig1.");

    grp1_sig2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_discrete, "signal2", GRP1_SIG2_SIZE, 1, ims_output, &grp1_sig2) == ims_no_error &&
                       grp1_sig2 != (ims_message_t)INVALID_POINTER && grp1_sig2 != NULL,
                       "We can get the grp1_sig2.");

    grp2_sig1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_discrete, "signal3", GRP2_SIG1_SIZE, 1, ims_output, &grp2_sig1) == ims_no_error &&
                       grp2_sig1 != (ims_message_t)INVALID_POINTER && grp2_sig1 != NULL,
                       "We can get the grp2_sig1.");

    grp2_sig2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_discrete, "signal4", GRP2_SIG2_SIZE, 1, ims_output, &grp2_sig2) == ims_no_error &&
                       grp2_sig2 != (ims_message_t)INVALID_POINTER && grp2_sig2 != NULL,
                       "We can get the grp2_sig2.");

    input_signal = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_discrete, "input_signal", 1, 1, ims_input, &input_signal) == ims_no_error &&
                       input_signal != (ims_message_t)INVALID_POINTER && input_signal != NULL,
                       "We can get the input_signal.");

    // Check invalid writes
    TEST_ASSERT(actor, ims_write_sampling_message(grp1_sig1, payload, GRP1_SIG1_SIZE + 1) == ims_message_invalid_size,
                "Cannot write an invalid size.");

    TEST_ASSERT(actor, ims_write_sampling_message(input_signal, payload, 1) == ims_invalid_configuration,
                "Cannot write an input signal.");

    // The first send will send all default values
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Tell actor2 we have sent
    TEST_WAIT(actor, 2);

    // Write messages but don't call send nor progress (nothing should be emmited)
    SET_VALUE(1, 1, DISC_TRUE);
    TEST_ASSERT(actor, ims_write_sampling_message(grp1_sig1, payload, GRP1_SIG1_SIZE) == ims_no_error, "grp1_sig1 write.");

    SET_VALUE(1, 2, DISC_TRUE);
    TEST_ASSERT(actor, ims_write_sampling_message(grp1_sig2, payload, GRP1_SIG2_SIZE) == ims_no_error, "grp1_sig2 write.");

    SET_VALUE(2, 1, GRP2_SIG1_DEFAULT);
    TEST_ASSERT(actor, ims_write_sampling_message(grp2_sig1, payload, GRP2_SIG1_SIZE) == ims_no_error, "grp2_sig1 write.");

    SET_VALUE(2, 2, GRP2_SIG2_DEFAULT);
    TEST_ASSERT(actor, ims_write_sampling_message(grp2_sig2, payload, GRP2_SIG2_SIZE) == ims_no_error, "grp2_sig2 write.");

    TEST_SIGNAL(actor, 2); // Tell actor2 we have sent
    TEST_WAIT(actor, 2);

    // Send. Discrete are send on change, but we don't have change anything in group2 (set to default value)
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Tell actor2 we have sent
    TEST_WAIT(actor, 2);

    // Call progress enought for group1, but not for group 2
    TEST_ASSERT(actor, ims_progress(ims_context, 50 * 1000) == ims_no_error, "Progress sucess, total 50ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Tell actor2 we have sent
    TEST_WAIT(actor, 2);

    // Call progress enought to reach group2 cycle, but not for group 1 again
    TEST_ASSERT(actor, ims_progress(ims_context, 20 * 1000) == ims_no_error, "Progress sucess, total 70ms.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have progress + sent
    TEST_WAIT(actor, 2);

    // Write several time grp1_sig1 and grp2_sig1, only last call should be sent
    SET_VALUE(1, 1, DISC_FALSE);
    TEST_ASSERT(actor, ims_write_sampling_message(grp1_sig1, payload, GRP1_SIG1_SIZE) == ims_no_error, "grp1_sig1 write.");

    SET_VALUE(1, 1, DISC_TRUE);
    TEST_ASSERT(actor, ims_write_sampling_message(grp1_sig1, payload, GRP1_SIG1_SIZE) == ims_no_error, "grp1_sig1 write.");

    SET_VALUE(2, 1, DISC_TRUE);
    TEST_ASSERT(actor, ims_write_sampling_message(grp2_sig1, payload, GRP2_SIG1_SIZE) == ims_no_error, "grp2_sig1 write.");

    SET_VALUE(2, 1, DISC_FALSE);
    TEST_ASSERT(actor, ims_write_sampling_message(grp2_sig1, payload, GRP2_SIG1_SIZE) == ims_no_error, "grp2_sig1 write.");

    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have sent

    // Done
    ims_free_context(ims_context);
    return ims_test_end(actor);
}
