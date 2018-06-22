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

#define GOOD_VC_NAME            "GoodVirtualComponent"

#define IMS_CONFIG_FILE         "config/actor1/ims.xml"
#define IMS_INIT_FILE           "config/actor1/init.xml"

#define VISTAS_CONFIG_FILE_0    "config/actor1/vistas0.xml"
#define VISTAS_CONFIG_FILE_1    "config/actor1/vistas1.xml"
#define VISTAS_CONFIG_FILE_2    "config/actor1/vistas2.xml"
#define VISTAS_CONFIG_FILE_3    "config/actor1/vistas3.xml"
#define VISTAS_CONFIG_FILE_4    "config/actor1/vistas4.xml"

#define ACTOR_ID 1
ims_test_actor_t actor;

#define INVALID_POINTER ((void*)42)

//
// Main
//
int main()
{
    ims_node_t              ims_context;

    ims_return_code_t       ims_return_code;

    actor = ims_test_init(ACTOR_ID);

    ims_create_context_parameter_t create_parameter = IMS_CREATE_CONTEXT_INITIALIZER;
    create_parameter.init_file_path = IMS_INIT_FILE;

    ims_context = (ims_node_t)INVALID_POINTER;

    // Case 0: Unique VC
    ims_return_code = ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE_0, &create_parameter, &ims_context);
    TEST_ASSERT(actor, ims_return_code == ims_no_error && ims_context != (ims_node_t)INVALID_POINTER && ims_context != NULL, "We can create context from vistas0.xml");
    if(ims_return_code == ims_no_error)TEST_ASSERT(actor, strcmp(ims_virtual_component_name(ims_context),GOOD_VC_NAME) == 0, "The virtual component has the right name: %s", GOOD_VC_NAME);
    if(ims_return_code == ims_no_error)ims_free_context(ims_context);

    // Case 1: Two VC and get the second one by its name
    ims_return_code = ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE_1, &create_parameter, &ims_context);
    TEST_ASSERT(actor, ims_return_code == ims_no_error && ims_context != (ims_node_t)INVALID_POINTER && ims_context != NULL, "We can create context from vistas1.xml");
    if(ims_return_code == ims_no_error)TEST_ASSERT(actor, strcmp(ims_virtual_component_name(ims_context),GOOD_VC_NAME) == 0, "The virtual component has the right name: %s", GOOD_VC_NAME);
    if(ims_return_code == ims_no_error)ims_free_context(ims_context);

    // Case 2: Two VC and get the first one automatically
    ims_return_code = ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE_2, &create_parameter, &ims_context);
    TEST_ASSERT(actor, ims_return_code == ims_no_error && ims_context != (ims_node_t)INVALID_POINTER && ims_context != NULL, "We can create context from vistas2.xml");
    if(ims_return_code == ims_no_error)TEST_ASSERT(actor, strcmp(ims_virtual_component_name(ims_context),GOOD_VC_NAME) == 0, "The virtual component has the right name: %s", GOOD_VC_NAME);
    if(ims_return_code == ims_no_error)ims_free_context(ims_context);

    // Case 3: One VC and no valid virtual component name
    ims_return_code = ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE_3, &create_parameter, &ims_context);
    TEST_ASSERT(actor, ims_return_code == ims_init_failure, "We cannot create context from vistas3.xml");

    // Case 4: Two VC with the same name
    ims_return_code = ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE_4, &create_parameter, &ims_context);
    TEST_ASSERT(actor, ims_return_code == ims_init_failure, "We cannot create context from vistas4.xml");

    return ims_test_end(actor);
}
