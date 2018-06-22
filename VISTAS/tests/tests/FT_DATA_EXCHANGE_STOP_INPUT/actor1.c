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

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

//
// Message data
//
#define MESSAGE_SIZE     42

static const char payload1[MESSAGE_SIZE] = "hello, world!";
static const char payload2[MESSAGE_SIZE] = "You too!";
static const char payload3[MESSAGE_SIZE] = "Ok, perfect!";

#define MESSAGE1_IP    "226.23.12.4"
#define MESSAGE1_PORT  5078

#define MESSAGE1_CHANNEL "firstEquipment_firstApplication_AFDX_IN_oneAFDX"

#define InstrumentationIP   "127.0.0.1"
#define InstrumentationPort 5467

#define F_INSTRUM_REGISTER 1
#define R_INSTRUM_REGISTER 2
#define F_FAILURE 3
#define R_FAILURE 4

#define STOP_RECEPTION 1
#define START_RECEPTION 5

#pragma pack (push, 1)

struct vistas_vcc_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t acknowledge_status;
};

struct vistas_vcc_f_failure {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t string_size;
    char channel_name[100];
    uint32_t failure_command_id;
};

#define REQUEST_MAX_STRING_SIZE          100

struct vistas_f_register_packet {
    uint32_t command_id;
    uint32_t payload_size;
    uint32_t register_command;
    uint32_t string_size;
    char     vc_name[REQUEST_MAX_STRING_SIZE];
};

#pragma pack (pop)

#define INVALID_POINTER ((void*)42)
#define VC_NAME "testVirtualComponent"

