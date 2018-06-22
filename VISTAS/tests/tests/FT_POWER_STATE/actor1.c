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

#define F_POWER_REGISTER 9
#define R_POWER_REGISTER 10
#define F_POWERSUPPLY 11
#define R_POWERSUPPLY 12

#define POWER_OFF 0
#define POWER_ON 1

#pragma pack (push, 1)

#define REQUEST_MAX_STRING_SIZE          100

struct vistas_f_register_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t register_command;
    uint32_t string_size;
    char     vc_name[REQUEST_MAX_STRING_SIZE];
};

struct vistas_vcc_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t acknowledge_status;
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

    struct vistas_f_register_packet register_packet;

    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &register_packet, sizeof(register_packet) , 1000*1000 ) ==  sizeof(register_packet),
                "Received the register packet.");
    TEST_ASSERT(actor, register_packet.command_id ==  htonl(F_POWER_REGISTER),
                "Received F_POWER_REGISTER.");
    TEST_ASSERT(actor, register_packet.payload_size ==  htonl(sizeof(register_packet)),
                "Received F_POWER_REGISTER with good packet size.");
    TEST_ASSERT(actor, register_packet.register_command ==  htonl(1),
                "Received F_POWER_REGISTER with good register command.");
    TEST_ASSERT(actor, ntohl(register_packet.string_size) ==  strlen(VC_NAME)+1 ,
                "Received F_POWER_REGISTER with good string size.");
    TEST_ASSERT(actor, strcmp(register_packet.vc_name, VC_NAME) == 0 ,
                "Received F_POWER_REGISTER with good VC name.");

    // we need to answer
    struct vistas_vcc_packet msg_packet;

    msg_packet.command_id = htonl(R_POWER_REGISTER);
    msg_packet.payload_size = htonl(sizeof(msg_packet));
    msg_packet.acknowledge_status = htonl(0);

    ims_test_tcp_send(control, (char*)&msg_packet, sizeof(msg_packet));

    TEST_WAIT(actor, 2); // Wait actor2 ready to read before starting our tests

    // Send power on request
    msg_packet.command_id = htonl(F_POWERSUPPLY);
    msg_packet.payload_size = htonl( sizeof(msg_packet) );
    msg_packet.acknowledge_status = htonl(POWER_ON);

    ims_test_tcp_send(control, (char*)&msg_packet, sizeof(msg_packet));

    TEST_SIGNAL(actor, 2); // We have send run

    // Check control ACK
    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &msg_packet, sizeof(msg_packet) , 1000*1000 ) ==  sizeof(msg_packet),
                "Instrumentation answer received and has the good size.");
    TEST_ASSERT(actor, msg_packet.command_id == htonl(R_POWERSUPPLY),
                "Received R_POWERSUPPLY.");
    TEST_ASSERT(actor, msg_packet.payload_size == htonl(sizeof(msg_packet)),
                "Received R_POWERSUPPLY with good packet size.");

    TEST_WAIT(actor, 2);

    // Send power off request
    msg_packet.command_id = htonl(F_POWERSUPPLY);
    msg_packet.payload_size = htonl( sizeof(msg_packet) );
    msg_packet.acknowledge_status = htonl(POWER_OFF);

    ims_test_tcp_send(control, (char*)&msg_packet, sizeof(msg_packet));

    TEST_SIGNAL(actor, 2); // We have send stop

    // Check control ACK
    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &msg_packet, sizeof(msg_packet) , 1000*1000 ) ==  sizeof(msg_packet),
                "Instrumentation answer received and has the good size.");
    TEST_ASSERT(actor, msg_packet.command_id == htonl(R_POWERSUPPLY),
                "Received R_POWERSUPPLY.");
    TEST_ASSERT(actor, msg_packet.payload_size == htonl(sizeof(msg_packet)),
                "Received R_POWERSUPPLY with good packet size.");

    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &register_packet, sizeof(register_packet) , 1000*1000 ) ==  sizeof(register_packet),
                "Received the register packet.");
    TEST_ASSERT(actor, register_packet.command_id ==  htonl(F_POWER_REGISTER),
                "Received F_POWER_REGISTER.");
    TEST_ASSERT(actor, register_packet.payload_size ==  htonl(sizeof(register_packet)),
                "Received F_POWER_REGISTER with good packet size.");
    TEST_ASSERT(actor, register_packet.register_command ==  htonl(0),
                "Received F_POWER_REGISTER with good register command.");
    TEST_ASSERT(actor, ntohl(register_packet.string_size) ==  strlen(VC_NAME)+1 ,
                "Received F_POWER_REGISTER with good string size.");
    TEST_ASSERT(actor, strcmp(register_packet.vc_name, VC_NAME) == 0 ,
                "Received F_POWER_REGISTER with good VC name.");

    return ims_test_end(actor);
}
