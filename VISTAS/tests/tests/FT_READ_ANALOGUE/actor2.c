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
// Analogue test - actor 2
//
#include "ims_test.h"
#include "math.h"

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"
#define IMS_INIT_FILE        "config/actor2/init.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE_SIZE 4

static const float GRP1_SIG1_DEFAULT = 0;
static const float GRP1_SIG2_DEFAULT = 42;
static const float GRP2_SIG1_DEFAULT = 1.5;
static const float GRP2_SIG2_DEFAULT = 0;

static const float GRP1_SIG1_ACTOR1_DEFAULT = 11.2;
static const float GRP1_SIG2_ACTOR1_DEFAULT = 12.0;
static const float GRP2_SIG1_ACTOR1_DEFAULT = 0;
static const float GRP2_SIG2_ACTOR1_DEFAULT = 0;

static const float EPSILON = 0.00001;
static int fequalto(float a, float b)
{
    return fabs(a - b) < EPSILON;
}

#define GROUP1_IP             "226.23.12.4"
#define GROUP1_PORT           5078

#define GROUP2_IP             "226.23.12.4"
#define GROUP2_PORT           5079

#define INVALID_POINTER ((void*)42)

int main()
{
    ims_node_t        ims_context;
    ims_node_t        ims_equipment;
    ims_node_t        ims_application;
    ims_message_t     grp1_sig1;
    ims_message_t     grp1_sig2;
    ims_message_t     grp2_sig1;
    ims_message_t     grp2_sig2;
    ims_message_t     output_signal;

    ims_validity_t    validity;
    uint32_t          received_size;
    char*             received_payload = (char*)malloc(8); // I cannot alias float* to char[4] ????

    // Internal data
    uint32_t data_internal_max_size = 100;
    char data_internal[data_internal_max_size];
    uint32_t data_internal_size;

    actor = ims_test_init(ACTOR_ID);

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
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_analogue, "signal1", MESSAGE_SIZE, 1, ims_input, &grp1_sig1) == ims_no_error &&
                       grp1_sig1 != (ims_message_t)INVALID_POINTER && grp1_sig1 != NULL,
                       "We can get the grp1_sig1.");

    grp1_sig2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_analogue, "signal2", MESSAGE_SIZE, 1, ims_input, &grp1_sig2) == ims_no_error &&
                       grp1_sig2 != (ims_message_t)INVALID_POINTER && grp1_sig2 != NULL,
                       "We can get the grp1_sig2.");

    grp2_sig1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_analogue, "signal3", MESSAGE_SIZE, 1, ims_input, &grp2_sig1) == ims_no_error &&
                       grp2_sig1 != (ims_message_t)INVALID_POINTER && grp2_sig1 != NULL,
                       "We can get the grp2_sig1.");

    grp2_sig2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_analogue, "signal4", MESSAGE_SIZE, 1, ims_input, &grp2_sig2) == ims_no_error &&
                       grp2_sig2 != (ims_message_t)INVALID_POINTER && grp2_sig2 != NULL,
                       "We can get the grp2_sig2.");

    output_signal = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_analogue, "output_signal", MESSAGE_SIZE, 1, ims_output, &output_signal) == ims_no_error &&
                       output_signal != (ims_message_t)INVALID_POINTER && output_signal != NULL,
                       "We can get the output_signal.");

    // Check empty messages
    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == 0, "grp1_sig1 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp1_Sig1 is valid.");

// #ifdef ENABLE_INSTRUMENTATION
    // memset(data_internal, 0, data_internal_max_size);
    // TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp1_sig1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                // "grp1_sig1 internal data retrieved.");
    // TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved grp1_sig1 internal data has the good size.");
    // TEST_ASSERT(actor, memcmp(received_payload, data_internal, MESSAGE_SIZE) == 0, "Retrieved grp1_sig1 internal data is good.");
// #endif

    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "grp1_sig2 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "grp1_sig2 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, GRP1_SIG2_DEFAULT), "grp1_sig2 has the expected value.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp1_sig2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "grp1_sig2 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved grp1_sig2 internal data has the good size.");
    TEST_ASSERT(actor, memcmp(received_payload, data_internal, MESSAGE_SIZE) == 0, "Retrieved grp1_sig2 internal data is good.");
