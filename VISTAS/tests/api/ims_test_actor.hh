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

/*
 * IMS test actor
 */
#ifndef _IMS_TEST_ACTOR_H_
#define _IMS_TEST_ACTOR_H_
#include "ims_test.h"
#include "ims_test_signal.hh"

// C interface
struct ims_test_actor_internal {};

namespace ims_test
{
struct actor : public ims_test_actor_internal
{
    uint32_t             id;
    bool                 success;
    signal_wait          sig_wait;
    signal_send          sig_send;
    
    actor(uint32_t actor_id) :
        id(actor_id),
        success(true),
        sig_wait(this),
        sig_send(this)
    {}
};
}

#endif
