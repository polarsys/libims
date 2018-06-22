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

//
// Main
//
int main()
{
    ims_node_t ims_context;
    ims_node_t ims_equipment;
    ims_node_t ims_application;

    actor = ims_test_init(ACTOR_ID);

    ims_create_context_parameter_t create_parameter = IMS_CREATE_CONTEXT_INITIALIZER;
    create_parameter.init_file_path = IMS_INIT_FILE;

    ims_context = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT(actor, ims_create_context(IMS_CONFIG_FILE, "/file/doesnt/exists", &create_parameter, &ims_context) == ims_init_failure &&
                ims_context == NULL,
                "On error ims_create_context return ims_init_failure.");

    ims_context = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT(actor, ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE, &create_parameter, &ims_context) == ims_no_error &&
                ims_context != (ims_node_t)INVALID_POINTER && ims_context != NULL,
                "We can create a valid context.");

    ims_equipment = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT(actor, ims_get_equipment(ims_context, "equip_doesnt_exists", &ims_equipment) == ims_invalid_configuration,
                "We can't get an invalid equipment.");

    ims_equipment = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT(actor, ims_get_equipment(ims_context, "firstEquipment", &ims_equipment) == ims_no_error &&
                ims_equipment != (ims_node_t)INVALID_POINTER && ims_equipment != NULL,
                "We can get the first equipment.");

    ims_application = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT(actor, ims_get_application(ims_equipment, "app_doesnt_exists", &ims_application) == ims_invalid_configuration,
                "We can't get an invalid application.");

    ims_application = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT(actor, ims_get_application(ims_equipment, "firstApplication", &ims_application) == ims_no_error &&
                ims_application != (ims_node_t)INVALID_POINTER && ims_application != NULL,
                "We can get the first application.");

    ims_free_context(ims_context);

    return ims_test_end(actor);
}
