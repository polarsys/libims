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

IF(${ENABLE_DOC})

    ###############################################################################
    # ASCIDOCTOR                                                                  #
    ###############################################################################

	FIND_PROGRAM(RUBY_EXECUTABLE ruby)
	FIND_FILE(ASCIIDOCTOR_EXECUTABLE asciidoctor)

	IF(EXISTS ${ASCIIDOCTOR_EXECUTABLE})
		MESSAGE("## Ascidoctor generation enabled.")
		SET(ASCIIDOCTOR_PROJECT_NAME ${PROJECT_NAME})
		SET(ASCIIDOCTOR_PROJECT_VERSION ${PROJECT_VERSION})
		STRING(TIMESTAMP ASCIIDOCTOR_PROJECT_DATE)
		FILE(COPY ${LIBIMS_SOURCE_DIR}/doc/images DESTINATION ${LIBIMS_BINARY_DIR}/../../doc/asciidoc)
		CONFIGURE_FILE(${LIBIMS_SOURCE_DIR}/doc/userguide.asciidoc.in ${LIBIMS_BINARY_DIR}/../../doc/asciidoc/userguide.asciidoc @ONLY)
		CONFIGURE_FILE(${LIBIMS_SOURCE_DIR}/doc/index.asciidoc.in ${LIBIMS_BINARY_DIR}/../../doc/index.asciidoc @ONLY)
        
        SET(ASCIIDOCTOR_ARGS -b html5 -a icons=font@ -a source-highlighter=highlight.js -r asciidoctor-diagram --trace)

		SET(ASCIIDOCTOR_CMD_1 ${RUBY_EXECUTABLE} ${ASCIIDOCTOR_EXECUTABLE} ./asciidoc/userguide.asciidoc ${ASCIIDOCTOR_ARGS} -D ./asciidoc)
		SET(ASCIIDOCTOR_CMD_2 ${RUBY_EXECUTABLE} ${ASCIIDOCTOR_EXECUTABLE} index.asciidoc ${ASCIIDOCTOR_ARGS})
		ADD_CUSTOM_TARGET(asciidoctor COMMAND ${ASCIIDOCTOR_CMD_1} COMMAND ${ASCIIDOCTOR_CMD_2} WORKING_DIRECTORY ${LIBIMS_BINARY_DIR}/../../doc)
		INSTALL(DIRECTORY ${LIBIMS_BINARY_DIR}/../../doc/asciidoc DESTINATION ../../doc)
		INSTALL(FILES ${LIBIMS_BINARY_DIR}/../../doc/index.html DESTINATION ../../doc)
	ELSE()
		MESSAGE("## Ascidoctor generation NOT enable.")
	ENDIF()

    
	###############################################################################
    # DOXYGEN                                                                     #
    ###############################################################################

	FIND_PACKAGE(Doxygen)

	IF(DOXYGEN_FOUND)
		MESSAGE("## Doxygen generation enabled.")
		SET(DOX_PROJECT_NAME ${PROJECT_NAME})
		SET(DOX_PROJECT_VERSION ${VERSION})
		SET(DOX_PROJECT_BRIEF "\"Integrated Modular Simulation Library\"")
		SET(DOX_OUTPUT_DIRECTORY ${LIBIMS_BINARY_DIR}/../../doc/doxygen)
		SET(DOX_DOC_LANGUAGE English)
		SET(DOX_STRIP_FROM_PATH ${LIBIMS_SOURCE_DIR}/doc/)
		SET(DOX_INPUT "${LIBIMS_SOURCE_DIR}/include/ ${LIBIMS_BINARY_DIR}/doc/ ${LIBIMS_SOURCE_DIR}/ims ${LIBIMS_SOURCE_DIR}/VISTAS/src")
		IF(DOXYGEN_DOT_FOUND)
			MESSAGE("### Dot generation found.")
			SET(DOX_EXTRACT_ALL YES)
			SET(DOX_HAVE_DOT YES)
			SET(DOX_UML_LOOK YES)
		ELSE()
			MESSAGE("### Dot generation NOT found.")
			SET(DOX_EXTRACT_ALL NO)
			SET(DOX_HAVE_DOT NO)
			SET(DOX_UML_LOOK NO)
		ENDIF()
		CONFIGURE_FILE(${LIBIMS_SOURCE_DIR}/doc/doxyfile.in ${LIBIMS_BINARY_DIR}/doc/doxyfile @ONLY)
		SET(DOX_LIBXML2_VERSION ${LIBXML2_VERSION})
		SET(DOX_DOXYGEN_VERSION ${DOXYGEN_VERSION})
		CONFIGURE_FILE(${LIBIMS_SOURCE_DIR}/doc/mainpage.dox.in ${LIBIMS_BINARY_DIR}/doc/mainpage.dox @ONLY)
		SET(DOX_CMD doxygen ${LIBIMS_BINARY_DIR}/doc/doxyfile)
		ADD_CUSTOM_TARGET (doc ${DOX_CMD})
		INSTALL(DIRECTORY ${LIBIMS_BINARY_DIR}/../../doc/doxygen DESTINATION ../../doc)
	ELSE()
		MESSAGE("## Doxygen generation enabled.")
	ENDIF()
ENDIF()

