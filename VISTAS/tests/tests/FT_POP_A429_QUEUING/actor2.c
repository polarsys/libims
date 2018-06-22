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

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE_SIZE 4

#define FILLED_BY_IMS 0

#define BUS1_LABEL1_SDI     1  //01
#define BUS1_LABEL1_NUMBER  ims_test_a429_label_number_encode("024")

#define BUS1_LABEL2_SDI     3  //11
#define BUS1_LABEL2_NUMBER  ims_test_a429_label_number_encode("024")
#define BUS1_LABEL2_DEPTH   4

#define BUS2_LABEL1_SDI     1  //01
#define BUS2_LABEL1_NUMBER  ims_test_a429_label_number_encode("024")

#define BUS2_LABEL2_SDI     1 //01
#define BUS2_LABEL2_NUMBER  ims_test_a429_label_number_encode("123")
#define BUS2_LABEL2_DEPTH   5

#define MAX_DEPTH           5

#define INVALID_POINTER ((void*)42)

#define FILL_LABEL(label, value) \
    ims_test_a429_fill_label(label, FILLED_BY_IMS, FILLED_BY_IMS,  value, 1, FILLED_BY_IMS)

#define CHECK_LABEL_PAYLOAD(label, name, value)                                      \
    do {                                                                               \
    ims_test_a429_exploded_label exploded;                                           \
    ims_test_a429_explode_label(label, &exploded, 0);                                \
    TEST_ASSERT(actor, exploded.payload == value, name " has the expected value.");  \
    } while (0)

void fill_q_label(char* labels, uint8_t number, uint8_t sdi, uint32_t first_value, uint32_t count)
{
    uint32_t i;
    for (i = 0; i < count; i++)
    {
        ims_test_a429_fill_label(labels + i * 4, number, sdi, first_value + i, 1, 0);
    }
}

