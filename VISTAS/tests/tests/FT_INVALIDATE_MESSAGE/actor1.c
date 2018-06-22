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
// A429 test - actor 1
//
#include "ims_test.h"
#include "a429_tools.h"

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

#define INVALID_POINTER ((void*)42)

#define FILLED_BY_IMS 0

#define FILL_LABEL(label, value) \
    ims_test_a429_fill_label(label, FILLED_BY_IMS, FILLED_BY_IMS,  value, 1, FILLED_BY_IMS)

//
// Main
//
int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;

    ims_message_t AFDX_S_1;
    ims_message_t AFDX_S_2;
    ims_message_t A429_BUS1_S_LABEL;
    ims_message_t A429_BUS2_S_LABEL;
    ims_message_t CAN_BUS1_M1;
    ims_message_t CAN_BUS1_M2;
    ims_message_t CAN_BUS2_M1;
    ims_message_t CAN_BUS2_M2;

    char  payload[100];

    actor = ims_test_init(ACTOR_ID);

    TEST_WAIT(actor, 2);  // Wait actor2 ready

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

    AFDX_S_1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_afdx, "AFDX_S_1", 42, 1, ims_output, &AFDX_S_1) == ims_no_error &&
                       AFDX_S_1 != NULL,
                       "AFDX_S_1 create");

    AFDX_S_2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_afdx, "AFDX_S_2", 21, 1, ims_output, &AFDX_S_2) == ims_no_error &&
                       AFDX_S_2 != NULL,
                       "AFDX_S_2 create");

    A429_BUS1_S_LABEL = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_a429, "A429_BUS1_S_LABEL", 4, 1, ims_output, &A429_BUS1_S_LABEL) == ims_no_error &&
                       A429_BUS1_S_LABEL != NULL,
                       "A429_BUS1_S_LABEL create");

    A429_BUS2_S_LABEL = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_a429, "A429_BUS2_S_LABEL", 4, 1, ims_output, &A429_BUS2_S_LABEL) == ims_no_error &&
                       A429_BUS2_S_LABEL != NULL,
                       "A429_BUS2_S_LABEL create");

    CAN_BUS1_M1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_can, "CAN_BUS1_M1", 4, 1, ims_output, &CAN_BUS1_M1) == ims_no_error &&
                       CAN_BUS1_M1 != NULL,
                       "CAN_BUS1_M1 create");

    CAN_BUS1_M2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_can, "CAN_BUS1_M2", 8, 1, ims_output, &CAN_BUS1_M2) == ims_no_error &&
                       CAN_BUS1_M2 != NULL,
                       "CAN_BUS1_M2 create");

    CAN_BUS2_M1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_can, "CAN_BUS2_M1", 8, 1, ims_output, &CAN_BUS2_M1) == ims_no_error &&
                       CAN_BUS2_M1 != NULL,
                       "CAN_BUS2_M1 create");

    CAN_BUS2_M2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_can, "CAN_BUS2_M2", 4, 1, ims_output, &CAN_BUS2_M2) == ims_no_error &&
                       CAN_BUS2_M2 != NULL,
                       "CAN_BUS2_M2 create");

    // Fill everything
    memset(payload, 0, 100); strcpy(payload, "First AFDX Message");
    TEST_ASSERT(actor, ims_write_sampling_message(AFDX_S_1, payload, 42) == ims_no_error, "AFDX_S_1 wrote.");
    memset(payload, 0, 100); strcpy(payload, "Second AFDX Message");
    TEST_ASSERT(actor, ims_write_sampling_message(AFDX_S_2, payload, 21) == ims_no_error, "AFDX_S_2 wrote.");

    FILL_LABEL(payload, 0x42);
    TEST_ASSERT(actor, ims_write_sampling_message(A429_BUS1_S_LABEL, payload, 4) == ims_no_error, "A429_BUS1_S_LABEL wrote.");
    FILL_LABEL(payload, 0x21);
    TEST_ASSERT(actor, ims_write_sampling_message(A429_BUS2_S_LABEL, payload, 4) == ims_no_error, "A429_BUS2_S_LABEL wrote.");

    memset(payload, 84, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS1_M1, payload, 4) == ims_no_error, "CAN_BUS1_M1 wrote.");
    memset(payload, 85, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS1_M2, payload, 8) == ims_no_error, "CAN_BUS1_M2 wrote.");
    memset(payload, 86, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS2_M1, payload, 8) == ims_no_error, "CAN_BUS2_M1 wrote.");
    memset(payload, 87, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS2_M2, payload, 4) == ims_no_error, "CAN_BUS2_M2 wrote.");

    // Check invalidate on emmiter does nothing
    TEST_ASSERT(actor, ims_invalidate_sampling_message(AFDX_S_1) == ims_no_error, "No errors on invalidate AFDX_S_1.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(AFDX_S_2) == ims_no_error, "No errors on invalidate AFDX_S_2.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(A429_BUS1_S_LABEL) == ims_no_error, "No errors on invalidate A429_BUS1_S_LABEL.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(A429_BUS2_S_LABEL) == ims_no_error, "No errors on invalidate A429_BUS2_S_LABEL.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(CAN_BUS1_M1) == ims_no_error, "No errors on invalidate CAN_BUS1_M1.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(CAN_BUS1_M2) == ims_no_error, "No errors on invalidate CAN_BUS1_M2.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(CAN_BUS2_M1) == ims_no_error, "No errors on invalidate CAN_BUS2_M1.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(CAN_BUS2_M2) == ims_no_error, "No errors on invalidate CAN_BUS2_M2.");

    // Ok, send
    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "ims_progress return ims_no_error.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have sent
    TEST_WAIT(actor, 2);

    // Fill everything again
    memset(payload, 0, 100); strcpy(payload, "First AFDX Message");
    TEST_ASSERT(actor, ims_write_sampling_message(AFDX_S_1, payload, 42) == ims_no_error, "AFDX_S_1 wrote.");
    memset(payload, 0, 100); strcpy(payload, "Second AFDX Message");
    TEST_ASSERT(actor, ims_write_sampling_message(AFDX_S_2, payload, 21) == ims_no_error, "AFDX_S_2 wrote.");

    FILL_LABEL(payload, 0x42);
    TEST_ASSERT(actor, ims_write_sampling_message(A429_BUS1_S_LABEL, payload, 4) == ims_no_error, "A429_BUS1_S_LABEL wrote.");
    FILL_LABEL(payload, 0x21);
    TEST_ASSERT(actor, ims_write_sampling_message(A429_BUS2_S_LABEL, payload, 4) == ims_no_error, "A429_BUS2_S_LABEL wrote.");

    memset(payload, 84, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS1_M1, payload, 4) == ims_no_error, "CAN_BUS1_M1 wrote.");
    memset(payload, 85, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS1_M2, payload, 8) == ims_no_error, "CAN_BUS1_M2 wrote.");
    memset(payload, 86, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS2_M1, payload, 8) == ims_no_error, "CAN_BUS2_M1 wrote.");
    memset(payload, 87, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS2_M2, payload, 4) == ims_no_error, "CAN_BUS2_M2 wrote.");

    // Ok, send
    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "ims_progress return ims_no_error.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have sent

    // Done
    ims_free_context(ims_context);
    return ims_test_end(actor);
}
