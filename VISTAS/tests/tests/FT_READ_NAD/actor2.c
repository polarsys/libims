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
// Nad test - actor 2
//
#include "ims_test.h"
#include "math.h"

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE1_SIZE  4
#define MESSAGE2_SIZE  1
#define MESSAGE3_SIZE  8
#define MESSAGE4_SIZE 10

#define INVALID_POINTER ((void*)42)

int main()
{
    ims_node_t        ims_context;
    ims_node_t        ims_equipment;
    ims_node_t        ims_application;
    ims_message_t     grp1_msg1;
    ims_message_t     grp1_msg2;
    ims_message_t     grp2_msg1;
    ims_message_t     grp2_msg2;
    ims_message_t     output_message;

    ims_validity_t    validity;
    uint32_t          received_size;
    char              received_payload[20];
    char              expected_payload[20];

    // Internal data
    uint32_t data_internal_max_size = 100;
    char data_internal[data_internal_max_size];
    uint32_t data_internal_size;

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

    grp1_msg1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message1", MESSAGE1_SIZE, 1, ims_input, &grp1_msg1) == ims_no_error &&
                       grp1_msg1 != (ims_message_t)INVALID_POINTER && grp1_msg1 != NULL,
                       "We can get the grp1_msg1.");

    grp1_msg2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message2", MESSAGE2_SIZE, 1, ims_input, &grp1_msg2) == ims_no_error &&
                       grp1_msg2 != (ims_message_t)INVALID_POINTER && grp1_msg2 != NULL,
                       "We can get the grp1_msg2.");

    grp2_msg1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message3", MESSAGE3_SIZE, 1, ims_input, &grp2_msg1) == ims_no_error &&
                       grp2_msg1 != (ims_message_t)INVALID_POINTER && grp2_msg1 != NULL,
                       "We can get the grp2_msg1.");

    grp2_msg2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message4", MESSAGE4_SIZE, 1, ims_input, &grp2_msg2) == ims_no_error &&
                       grp2_msg2 != (ims_message_t)INVALID_POINTER && grp2_msg2 != NULL,
                       "We can get the grp2_msg2.");

    output_message = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "output_message", 4, 1, ims_output, &output_message) == ims_no_error &&
                       output_message != (ims_message_t)INVALID_POINTER && output_message != NULL,
                       "We can get the output_message.");

    // Check empty messages
    memset(received_payload, 42, 10);
    memset(expected_payload, 42, 10);

    TEST_ASSERT(actor, ims_read_nad_message(grp1_msg1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp1_Msg1 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    TEST_ASSERT(actor, ims_read_nad_message(grp1_msg2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp1_Msg2 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    TEST_ASSERT(actor, ims_read_nad_message(grp2_msg1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp2_Msg1 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    TEST_ASSERT(actor, ims_read_nad_message(grp2_msg2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp2_Msg2 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_read_nad_message(grp1_msg1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message after import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp1_Msg1 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    TEST_ASSERT(actor, ims_read_nad_message(grp1_msg2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message after import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp1_Msg2 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    TEST_ASSERT(actor, ims_read_nad_message(grp2_msg1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message after import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp2_Msg1 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    TEST_ASSERT(actor, ims_read_nad_message(grp2_msg2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message after import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp2_Msg2 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    // Invalid configuration test
    TEST_ASSERT(actor, ims_read_nad_message(output_message, received_payload, &received_size, &validity) == ims_invalid_configuration,
                "Reading output message return ims_invalid_configuration.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write
    TEST_WAIT(actor, 1);

    // Without import, message still at their default value
    TEST_ASSERT(actor, ims_read_nad_message(grp1_msg1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp1_Msg1 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    TEST_ASSERT(actor, ims_read_nad_message(grp1_msg2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp1_Msg2 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    TEST_ASSERT(actor, ims_read_nad_message(grp2_msg1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp2_Msg1 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    TEST_ASSERT(actor, ims_read_nad_message(grp2_msg2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message without any import return no_error.");
    TEST_ASSERT(actor, validity == ims_never_received, "Grp2_Msg2 has never been received.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, 10) == 0, "Payload has not been modified.");

    // After import, all messages are filled in
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    memset(expected_payload, 5, 10);
    TEST_ASSERT(actor, ims_read_nad_message(grp1_msg1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message after import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "Grp1_Msg1 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp1_Msg1 is valid.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, MESSAGE1_SIZE) == 0, "Payload has the expected content.");

    memset(data_internal, 0, data_internal_max_size);
#ifdef ENABLE_INSTRUMENTATION
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp1_msg1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "grp1_msg1 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE1_SIZE, "Retrieved grp1_msg1 internal data has the good size.");
    TEST_ASSERT(actor, memcmp(received_payload, data_internal, MESSAGE1_SIZE) == 0, "Retrieved grp1_msg1 internal data is good.");
#endif

    memset(expected_payload, 6, 10);
    TEST_ASSERT(actor, ims_read_nad_message(grp1_msg2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message after import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE2_SIZE, "Grp1_Msg2 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp1_Msg2 is valid.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, MESSAGE2_SIZE) == 0, "Payload has the expected content.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp1_msg2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "grp1_msg2 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE2_SIZE, "Retrieved grp1_msg2 internal data has the good size.");
    TEST_ASSERT(actor, memcmp(received_payload, data_internal, MESSAGE2_SIZE) == 0, "Retrieved grp1_msg2 internal data is good.");
#endif

    memset(expected_payload, 7, 10);
    TEST_ASSERT(actor, ims_read_nad_message(grp2_msg1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message after import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE3_SIZE, "Grp2_Msg1 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp2_Msg1 is valid.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, MESSAGE3_SIZE) == 0, "Payload has the expected content.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp2_msg1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "grp2_msg1 internal data retrieved.");

    TEST_ASSERT(actor, data_internal_size == MESSAGE3_SIZE, "Retrieved grp2_msg1 internal data has the good size.");
    TEST_ASSERT(actor, memcmp(received_payload, data_internal, MESSAGE3_SIZE) == 0, "Retrieved grp2_msg1 internal data is good.");
#endif

    memset(expected_payload, 8, 10);
    TEST_ASSERT(actor, ims_read_nad_message(grp2_msg2, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message after import return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE4_SIZE, "Grp2_Msg2 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp2_Msg2 is valid.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, MESSAGE4_SIZE) == 0, "Payload has the expected content.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp2_msg2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "grp2_msg2 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE4_SIZE, "Retrieved grp2_msg2 internal data has the good size.");
    TEST_ASSERT(actor, memcmp(received_payload, data_internal, MESSAGE4_SIZE) == 0, "Retrieved grp2_msg2 internal data is good.");
#endif

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write several times
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    // Actor 1 has send several times, import sould drop all values except the last one
    memset(expected_payload, 12, 10);
    TEST_ASSERT(actor, ims_read_nad_message(grp1_msg1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "Grp1_Msg1 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp1_Msg1 is valid.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, MESSAGE1_SIZE) == 0, "Payload has the expected content.");

    // Validity tests : nad are always valid
    TEST_ASSERT(actor, ims_progress(ims_context, 500 * 1000) == ims_no_error, "ims progress success. (total 500ms)");

    memset(expected_payload, 12, 10);
    TEST_ASSERT(actor, ims_read_nad_message(grp1_msg1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "Grp1_Msg1 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp1_Msg1 still valid.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, MESSAGE1_SIZE) == 0, "Payload has the expected content.");

    // We can't update validity on nad message
    TEST_ASSERT(actor, ims_message_set_sampling_timeout(grp1_msg1, 10 * 1000) == ims_invalid_configuration, "We can't update nad validity");

    memset(expected_payload, 12, 10);
    TEST_ASSERT(actor, ims_read_nad_message(grp1_msg1, received_payload, &received_size, &validity) == ims_no_error,
                "Reading message return no_error.");
    TEST_ASSERT(actor, received_size == MESSAGE1_SIZE, "Grp1_Msg1 has the good size.");
    TEST_ASSERT(actor, validity == ims_valid, "Grp1_Msg1 still valid.");
    TEST_ASSERT(actor, memcmp(expected_payload, received_payload, MESSAGE1_SIZE) == 0, "Payload has the expected content.");

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
