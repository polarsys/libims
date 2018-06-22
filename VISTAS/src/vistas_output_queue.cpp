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
 */
#include "vistas_output_queue.hh"
#include "vistas_port.hh"


namespace vistas
{

// Add a port to the queue
// (We use a weak ptr to prevent shared ptr cycles)
void output_queue::push(port_weak_ptr port)
{
    
    if (port->_next_queued_port || _last_port == port) {
        // Port already in the queue
        return;
    }

    if (_first_port != NULL) {
        if (_last_port == NULL) {
            THROW_IMS_ERROR(ims_implementation_specific_error,
                            "Cannot prepare this port ! (internal error).");
        }
        _last_port->_next_queued_port = port;
    } else {
        _first_port = port;
    }

    port->_next_queued_port = NULL;
    _last_port = port;
}

//
// Call send() on all queued ports.
// Remove all ports from the queue.
//
ims_return_code_t output_queue::send_all()
{
    port_weak_ptr current_port;
    while (_first_port) {
        current_port = _first_port;
        _first_port = _first_port->_next_queued_port;

        current_port->send();
        current_port->_next_queued_port = NULL;
    }
    _last_port = NULL;

    return ims_no_error;
}

}
