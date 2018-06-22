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
# VISTAS                                                                      #
###############################################################################

GET_FILENAME_COMPONENT(CURRENT_DIR_NAME ${CMAKE_CURRENT_LIST_DIR} NAME)
MESSAGE("# Subirectory: ${CURRENT_DIR_NAME}")

PROJECT(${CURRENT_DIR_NAME} CXX)
MESSAGE("# Project [${CMAKE_PROJECT_NAME}]:[${PROJECT_NAME}]")

_GENERATE_XSD_HEADER(${CMAKE_CURRENT_LIST_DIR}/doc vistas_config.xsd ${CMAKE_CURRENT_LIST_DIR}/src/vistas_config_xsd.h vistas_config_xsd)

#####################################################################
# Compilation options                                               #
#####################################################################

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "## OS [LINUX]")
    SET(CMAKE_C_FLAGS "-g -O0 -Wall -Wextra -fvisibility=hidden -fPIC -D_GNU_SOURCE")
    SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -Wextra -fvisibility=hidden -fPIC -D_GNU_SOURCE")
    IF(NOT WORDSIZE MATCHES "x86_64")
        MESSAGE(STATUS "## WS [32]")
        SET(CMAKE_C_FLAGS      "${CMAKE_C_FLAGS} -m32 -march=i686")
        SET(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -m32")
        SET(CMAKE_CXX_FLAGS      "${CMAKE_C_FLAGS} -m32 -march=i686")
        SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -m32")
    ELSE()
        MESSAGE(STATUS "## WORDSIZE [x86_64]")
    ENDIF()
    LIBIMS_CHECK_C_COMPILER_FLAG("-Wl,--hash-style=sysv" HAVE_HASHSTYLE)
    IF(HAVE_HASHSTYLE)
        SET(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -Wl,--hash-style=both")
        SET(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -Wl,--hash-style=both")
        SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -Wl,--hash-style=both")
        SET(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -Wl,--hash-style=both")
    ENDIF()
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "## OS [WINDOWS]")
	ADD_DEFINITIONS(-DLIBIMS_EXPORTS)
    SET(CMAKE_C_FLAGS "-g -O0 -Wall -Wextra -fvisibility=hidden")
    SET(CMAKE_CXX_FLAGS "-g -O0 -Wall -Wextra -fvisibility=hidden")
    IF(NOT WORDSIZE MATCHES "x86_64")
        MESSAGE(STATUS "## WORDSIZE [i686]")
        SET(CMAKE_C_FLAGS      "${CMAKE_C_FLAGS} -m32 -march=i686")
        SET(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -m32")
        SET(CMAKE_CXX_FLAGS      "${CMAKE_C_FLAGS} -m32 -march=i686")
        SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} -m32")
    ELSE()
        MESSAGE(STATUS "## WORDSIZE [x86_64]")
    ENDIF()
ELSE()
    MESSAGE(FATAL_ERROR "## OS [UNKNOWN]")
ENDIF()

IF(${ENABLE_TEST})
	MESSAGE(STATUS "# APPEND COVERAGE FLAGS")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    SET(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} --coverage")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} --coverage")
	SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
ENDIF()

#####################################################################
# Includes                                                          #
#####################################################################

INCLUDE(${CMAKE_SOURCE_DIR}/VISTAS/python/CMakeLists.txt)
INCLUDE(${CMAKE_SOURCE_DIR}/ims/CMakeLists.txt)

#####################################################################
# Sources                                                           #
#####################################################################

FILE(GLOB SRCS LIST_DIRECTORIES false RELATIVE ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
LIST(APPEND SOURCES ${SRCS})

#####################################################################
# Targets                                                           #
#####################################################################

IF(ENABLE_INSTRUMENTATION)
    ADD_DEFINITIONS(-DENABLE_INSTRUMENTATION)
ENDIF()

ADD_LIBRARY(${PROJECT_NAME} OBJECT ${SOURCES})
SET_PROPERTY(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE 1)
ADD_LIBRARY(${PROJECT_NAME}_shared SHARED $<TARGET_OBJECTS:${PROJECT_NAME}>)
SET_TARGET_PROPERTIES(${PROJECT_NAME}_shared PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
ADD_LIBRARY(${PROJECT_NAME}_static STATIC $<TARGET_OBJECTS:${PROJECT_NAME}>)
SET_TARGET_PROPERTIES(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

TARGET_LINK_LIBRARIES(${PROJECT_NAME}_shared ${LIBRARIES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}_static ${LIBRARIES})

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${INCLUDE_DIRS})
ADD_DEFINITIONS(${DEFINITIONS})

IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(STATUS "## OS [LINUX]")
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
    MESSAGE(STATUS "## OS [WINDOWS]")
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}_shared wsock32 ws2_32)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME}_static wsock32 ws2_32)
ELSE()
    MESSAGE(FATAL_ERROR "## OS [UNKNOWN]")
