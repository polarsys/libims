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
// AFDX test - actor 2
//
#include "ims_test.h"

#define IMS_CONFIG_FILE      "config/actor2/ims.xml"
#define VISTAS_CONFIG_FILE   "config/actor2/vistas.xml"

#define ACTOR_ID 2
ims_test_actor_t actor;

//
// Message data
//
#define INVALID_POINTER ((void*)42)

int main()
{
    ims_node_t ims_context;

    actor = ims_test_init(ACTOR_ID);

    ims_context = (ims_node_t)INVALID_POINTER;
    TEST_ASSERT_SILENT(actor, ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE, NULL, &ims_context) == ims_no_error &&
                       ims_context != (ims_node_t)INVALID_POINTER && ims_context != NULL,
                       "We can create a valid context.");

    TEST_ASSERT(actor, ims_get_running_state(ims_context) == ims_running_state_hold, "Actor is in HOLD.");
    TEST_ASSERT(actor, ims_get_time_ratio(ims_context) == 1.0, "Time ratio is 1.0");

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
    TEST_ASSERT(actor, ims_get_running_state(ims_context) == ims_running_state_run, "Actor is in RUN.");
    TEST_ASSERT(actor, ims_get_time_ratio(ims_context) == 1.5, "Time ratio is 1.5");

    TEST_SIGNAL(actor, 1);
    TEST_WAIT(actor, 1);

    TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
    TEST_ASSERT(actor, ims_get_running_state(ims_context) == ims_running_state_step, "Actor is in STEP.");
    TEST_ASSERT(actor, ims_get_time_ratio(ims_context) == 1.5, "Time ratio is 1.5");

    ims_free_context(ims_context);

    TEST_SIGNAL(actor, 1);

    return ims_test_end(actor);
}
