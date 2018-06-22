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
# LIBIMS                                                                      #
###############################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

ENABLE_TESTING()

PROJECT(LIBIMS)
MESSAGE("##########")
MESSAGE("# System Name[${CMAKE_SYSTEM_NAME}]")
MESSAGE("# Processor[${CMAKE_SYSTEM_PROCESSOR}]")
MESSAGE("# Project : ${PROJECT_NAME}")
MESSAGE("# Directory : ${CMAKE_SOURCE_DIR}")
MESSAGE("##########")

#####################################################################
# Main options                                                      #
#####################################################################

SET(WORDSIZE "x86_64" CACHE STRING "Compilation word size (i686 or x86_64)")
MARK_AS_ADVANCED(CLEAR WORDSIZE)

SET(ENABLE_INSTRUMENTATION true CACHE BOOL "Enable instrumentation API")
MARK_AS_ADVANCED(CLEAR ENABLE_INSTRUMENTATION)

SET(ENABLE_COVERAGE true CACHE BOOL "Enable code coverage (lcov/gcov)")
MARK_AS_ADVANCED(CLEAR ENABLE_COVERAGE)

SET(ENABLE_DOC false CACHE BOOL "Enable documentation")
MARK_AS_ADVANCED(CLEAR ENABLE_DOC)

SET(ENABLE_TEST false CACHE BOOL "Enable test")
MARK_AS_ADVANCED(CLEAR ENABLE_TEST)

SET(VERSION "VX.Y.Z" CACHE STRING "Version (VX.Y.Z)")
MARK_AS_ADVANCED(CLEAR VERSION)

MESSAGE("# VERSION: ${VERSION}")

#####################################################################
# Macro                                                             #
#####################################################################

# Check if C source compiles
INCLUDE(CheckCSourceCompiles)
MACRO (LIBIMS_CHECK_C_COMPILER_FLAG _FLAG _RESULT)
   SET(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
   SET(CMAKE_REQUIRED_FLAGS "${_FLAG}")
   CHECK_C_SOURCE_COMPILES("int main() { return 0;}" ${_RESULT})
   SET (CMAKE_REQUIRED_FLAGS "${SAFE_CMAKE_REQUIRED_FLAGS}")
ENDMACRO ()

# Check if C++ source compiles
INCLUDE(CheckCXXSourceCompiles)
MACRO (LIBIMS_CHECK_CXX_COMPILER_FLAG _FLAG _RESULT)
   SET(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
   SET(CMAKE_REQUIRED_FLAGS "${_FLAG}")
   CHECK_C_SOURCE_COMPILES("int main() { return 0;}" ${_RESULT})
   SET (CMAKE_REQUIRED_FLAGS "${SAFE_CMAKE_REQUIRED_FLAGS}")
ENDMACRO ()

# Import custom CMake modules
MACRO(_FIND_PACKAGE _PACKAGE_NAME)
    MESSAGE("###")
    FIND_PACKAGE(${_PACKAGE_NAME} REQUIRED)
    STRING(TOUPPER ${_PACKAGE_NAME} _PACKAGE_NAME_)
    MESSAGE("# Find package ${_PACKAGE_NAME} [${_PACKAGE_NAME_}]")
    IF(NOT ${${_PACKAGE_NAME_}_FOUND})
        MESSAGE(FATAL_ERROR "Cannot find package [${_PACKAGE_NAME}].")
    ENDIF()
    MESSAGE("## ${_PACKAGE_NAME} VERSION:${${_PACKAGE_NAME_}_VERSION} ${${_PACKAGE_NAME_}_VERSION_STRING}")
    MESSAGE("## ${_PACKAGE_NAME} LIBRARIES:\n${${_PACKAGE_NAME_}_LIBRARIES}")
    LIST(APPEND LIBRARIES ${${_PACKAGE_NAME_}_LIBRARIES})
    MESSAGE("## ${_PACKAGE_NAME} INCLUDE DIRECTORIES:\n${${_PACKAGE_NAME_}_INCLUDE_DIRS} ${${_PACKAGE_NAME_}_INCLUDE_DIR}")
    LIST(APPEND INCLUDE_DIRS ${${_PACKAGE_NAME_}_INCLUDE_DIRS})
    LIST(APPEND INCLUDE_DIRS ${${_PACKAGE_NAME_}_INCLUDE_DIR})
    MESSAGE("## ${_PACKAGE_NAME} DEFINITIONS:\n${${_PACKAGE_NAME_}_DEFINITIONS}")
    LIST(APPEND DEFINITIONS ${${_PACKAGE_NAME_}_DEFINITIONS})
    MESSAGE("###")
ENDMACRO(_FIND_PACKAGE)

# Macro used to reset compilation main variables
MACRO(_RESET)
    UNSET(SOURCES)
    UNSET(LIBRARIES)
    UNSET(INCLUDE_DIRS)
    UNSET(DEFINITIONS)
ENDMACRO(_RESET)

# Generate XSD header file containing the XSD as a string
MACRO(_GENERATE_XSD_HEADER _XSD_PATH _XSD_FILENAME _HEADER_FILEPATH _VAR_NAME)
    FILE(READ ${_XSD_PATH}/${_XSD_FILENAME} XSD_CONTENT)
    STRING(REPLACE "\\" "\\\\" XSD_CONTENT ${XSD_CONTENT})
    STRING(REPLACE "\"" "\\\"" XSD_CONTENT ${XSD_CONTENT})
    STRING(REPLACE "\n" "\\n\"\n\"" XSD_CONTENT ${XSD_CONTENT})
    STRING(TIMESTAMP GENERATION_TIME)
    FILE(WRITE ${_HEADER_FILEPATH} "// File generated from <${_XSD_FILENAME}> at ${GENERATION_TIME}
static const char* ${_VAR_NAME} =
\"${XSD_CONTENT}\\n\";
")
ENDMACRO(_GENERATE_XSD_HEADER _XSD_PATH _XSD_FILENAME _HEADER_FILEPATH _VAR_NAME)


#####################################################################
# Modules                                                           #
#####################################################################

IF(DEFINED ENV{CMAKE_MODULE_PATH})
    STRING(REPLACE "\\" "/" CMAKE_CUSTOM_MODULE_PATH "$ENV{CMAKE_MODULE_PATH}")
    SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_CUSTOM_MODULE_PATH};${CMAKE_MODULE_PATH}")
ENDIF()

#####################################################################
# Includes                                                          #
#####################################################################

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/ims)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)

#####################################################################
# Subdirectories                                                    #
#####################################################################

ADD_SUBDIRECTORY(VISTAS)
ADD_SUBDIRECTORY(doc)

#####################################################################
# Licensing                                                         #
#####################################################################

INSTALL(FILES ${CMAKE_SOURCE_DIR}/contributing.md DESTINATION .)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/copyright.md DESTINATION .)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/license.md DESTINATION .)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/notice.md DESTINATION .)

#####################################################################
# CPack                                                             #
#####################################################################

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "LIBIMS")
SET(CPACK_PACKAGE_VENDOR "Airbus Operations S.A.S")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "2")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
INCLUDE(CPack)

#####################################################################
# CTest                                                             #
#####################################################################

INCLUDE(CTest)

MESSAGE("##########")
MESSAGE("# DONE")
MESSAGE("##########")

