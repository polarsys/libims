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
 * Base class of all ports
 */
#include "vistas_port.hh"
#include "vistas_context.hh"

namespace vistas
{
//Ctor
port::port(context_weak_ptr context, socket_ptr socket) :
    _context(context),
    _socket(socket),
    _next_queued_port(NULL)
{}
}
