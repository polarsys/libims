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
 
#####################################################################
# Includes                                                          #
#####################################################################

INCLUDE(${CMAKE_SOURCE_DIR}/ims/python/CMakeLists.txt)

#####################################################################
# Dependencies                                                      #
#####################################################################

_FIND_PACKAGE(LibXml2)

IF(CMAKE_SYSTEM_NAME MATCHES "Windows")
    SET(LIBXML2_INSTALL true CACHE BOOL "Enable LibXml2 dynamic library auto copy to binary directory")
    MARK_AS_ADVANCED(CLEAR LIBXML2_INSTALL)
    IF(LIBXML2_INSTALL)
        GET_FILENAME_COMPONENT(LIBXML2_LIBRARIES_DIR ${LIBXML2_LIBRARIES} DIRECTORY)
        FILE(GLOB LIBXML2_LIBRARIES_FILES ${LIBXML2_LIBRARIES_DIR}/../bin/*.dll)
        MESSAGE("# LIBXML2 INSTALL: ${LIBXML2_LIBRARIES_FILES}")
        INSTALL(FILES ${LIBXML2_LIBRARIES_FILES} DESTINATION bin)
    ENDIF()
ENDIF()

#####################################################################
# Sources                                                           #
#####################################################################

IF(ENABLE_INSTRUMENTATION)
    ADD_DEFINITIONS(-DENABLE_INSTRUMENTATION)
ENDIF()

_GENERATE_XSD_HEADER(${CMAKE_SOURCE_DIR}/doc ims_config.xsd ${CMAKE_CURRENT_LIST_DIR}/ims_config_xsd.h ims_config_xsd)
_GENERATE_XSD_HEADER(${CMAKE_SOURCE_DIR}/doc ims_init.xsd ${CMAKE_CURRENT_LIST_DIR}/ims_init_xsd.h ims_init_xsd)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/doc/ims_config.xsd DESTINATION ../../doc)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/doc/ims_init.xsd DESTINATION ../../doc)

FILE(GLOB SRCS LIST_DIRECTORIES false RELATIVE ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
LIST(APPEND SOURCES ${SRCS})

#####################################################################
# Installation                                                      #
#####################################################################

INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/ims.h DESTINATION include)
IF(ENABLE_INSTRUMENTATION)
    INSTALL(FILES ${CMAKE_SOURCE_DIR}/include/ims_instrumentation.h DESTINATION include)
ENDIF()

INSTALL(FILES ${CMAKE_SOURCE_DIR}/cmake/FindLIBIMS.cmake DESTINATION cmake)

