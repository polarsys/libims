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
 * Manage the queue of ports to send.
 * This queue is intrusive: it need a variable in ports class.
 * It is intrusive for better efficency and to prevent malloc on each push.
 */
#ifndef _VISTAS_OUTPUT_QUEUE_HH_
#define _VISTAS_OUTPUT_QUEUE_HH_
#include "ims.h"
#include "shared_ptr.hh"

namespace vistas
{
// Forward port declaration
class port;
typedef port* port_weak_ptr;

// Pointer to queue
class output_queue;
typedef shared_ptr<output_queue> output_queue_ptr;

class output_queue
{
public:

    // Add a port to the queue
    // (We use a weak ptr to prevent shared ptr cycles)
    void push(port_weak_ptr port);

    // Call send() on all queued ports.
    // Remove all ports from the queue.
    ims_return_code_t send_all();

private:
    port_weak_ptr _first_port;
    port_weak_ptr _last_port;
};
}

#endif