//
// Main
//
int main()
{
    ims_node_t     ims_context;
    ims_node_t     ims_equipment;
    ims_node_t     ims_application;
    ims_message_t  ims_message1;

    actor = ims_test_init(ACTOR_ID);

    ims_test_tcp_t instrum = ims_test_tcp_create(actor, InstrumentationIP, InstrumentationPort);

    struct vistas_f_register_packet register_packet;

    TEST_ASSERT(actor, ims_test_tcp_receive(instrum, (char*) &register_packet, sizeof(register_packet) , 1000*1000 ) ==  sizeof(register_packet),
                "Received the register packet.");
    TEST_ASSERT(actor, register_packet.command_id ==  htonl(F_INSTRUM_REGISTER),
                "Received F_INSTRUM_REGISTER.");
    TEST_ASSERT(actor, register_packet.payload_size ==  htonl(sizeof(register_packet)),
                "Received F_INSTRUM_REGISTER with good packet size.");
    TEST_ASSERT(actor, register_packet.register_command ==  htonl(1),
                "Received F_INSTRUM_REGISTER with good register command.");
    TEST_ASSERT(actor, ntohl(register_packet.string_size) ==  strlen(VC_NAME)+1 ,
                "Received F_INSTRUM_REGISTER with good string size.");
    TEST_ASSERT(actor, strcmp(register_packet.vc_name, VC_NAME) == 0 ,
                "Received F_INSTRUM_REGISTER with good VC name.");

    // we need to answer
    struct vistas_vcc_packet msg_packet;

    msg_packet.command_id = htonl(R_INSTRUM_REGISTER);
    msg_packet.payload_size = htonl(sizeof(msg_packet));
    msg_packet.acknowledge_status = htonl(0);

    ims_test_tcp_send(instrum, (char*)&msg_packet, sizeof(msg_packet));

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

    ims_message1 = (ims_message_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_get_message(ims_application, ims_afdx, "AFDXLocalName", MESSAGE_SIZE, 1, ims_output, &ims_message1) == ims_no_error &&
                       ims_message1 != (ims_message_t)INVALID_POINTER && ims_message1 != NULL,
                       "We can get the first AFDX message.");

    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, payload1, MESSAGE_SIZE) == ims_no_error,
                "Message 1 wrote.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "Messages sent.");

    TEST_SIGNAL(actor, 2); // We have sent
    TEST_WAIT(actor, 2);

    // Ask to stop
    struct vistas_vcc_f_failure f_failure;
    f_failure.command_id = htonl(F_FAILURE);
    f_failure.payload_size = htonl( sizeof(f_failure) );
    f_failure.string_size = htonl( sizeof(f_failure.channel_name) );
    strcpy( f_failure.channel_name,  MESSAGE1_CHANNEL );
    f_failure.failure_command_id = htonl(STOP_RECEPTION);

    ims_test_tcp_send(instrum, (char*)&f_failure, sizeof(f_failure));

    TEST_SIGNAL(actor, 2); // We have stop

    // Check instrum ACK
    TEST_ASSERT(actor, ims_test_tcp_receive(instrum, (char*) &msg_packet, sizeof(msg_packet) , 1000*1000 ) ==  sizeof(msg_packet),
                "Instrumentation answer received and has the good size.");
    TEST_ASSERT(actor, msg_packet.command_id == htonl(R_FAILURE),
                "Received R_FAILURE.");
    TEST_ASSERT(actor, msg_packet.payload_size == htonl(sizeof(msg_packet)),
                "Received R_FAILURE with good packet size.");

    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, payload2, MESSAGE_SIZE) == ims_no_error,
                "Message 1 wrote.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "Messages sent.");

    TEST_SIGNAL(actor, 2); // We have sent
    TEST_WAIT(actor, 2);

    // Ack to restart
    f_failure.command_id = htonl(F_FAILURE);
    f_failure.payload_size = htonl( sizeof(f_failure) );
    f_failure.string_size = htonl( sizeof(f_failure.channel_name) );
    strcpy( f_failure.channel_name,  MESSAGE1_CHANNEL );
    f_failure.failure_command_id = htonl(START_RECEPTION);

    ims_test_tcp_send(instrum, (char*)&f_failure, sizeof(f_failure));

    TEST_SIGNAL(actor, 2); // We have restart

    // Check instrum ACK
    TEST_ASSERT(actor, ims_test_tcp_receive(instrum, (char*) &msg_packet, sizeof(msg_packet) , 1000*1000 ) ==  sizeof(msg_packet),
                "Instrumentation answer received and has the good size.");
    TEST_ASSERT(actor, msg_packet.command_id ==  htonl(R_FAILURE),
                "Received R_FAILURE.");
    TEST_ASSERT(actor, msg_packet.payload_size ==  htonl(sizeof(msg_packet)),
                "Received R_FAILURE with good packet size.");

    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_write_sampling_message(ims_message1, payload3, MESSAGE_SIZE) == ims_no_error,
                "Message 1 wrote.");
    TEST_ASSERT(actor, ims_send_all(ims_context) == ims_no_error, "Messages sent.");

    TEST_SIGNAL(actor, 2); // We have sent

    TEST_WAIT(actor, 2);

    TEST_ASSERT(actor, ims_test_tcp_receive(instrum, (char*) &register_packet, sizeof(register_packet) , 1000*1000 ) ==  sizeof(register_packet),
                "Received the register packet.");
    TEST_ASSERT(actor, register_packet.command_id ==  htonl(F_INSTRUM_REGISTER),
                "Received F_INSTRUM_REGISTER.");
    TEST_ASSERT(actor, register_packet.payload_size ==  htonl(sizeof(register_packet)),
                "Received F_INSTRUM_REGISTER with good packet size.");
    TEST_ASSERT(actor, register_packet.register_command ==  htonl(0),
                "Received F_INSTRUM_REGISTER with good register command.");
    TEST_ASSERT(actor, ntohl(register_packet.string_size) ==  strlen(VC_NAME)+1 ,
                "Received F_INSTRUM_REGISTER with good string size.");
    TEST_ASSERT(actor, strcmp(register_packet.vc_name, VC_NAME) == 0 ,
                "Received F_INSTRUM_REGISTER with good VC name.");

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
