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
// AFDX test - actor 1
//
#include "ims_test.h"
#include "htonl.h"

#define ACTOR_ID 1
ims_test_actor_t actor;

//
// Message data
//
#define InstrumentationIP   "127.0.0.1"
#define InstrumentationPort 5467

#define F_MODE_REGISTER 5
#define R_MODE_REGISTER 6
#define F_STATE 7
#define R_STATE 8

#define RESET 0
#define RUN 1
#define STEP 2
#define HOLD 3

#pragma pack (push, 1)

struct vistas_vcc_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t acknowledge_status;
};

struct vistas_vcc_f_state {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t state_command_id;
    uint32_t synchro_mode;
    uint32_t time_ratio;
};

union union_int_float {
    uint32_t as_int;
    float as_float;
};

#define REQUEST_MAX_STRING_SIZE 100

struct vistas_f_mode_register_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t register_command;
    uint32_t step_command;
    uint32_t string_size;
    char     vc_name[REQUEST_MAX_STRING_SIZE];
};

#pragma pack (pop)

#define VC_NAME "testVirtualComponent"

//
// Main
//
int main()
{
    actor = ims_test_init(ACTOR_ID);

    ims_test_tcp_t control = ims_test_tcp_create(actor, InstrumentationIP, InstrumentationPort);

    struct vistas_vcc_packet msg_packet;
    struct vistas_f_mode_register_packet f_mode_packet;

    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &f_mode_packet, sizeof(f_mode_packet) , 1000*1000 ) ==  sizeof(f_mode_packet),
                "Received the register packet.");
    TEST_ASSERT(actor, f_mode_packet.command_id ==  htonl(F_MODE_REGISTER),
                "Received F_MODE_REGISTER.");
    TEST_ASSERT(actor, f_mode_packet.payload_size ==  htonl(sizeof(f_mode_packet)),
                "Received F_MODE_REGISTER with good packet size.");
    TEST_ASSERT(actor, f_mode_packet.register_command ==  htonl(1),
                "Received F_MODE_REGISTER with good register command.");
    TEST_ASSERT(actor, f_mode_packet.step_command ==  htonl(0),
                "Received F_MODE_REGISTER with good step command.");
    TEST_ASSERT(actor, ntohl(f_mode_packet.string_size) ==  strlen(VC_NAME)+1 ,
                "Received F_MODE_REGISTER with good string size.");
    TEST_ASSERT(actor, strcmp(f_mode_packet.vc_name, VC_NAME) == 0 ,
                "Received F_MODE_REGISTER with good VC name.");

    // we need to answer
    msg_packet.command_id = htonl(R_MODE_REGISTER);
    msg_packet.payload_size = htonl(sizeof(msg_packet));
    msg_packet.acknowledge_status = htonl(0);

    ims_test_tcp_send(control, (char*)&msg_packet, sizeof(msg_packet));

    TEST_WAIT(actor, 2); // Wait actor2 ready to read before starting our tests

    // Send run request
    struct vistas_vcc_f_state f_state;

    union union_int_float my_union;
    my_union.as_float = 1.5;

    f_state.command_id = htonl(F_STATE);
    f_state.payload_size = htonl( sizeof(f_state) );
    f_state.state_command_id = htonl(RUN);
    f_state.synchro_mode = htonl(0);
    f_state.time_ratio = htonl(my_union.as_int);

    ims_test_tcp_send(control, (char*)&f_state, sizeof(f_state));

    TEST_SIGNAL(actor, 2); // We have send run

    // Check control ACK
    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &f_state, sizeof(f_state) , 1000*1000 ) ==  sizeof(f_state),
                "Instrumentation answer received and has the good size.");
    TEST_ASSERT(actor, f_state.command_id == htonl(R_STATE),
                "Received R_STATE.");
    TEST_ASSERT(actor, f_state.payload_size == htonl(sizeof(f_state)),
                "Received R_STATE with good packet size.");

    TEST_WAIT(actor, 2);

    // Send stop request
    f_state.command_id = htonl(F_STATE);
    f_state.payload_size = htonl( sizeof(f_state) );
    f_state.state_command_id = htonl(STEP);
    f_state.synchro_mode = htonl(0);
    f_state.time_ratio = htonl(my_union.as_int);

    ims_test_tcp_send(control, (char*)&f_state, sizeof(f_state));

    TEST_SIGNAL(actor, 2); // We have send stop

    // Check control ACK
    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &f_state, sizeof(f_state) , 1000*1000 ) ==  sizeof(f_state),
                "Instrumentation answer received and has the good size.");
    TEST_ASSERT(actor, f_state.command_id == htonl(R_STATE),
                "Received R_STATE.");
    TEST_ASSERT(actor, f_state.payload_size == htonl(sizeof(f_state)),
                "Received R_STATE with good packet size.");

    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &f_mode_packet, sizeof(f_mode_packet) , 1000*1000 ) ==  sizeof(f_mode_packet),
                "Received the register packet.");
    TEST_ASSERT(actor, f_mode_packet.command_id ==  htonl(F_MODE_REGISTER),
                "Received F_MODE_REGISTER.");
    TEST_ASSERT(actor, f_mode_packet.payload_size ==  htonl(sizeof(f_mode_packet)),
                "Received F_MODE_REGISTER with good packet size.");
    TEST_ASSERT(actor, f_mode_packet.register_command ==  htonl(0),
                "Received F_MODE_REGISTER with good register command.");
    TEST_ASSERT(actor, f_mode_packet.step_command ==  htonl(0),
                "Received F_MODE_REGISTER with good step command.");
    TEST_ASSERT(actor, ntohl(f_mode_packet.string_size) ==  strlen(VC_NAME)+1 ,
                "Received F_MODE_REGISTER with good string size.");
    TEST_ASSERT(actor, strcmp(f_mode_packet.vc_name, VC_NAME) == 0 ,
                "Received F_MODE_REGISTER with good VC name.");

    return ims_test_end(actor);
}
