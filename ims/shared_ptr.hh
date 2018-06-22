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
//  Try to find the shared_ptr implementation
//
#ifndef __SHARED_PTR_HH__
#define __SHARED_PTR_HH__

// Before GCC 4.1, no shared_ptr in tr1
// We hope boost library is installed...
#if __GNUC__ < 4 || (__GNUC__ == 4 &&  __GNUC_MINOR__ < 1)

#  include <boost/shared_ptr.hpp>
#  include <boost/weak_ptr.hpp>
#  define SHARED_PTR_NAMESPACE boost

#else

//shared_ptr is provided with tr1
#  if __GNUC__ >= 4
#    include <tr1/memory>
#    define SHARED_PTR_NAMESPACE std::tr1
#  endif

#endif

//Shortcut
using SHARED_PTR_NAMESPACE::shared_ptr;
using SHARED_PTR_NAMESPACE::weak_ptr;
using SHARED_PTR_NAMESPACE::dynamic_pointer_cast;
using SHARED_PTR_NAMESPACE::static_pointer_cast;

#endif