int main()
{
    ims_node_t     ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;
    ims_message_t     bus1_label1;
    ims_message_t     bus1_label2;
    ims_message_t     bus2_label1;
    ims_message_t     bus2_label2;
    ims_message_t     output_label;
    uint32_t          count;

    ims_validity_t    validity;
    ims_return_code_t ims_retcode;
    uint32_t          received_size;
    char              received_payload[MESSAGE_SIZE * MAX_DEPTH];

    char bus1_label1_payload[MESSAGE_SIZE];
    char bus2_label1_payload[MESSAGE_SIZE];
    ims_test_a429_fill_label(bus1_label1_payload, BUS1_LABEL1_NUMBER, BUS1_LABEL1_SDI,  0x7FFFF, 1, 0);
    ims_test_a429_fill_label(bus2_label1_payload, BUS2_LABEL1_NUMBER, BUS2_LABEL1_SDI,  0x00F00, 2, 1);

    // Internal data
    const uint32_t data_internal_max_size = 100;
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

    bus1_label1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l1", MESSAGE_SIZE, 1, ims_input, &bus1_label1) == ims_no_error &&
                       bus1_label1 != (ims_message_t)INVALID_POINTER && bus1_label1 != NULL,
                       "We can get the bus1_label1.");

    bus1_label2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus1_l2", MESSAGE_SIZE, BUS1_LABEL2_DEPTH, ims_input, &bus1_label2) == ims_no_error &&
                       bus1_label2 != (ims_message_t)INVALID_POINTER && bus1_label2 != NULL,
                       "We can get the bus1_label2.");

    bus2_label1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus2_l1", MESSAGE_SIZE, 1, ims_input, &bus2_label1) == ims_no_error &&
                       bus2_label1 != (ims_message_t)INVALID_POINTER && bus2_label1 != NULL,
                       "We can get the bus2_label1.");

    bus2_label2 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "bus2_l2", MESSAGE_SIZE, BUS2_LABEL2_DEPTH, ims_input, &bus2_label2) == ims_no_error &&
                       bus2_label2 != (ims_message_t)INVALID_POINTER && bus2_label2 != NULL,
                       "We can get the bus2_label2.");

    output_label = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_a429, "output_label", MESSAGE_SIZE, 4, ims_output, &output_label) == ims_no_error &&
                       output_label != (ims_message_t)INVALID_POINTER && output_label != NULL,
                       "We can get the output_label.");

    // Invalid configuration test
    TEST_ASSERT(actor, ims_pop_queuing_message(bus1_label1, received_payload, &received_size) == ims_invalid_configuration,
                "Cannot pop a sampling message.");

    TEST_ASSERT(actor, ims_pop_queuing_message(output_label, received_payload, &received_size) == ims_invalid_configuration,
                "Cannot pop an output message.");

    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label1, &count) == ims_invalid_configuration,
                "cannot call ims_queuing_message_pending on a sampling message.");

    // Check empty messages
    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(bus1_label2, received_payload, &received_size) == ims_no_error,
                "pop bus1_label2 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "bus1_label2 is empty.");

    TEST_ASSERT(actor, ims_queuing_message_pending(bus2_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(bus2_label2, received_payload, &received_size) == ims_no_error,
                "pop bus2_label2 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "bus2_label2 is empty.");

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(bus1_label2, received_payload, &received_size) == ims_no_error,
                "pop bus1_label2 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "bus1_label2 is empty.");

    TEST_ASSERT(actor, ims_queuing_message_pending(bus2_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(bus2_label2, received_payload, &received_size) == ims_no_error,
                "pop bus2_label2 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "bus2_label2 is empty.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write
    TEST_WAIT(actor, 1);

    // Without import, message still empty
    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(bus1_label2, received_payload, &received_size) == ims_no_error,
                "pop bus1_label2 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "bus1_label2 is empty.");

    TEST_ASSERT(actor, ims_queuing_message_pending(bus2_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, ims_pop_queuing_message(bus2_label2, received_payload, &received_size) == ims_no_error,
                "pop bus2_label2 return no_error.");
    TEST_ASSERT(actor, count == 0 && received_size == 0, "bus2_label2 is empty.");

    // After import, all messages are filled in
    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    ims_retcode = ims_read_sampling_message(bus1_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "bus1_label1 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "bus1_label1 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus1_label1_payload, MESSAGE_SIZE) == 0, "bus1_label1 (%x) content is good (%x).", *(uint32_t*)received_payload, *(uint32_t*)bus1_label1_payload);

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus1_label1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus1_label1 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, received_payload, MESSAGE_SIZE) == 0, "Retrieved bus1_label1 internal data (%x) is good (%x).", *(uint32_t*)data_internal, *(uint32_t*)(received_payload));
#endif

    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");

    fill_q_label(received_payload, BUS1_LABEL2_NUMBER, BUS1_LABEL2_SDI, 1, 3);

#ifdef ENABLE_INSTRUMENTATION
    unsigned short i;
    for(i = 0 ; i < 3 ; i++){
        memset(data_internal,0,data_internal_max_size);
        TEST_ASSERT(actor, ims_instrumentation_message_get_queued_data(bus1_label2, data_internal, &data_internal_size, data_internal_max_size, i) == ims_no_error,
                    "bus1_label2 internal queued data [%hu] retrieved.",i);
        TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved bus1_label2 [%hu] internal data has the right size.",i);
        TEST_ASSERT(actor, memcmp(data_internal, received_payload + i * MESSAGE_SIZE, MESSAGE_SIZE) == 0, "Retrieved bus1_label2 [%hu] (%x) internal data is good (%x).", i, *(uint32_t*)data_internal, *(uint32_t*)(received_payload + i * MESSAGE_SIZE));
    }

    for(i = 0 ; i < 3 ; i++){
        memset(data_internal,0,data_internal_max_size);
        TEST_ASSERT(actor, ims_instrumentation_message_get_queued_data(bus1_label2, data_internal, &data_internal_size, data_internal_max_size, i) == ims_no_error,
                    "bus1_label2 internal queued data [%hu] retrieved.",i);
        TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved bus1_label2 [%hu] internal data has the right size.",i);
        TEST_ASSERT(actor, memcmp(data_internal, received_payload + i * MESSAGE_SIZE, MESSAGE_SIZE) == 0, "Retrieved bus1_label2 [%hu] (%x) internal data is good (%x).", i, *(uint32_t*)data_internal, *(uint32_t*)(received_payload + i * MESSAGE_SIZE));
    }
#endif

    TEST_ASSERT(actor, ims_pop_queuing_message(bus1_label2, received_payload, &received_size) == ims_no_error,
                "pop bus1_label2 return no_error.");
    TEST_ASSERT(actor, count == 3 && received_size == 3 * MESSAGE_SIZE, "bus1_label2 has the expected size.");
    CHECK_LABEL_PAYLOAD(received_payload + 0, "bus1_label2, label 1", 1);
    CHECK_LABEL_PAYLOAD(received_payload + 4, "bus1_label2, label 2", 2);
    CHECK_LABEL_PAYLOAD(received_payload + 8, "bus1_label2, label 3", 3);

    ims_retcode = ims_read_sampling_message(bus2_label1, received_payload, &received_size, &validity);
    TEST_ASSERT(actor, validity == ims_valid && ims_retcode == ims_no_error, "bus2_label1 is valid.");
    TEST_ASSERT(actor, received_size == MESSAGE_SIZE, "bus2_label1 has the correct len.");
    TEST_ASSERT(actor, memcmp(received_payload, bus2_label1_payload, MESSAGE_SIZE) == 0, "bus2_label1 content is good.");

#ifdef ENABLE_INSTRUMENTATION
    memset(data_internal, 0, data_internal_max_size);
    TEST_ASSERT(actor, ims_instrumentation_message_get_sampling_data(bus2_label1, data_internal, &data_internal_size, data_internal_max_size) == ims_no_error,
                "bus2_label1 internal data retrieved.");
    TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved data has the right size.");
    TEST_ASSERT(actor, memcmp(data_internal, received_payload, MESSAGE_SIZE) == 0, "Retrieved bus2_label1 internal data is good.");
#endif

    TEST_ASSERT(actor, ims_queuing_message_pending(bus2_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");

    fill_q_label(received_payload, BUS2_LABEL2_NUMBER, BUS2_LABEL2_SDI, 5, 2);

#ifdef ENABLE_INSTRUMENTATION
    for(i = 0 ; i < 2 ; i++){
        memset(data_internal,0,data_internal_max_size);
        TEST_ASSERT(actor, ims_instrumentation_message_get_queued_data(bus2_label2, data_internal, &data_internal_size, data_internal_max_size, i) == ims_no_error,
                    "bus2_label2 internal queued data [%hu] retrieved.",i);
        TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved bus2_label2 [%hu] internal data has the right size.",i);
        TEST_ASSERT(actor, memcmp(data_internal, received_payload + i * MESSAGE_SIZE, MESSAGE_SIZE) == 0, "Retrieved bus2_label2 [%hu] (%x) internal data is good (%x).", i, *(uint32_t*)data_internal, *(uint32_t*)(received_payload + i * MESSAGE_SIZE));
    }

    for(i = 0 ; i < 2 ; i++){
        memset(data_internal,0,data_internal_max_size);
        TEST_ASSERT(actor, ims_instrumentation_message_get_queued_data(bus2_label2, data_internal, &data_internal_size, data_internal_max_size, i) == ims_no_error,
                    "bus2_label2 internal queued data [%hu] retrieved.",i);
        TEST_ASSERT(actor, data_internal_size == MESSAGE_SIZE, "Retrieved bus2_label2 [%hu] internal data has the right size.",i);
        TEST_ASSERT(actor, memcmp(data_internal, received_payload + i * MESSAGE_SIZE, MESSAGE_SIZE) == 0, "Retrieved bus2_label2 [%hu] (%x) internal data is good (%x).", i, *(uint32_t*)data_internal, *(uint32_t*)(received_payload + i * MESSAGE_SIZE));
    }
#endif

    TEST_ASSERT(actor, ims_pop_queuing_message(bus2_label2, received_payload, &received_size) == ims_no_error,
                "pop bus2_label2 return no_error.");
    TEST_ASSERT(actor, count == 2 && received_size == 2 * MESSAGE_SIZE, "bus2_label2 has the expected size.");
    CHECK_LABEL_PAYLOAD(received_payload + 0, "bus2_label2, label 1", 5);
    CHECK_LABEL_PAYLOAD(received_payload + 4, "bus2_label2, label 2", 6);

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write several times
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    // We should have 10 labels waiting
    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, count == 10, "10 labels availables on bus1_label2.");

    // We read max_deph of this message : 4
    TEST_ASSERT(actor, ims_pop_queuing_message(bus1_label2, received_payload, &received_size) == ims_no_error,
                "pop bus1_label2 return no_error.");
    TEST_ASSERT(actor, received_size == 4 * MESSAGE_SIZE, "bus1_label2 has the expected size.");
    CHECK_LABEL_PAYLOAD(received_payload +  0, "bus1_label2, label 1", 20);
    CHECK_LABEL_PAYLOAD(received_payload +  4, "bus1_label2, label 2", 21);
    CHECK_LABEL_PAYLOAD(received_payload +  8, "bus1_label2, label 3", 22);
    CHECK_LABEL_PAYLOAD(received_payload + 12, "bus1_label2, label 4", 30);

    // it still 6 labels waiting
    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, count == 6, "6 labels availables on bus1_label2.");

    // We read again max_deph of this message : 4
    TEST_ASSERT(actor, ims_pop_queuing_message(bus1_label2, received_payload, &received_size) == ims_no_error,
                "pop bus1_label2 return no_error.");
    TEST_ASSERT(actor, received_size == 4 * MESSAGE_SIZE, "bus1_label2 has the expected size.");
    CHECK_LABEL_PAYLOAD(received_payload +  0, "bus1_label2, label 1", 31);
    CHECK_LABEL_PAYLOAD(received_payload +  4, "bus1_label2, label 2", 40);
    CHECK_LABEL_PAYLOAD(received_payload +  8, "bus1_label2, label 3", 41);
    CHECK_LABEL_PAYLOAD(received_payload + 12, "bus1_label2, label 4", 42);

    // it still 2 labels waiting
    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, count == 2, "2 labels availables on bus1_label2.");

    TEST_SIGNAL(actor, 1); // Ask actor 1 to write again
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");

    // We should have 3 labels waiting
    TEST_ASSERT(actor, ims_queuing_message_pending(bus1_label2, &count) == ims_no_error,
                "queuing_message_pending return no_error.");
    TEST_ASSERT(actor, count == 3, "3 labels availables on bus1_label2.");

    // We read the 3 labels
    TEST_ASSERT(actor, ims_pop_queuing_message(bus1_label2, received_payload, &received_size) == ims_no_error,
                "pop bus1_label2 return no_error.");
    TEST_ASSERT(actor, received_size == 3 * MESSAGE_SIZE, "bus1_label2 has the expected size.");
    CHECK_LABEL_PAYLOAD(received_payload +  0, "bus1_label2, label 1", 43);
    CHECK_LABEL_PAYLOAD(received_payload +  4, "bus1_label2, label 2", 44);
    CHECK_LABEL_PAYLOAD(received_payload +  8, "bus1_label2, label 3", 50);

    ims_free_context(ims_context);
    return ims_test_end(actor);
}
