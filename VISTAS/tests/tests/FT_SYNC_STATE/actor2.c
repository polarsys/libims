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
  
  ims_create_context_parameter_t init_params = IMS_CREATE_CONTEXT_INITIALIZER;
  init_params.period_us = 100000;
  init_params.step_by_step_supported = 1;
  
  
  TEST_ASSERT_SILENT(actor, ims_create_context(IMS_CONFIG_FILE, VISTAS_CONFIG_FILE, &init_params, &ims_context) == ims_no_error &&
                     ims_context != (ims_node_t)INVALID_POINTER && ims_context != NULL,
                     "We can create a valid context.");


  TEST_ASSERT(actor, ims_get_synchro_steps_request(ims_context) == 0, "0 steps asked.");
  TEST_WAIT(actor, 1);
  
  TEST_ASSERT(actor, ims_progress(ims_context, 100*1000) == ims_no_error, "Progress success.");
  TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
  TEST_ASSERT(actor, ims_get_synchro_steps_request(ims_context) == 2, "2 steps asked.");

  TEST_SIGNAL(actor, 1);
  TEST_WAIT(actor, 1);

  TEST_ASSERT(actor, ims_progress(ims_context, 100*1000) == ims_no_error, "Progress success.");
  TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
  TEST_ASSERT(actor, ims_get_synchro_steps_request(ims_context) == 0, "0 steps asked.");

  TEST_SIGNAL(actor, 1);
  TEST_WAIT(actor, 1);

  TEST_ASSERT(actor, ims_progress(ims_context, 100*1000) == ims_no_error, "Progress success.");
  TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
  TEST_ASSERT(actor, ims_get_synchro_steps_request(ims_context) == 0, "0 steps asked.");

  TEST_SIGNAL(actor, 1);
  TEST_WAIT(actor, 1);

  TEST_ASSERT(actor, ims_progress(ims_context, 100*1000) == ims_no_error, "Progress success.");
  TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
  TEST_ASSERT(actor, ims_get_synchro_steps_request(ims_context) == 2, "2 steps asked.");

  TEST_SIGNAL(actor, 1);
  TEST_WAIT(actor, 1);

  TEST_ASSERT(actor, ims_progress(ims_context, 100*1000) == ims_no_error, "Progress success.");
  TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
  TEST_ASSERT(actor, ims_get_synchro_steps_request(ims_context) == 0, "0 steps asked.");

  TEST_SIGNAL(actor, 1);
  TEST_WAIT(actor, 1);

  TEST_ASSERT(actor, ims_progress(ims_context, 100*1000) == ims_no_error, "Progress success.");
  TEST_ASSERT(actor, ims_import(ims_context, 1000*1000) == ims_no_error, "Import success.");
  TEST_ASSERT(actor, ims_get_synchro_steps_request(ims_context) == 0, "0 steps asked.");

  TEST_SIGNAL(actor, 1);
  TEST_WAIT(actor, 1);

  ims_free_context(ims_context);
  
  TEST_SIGNAL(actor, 1);

  return ims_test_end(actor);
}
