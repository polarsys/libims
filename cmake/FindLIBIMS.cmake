###############################################################################
# Copyright (c) 2018 Airbus Operations S.A.S                                  #
#                                                                             #
# This program and the accompanying materials are made available under the    #
# terms of the Eclipse Public License v. 2.0 which is available at            #
# http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License   #
# v. 1.0 which is available at                                                #
# http://www.eclipse.org/org/documents/edl-v10.php.                           #
#                                                                             #
# SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause                            #
###############################################################################
 
# Try to find LIBIMS library

# Include directory
FIND_PATH(LIBIMS_INCLUDE_DIRS NAMES ims.h)

STRING(REGEX MATCH "^.*[^0-9]([0-9]+\\.[0-9]*\\.[0-9]*)/.*$" LIBIMS_VERSION ${LIBIMS_INCLUDE_DIRS})
SET(LIBIMS_VERSION ${CMAKE_MATCH_1})
IF(NOT DEFINED LIBIMS_VERSION)
	SET(LIBIMS_VERSION "dev")
ENDIF()

# Libraries directory
FIND_LIBRARY(LIBIMS_LIBRARIES NAMES libVISTAS VISTAS)

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
	LIST(APPEND LIBIMS_LIBRARIES wsock32 ws2_32)
ENDIF()

ADD_DEFINITIONS(-DUSE_LIBIMS)

find_package_handle_standard_args(LIBIMS DEFAULT_MSG LIBIMS_INCLUDE_DIRS LIBIMS_LIBRARIES LIBIMS_VERSION)

MARK_AS_ADVANCED(SET LIBIMS_INCLUDE_DIRS LIBIMS_LIBRARIES LIBIMS_VERSION)