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

#define IMS_CONFIG_FILE      "config/actor1/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor1/vistas.xml"
#define IMS_INIT_FILE        "config/actor1/init.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

#define INVALID_POINTER ((void*)42)

static char * get_type_string(ims_nad_type_t type)
{
    switch(type)
    {
    case ims_nad_boolean:
        return "BOOLEAN";
    case ims_nad_c08:
        return "C08";
    case ims_nad_f32:
        return "F32";
    case ims_nad_f64:
        return "F64";
    case ims_nad_i16:
        return "I16";
    case ims_nad_i32:
        return "I32";
    case ims_nad_i64:
        return "I64";
    default:
        return "UNDEFINED";
    }
}

void dump_node_messages(ims_node_t parent, int level)
{
    ims_message_t       ims_message;
    ims_messages_list_t  ims_messages_list;
    char             indent[50];

    memset(indent, ' ', 2 * level);
    memset(indent + 2 * level, 0, 1);

    ims_node_get_messages(parent, &ims_messages_list);

    while (ims_messages_list_next(ims_messages_list, &ims_message) == ims_no_error && ims_message != NULL) {
        TEST_LOG(actor, "%s + Message %s (%s %s %s, max size: %d, depth: %d, validity duration: %u, Local name : %s, Bus name : %s, Period us %d, Nad type %s[%d][%d])", indent,
                 ims_message_get_name(ims_message),
                 ims_direction_string(ims_message_get_direction(ims_message)),
                 ims_protocol_string(ims_message_get_protocol(ims_message)),
                 ims_mode_string(ims_message_get_mode(ims_message)),
                 ims_message_get_max_size(ims_message),
                 ims_message_get_depth(ims_message),
                 ims_message_get_validity_duration_us(ims_message),
                 ims_message_get_local_name(ims_message),
                 ims_message_get_bus_name(ims_message),
                 ims_message_get_period_us(ims_message),
                 get_type_string(ims_message_get_nad_type(ims_message)),
                 ims_message_get_nad_dim1(ims_message),
                 ims_message_get_nad_dim2(ims_message)
                 );
    }

    ims_messages_list_free(ims_messages_list);

}

void dump_node_children(ims_node_t parent, int level)
{
    ims_node_t       ims_node;
    ims_node_list_t  ims_node_list;
    char             indent[50];

    memset(indent, ' ', 2 * level);
    memset(indent + 2 * level, 0, 1);

    ims_node_get_children(parent, &ims_node_list);

    while (ims_node_list_next(ims_node_list, &ims_node) == ims_no_error && ims_node != NULL) {
        TEST_LOG(actor, "%s + Node %s", indent, ims_node_get_name(ims_node));
        dump_node_messages(ims_node, level + 1);
        dump_node_children(ims_node, level + 1);
    }

    ims_node_list_free(ims_node_list);

}

//
// Main
//
int main()
{
    ims_node_t           ims_context;
    ims_node_t           ims_node, ims_node_temp;
    ims_node_list_t      ims_node_list;
    ims_messages_list_t  ims_messages_list;
    ims_message_t        ims_message;

    actor = ims_test_init(ACTOR_ID);

    ims_create_context_parameter_t create_parameter = IMS_CREATE_CONTEXT_INITIALIZER;
    create_parameter.init_file_path = IMS_INIT_FILE;

    ims_context = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE, &create_parameter, &ims_context) == ims_no_error &&
                       ims_context != (ims_node_t)INVALID_POINTER && ims_context != NULL,
                       "We can create a valid context.");

    // Check browse of nodes

    TEST_ASSERT(actor, ims_node_get_children(ims_context, &ims_node_list) == ims_no_error && ims_node_list != NULL,
                "We can get a node list from the context.");

    TEST_ASSERT(actor, ims_node_list_next(ims_node_list, &ims_node) == ims_no_error && ims_node != NULL,
                "We can get the first equipment named '%s'.", ims_node_get_name(ims_node));

    TEST_ASSERT(actor, ims_node_get_child(ims_context, ims_node_get_name(ims_node), &ims_node_temp) == ims_no_error && ims_node_temp != NULL,
                "We can get again the first equipment named '%s'.", ims_node_get_name(ims_node_temp));

    TEST_ASSERT(actor, ims_node_list_next(ims_node_list, &ims_node) == ims_no_error && ims_node != NULL,
                "We can get the second equipment named '%s'.", ims_node_get_name(ims_node));

    TEST_ASSERT(actor, ims_node_list_next(ims_node_list, &ims_node) == ims_no_error && ims_node == NULL,
                "No more equipment.");

    TEST_ASSERT(actor, ims_node_list_next(ims_node_list, &ims_node) == ims_no_error && ims_node != NULL,
                "We can get again the first equipment named '%s'.", ims_node_get_name(ims_node));

    ims_node_list_free(ims_node_list);

    // Check browse of messages
    TEST_ASSERT(actor, ims_node_get_messages(ims_context, &ims_messages_list) == ims_no_error && ims_messages_list != NULL,
                "We can get a message list from the context.");

    TEST_ASSERT(actor, ims_messages_list_next(ims_messages_list, &ims_message) == ims_no_error && ims_message != NULL,
                "We can get the first message named '%s'.", ims_message_get_name(ims_message));

    TEST_ASSERT(actor, ims_messages_list_next(ims_messages_list, &ims_message) == ims_no_error && ims_message != NULL,
                "We can get the second message named '%s'.", ims_message_get_name(ims_message));

    TEST_ASSERT(actor, ims_messages_list_next(ims_messages_list, &ims_message) == ims_no_error && ims_message == NULL,
                "No more messages.");

    TEST_ASSERT(actor, ims_messages_list_next(ims_messages_list, &ims_message) == ims_no_error && ims_message != NULL,
                "We can get again the first message named '%s'.", ims_message_get_name(ims_message));

    ims_messages_list_free(ims_messages_list);

    // Traverse all nodes
    TEST_LOG(actor, "Dump configuration :");
    TEST_LOG(actor, " + Node %s", ims_node_get_name(ims_context));
    dump_node_messages(ims_context, 1);
    dump_node_children(ims_context, 1);

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
