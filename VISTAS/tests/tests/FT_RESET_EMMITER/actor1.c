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

#define A429_Q1_SDI     3  //11
#define A429_Q1_NUMBER  ims_test_a429_label_number_encode("024")

#define A429_Q2_SDI     1  //01
#define A429_Q2_NUMBER  ims_test_a429_label_number_encode("123")

#define FILLED_BY_IMS 0

#define FILL_LABEL(label, value) \
    ims_test_a429_fill_label(label, FILLED_BY_IMS, FILLED_BY_IMS,  value, 1, FILLED_BY_IMS)

void fill_q_label(char* labels, uint8_t number, uint8_t sdi, uint32_t first_value, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i++)
    {
        ims_test_a429_fill_label(labels + i * 4, number, sdi, first_value + i, 1, 0);
    }
}

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

    char  payload[100];
    float ana_value;

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

    AFDX_Q_1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_afdx, "AFDX_Q_1", 42, 2, ims_output, &AFDX_Q_1) == ims_no_error &&
                       AFDX_Q_1 != NULL,
                       "AFDX_Q_1 create");

    AFDX_Q_2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_afdx, "AFDX_Q_2", 21, 4, ims_output, &AFDX_Q_2) == ims_no_error &&
                       AFDX_Q_2 != NULL,
                       "AFDX_Q_2 create");

    A429_BUS1_S_LABEL = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_a429, "A429_BUS1_S_LABEL", 4, 1, ims_output, &A429_BUS1_S_LABEL) == ims_no_error &&
                       A429_BUS1_S_LABEL != NULL,
                       "A429_BUS1_S_LABEL create");

    A429_BUS1_Q_LABEL = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_a429, "A429_BUS1_Q_LABEL", 4, 4, ims_output, &A429_BUS1_Q_LABEL) == ims_no_error &&
                       A429_BUS1_Q_LABEL != NULL,
                       "A429_BUS1_Q_LABEL create");

    A429_BUS2_S_LABEL = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_a429, "A429_BUS2_S_LABEL", 4, 1, ims_output, &A429_BUS2_S_LABEL) == ims_no_error &&
                       A429_BUS2_S_LABEL != NULL,
                       "A429_BUS2_S_LABEL create");

    A429_BUS2_Q_LABEL = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_a429, "A429_BUS2_Q_LABEL", 4, 5, ims_output, &A429_BUS2_Q_LABEL) == ims_no_error &&
                       A429_BUS2_Q_LABEL != NULL,
                       "A429_BUS2_Q_LABEL create");

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

    DISC1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_discrete, "DISC1", 4, 1, ims_output, &DISC1) == ims_no_error &&
                       DISC1 != NULL,
                       "DISC1 create");

    DISC2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_discrete, "DISC2", 4, 1, ims_output, &DISC2) == ims_no_error &&
                       DISC2 != NULL,
                       "DISC2 create");

    DISC3 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_discrete, "DISC3", 4, 1, ims_output, &DISC3) == ims_no_error &&
                       DISC3 != NULL,
                       "DISC3 create");

    DISC4 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_discrete, "DISC4", 4, 1, ims_output, &DISC4) == ims_no_error &&
                       DISC4 != NULL,
                       "DISC4 create");

    ANA1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_analogue, "ANA1", 4, 1, ims_output, &ANA1) == ims_no_error &&
                       ANA1 != NULL,
                       "ANA1 create");

    ANA2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_analogue, "ANA2", 4, 1, ims_output, &ANA2) == ims_no_error &&
                       ANA2 != NULL,
                       "ANA2 create");

    ANA3 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_analogue, "ANA3", 4, 1, ims_output, &ANA3) == ims_no_error &&
                       ANA3 != NULL,
                       "ANA3 create");

    ANA4 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor,
                       ims_get_message(ims_application, ims_analogue, "ANA4", 4, 1, ims_output, &ANA4) == ims_no_error &&
                       ANA4 != NULL,
                       "ANA4 create");

    NAD_GRP1_M1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "NAD1", 4, 1, ims_output, &NAD_GRP1_M1) == ims_no_error &&
                       NAD_GRP1_M1 != (ims_message_t)INVALID_POINTER && NAD_GRP1_M1 != NULL,
                       "We can get the NAD_GRP1_M1.");

    NAD_GRP1_M2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "NAD2", 1, 1, ims_output, &NAD_GRP1_M2) == ims_no_error &&
                       NAD_GRP1_M2 != (ims_message_t)INVALID_POINTER && NAD_GRP1_M2 != NULL,
                       "We can get the NAD_GRP1_M2.");

    NAD_GRP2_M1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "NAD3", 8, 1, ims_output, &NAD_GRP2_M1) == ims_no_error &&
                       NAD_GRP2_M1 != (ims_message_t)INVALID_POINTER && NAD_GRP2_M1 != NULL,
                       "We can get the NAD_GRP2_M1.");

    NAD_GRP2_M2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_nad, "NAD4", 10, 1, ims_output, &NAD_GRP2_M2) == ims_no_error &&
                       NAD_GRP2_M2 != (ims_message_t)INVALID_POINTER && NAD_GRP2_M2 != NULL,
                       "We can get the NAD_GRP2_M2.");

    // Fill everything
    memset(payload, 0, 100); strcpy(payload, "First AFDX Message");
    TEST_ASSERT(actor, ims_write_sampling_message(AFDX_S_1, payload, 42) == ims_no_error, "AFDX_S_1 wrote.");
    memset(payload, 0, 100); strcpy(payload, "Second AFDX Message");
    TEST_ASSERT(actor, ims_write_sampling_message(AFDX_S_2, payload, 21) == ims_no_error, "AFDX_S_2 wrote.");
    memset(payload, 0, 100); strcpy(payload, "Third AFDX Message");
    TEST_ASSERT(actor, ims_push_queuing_message(AFDX_Q_1, payload, 10) == ims_no_error, "AFDX_Q_1 wrote.");
    memset(payload, 0, 100); strcpy(payload, "Last AFDX Message");
    TEST_ASSERT(actor, ims_push_queuing_message(AFDX_Q_2, payload, 11) == ims_no_error, "AFDX_Q_2 wrote.");

    FILL_LABEL(payload, 0x42);
    TEST_ASSERT(actor, ims_write_sampling_message(A429_BUS1_S_LABEL, payload, 4) == ims_no_error, "A429_BUS1_S_LABEL wrote.");
    fill_q_label(payload, A429_Q1_NUMBER, A429_Q1_SDI, 1, 3);
    TEST_ASSERT(actor, ims_push_queuing_message(A429_BUS1_Q_LABEL, payload, 4 * 3) == ims_no_error, "A429_BUS1_Q_LABEL wrote.");
    FILL_LABEL(payload, 0x21);
    TEST_ASSERT(actor, ims_write_sampling_message(A429_BUS2_S_LABEL, payload, 4) == ims_no_error, "A429_BUS2_S_LABEL wrote.");
    fill_q_label(payload, A429_Q2_NUMBER, A429_Q2_SDI, 5, 2);
    TEST_ASSERT(actor, ims_push_queuing_message(A429_BUS2_Q_LABEL, payload, 4 * 2) == ims_no_error, "A429_BUS2_Q_LABEL wrote.");

    memset(payload, 84, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS1_M1, payload, 4) == ims_no_error, "CAN_BUS1_M1 wrote.");
    memset(payload, 85, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS1_M2, payload, 8) == ims_no_error, "CAN_BUS1_M2 wrote.");
    memset(payload, 86, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS2_M1, payload, 8) == ims_no_error, "CAN_BUS2_M1 wrote.");
    memset(payload, 87, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS2_M2, payload, 4) == ims_no_error, "CAN_BUS2_M2 wrote.");

    memset(payload, 1, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(DISC1, payload, 4) == ims_no_error, "DISC1 wrote.");
    TEST_ASSERT(actor, ims_write_sampling_message(DISC2, payload, 4) == ims_no_error, "DISC2 wrote.");
    TEST_ASSERT(actor, ims_write_sampling_message(DISC3, payload, 4) == ims_no_error, "DISC3 wrote.");
    TEST_ASSERT(actor, ims_write_sampling_message(DISC4, payload, 4) == ims_no_error, "DISC4 wrote.");

    ana_value = 12.3;
    TEST_ASSERT(actor, ims_write_sampling_message(ANA1, (char*)&ana_value, 4) == ims_no_error, "ANA1 wrote.");
    ana_value = 13.3;
    TEST_ASSERT(actor, ims_write_sampling_message(ANA2, (char*)&ana_value, 4) == ims_no_error, "ANA2 wrote.");
    ana_value = 14.3;
    TEST_ASSERT(actor, ims_write_sampling_message(ANA3, (char*)&ana_value, 4) == ims_no_error, "ANA3 wrote.");
    ana_value = 15.3;
    TEST_ASSERT(actor, ims_write_sampling_message(ANA4, (char*)&ana_value, 4) == ims_no_error, "ANA4 wrote.");

    memset(payload, 5, 10);
    TEST_ASSERT(actor, ims_write_nad_message(NAD_GRP1_M1, payload, 4) == ims_no_error, "NAD_GRP1_M1 wrote.");
    memset(payload, 6, 10);
    TEST_ASSERT(actor, ims_write_nad_message(NAD_GRP1_M2, payload, 1) == ims_no_error, "NAD_GRP1_M2 wrote.");
    memset(payload, 7, 10);
    TEST_ASSERT(actor, ims_write_nad_message(NAD_GRP2_M1, payload, 8) == ims_no_error, "NAD_GRP2_M1 wrote.");
    memset(payload, 8, 10);
    TEST_ASSERT(actor, ims_write_nad_message(NAD_GRP2_M2, payload, 10) == ims_no_error, "NAD_GRP2_M2 wrote.");

    // Reset some messages
    TEST_ASSERT(actor, ims_reset_message(AFDX_S_1) == ims_no_error, "No errors on reset AFDX_S_1.");
    //  TEST_ASSERT(actor, ims_reset_message(AFDX_S_2) == ims_no_error, "No errors on reset AFDX_S_2.");
    TEST_ASSERT(actor, ims_reset_message(AFDX_Q_1) == ims_no_error, "No errors on reset AFDX_Q_1.");
    //  TEST_ASSERT(actor, ims_reset_message(AFDX_Q_2) == ims_no_error, "No errors on reset AFDX_Q_2.");
    TEST_ASSERT(actor, ims_reset_message(A429_BUS1_S_LABEL) == ims_no_error, "No errors on reset A429_BUS1_S_LABEL.");
    TEST_ASSERT(actor, ims_reset_message(A429_BUS1_Q_LABEL) == ims_no_error, "No errors on reset A429_BUS1_Q_LABEL.");
    //  TEST_ASSERT(actor, ims_reset_message(A429_BUS2_S_LABEL) == ims_no_error, "No errors on reset A429_BUS2_S_LABEL.");
    //  TEST_ASSERT(actor, ims_reset_message(A429_BUS2_Q_LABEL) == ims_no_error, "No errors on reset A429_BUS2_Q_LABEL.");
    TEST_ASSERT(actor, ims_reset_message(CAN_BUS1_M1) == ims_no_error, "No errors on reset CAN_BUS1_M1.");
    TEST_ASSERT(actor, ims_reset_message(CAN_BUS1_M2) == ims_no_error, "No errors on reset CAN_BUS1_M2.");
    //  TEST_ASSERT(actor, ims_reset_message(CAN_BUS2_M1) == ims_no_error, "No errors on reset CAN_BUS2_M1.");
    //  TEST_ASSERT(actor, ims_reset_message(CAN_BUS2_M2) == ims_no_error, "No errors on reset CAN_BUS2_M2.");
    TEST_ASSERT(actor, ims_reset_message(DISC1) == ims_no_error, "No errors on reset DISC1.");
    //  TEST_ASSERT(actor, ims_reset_message(DISC2) == ims_no_error, "No errors on reset DISC2.");
    TEST_ASSERT(actor, ims_reset_message(DISC3) == ims_no_error, "No errors on reset DISC3.");
    //  TEST_ASSERT(actor, ims_reset_message(DISC4) == ims_no_error, "No errors on reset DISC4.");
    TEST_ASSERT(actor, ims_reset_message(ANA1) == ims_no_error, "No errors on reset ANA1.");
    //  TEST_ASSERT(actor, ims_reset_message(ANA2) == ims_no_error, "No errors on reset ANA2.");
    TEST_ASSERT(actor, ims_reset_message(ANA3) == ims_no_error, "No errors on reset ANA3.");
    //  TEST_ASSERT(actor, ims_reset_message(ANA4) == ims_no_error, "No errors on reset ANA4.");
    TEST_ASSERT(actor, ims_reset_message(NAD_GRP1_M1) == ims_no_error, "No errors on reset NAD_GRP1_M1.");
    //  TEST_ASSERT(actor, ims_reset_message(NAD_GRP1_M2) == ims_no_error, "No errors on reset NAD_GRP1_M2.");
    TEST_ASSERT(actor, ims_reset_message(NAD_GRP2_M1) == ims_no_error, "No errors on reset NAD_GRP2_M1.");
    //  TEST_ASSERT(actor, ims_reset_message(NAD_GRP2_M2) == ims_no_error, "No errors on reset NAD_GRP2_M2.");

    // Ok, send
    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "ims_progress return ims_no_error.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have sent
    TEST_WAIT(actor, 2);

    // Fill again everything with != values
    memset(payload, 0, 100); strcpy(payload, "First AFDX Message2");
    TEST_ASSERT(actor, ims_write_sampling_message(AFDX_S_1, payload, 42) == ims_no_error, "AFDX_S_1 wrote.");
    memset(payload, 0, 100); strcpy(payload, "Second AFDX Message2");
    TEST_ASSERT(actor, ims_write_sampling_message(AFDX_S_2, payload, 21) == ims_no_error, "AFDX_S_2 wrote.");
    memset(payload, 0, 100); strcpy(payload, "Third AFDX Message2");
    TEST_ASSERT(actor, ims_push_queuing_message(AFDX_Q_1, payload, 10) == ims_no_error, "AFDX_Q_1 wrote.");
    memset(payload, 0, 100); strcpy(payload, "Last AFDX Message2");
    TEST_ASSERT(actor, ims_push_queuing_message(AFDX_Q_2, payload, 11) == ims_no_error, "AFDX_Q_2 wrote.");

    FILL_LABEL(payload, 0x44);
    TEST_ASSERT(actor, ims_write_sampling_message(A429_BUS1_S_LABEL, payload, 4) == ims_no_error, "A429_BUS1_S_LABEL wrote.");
    fill_q_label(payload, A429_Q1_NUMBER, A429_Q1_SDI, 4, 3);
    TEST_ASSERT(actor, ims_push_queuing_message(A429_BUS1_Q_LABEL, payload, 4 * 3) == ims_no_error, "A429_BUS1_Q_LABEL wrote.");
    FILL_LABEL(payload, 0x22);
    TEST_ASSERT(actor, ims_write_sampling_message(A429_BUS2_S_LABEL, payload, 4) == ims_no_error, "A429_BUS2_S_LABEL wrote.");
    fill_q_label(payload, A429_Q2_NUMBER, A429_Q2_SDI, 8, 2);
    TEST_ASSERT(actor, ims_push_queuing_message(A429_BUS2_Q_LABEL, payload, 4 * 2) == ims_no_error, "A429_BUS2_Q_LABEL wrote.");

    memset(payload, 14, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS1_M1, payload, 4) == ims_no_error, "CAN_BUS1_M1 wrote.");
    memset(payload, 15, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS1_M2, payload, 8) == ims_no_error, "CAN_BUS1_M2 wrote.");
    memset(payload, 16, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS2_M1, payload, 8) == ims_no_error, "CAN_BUS2_M1 wrote.");
    memset(payload, 17, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(CAN_BUS2_M2, payload, 4) == ims_no_error, "CAN_BUS2_M2 wrote.");

    memset(payload, 1, 10);
    TEST_ASSERT(actor, ims_write_sampling_message(DISC1, payload, 4) == ims_no_error, "DISC1 wrote.");
    TEST_ASSERT(actor, ims_write_sampling_message(DISC2, payload, 4) == ims_no_error, "DISC2 wrote.");
    TEST_ASSERT(actor, ims_write_sampling_message(DISC3, payload, 4) == ims_no_error, "DISC3 wrote.");
    TEST_ASSERT(actor, ims_write_sampling_message(DISC4, payload, 4) == ims_no_error, "DISC4 wrote.");

    ana_value = 22.3;
    TEST_ASSERT(actor, ims_write_sampling_message(ANA1, (char*)&ana_value, 4) == ims_no_error, "ANA1 wrote.");
    ana_value = 23.3;
    TEST_ASSERT(actor, ims_write_sampling_message(ANA2, (char*)&ana_value, 4) == ims_no_error, "ANA2 wrote.");
    ana_value = 24.3;
    TEST_ASSERT(actor, ims_write_sampling_message(ANA3, (char*)&ana_value, 4) == ims_no_error, "ANA3 wrote.");
    ana_value = 25.3;
    TEST_ASSERT(actor, ims_write_sampling_message(ANA4, (char*)&ana_value, 4) == ims_no_error, "ANA4 wrote.");

    memset(payload, 15, 10);
    TEST_ASSERT(actor, ims_write_nad_message(NAD_GRP1_M1, payload, 4) == ims_no_error, "NAD_GRP1_M1 wrote.");
    memset(payload, 16, 10);
    TEST_ASSERT(actor, ims_write_nad_message(NAD_GRP1_M2, payload, 1) == ims_no_error, "NAD_GRP1_M2 wrote.");
    memset(payload, 17, 10);
    TEST_ASSERT(actor, ims_write_nad_message(NAD_GRP2_M1, payload, 8) == ims_no_error, "NAD_GRP2_M1 wrote.");
    memset(payload, 18, 10);
    TEST_ASSERT(actor, ims_write_nad_message(NAD_GRP2_M2, payload, 10) == ims_no_error, "NAD_GRP2_M2 wrote.");

    // Reset all messages
    TEST_ASSERT(actor, ims_reset_all(ims_context) == ims_no_error, "All messages reseted.");

    // Ok, send
    TEST_ASSERT(actor, ims_progress(ims_context, 100 * 1000) == ims_no_error, "ims_progress return ims_no_error.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "ims_send_all return ims_no_error.");

    TEST_SIGNAL(actor, 2); // Signal we have sent

    // Done
    ims_free_context(ims_context);
    return ims_test_end(actor);
}
