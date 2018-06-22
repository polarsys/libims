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

#define F_SYNC_REGISTER 13
#define R_SYNC_REGISTER 14
#define F_SYNCHRO       15
#define R_SYNCHRO       16
#define F_CONFIG        17
#define R_CONFIG        18

#define LOAD         0
#define APPLY_CONFIG 1

#pragma pack (push, 1)

struct vistas_vcc_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t acknowledge_status;
};

#define REQUEST_MAX_STRING_SIZE 100

struct vistas_f_sync_register_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t application_period_ms;
    uint32_t register_command;
    uint32_t string_size;
    char     vc_name[REQUEST_MAX_STRING_SIZE];
};

struct vistas_f_config_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t config_command_id;
    uint32_t string_size;
    char     config_name[REQUEST_MAX_STRING_SIZE];
};

struct vistas_r_config_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t ack_id;
    uint32_t ack_value;
};

#pragma pack (pop)

#define VC_NAME "testVirtualComponent"
#define CONFIG_NAME "testConfig"

//
// Main
//
int main()
{
    actor = ims_test_init(ACTOR_ID);

    ims_test_tcp_t control = ims_test_tcp_create(actor, InstrumentationIP, InstrumentationPort);

    struct vistas_vcc_packet msg_packet;
    struct vistas_f_sync_register_packet f_sync_packet;

    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &f_sync_packet, sizeof(f_sync_packet) , 1000*1000 ) ==  sizeof(f_sync_packet),
                "Received the register packet.");
    TEST_ASSERT(actor, f_sync_packet.command_id ==  htonl(F_SYNC_REGISTER),
                "Received F_SYNC_REGISTER.");
    TEST_ASSERT(actor, f_sync_packet.payload_size ==  htonl(sizeof(f_sync_packet)),
                "Received F_SYNC_REGISTER with good packet size.");
    TEST_ASSERT(actor, f_sync_packet.application_period_ms ==  htonl(100),
                "Received F_SYNC_REGISTER with good period.");
    TEST_ASSERT(actor, f_sync_packet.register_command ==  htonl(1),
                "Received F_SYNC_REGISTER with good register command.");
    TEST_ASSERT(actor, ntohl(f_sync_packet.string_size) ==  strlen(VC_NAME)+1 ,
                "Received F_SYNC_REGISTER with good string size.");
    TEST_ASSERT(actor, strcmp(f_sync_packet.vc_name, VC_NAME) == 0 ,
                "Received F_SYNC_REGISTER with good VC name.");

    // we need to answer
    msg_packet.command_id = htonl(R_SYNC_REGISTER);
    msg_packet.payload_size = htonl(sizeof(msg_packet));
    msg_packet.acknowledge_status = htonl(0);

    ims_test_tcp_send(control, (char*)&msg_packet, sizeof(msg_packet));

    // Send a synchro request
    msg_packet.command_id = htonl(F_SYNCHRO);
    msg_packet.payload_size = htonl( sizeof(msg_packet) );
    msg_packet.acknowledge_status = htonl(2);

    ims_test_tcp_send(control, (char*)&msg_packet, sizeof(msg_packet));

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    // Send a config LOAD request
    struct vistas_f_config_packet f_config_packet;
    f_config_packet.command_id = htonl(F_CONFIG);
    f_config_packet.payload_size = htonl( sizeof(f_config_packet) );
    f_config_packet.config_command_id = htonl(LOAD);
    f_config_packet.string_size = htonl( strlen(CONFIG_NAME) + 1 );
    strcpy( f_config_packet.config_name, CONFIG_NAME );

    ims_test_tcp_send(control, (char*)&f_config_packet, sizeof(f_config_packet));

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    // Check config ACK
    struct vistas_r_config_packet r_config_packet;
    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &r_config_packet, sizeof(r_config_packet) , 1000*1000 ) ==  sizeof(r_config_packet),
                "Instrumentation answer received and has the good size.");
    TEST_ASSERT(actor, r_config_packet.command_id == htonl(R_CONFIG),
                "Received R_CONFIG.");
    TEST_ASSERT(actor, r_config_packet.payload_size == htonl(sizeof(r_config_packet)),
                "Received R_CONFIG with good packet size.");
    TEST_ASSERT(actor, r_config_packet.ack_id == htonl(LOAD),
                "Received R_CONFIG with good command id.");
    TEST_ASSERT(actor, r_config_packet.ack_value == htonl(0),
                "Received R_CONFIG with good ack value.");

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    // Check control ACK

    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &msg_packet, sizeof(msg_packet) , 1000*1000 ) ==  sizeof(msg_packet),
                "Instrumentation answer received and has the good size.");
    TEST_ASSERT(actor, msg_packet.command_id == htonl(R_SYNCHRO),
                "Received R_SYNCHRO.");
    TEST_ASSERT(actor, msg_packet.payload_size == htonl(sizeof(msg_packet)),
                "Received R_SYNCHRO with good packet size.");

    // Send another synchro request
    msg_packet.command_id = htonl(F_SYNCHRO);
    msg_packet.payload_size = htonl( sizeof(msg_packet) );
    msg_packet.acknowledge_status = htonl(2);

    ims_test_tcp_send(control, (char*)&msg_packet, sizeof(msg_packet));

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    // Send a config APPLY request

    f_config_packet.command_id = htonl(F_CONFIG);
    f_config_packet.payload_size = htonl( sizeof(f_config_packet) );
    f_config_packet.config_command_id = htonl(APPLY_CONFIG);
    f_config_packet.string_size = htonl( strlen(CONFIG_NAME) + 1 );
    strcpy( f_config_packet.config_name, CONFIG_NAME );

    ims_test_tcp_send(control, (char*)&f_config_packet, sizeof(f_config_packet));

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    // Check the two config ACK !
    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &r_config_packet, sizeof(r_config_packet) , 1000*1000 ) ==  sizeof(r_config_packet),
                "Instrumentation answer received and has the good size.");
    TEST_ASSERT(actor, r_config_packet.command_id == htonl(R_CONFIG),
                "Received R_CONFIG.");
    TEST_ASSERT(actor, r_config_packet.payload_size == htonl(sizeof(r_config_packet)),
                "Received R_CONFIG with good packet size.");
    TEST_ASSERT(actor, r_config_packet.ack_id == htonl(APPLY_CONFIG),
                "Received R_CONFIG with good command id.");
    TEST_ASSERT(actor, r_config_packet.ack_value == htonl(0),
                "Received R_CONFIG with good ack value.");

    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &r_config_packet, sizeof(r_config_packet) , 1000*1000 ) ==  sizeof(r_config_packet),
                "Instrumentation answer received and has the good size.");
    TEST_ASSERT(actor, r_config_packet.command_id == htonl(R_CONFIG),
                "Received R_CONFIG.");
    TEST_ASSERT(actor, r_config_packet.payload_size == htonl(sizeof(r_config_packet)),
                "Received R_CONFIG with good packet size.");
    TEST_ASSERT(actor, r_config_packet.ack_id == htonl(APPLY_CONFIG),
                "Received R_CONFIG with good command id.");
    TEST_ASSERT(actor, r_config_packet.ack_value == htonl(2),
                "Received R_CONFIG with good ack value.");

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    // Check control ACK
    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &msg_packet, sizeof(msg_packet) , 1000*1000 ) ==  sizeof(msg_packet),
                "Instrumentation answer received and has the good size.");
    TEST_ASSERT(actor, msg_packet.command_id == htonl(R_SYNCHRO),
                "Received R_SYNCHRO.");
    TEST_ASSERT(actor, msg_packet.payload_size == htonl(sizeof(msg_packet)),
                "Received R_SYNCHRO with good packet size.");

    TEST_SIGNAL(actor, 2);
    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_test_tcp_receive(control, (char*) &f_sync_packet, sizeof(f_sync_packet) , 1000*1000 ) ==  sizeof(f_sync_packet),
                "Received the register packet.");
    TEST_ASSERT(actor, f_sync_packet.command_id ==  htonl(F_SYNC_REGISTER),
                "Received F_SYNC_REGISTER.");
    TEST_ASSERT(actor, f_sync_packet.payload_size ==  htonl(sizeof(f_sync_packet)),
                "Received F_SYNC_REGISTER with good packet size.");
    TEST_ASSERT(actor, f_sync_packet.application_period_ms ==  htonl(100),
                "Received F_SYNC_REGISTER with good period.");
    TEST_ASSERT(actor, f_sync_packet.register_command ==  htonl(0),
                "Received F_SYNC_REGISTER with good register command.");
    TEST_ASSERT(actor, ntohl(f_sync_packet.string_size) ==  strlen(VC_NAME)+1 ,
                "Received F_SYNC_REGISTER with good string size.");
    TEST_ASSERT(actor, strcmp(f_sync_packet.vc_name, VC_NAME) == 0 ,
                "Received F_SYNC_REGISTER with good VC name.");

    return ims_test_end(actor);
}
