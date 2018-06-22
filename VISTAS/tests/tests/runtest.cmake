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
 ##############################################################################

###############################################################################
# Run test                                                                    #
###############################################################################

SET($ENV{IMS_LOG_LEVEL} 4)

MESSAGE("PATH=$ENV{PATH}")

# 3 parallel actors
IF(CMD1 AND (CMD2 AND CMD3))
    MESSAGE("## Run test: 3 Actors")
    EXECUTE_PROCESS(COMMAND ${CMD1} COMMAND ${CMD2} COMMAND ${CMD3} TIMEOUT 3600 RESULT_VARIABLE CMD_RESULT WORKING_DIRECTORY ${WD})
# 2 parallel actors
ELSEIF(CMD1 AND CMD2)
    MESSAGE("## Run test: 2 Actors")
    EXECUTE_PROCESS(COMMAND ${CMD1} COMMAND ${CMD2} TIMEOUT 3600 RESULT_VARIABLE CMD_RESULT WORKING_DIRECTORY ${WD})
# 1 single actor
ELSEIF(CMD1)
    MESSAGE("## Run test: 1 Actor")
    EXECUTE_PROCESS(COMMAND ${CMD1} TIMEOUT 3600 RESULT_VARIABLE CMD_RESULT WORKING_DIRECTORY ${WD})
ELSE(CMD1 AND (CMD2 AND CMD3))
    MESSAGE(FATAL_ERROR "ERROR: Missing commands !")
ENDIF(CMD1 AND (CMD2 AND CMD3))

#####################################################################
# Fill results                                                      #
#####################################################################

IF(CMD_RESULT)
    SET(REQ_TEST_RESULT "NOK")
ELSE()
    SET(REQ_TEST_RESULT "OK")
ENDIF()

IF(REQ_TEST_FILEPATH AND REQ_TEST_FILENAME)
    IF(EXISTS ${WD}/req_tr.xml AND EXISTS ${REQ_TEST_FILEPATH}/${REQ_TEST_FILENAME})
        STRING(TIMESTAMP REQ_TEST_TIME)
        GET_FILENAME_COMPONENT(REQ_TEST_NAME ${WD} NAME)
        GET_FILENAME_COMPONENT(CMD_BINARY_PATH ${CMD1} DIRECTORY)
        CONFIGURE_FILE(${WD}/req_tr.xml ${CMD_BINARY_PATH}/req_tr_res.xml)
        FILE(READ ${CMD_BINARY_PATH}/req_tr_res.xml REQ_TEST_RESULT)
        SET(REQ_TEST_RESULT "${REQ_TEST_RESULT}\
<!-- TestCase -->")
        FILE(READ ${REQ_TEST_FILEPATH}/${REQ_TEST_FILENAME} REQ_TEST_FILE)
        STRING(REPLACE "<!-- TestCase -->" ${REQ_TEST_RESULT} REQ_TEST_FILE ${REQ_TEST_FILE})
        FILE(WRITE ${REQ_TEST_FILEPATH}/${REQ_TEST_FILENAME} ${REQ_TEST_FILE})
    ENDIF()
ENDIF()

IF(CMD_RESULT)
    MESSAGE(FATAL_ERROR "Error: ${CMD_RESULT}")
ENDIF()
