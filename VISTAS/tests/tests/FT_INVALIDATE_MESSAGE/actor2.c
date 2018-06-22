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
// A429 test - actor 2
//
#include "ims_test.h"
#include "a429_tools.h"
#include "math.h"

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

#define INVALID_POINTER ((void*)42)

#define CHECK_LABEL_PAYLOAD(label, name, value)                                      \
    do {                                                                               \
    ims_test_a429_exploded_label exploded;                                           \
    ims_test_a429_explode_label(label, &exploded, 0);                                \
    TEST_ASSERT(actor, exploded.payload == value, name " has the expected value.");  \
    } while (0)

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
    ims_message_t AFDX_Q_1;
    ims_message_t AFDX_Q_2;
    ims_message_t A429_BUS1_S_LABEL;
    ims_message_t A429_BUS1_Q_LABEL;
    ims_message_t A429_BUS2_S_LABEL;
    ims_message_t A429_BUS2_Q_LABEL;
    ims_message_t CAN_BUS1_M1;
    ims_message_t CAN_BUS1_M2;
    ims_message_t CAN_BUS2_M1;
    ims_message_t CAN_BUS2_M2;
    ims_message_t DISC1;
    ims_message_t DISC2;
    ims_message_t DISC3;
    ims_message_t DISC4;
    ims_message_t ANA1;
    ims_message_t ANA2;
    ims_message_t ANA3;
    ims_message_t ANA4;
    ims_message_t NAD_GRP1_M1;
    ims_message_t NAD_GRP1_M2;
    ims_message_t NAD_GRP2_M1;
    ims_message_t NAD_GRP2_M2;

    char           received_payload[100];
    char           expected_payload[100];
    uint32_t       received_size;
    ims_validity_t validity;

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

    AFDX_S_1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_afdx, "AFDX_S_1", 42, 1, ims_input, &AFDX_S_1) == ims_no_error &&
                       AFDX_S_1 != NULL,
                       "AFDX_S_1 create");

    AFDX_S_2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_afdx, "AFDX_S_2", 21, 1, ims_input, &AFDX_S_2) == ims_no_error &&
                       AFDX_S_2 != NULL,
                       "AFDX_S_2 create");

    AFDX_Q_1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_afdx, "AFDX_Q_1", 42, 2, ims_input, &AFDX_Q_1) == ims_no_error &&
                       AFDX_Q_1 != NULL,
                       "AFDX_Q_1 create");

    AFDX_Q_2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_afdx, "AFDX_Q_2", 21, 4, ims_input, &AFDX_Q_2) == ims_no_error &&
                       AFDX_Q_2 != NULL,
                       "AFDX_Q_2 create");

    A429_BUS1_S_LABEL = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_a429, "A429_BUS1_S_LABEL", 4, 1, ims_input, &A429_BUS1_S_LABEL) == ims_no_error &&
                       A429_BUS1_S_LABEL != NULL,
                       "A429_BUS1_S_LABEL create");

    A429_BUS1_Q_LABEL = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_a429, "A429_BUS1_Q_LABEL", 4, 4, ims_input, &A429_BUS1_Q_LABEL) == ims_no_error &&
                       A429_BUS1_Q_LABEL != NULL,
                       "A429_BUS1_Q_LABEL create");

    A429_BUS2_S_LABEL = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_a429, "A429_BUS2_S_LABEL", 4, 1, ims_input, &A429_BUS2_S_LABEL) == ims_no_error &&
                       A429_BUS2_S_LABEL != NULL,
                       "A429_BUS2_S_LABEL create");

    A429_BUS2_Q_LABEL = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_a429, "A429_BUS2_Q_LABEL", 4, 5, ims_input, &A429_BUS2_Q_LABEL) == ims_no_error &&
                       A429_BUS2_Q_LABEL != NULL,
                       "A429_BUS2_Q_LABEL create");

    CAN_BUS1_M1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_can, "CAN_BUS1_M1", 4, 1, ims_input, &CAN_BUS1_M1) == ims_no_error &&
                       CAN_BUS1_M1 != NULL,
                       "CAN_BUS1_M1 create");

    CAN_BUS1_M2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_can, "CAN_BUS1_M2", 8, 1, ims_input, &CAN_BUS1_M2) == ims_no_error &&
                       CAN_BUS1_M2 != NULL,
                       "CAN_BUS1_M2 create");

    CAN_BUS2_M1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_can, "CAN_BUS2_M1", 8, 1, ims_input, &CAN_BUS2_M1) == ims_no_error &&
                       CAN_BUS2_M1 != NULL,
                       "CAN_BUS2_M1 create");

    CAN_BUS2_M2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_can, "CAN_BUS2_M2", 4, 1, ims_input, &CAN_BUS2_M2) == ims_no_error &&
                       CAN_BUS2_M2 != NULL,
                       "CAN_BUS2_M2 create");

    DISC1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_discrete, "DISC1", 4, 1, ims_input, &DISC1) == ims_no_error &&
                       DISC1 != NULL,
                       "DISC1 create");

    DISC2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_discrete, "DISC2", 4, 1, ims_input, &DISC2) == ims_no_error &&
                       DISC2 != NULL,
                       "DISC2 create");

    DISC3 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_discrete, "DISC3", 4, 1, ims_input, &DISC3) == ims_no_error &&
                       DISC3 != NULL,
                       "DISC3 create");

    DISC4 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_discrete, "DISC4", 4, 1, ims_input, &DISC4) == ims_no_error &&
                       DISC4 != NULL,
                       "DISC4 create");

    ANA1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_analogue, "ANA1", 4, 1, ims_input, &ANA1) == ims_no_error &&
                       ANA1 != NULL,
                       "ANA1 create");

    ANA2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_analogue, "ANA2", 4, 1, ims_input, &ANA2) == ims_no_error &&
                       ANA2 != NULL,
                       "ANA2 create");

    ANA3 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_analogue, "ANA3", 4, 1, ims_input, &ANA3) == ims_no_error &&
                       ANA3 != NULL,
                       "ANA3 create");

    ANA4 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_analogue, "ANA4", 4, 1, ims_input, &ANA4) == ims_no_error &&
                       ANA4 != NULL,
                       "ANA4 create");

    NAD_GRP1_M1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "NAD1", 4, 1, ims_input, &NAD_GRP1_M1) == ims_no_error &&
                       NAD_GRP1_M1 != (ims_message_t)INVALID_POINTER && NAD_GRP1_M1 != NULL,
                       "We can get the NAD_GRP1_M1.");

    NAD_GRP1_M2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "NAD2", 1, 1, ims_input, &NAD_GRP1_M2) == ims_no_error &&
                       NAD_GRP1_M2 != (ims_message_t)INVALID_POINTER && NAD_GRP1_M2 != NULL,
                       "We can get the NAD_GRP1_M2.");


    NAD_GRP2_M1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "NAD3", 8, 1, ims_input, &NAD_GRP2_M1) == ims_no_error &&
                       NAD_GRP2_M1 != (ims_message_t)INVALID_POINTER && NAD_GRP2_M1 != NULL,
                       "We can get the NAD_GRP2_M1.");

    NAD_GRP2_M2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "NAD4", 10, 1, ims_input, &NAD_GRP2_M2) == ims_no_error &&
                       NAD_GRP2_M2 != (ims_message_t)INVALID_POINTER && NAD_GRP2_M2 != NULL,
                       "We can get the NAD_GRP2_M2.");

    // Check we cannot invalidate queuings, wired nor NAD messages
    TEST_ASSERT(actor, ims_invalidate_sampling_message(AFDX_Q_1) == ims_invalid_configuration, "we can't invalidate AFDX_Q_1.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(A429_BUS1_Q_LABEL) == ims_invalid_configuration, "we can't invalidate A429_BUS1_Q_LABEL.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(DISC1) == ims_no_error, "we can invalidate DISC1.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(ANA1) == ims_no_error, "we can invalidate ANA1.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(NAD_GRP1_M1) == ims_invalid_configuration, "we can't invalidate NAD_GRP1_M1.");

    TEST_SIGNAL(actor, 1);  // We are ready
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    // All sampling messages are valid
    memset(expected_payload, 0, 100); strcpy(expected_payload, "First AFDX Message");
    TEST_ASSERT(actor, ims_read_sampling_message(AFDX_S_1, received_payload, &received_size, &validity) == ims_no_error, "AFDX_S_1 read.");
    TEST_ASSERT(actor, received_size == 42 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 42) == 0, "AFDX_S_1 has the expected content.");

    memset(expected_payload, 0, 100); strcpy(expected_payload, "Second AFDX Message");
    TEST_ASSERT(actor, ims_read_sampling_message(AFDX_S_2, received_payload, &received_size, &validity) == ims_no_error, "AFDX_S_2 read.");
    TEST_ASSERT(actor, received_size == 21 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 21) == 0, "AFDX_S_2 has the expected content.");

    TEST_ASSERT(actor, ims_read_sampling_message(A429_BUS1_S_LABEL, received_payload, &received_size, &validity) == ims_no_error, "A429_BUS1_S_LABEL read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_valid, "A429_BUS1_S_LABEL never received.");
    CHECK_LABEL_PAYLOAD(received_payload, "A429_BUS2_S_LABEL", 0x42);

    TEST_ASSERT(actor, ims_read_sampling_message(A429_BUS2_S_LABEL, received_payload, &received_size, &validity) == ims_no_error, "A429_BUS2_S_LABEL read.");
    TEST_ASSERT(actor, received_size == 4 && validity == ims_valid, "A429_BUS2_S_LABEL has the expected lenght");
    CHECK_LABEL_PAYLOAD(received_payload, "A429_BUS2_S_LABEL", 0x21);

    memset(expected_payload, 84, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS1_M1, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS1_M1 read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 4) == 0, "CAN_BUS1_M1 has the expected content.");

    memset(expected_payload, 85, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS1_M2, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS1_M2 read.");
    TEST_ASSERT(actor, received_size == 8 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 8) == 0, "CAN_BUS1_M2 has the expected content.");

    memset(expected_payload, 86, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS2_M1, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS2_M1 read.");
    TEST_ASSERT(actor, received_size == 8 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 8) == 0, "CAN_BUS2_M1 has the expected content.");

    memset(expected_payload, 87, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS2_M2, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS2_M2 read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 4) == 0, "CAN_BUS2_M2 has the expected content.");

    // invalidate some messages
    TEST_ASSERT(actor, ims_invalidate_sampling_message(AFDX_S_1) == ims_no_error, "No errors on invalidate AFDX_S_1.");
    //  TEST_ASSERT(actor, ims_invalidate_sampling_message(AFDX_S_2) == ims_no_error, "No errors on invalidate AFDX_S_2.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(A429_BUS1_S_LABEL) == ims_no_error, "No errors on invalidate A429_BUS1_S_LABEL.");
    //  TEST_ASSERT(actor, ims_invalidate_sampling_message(A429_BUS2_S_LABEL) == ims_no_error, "No errors on invalidate A429_BUS2_S_LABEL.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(CAN_BUS1_M1) == ims_no_error, "No errors on invalidate CAN_BUS1_M1.");
    //  TEST_ASSERT(actor, ims_invalidate_sampling_message(CAN_BUS1_M2) == ims_no_error, "No errors on invalidate CAN_BUS1_M2.");
    TEST_ASSERT(actor, ims_invalidate_sampling_message(CAN_BUS2_M1) == ims_no_error, "No errors on invalidate CAN_BUS2_M1.");
    //  TEST_ASSERT(actor, ims_invalidate_sampling_message(CAN_BUS2_M2) == ims_no_error, "No errors on invalidate CAN_BUS2_M2.");

    // invalidate sampling messages become invalid
    memset(expected_payload, 0, 100); strcpy(expected_payload, "First AFDX Message");
    TEST_ASSERT(actor, ims_read_sampling_message(AFDX_S_1, received_payload, &received_size, &validity) == ims_no_error, "AFDX_S_1 read.");
    TEST_ASSERT(actor, received_size == 42 &&
                validity == ims_invalid &&
                memcmp(received_payload, expected_payload, 42) == 0, "AFDX_S_1 has the expected content.");

    memset(expected_payload, 0, 100); strcpy(expected_payload, "Second AFDX Message");
    TEST_ASSERT(actor, ims_read_sampling_message(AFDX_S_2, received_payload, &received_size, &validity) == ims_no_error, "AFDX_S_2 read.");
    TEST_ASSERT(actor, received_size == 21 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 21) == 0, "AFDX_S_2 has the expected content.");

    TEST_ASSERT(actor, ims_read_sampling_message(A429_BUS1_S_LABEL, received_payload, &received_size, &validity) == ims_no_error, "A429_BUS1_S_LABEL read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_invalid, "A429_BUS1_S_LABEL never received.");
    CHECK_LABEL_PAYLOAD(received_payload, "A429_BUS2_S_LABEL", 0x42);

    TEST_ASSERT(actor, ims_read_sampling_message(A429_BUS2_S_LABEL, received_payload, &received_size, &validity) == ims_no_error, "A429_BUS2_S_LABEL read.");
    TEST_ASSERT(actor, received_size == 4 && validity == ims_valid, "A429_BUS2_S_LABEL has the expected lenght");
    CHECK_LABEL_PAYLOAD(received_payload, "A429_BUS2_S_LABEL", 0x21);

    memset(expected_payload, 84, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS1_M1, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS1_M1 read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_invalid &&
                memcmp(received_payload, expected_payload, 4) == 0, "CAN_BUS1_M1 has the expected content.");

    memset(expected_payload, 85, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS1_M2, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS1_M2 read.");
    TEST_ASSERT(actor, received_size == 8 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 8) == 0, "CAN_BUS1_M2 has the expected content.");

    memset(expected_payload, 86, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS2_M1, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS2_M1 read.");
    TEST_ASSERT(actor, received_size == 8 &&
                validity == ims_invalid &&
                memcmp(received_payload, expected_payload, 8) == 0, "CAN_BUS2_M1 has the expected content.");

    memset(expected_payload, 87, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS2_M2, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS2_M2 read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 4) == 0, "CAN_BUS2_M2 has the expected content.");

    TEST_SIGNAL(actor, 1);  // Ask to write again
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    // All sampling messages are valid
    memset(expected_payload, 0, 100); strcpy(expected_payload, "First AFDX Message");
    TEST_ASSERT(actor, ims_read_sampling_message(AFDX_S_1, received_payload, &received_size, &validity) == ims_no_error, "AFDX_S_1 read.");
    TEST_ASSERT(actor, received_size == 42 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 42) == 0, "AFDX_S_1 has the expected content.");

    memset(expected_payload, 0, 100); strcpy(expected_payload, "Second AFDX Message");
    TEST_ASSERT(actor, ims_read_sampling_message(AFDX_S_2, received_payload, &received_size, &validity) == ims_no_error, "AFDX_S_2 read.");
    TEST_ASSERT(actor, received_size == 21 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 21) == 0, "AFDX_S_2 has the expected content.");

    TEST_ASSERT(actor, ims_read_sampling_message(A429_BUS1_S_LABEL, received_payload, &received_size, &validity) == ims_no_error, "A429_BUS1_S_LABEL read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_valid, "A429_BUS1_S_LABEL never received.");
    CHECK_LABEL_PAYLOAD(received_payload, "A429_BUS2_S_LABEL", 0x42);

    TEST_ASSERT(actor, ims_read_sampling_message(A429_BUS2_S_LABEL, received_payload, &received_size, &validity) == ims_no_error, "A429_BUS2_S_LABEL read.");
    TEST_ASSERT(actor, received_size == 4 && validity == ims_valid, "A429_BUS2_S_LABEL has the expected lenght");
    CHECK_LABEL_PAYLOAD(received_payload, "A429_BUS2_S_LABEL", 0x21);

    memset(expected_payload, 84, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS1_M1, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS1_M1 read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 4) == 0, "CAN_BUS1_M1 has the expected content.");

    memset(expected_payload, 85, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS1_M2, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS1_M2 read.");
    TEST_ASSERT(actor, received_size == 8 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 8) == 0, "CAN_BUS1_M2 has the expected content.");

    memset(expected_payload, 86, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS2_M1, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS2_M1 read.");
    TEST_ASSERT(actor, received_size == 8 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 8) == 0, "CAN_BUS2_M1 has the expected content.");

    memset(expected_payload, 87, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS2_M2, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS2_M2 read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_valid &&
                memcmp(received_payload, expected_payload, 4) == 0, "CAN_BUS2_M2 has the expected content.");

    // Invalidate all messages
    TEST_ASSERT(actor, ims_invalidate_all_sampling_messages(ims_context) == ims_no_error, "No errors on invalidate_all_sampling_messages");

    // All sampling messages are now invalid
    memset(expected_payload, 0, 100); strcpy(expected_payload, "First AFDX Message");
    TEST_ASSERT(actor, ims_read_sampling_message(AFDX_S_1, received_payload, &received_size, &validity) == ims_no_error, "AFDX_S_1 read.");
    TEST_ASSERT(actor, received_size == 42 &&
                validity == ims_invalid &&
                memcmp(received_payload, expected_payload, 42) == 0, "AFDX_S_1 has the expected content.");

    memset(expected_payload, 0, 100); strcpy(expected_payload, "Second AFDX Message");
    TEST_ASSERT(actor, ims_read_sampling_message(AFDX_S_2, received_payload, &received_size, &validity) == ims_no_error, "AFDX_S_2 read.");
    TEST_ASSERT(actor, received_size == 21 &&
                validity == ims_invalid &&
                memcmp(received_payload, expected_payload, 21) == 0, "AFDX_S_2 has the expected content.");

    TEST_ASSERT(actor, ims_read_sampling_message(A429_BUS1_S_LABEL, received_payload, &received_size, &validity) == ims_no_error, "A429_BUS1_S_LABEL read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_invalid, "A429_BUS1_S_LABEL never received.");
    CHECK_LABEL_PAYLOAD(received_payload, "A429_BUS2_S_LABEL", 0x42);

    TEST_ASSERT(actor, ims_read_sampling_message(A429_BUS2_S_LABEL, received_payload, &received_size, &validity) == ims_no_error, "A429_BUS2_S_LABEL read.");
    TEST_ASSERT(actor, received_size == 4 && validity == ims_invalid, "A429_BUS2_S_LABEL has the expected lenght");
    CHECK_LABEL_PAYLOAD(received_payload, "A429_BUS2_S_LABEL", 0x21);

    memset(expected_payload, 84, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS1_M1, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS1_M1 read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_invalid &&
                memcmp(received_payload, expected_payload, 4) == 0, "CAN_BUS1_M1 has the expected content.");

    memset(expected_payload, 85, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS1_M2, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS1_M2 read.");
    TEST_ASSERT(actor, received_size == 8 &&
                validity == ims_invalid &&
                memcmp(received_payload, expected_payload, 8) == 0, "CAN_BUS1_M2 has the expected content.");

    memset(expected_payload, 86, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS2_M1, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS2_M1 read.");
    TEST_ASSERT(actor, received_size == 8 &&
                validity == ims_invalid &&
                memcmp(received_payload, expected_payload, 8) == 0, "CAN_BUS2_M1 has the expected content.");

    memset(expected_payload, 87, 10);
    TEST_ASSERT(actor, ims_read_sampling_message(CAN_BUS2_M2, received_payload, &received_size, &validity) == ims_no_error, "CAN_BUS2_M2 read.");
    TEST_ASSERT(actor, received_size == 4 &&
                validity == ims_invalid &&
                memcmp(received_payload, expected_payload, 4) == 0, "CAN_BUS2_M2 has the expected content.");

    // Done
    ims_free_context(ims_context);
    return ims_test_end(actor);
}
