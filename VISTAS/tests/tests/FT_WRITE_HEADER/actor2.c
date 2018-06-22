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
// VISTAS header test - actor 2
//
#include "ims_test.h"
#include "math.h"
#include "htonl.h"

#define ACTOR_ID 2
ims_test_actor_t actor;

#ifdef __linux
#include <byteswap.h>
#else
#define bswap_16(x) __builtin_bswap16(x)
#define bswap_32(x) __builtin_bswap32(x)
#define bswap_64(x) __builtin_bswap64(x)
#endif

//
// Message data
//
#define NAD_SIZE           10
#define NAD_IP             "226.23.21.9"
#define NAD_PORT           5078

#define A429_SIZE           4
#define A429_IP             "226.23.12.4"
#define A429_PORT           6078

#define CAN_SIZE           14
#define CAN_IP             "226.23.12.11"
#define CAN_PORT           5078

#define AFDX_SIZE           42
#define AFDX_IP             "226.23.12.5"
#define AFDX_PORT           7078

#define DISCRETE_SIZE           2
#define DISCRETE_IP             "226.23.12.33"
#define DISCRETE_PORT           5078

#ifdef __linux
#define TIMESTAMP_TOLERANCE 1000
#else
#define TIMESTAMP_TOLERANCE 100000
#endif

#define INVALID_POINTER ((void*)42)

#define TEST_HEADER(header, ref_prod_id, ref_seq_num, ref_qos_timestamp, ref_data_timestamp) \
{\
    uint16_t prod_id = bswap_16(*(uint16_t*)((void*)header));\
    uint16_t seq_num =  bswap_16(*(uint16_t*)((void*)header+2));\
    uint64_t qos_timestamp =  bswap_64(*(uint64_t*)(header+4));\
    uint64_t data_timestamp = bswap_64(*(uint64_t*)(header+12));\
    TEST_ASSERT(actor, prod_id==ref_prod_id, "The prod_id has the good value.");\
    TEST_ASSERT(actor, seq_num==ref_seq_num, "The seq_num has the good value.");\
    if (ref_qos_timestamp)\
{\
    TEST_ASSERT(actor, abs(qos_timestamp-ref_qos_timestamp) < TIMESTAMP_TOLERANCE, "The qos_timestamp has the good value.");\
    }\
    else\
{\
    TEST_ASSERT(actor, qos_timestamp==0, "The qos_timestamp is null.");\
    }\
    TEST_ASSERT(actor, data_timestamp==ref_data_timestamp, "The data_timestamp has the good value.");\
    }

int main()
{
    char received_payload[100];
    uint64_t ref_qos_timestamp;

    actor = ims_test_init(ACTOR_ID);

    ims_test_mc_input_t socket_nad = ims_test_mc_input_create(actor, NAD_IP, NAD_PORT);
    ims_test_mc_input_t socket_a429 = ims_test_mc_input_create(actor, A429_IP, A429_PORT);
    ims_test_mc_input_t socket_can = ims_test_mc_input_create(actor, CAN_IP, CAN_PORT);
    ims_test_mc_input_t socket_afdx = ims_test_mc_input_create(actor, AFDX_IP, AFDX_PORT);
    ims_test_mc_input_t socket_discrete = ims_test_mc_input_create(actor, DISCRETE_IP, DISCRETE_PORT);

    TEST_SIGNAL(actor, 1); // We are ready
    TEST_WAIT(actor, 1);

    // test a NAD message with everything
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_nad, received_payload, 100, 1000 * 100) == NAD_SIZE + VISTAS_HEADER_SIZE,
                "We have received the NAD.");

    ref_qos_timestamp = ims_test_time_us();

    TEST_HEADER(received_payload, 42, 1, ref_qos_timestamp, 100*1000);

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // test a A429 with only the prod_id
    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_a429, received_payload, 100, 1000 * 100) == A429_SIZE + VISTAS_HEADER_SIZE,
                "We have received the A429.");

    TEST_HEADER(received_payload, 42, 0, 0, 0);

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // test a CAN with only the SN

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_can, received_payload, 100, 1000 * 100) == 2*CAN_SIZE + VISTAS_HEADER_SIZE,
                "We have received the CAN.");

    TEST_HEADER(received_payload, 0, 1, 0, 0);

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_can, received_payload, 100, 1000 * 100) == 2*CAN_SIZE + VISTAS_HEADER_SIZE,
                "We have received the CAN.");

    TEST_HEADER(received_payload, 0, 2, 0, 0);

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_can, received_payload, 100, 1000 * 100) == 2*CAN_SIZE + VISTAS_HEADER_SIZE,
                "We have received the CAN.");

    TEST_HEADER(received_payload, 0, 1, 0, 0);

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // test an AFDX with only the POSIX timestamp

    TEST_ASSERT(actor, ims_test_mc_input_receive(socket_afdx, received_payload, 100, 1000 * 100) == AFDX_SIZE + VISTAS_HEADER_SIZE,
                "We have received the AFDX.");

    ref_qos_timestamp = ims_test_time_us();

    TEST_HEADER(received_payload, 0, 0, ref_qos_timestamp, 0);

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    // test a discrete with only the data timestamp

    // the discrete is sent every step, we need to unstack every messages !
    uint64_t ref_data_timestamp = 0;
    while ( ims_test_mc_input_receive(socket_discrete, received_payload, 100, 1000 * 100) > 0 )
    {
        ref_data_timestamp+=100*1000;
        if ( ref_data_timestamp >= 500*1000 )
        {
            // actor1 call "ims_reset_all" after 5 steps, which resets the simulation time, so we must reboot the simulation time
            ref_data_timestamp = 100*1000;
        }
        TEST_HEADER(received_payload, 0, 0, 0, ref_data_timestamp);
    }

    ims_test_mc_input_free(socket_nad);
    ims_test_mc_input_free(socket_a429);
    ims_test_mc_input_free(socket_can);
    ims_test_mc_input_free(socket_afdx);
    ims_test_mc_input_free(socket_discrete);

    return ims_test_end(actor);
}
