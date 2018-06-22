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
 * Base vistas message class
 */
#include "vistas_message.hh"

namespace vistas
{

void message::dump(std::string prefix)
{
    LOG_SAY(prefix << "Message " << ims_protocol_string(get_protocol()) << " " <<
            ims_direction_string(get_direction()) << " " <<
            ims_mode_string(get_mode()) << " '" << get_name() << "' on socket " <<
            _port->get_socket()->to_string());
}

}
