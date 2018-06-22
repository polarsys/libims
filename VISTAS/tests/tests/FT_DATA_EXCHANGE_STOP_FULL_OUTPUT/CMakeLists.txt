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
# FT_DATA_EXCHANGE_STOP_FULL_OUTPUT                                           #
###############################################################################

GET_FILENAME_COMPONENT(CURRENT_DIR_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)

_RESET()

PROJECT(${CURRENT_DIR_NAME})
MESSAGE("## Project [${CMAKE_PROJECT_NAME}]:[${PROJECT_NAME}]")

#####################################################################
# Includes                                                          #
#####################################################################

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../../api/CMakeLists.txt)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

#####################################################################
# Targets                                                           #
#####################################################################

IF(ENABLE_INSTRUMENTATION)
    ADD_DEFINITIONS(-DENABLE_INSTRUMENTATION)
ENDIF()

SET(ACTOR ${CURRENT_DIR_NAME}_actor1)
ADD_EXECUTABLE(${ACTOR} ${SOURCES} actor1.c)
TARGET_LINK_LIBRARIES(${ACTOR} $<TARGET_FILE:VISTAS_shared>)

SET(ACTOR ${CURRENT_DIR_NAME}_actor2)
ADD_EXECUTABLE(${ACTOR} ${SOURCES} actor2.c)
TARGET_LINK_LIBRARIES(${ACTOR} $<TARGET_FILE:VISTAS_shared>)

#####################################################################
# Test                                                              #
#####################################################################

ADD_TEST(NAME ${PROJECT_NAME} COMMAND ${CMAKE_COMMAND}
    -DCMD1=$<TARGET_FILE:${CURRENT_DIR_NAME}_actor1>
    -DCMD2=$<TARGET_FILE:${CURRENT_DIR_NAME}_actor2>
    -DWD=${CMAKE_CURRENT_LIST_DIR}
    -DREQ_TEST_FILEPATH=${REQ_TEST_FILEPATH}
    -DREQ_TEST_FILENAME=${REQ_TEST_FILENAME}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/../runtest.cmake
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
STRING(REPLACE ";" "\\;" TPATH "$ENV{PATH}")
SET_PROPERTY(TEST ${PROJECT_NAME} PROPERTY ENVIRONMENT "PATH=${TPATH}\;${VISTAS_BINARY_DIR}")
