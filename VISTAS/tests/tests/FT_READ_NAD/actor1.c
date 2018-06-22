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
// NAD test - actor 1
//
#include "ims_test.h"

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE1_SIZE  4
#define MESSAGE2_SIZE  1
#define MESSAGE3_SIZE  8
#define MESSAGE4_SIZE 10

#define INVALID_POINTER ((void*)42)

//
// Main
//
int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     grp1_msg1;
    ims_message_t     grp1_msg2;
    ims_message_t     grp2_msg1;
    ims_message_t     grp2_msg2;
    char              data[20];

    // Internal data
    uint32_t data_internal_max_size = 100;
    char data_internal[data_internal_max_size];
    uint32_t data_internal_size;

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

    grp1_msg1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message1", MESSAGE1_SIZE, 1, ims_output, &grp1_msg1) == ims_no_error &&
                       grp1_msg1 != (ims_message_t)INVALID_POINTER && grp1_msg1 != NULL,
                       "We can get the grp1_msg1.");

    grp1_msg2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message2", MESSAGE2_SIZE, 1, ims_output, &grp1_msg2) == ims_no_error &&
                       grp1_msg2 != (ims_message_t)INVALID_POINTER && grp1_msg2 != NULL,
                       "We can get the grp1_msg2.");

    grp2_msg1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message3", MESSAGE3_SIZE, 1, ims_output, &grp2_msg1) == ims_no_error &&
                       grp2_msg1 != (ims_message_t)INVALID_POINTER && grp2_msg1 != NULL,
                       "We can get the grp2_msg1.");

    grp2_msg2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "message4", MESSAGE4_SIZE, 1, ims_output, &grp2_msg2) == ims_no_error &&
                       grp2_msg2 != (ims_message_t)INVALID_POINTER && grp2_msg2 != NULL,
                       "We can get the grp2_msg2.");

    // Fill all data and send them
    memset(data, 5, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp1_msg1, data, MESSAGE1_SIZE) == ims_no_error, "grp1_msg1 write.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp1_msg1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "grp1_msg1 internal data retrieved.");

    TEST_ASSERT(actor, data_internal_size == MESSAGE1_SIZE, "Retrieved grp1_msg1 internal data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, (char*)&data, MESSAGE1_SIZE) == 0, "Retrived grp1_msg1 internal data is good.");
#endif

    memset(data, 6, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp1_msg2, data, MESSAGE2_SIZE) == ims_no_error, "grp1_msg2 write.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp1_msg2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "grp1_msg2 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE2_SIZE, "Retrieved grp1_msg2 internal data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, (char*)&data, MESSAGE2_SIZE) == 0, "Retrived grp1_msg2 internal data is good.");
#endif

    memset(data, 7, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp2_msg1, data, MESSAGE3_SIZE) == ims_no_error, "grp2_msg1 write.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp2_msg1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "grp2_msg1 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE3_SIZE, "Retrieved grp2_msg1 internal data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, (char*)&data, MESSAGE3_SIZE) == 0, "Retrived grp2_msg1 internal data is good.");
#endif

    memset(data, 8, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp2_msg2, data, MESSAGE4_SIZE) == ims_no_error, "grp2_msg2 write.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(grp2_msg2, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "grp2_msg2 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE4_SIZE, "Retrieved grp2_msg2 internal data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, (char*)&data, MESSAGE4_SIZE) == 0, "Retrived grp2_msg2 internal data is good.");
#endif

    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "ims progress success (total 100ms).");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    // Send several time the same message
    memset(data, 10, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp1_msg1, data, MESSAGE1_SIZE) == ims_no_error, "grp1_msg1 write.");
    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "ims progress success (total 100ms).");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    memset(data, 11, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp1_msg1, data, MESSAGE1_SIZE) == ims_no_error, "grp1_msg1 write.");
    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "ims progress success (total 100ms).");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    memset(data, 12, 10);
    TEST_ASSERT(actor, ims_write_nad_message(grp1_msg1, data, MESSAGE1_SIZE) == ims_no_error, "grp1_msg1 write.");
    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "ims progress success (total 100ms).");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2);

    // Done
    ims_free_context(ims_context);

    return ims_test_end(actor);
}
