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
# FT_PORT_LIMIT                                                               #
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

SET(NUMBER_OF_PORTS 1000)

IF(ENABLE_INSTRUMENTATION)
    ADD_DEFINITIONS(-DENABLE_INSTRUMENTATION)
ENDIF()

ADD_DEFINITIONS(-DNUMBER_OF_PORTS=${NUMBER_OF_PORTS})

SET(ACTOR ${CURRENT_DIR_NAME}_actor1)
ADD_EXECUTABLE(${ACTOR} ${SOURCES} actor1.c)
TARGET_LINK_LIBRARIES(${ACTOR} $<TARGET_FILE:VISTAS_shared>)

SET(ACTOR ${CURRENT_DIR_NAME}_actor2)
ADD_EXECUTABLE(${ACTOR} ${SOURCES} actor2.c)
TARGET_LINK_LIBRARIES(${ACTOR} $<TARGET_FILE:VISTAS_shared>)

#####################################################################
# Test                                                              #
#####################################################################

# Generate IMS & VISTAS configuration files
MATH(EXPR TEST_PORT_NUMBER '${NUMBER_OF_PORTS}-1')

MACRO(_TEST_CONFIG_IMS _FILEPATH _DIRECTION)
    FILE(WRITE ${_FILEPATH} "<?xml version=\"1.0\"?>\n\
<VirtualComponent xmlns:ex=\"http://airbus.com/modelingandsimulation/vsim/extension\" Name=\"testVirtualComponent\">\n\
    <Equipment Name=\"firstEquipment\">\n\
        <Application Name=\"firstApplication\">\n\
            <DataExchange>\n\
                <AFDX>\n\
                    <${_DIRECTION}>\n")
    FOREACH(INDEX RANGE 0 ${TEST_PORT_NUMBER} 1)
        FILE(APPEND ${_FILEPATH} "                        <SamplingMessage Name=\"AFDX_${INDEX}\" LocalName=\"AFDX_${INDEX}\" MessageSizeBytes=\"10\" ValidityDurationUs=\"50000\"/>\n")
    ENDFOREACH(INDEX)
    FILE(APPEND ${_FILEPATH} "                    </${_DIRECTION}>\n\
                </AFDX>\n\
            </DataExchange>\n\
        </Application>\n\
    </Equipment>\n\
</VirtualComponent>")
ENDMACRO(_TEST_CONFIG_IMS)

MACRO(_TEST_CONFIG_VISTAS _FILEPATH _DIRECTION)
    FILE(WRITE ${_FILEPATH} "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n\
<Network xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"vistas_config.xsd\">\n\
    <VirtualComponent Name=\"testVirtualComponent\">\n")
    FOREACH(INDEX RANGE 0 ${TEST_PORT_NUMBER} 1)
        MATH(EXPR TEST_PORT "7000+${INDEX}")
        FILE(APPEND ${_FILEPATH} "        <A664_Channel Name=\"AFDX_${INDEX}\" Direction=\"${_DIRECTION}\" MessageMaxSize=\"10\" FifoSize=\"1\">\n\
            <Socket DstIP=\"226.23.12.4\" DstPort=\"${TEST_PORT}\" />\n\
            <Header Src_Id=\"No\" SN=\"No\" QoS_Timestamp=\"No\" Data_Timestamp=\"No\" />\n\
        </A664_Channel>\n")
    ENDFOREACH(INDEX)
    FILE(APPEND ${_FILEPATH} "    </VirtualComponent>\n\
</Network>")
ENDMACRO(_TEST_CONFIG_VISTAS)

#IF(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config/actor1/ims.xml")
	_TEST_CONFIG_IMS("${CMAKE_CURRENT_SOURCE_DIR}/config/actor1/ims.xml" "ProducedData")
	_TEST_CONFIG_VISTAS("${CMAKE_CURRENT_SOURCE_DIR}/config/actor1/vistas.xml" "Out")
	_TEST_CONFIG_IMS("${CMAKE_CURRENT_SOURCE_DIR}/config/actor2/ims.xml" "ConsumedData")
	_TEST_CONFIG_VISTAS("${CMAKE_CURRENT_SOURCE_DIR}/config/actor2/vistas.xml" "In")
#ENDIF()

ADD_TEST(NAME ${PROJECT_NAME} COMMAND ${CMAKE_COMMAND}
    -DCMD1=$<TARGET_FILE:${CURRENT_DIR_NAME}_actor1>
    -DCMD2=$<TARGET_FILE:${CURRENT_DIR_NAME}_actor2>
    -DWD=${CMAKE_CURRENT_LIST_DIR}
    -DREQ_TEST_FILEPATH=${REQ_TEST_FILEPATH}
    -DREQ_TEST_FILENAME=${REQ_TEST_FILENAME}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/../runtest.cmake
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
STRING(REPLACE ";" "\\;" TPATH "$ENV{PATH}")
SET_TESTS_PROPERTIES(${PROJECT_NAME} PROPERTIES TIMEOUT 30000)
SET_PROPERTY(TEST ${PROJECT_NAME} PROPERTY ENVIRONMENT "PATH=${TPATH}\;${VISTAS_BINARY_DIR}")
