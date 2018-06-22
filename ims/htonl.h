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
// Define htonl
//
#ifndef _HTONL_H_
#define _HTONL_H_

#ifdef _WIN32
# ifndef _WIN32_WINNT
#  define _WIN32_WINNT 0x0400
# endif
# include "winsock2.h"
#else
# include <arpa/inet.h>
#endif

inline float htonf(float value) 
{
    union {
        float    vf;
        uint32_t vi;
    } u = { value };

    u.vi = htonl(u.vi);
    return u.vf;
}

#endif
