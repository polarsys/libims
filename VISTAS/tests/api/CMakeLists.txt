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
 
###############################################################################
# Test api                                                                    #
###############################################################################

#####################################################################
# Includes                                                          #
#####################################################################

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR})

#####################################################################
# Libraries                                                         #
#####################################################################

LINK_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/VISTAS)
LINK_LIBRARIES(VISTAS_shared)

#####################################################################
# Sources                                                           #
#####################################################################

FILE(GLOB SRCS LIST_DIRECTORIES false RELATIVE ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
LIST(APPEND SOURCES ${SRCS})