ENDIF()

#####################################################################
# Coverage                                                          #
#####################################################################

IF(${ENABLE_TEST})
	IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
		FIND_PROGRAM(LCOV_PROGRAM "lcov")
		FIND_PROGRAM(GENHTML_PROGRAM "genhtml")
		IF(EXISTS ${LCOV_PROGRAM})
			MESSAGE(STATUS "Found LCOV: ${LCOV_PROGRAM}")
			ADD_CUSTOM_TARGET(precoverage
				COMMAND ${LCOV_PROGRAM} --zerocounters --directory ${CMAKE_CURRENT_BINARY_DIR}
				COMMAND ${LCOV_PROGRAM} --capture --directory ${CMAKE_CURRENT_BINARY_DIR} --output-file ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.init.tmp -initial
				COMMAND ${LCOV_PROGRAM} --remove ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.init.tmp '*tests/*' '/usr/*' -o ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.init
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				)
			ADD_CUSTOM_TARGET(postcoverage
				COMMAND ${LCOV_PROGRAM} --capture --directory ${CMAKE_CURRENT_BINARY_DIR} --output-file ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.info.tmp
				COMMAND ${LCOV_PROGRAM} --remove ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.info.tmp '*tests/*' '/usr/*' -o ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.info
				COMMAND ${LCOV_PROGRAM} --add-tracefile ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.init --add-tracefile ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.info -o ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.final
				COMMAND ${GENHTML_PROGRAM} --prefix ${CMAKE_SOURCE_DIR} --demangle-cpp --num-space 2 --sort --function-coverage --no-branch-coverage --legend --title ${CMAKE_PROJECT_NAME} -o ${CMAKE_BINARY_DIR}/../../doc/cov/${CMAKE_SYSTEM_NAME} ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.final
				WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				)
			INSTALL(DIRECTORY ${CMAKE_BINARY_DIR}/../../doc/cov DESTINATION ../../doc)
		ELSE()
			MESSAGE("LCOV not found")
		ENDIF()
	ENDIF()
ENDIF()

#####################################################################
# Install                                                           #
#####################################################################

INSTALL(TARGETS ${PROJECT_NAME}_shared
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)
INSTALL(TARGETS ${PROJECT_NAME}_static
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)

INSTALL(FILES ${CMAKE_CURRENT_LIST_DIR}/doc/vistas_config.xsd DESTINATION ../../doc)

#####################################################################
# Tests                                                             #
#####################################################################

IF(${ENABLE_TEST})

    SET(REQ_TEST_FILEPATH ${CMAKE_CURRENT_BINARY_DIR})
    SET(REQ_TEST_FILENAME TR_${CMAKE_PROJECT_NAME}_${PROJECT_NAME}_${CMAKE_SYSTEM_NAME}_${WORDSIZE}.xml)
    STRING(TIMESTAMP REQ_TEST_TIME)
    FILE(WRITE ${REQ_TEST_FILEPATH}/${REQ_TEST_FILENAME} "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>
<!-- Generated on ${REQ_TEST_TIME}) -->
<TestCases>
    <Title>Test Results</Title>
    <Project>${CMAKE_PROJECT_NAME}</Project>
    <Issue>${VERSION}/${CMAKE_SYSTEM_NAME}/${WORDSIZE}</Issue>
<!-- TestCase -->
</TestCases>
")

    FILE(GLOB TESTS LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_LIST_DIR}/tests/tests ${CMAKE_CURRENT_LIST_DIR}/tests/tests/*)

    FOREACH(TESTNAME IN ITEMS ${TESTS})
        IF(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/tests/tests/${TESTNAME})
            IF(EXISTS ${CMAKE_CURRENT_LIST_DIR}/tests/tests/${TESTNAME}/CMakeLists.txt)
                ADD_SUBDIRECTORY(tests/tests/${TESTNAME})
            ENDIF(EXISTS ${CMAKE_CURRENT_LIST_DIR}/tests/tests/${TESTNAME}/CMakeLists.txt)
        ENDIF(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/tests/tests/${TESTNAME})
    ENDFOREACH(TESTNAME)

    INSTALL(FILES ${REQ_TEST_FILEPATH}/${REQ_TEST_FILENAME} DESTINATION ../../doc/reqs)

ENDIF()