#endif

    TEST_ASSERT(actor, ims_read_sampling_message(grp2_sig1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp2_Sig1 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Grp2_Sig1 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, GRP2_SIG1_DEFAULT), "grp2_sig1 has the expected value.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp2_sig1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "grp2_sig1 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved grp2_sig1 internal data has the good size.");
    TEST_ASSERT(actor, memcmp(received_payload, data_internal, MESSAGE_SIZE) == 0, "Retrieved grp2_sig1 internal data is good.");
#endif

    TEST_ASSERT(actor, ims_read_sampling_message(grp2_sig2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == 0, "Grp2_Sig2 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp2_Sig2 is valid.");

// #ifdef ENABLE_INSTRUMENTATION
    // memset(data_internal, 0, data_internal_max_size);
    // TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp2_sig2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                // "grp2_sig2 internal data retrieved.");
    // TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved grp2_sig2 internal data has the good size.");
    // TEST_ASSERT(actor, memcmp(received_payload, data_internal, MESSAGE_SIZE) == 0, "Retrieved grp2_sig2 internal data is good.");
// #endif

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == 0, "Grp1_Sig1 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp1_Sig1 is valid.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp1_Sig2 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Grp1_Sig2 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, GRP1_SIG2_DEFAULT), "grp1_sig2 has the expected value.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp2_sig1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp2_Sig1 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Grp2_Sig1 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, GRP2_SIG1_DEFAULT), "grp2_sig1 has the expected value.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp2_sig2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == 0, "Grp2_Sig2 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp2_Sig2 is valid.");

    // Invalid configuration test
    TEST_ASSERT(actor, ims_read_sampling_message(output_signal, received_payload, &received_size, &validity) == ims_invalid_configuration,
                "Reading output message return ims_invalid_configuration.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write
    TEST_WAIT(actor, 1);

    // Without import, message still hat their default value
    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == 0, "Grp1_Sig1 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp1_Sig1 is valid.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp1_Sig2 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Grp1_Sig2 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, GRP1_SIG2_DEFAULT), "grp1_sig2 has the expected value.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp2_sig1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp2_Sig1 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received_but_initialized, "Grp2_Sig1 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, GRP2_SIG1_DEFAULT), "grp2_sig1 has the expected value.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp2_sig2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == 0, "Grp2_Sig2 has the good size.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp2_Sig2 is valid.");

    // After import, all messages are filled in
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp1_Sig1 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp1_Sig1 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, 5), "grp1_sig1 has the expected value.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp1_Sig2 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp1_Sig2 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, 6), "grp1_sig2 has the expected value.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp2_sig1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp2_Sig1 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp2_Sig1 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, 7), "grp2_sig1 has the expected value.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp2_sig2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp2_Sig2 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp2_Sig2 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, 8), "grp2_sig2 has the expected value.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write several times
    TEST_WAIT(actor, 1);

    // Actor 1 has send several times, import sould drop all values except the last one
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp1_Sig1 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp1_Sig1 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, 13), "grp1_sig1 has the expected value.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp1_Sig2 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp1_Sig2 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, 13), "grp1_sig2 has the expected value.");


    // Validity tests : grp1_sig1 is always valid, grp1_sig2 is not
    TEST_ASSERT(actor, ims_progress(ims_context, 500 * 1000) == ims_no_error, "ims progress success. (total 500ms)");

    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp1_Sig1 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp1_Sig1 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, 13), "grp1_sig1 has the expected value.");

    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp1_Sig2 has the good size.");
    TEST_ASSERT(actor, validity == ims_invalid, "Grp1_Sig2 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, 13), "grp1_sig2 has the expected value.");


    // We can update validity on analogue signal
    TEST_ASSERT(actor, ims_message_set_sampling_timeout(grp1_sig1, 10 * 1000) == ims_no_error, "We can update analogue validity");

    TEST_ASSERT(actor, ims_read_sampling_message(grp1_sig1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "Grp1_Sig1 has the good size.");
    TEST_ASSERT(actor, validity == ims_invalid, "Grp1_Sig1 is valid.");
    TEST_ASSERT(actor, fequalto(*(float*)received_payload, 13), "grp1_sig1 has the expected value.");

    ims_free_context(ims_context);
    free(received_payload);

    return ims_test_end(actor);
}
