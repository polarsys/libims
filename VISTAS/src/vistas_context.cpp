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
// vistas root class
//
#include "vistas_context.hh"

#include <sys/time.h>

namespace vistas
{
// Send All prepared and periodic ports
ims_return_code_t context::send_all()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    _posix_timestamp = (tv.tv_sec * 1000000ULL) + tv.tv_usec;
    
    _output_queue->send_all();

    for (port_vector_t::iterator iperiodic = _periodic_output_ports.begin();
         iperiodic != _periodic_output_ports.end();
         iperiodic++)
    {
        (*iperiodic)->send();
    }

    return ims_no_error;
}

// Reset all messages
ims_return_code_t context::reset_all()
{
    ims_return_code_t result = ims_no_error;
    
    _time_us = 0;
    _time_us_before_notify = 0;

    for(port_list_t::iterator iport = _port_list.begin();
        iport != _port_list.end();
        iport++)
    {
        ims_return_code_t port_result = (*iport)->reset_messages();
        if (port_result > result) result = port_result;
    }

    return result;
}

// Invalidate all messages
ims_return_code_t context::invalidate_all()
{
    for(port_list_t::iterator iport = _port_list.begin();
        iport != _port_list.end();
        iport++)
    {
        try {
            (*iport)->invalidate_messages();
        } catch(...) {} //ignore error because queuings and wired will return errors
    }

    return ims_no_error;
}

ims_return_code_t context::progress(uint32_t duration_us)
{
    _time_us += duration_us;
    
    // check if a F_SYNCHRO has been received
    if (_time_us_before_notify > 0)
    {
        if ( _time_us_before_notify > duration_us )
        {
            // we have not finished the F_SYNCHRO yet
            _time_us_before_notify -= duration_us;
        }
        else
        {
            // we have finished the F_SYNCHRO, send the R_SYNCHRO
            _time_us_before_notify = 0;
            LOG_DEBUG("Sending R_SYNCHRO acknowledge");
            _port_instrumentation->acknowledge_synchro();
        }
    }
    
    return ims_no_error;
}

}
