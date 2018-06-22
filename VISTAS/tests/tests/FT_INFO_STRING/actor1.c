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
// String infos tests
//
#include "ims_test.h"

#define ACTOR_ID 1
ims_test_actor_t actor;

//
// Main
//
int main()
{
    actor = ims_test_init(ACTOR_ID);


    //@R ims_implementation_name should return 'IMS Vistas' for the Vistas implementation
    TEST_ASSERT(actor, strcmp(ims_implementation_name(), "Vistas") == 0,
                "Using IMS VISTAS.");


    TEST_ASSERT(actor, ims_implementation_version() != NULL, "implementation_version return a string.");
    TEST_LOG(actor, "IMS VERSION %s.", ims_implementation_version());

    //@R ims_direction_string should return :
    //  - 'INPUT'  for ims_input
    //  - 'OUTPUT' for ims_output
    TEST_ASSERT(actor,
                strcmp(ims_direction_string(ims_input), "INPUT") == 0 &&
                strcmp(ims_direction_string(ims_output), "OUTPUT") == 0,
                "ims_direction_string return the expected values.");


    //@R ims_mode_string should return :
    //  - 'sampling'  for ims_sampling
    //  - 'queuing' for ims_queuing
    TEST_ASSERT(actor,
                strcmp(ims_mode_string(ims_sampling), "sampling") == 0 &&
                strcmp(ims_mode_string(ims_queuing), "queuing") == 0,
                "ims_mode_string return the expected values.");

    //@R ims_protocol_string should return :
    // - 'AFDX' for ims_afdx
    // - 'A429' for ims_a429
    // - 'CAN' for ims_can
    // - 'analogue' for ims_analogue
    // - 'discrete' for ims_discrete
    // - 'NAD' for ims_nad
    TEST_ASSERT(actor,
                strcmp(ims_protocol_string(ims_afdx), "AFDX") == 0 &&
                strcmp(ims_protocol_string(ims_a429), "A429") == 0 &&
                strcmp(ims_protocol_string(ims_can), "CAN") == 0 &&
                strcmp(ims_protocol_string(ims_analogue), "analogue") == 0 &&
                strcmp(ims_protocol_string(ims_discrete), "discrete") == 0 &&
                strcmp(ims_protocol_string(ims_nad), "NAD") == 0,
                "ims_protocol_string return the expected values.");

    return ims_test_end(actor);
}
