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
// actor 2
//
#include "ims_test.h"
#include "htonl.h"

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"


#define ACTOR_ID 2
ims_test_actor_t actor;


//
// Message data
//
#define MESSAGE_SIZE 42
static const char expected_payload1[MESSAGE_SIZE] = "hello, world!";
static const char expected_payload2[MESSAGE_SIZE] = "You too!";

#define MESSAGE1_IP    "226.23.12.4"
#define MESSAGE1_PORT  5078

#define MESSAGE1_CHANNEL "firstEquipment_firstApplication_AFDX_OUT_oneAFDX"

#define MESSAGE2_IP    "226.23.12.42"
#define MESSAGE2_PORT  5079

#define MESSAGE2_CHANNEL "226.23.12.42:5079"

#define InstrumentationIP   "127.0.0.1"
#define InstrumentationPort 5467

#define INVALID_POINTER ((void*)42)


#define F_INSTRUM_REGISTER 1
#define R_INSTRUM_REGISTER 2
#define F_FAILURE 3
#define R_FAILURE 4

#define OVERRIDE_EMISSION 6
#define STOP_OVERRIDE_EMISSION 8

#pragma pack (push, 1)

struct vistas_vcc_packet {
   uint32_t command_id;
   uint32_t payload_size;
   uint32_t acknowledge_status;
};


// the channel name is at worst "xxx.xxx.xxx.xxx:xxxxx", so 24 bytes should always be big enough

struct vistas_vcc_f_failure {
   uint32_t command_id;
   uint32_t payload_size;
   uint32_t string_size;
   char channel_name[100];
   uint32_t failure_command_id;
};

struct vistas_vcc_f_failure_target {
   uint32_t command_id;
   uint32_t payload_size;
   uint32_t string_size;
   char channel_name[100];
   uint32_t failure_command_id;
   uint32_t target_string_size;
   char target_channel_name[24];
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

#define VC_NAME "testVirtualComponent"

int main()
{
  char received_payload[100];

  actor = ims_test_init(ACTOR_ID);

  ims_test_mc_input_t socket1 = ims_test_mc_input_create(actor, MESSAGE1_IP, MESSAGE1_PORT);
  ims_test_mc_input_t socket2 = ims_test_mc_input_create(actor, MESSAGE2_IP, MESSAGE2_PORT);
  
  TEST_SIGNAL(actor, 1);  // We are ready
  
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

  TEST_SIGNAL(actor, 1);  // We are ready
  TEST_WAIT(actor, 1);    // Wait write

  // Only message1 has been written
  TEST_ASSERT(actor, ims_test_mc_input_receive(socket1, received_payload, MESSAGE_SIZE + VISTAS_HEADER_SIZE, 100 * 1000) == MESSAGE_SIZE + VISTAS_HEADER_SIZE,
              "Message1 received and has the good length.");
  TEST_ASSERT(actor, strcmp(received_payload + VISTAS_HEADER_SIZE, expected_payload1) == 0, "Message1 has the expected content.");

  TEST_ASSERT(actor, ims_test_mc_input_receive(socket2, received_payload, MESSAGE_SIZE, 100 * 1000) == 0,
              "No message2 received");


  // Ask to redirect
  struct vistas_vcc_f_failure_target f_failure_target;
  f_failure_target.command_id = htonl(F_FAILURE);
  f_failure_target.payload_size = htonl( sizeof(f_failure_target) );
  f_failure_target.string_size = htonl( sizeof(f_failure_target.channel_name) );
  strcpy( f_failure_target.channel_name,  MESSAGE1_CHANNEL );
  f_failure_target.failure_command_id = htonl(OVERRIDE_EMISSION);
  f_failure_target.target_string_size = htonl( sizeof(f_failure_target.target_channel_name) );
  strcpy( f_failure_target.target_channel_name,  MESSAGE2_CHANNEL );
  
  ims_test_tcp_send(instrum, (char*)&f_failure_target, sizeof(f_failure_target));

  TEST_SIGNAL(actor, 1);  // We are ready
  TEST_WAIT(actor, 1);    // Wait write


  // Check instrum ACK
  TEST_ASSERT(actor, ims_test_tcp_receive(instrum, (char*) &msg_packet, sizeof(msg_packet) , 1000*1000 ) ==  sizeof(msg_packet),
              "Instrumentation answer received and has the good size.");
  TEST_ASSERT(actor, msg_packet.command_id == htonl(R_FAILURE),
              "Received R_FAILURE.");
  TEST_ASSERT(actor, msg_packet.payload_size == htonl(sizeof(msg_packet)),
              "Received R_FAILURE with good packet size.");


  // actor1 wrote the same message but it should have been redirected to socket2
  TEST_ASSERT(actor, ims_test_mc_input_receive(socket1, received_payload, MESSAGE_SIZE, 100 * 1000) == 0,
              "No message1 received.");

  TEST_ASSERT(actor, ims_test_mc_input_receive(socket2, received_payload, MESSAGE_SIZE + VISTAS_HEADER_SIZE, 100 * 1000) == MESSAGE_SIZE + VISTAS_HEADER_SIZE,
              "Message2 received and has the good length.");
  TEST_ASSERT(actor, strcmp(received_payload + VISTAS_HEADER_SIZE, expected_payload2) == 0, "Message2 has the expected content.");



  // Ack to stop the redirection
  struct vistas_vcc_f_failure f_failure;
  f_failure.command_id = htonl(F_FAILURE);
  f_failure.payload_size = htonl( sizeof(f_failure) );
  f_failure.string_size = htonl( sizeof(f_failure.channel_name) );
  strcpy( f_failure.channel_name,  MESSAGE1_CHANNEL );
  f_failure.failure_command_id = htonl(STOP_OVERRIDE_EMISSION);
  
  ims_test_tcp_send(instrum, (char*)&f_failure, sizeof(f_failure));

  TEST_SIGNAL(actor, 1);  // We are ready
  TEST_WAIT(actor, 1);    // Wait write

  // Check instrum ACK
  TEST_ASSERT(actor, ims_test_tcp_receive(instrum, (char*) &msg_packet, sizeof(msg_packet) , 1000*1000 ) ==  sizeof(msg_packet),
              "Instrumentation answer received and has the good size.");
  TEST_ASSERT(actor, msg_packet.command_id ==  htonl(R_FAILURE),
              "Received R_FAILURE.");
  TEST_ASSERT(actor, msg_packet.payload_size ==  htonl(sizeof(msg_packet)),
              "Received R_FAILURE with good packet size.");


  // actor1 should have write to socket1 again
  TEST_ASSERT(actor, ims_test_mc_input_receive(socket1, received_payload, MESSAGE_SIZE + VISTAS_HEADER_SIZE, 100 * 1000) == MESSAGE_SIZE + VISTAS_HEADER_SIZE,
              "Message1 received and has the good length.");
  TEST_ASSERT(actor, strcmp(received_payload + VISTAS_HEADER_SIZE, expected_payload1) == 0, "Message1 has the expected content.");

  // No message2 has been sent
  TEST_ASSERT(actor, ims_test_mc_input_receive(socket2, received_payload, MESSAGE_SIZE, 100 * 1000) == 0,
              "No message2 received");

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

  ims_test_mc_input_free(socket1);
  ims_test_mc_input_free(socket2);

  return ims_test_end(actor);
}
