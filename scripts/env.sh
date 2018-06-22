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
 
#!/bin/bash

# File: env.sh
# Brief: Define global environment variable of the project

echo "## ENVIRONMENT - BEGIN"
export ENVIRONMENT_SETUP=1

PROGRAM=${0}
function usage {
	echo "usage: ${0} [-h] wordsize build_type"
	echo " -h         : Help"
	echo " wordsize   : Computer wordsize [x86_64|x86_64]"
	echo " build_type : Build type [debug|release]"
	if [[ -z ${RES} ]]; then exit ${RES}; else exit 1; fi
}

while getopts ":h" opt
do
	case ${opt} in
		h)
			echo "# Help"
			usage
			exit 0
			;;
		*)
			echo "## Unknown option [${opt}]"
			;;
	esac
done

shift $((OPTIND-1))

# ROOT
export _ROOT=$(dirname $(readlink -e $(dirname ${0})))
echo "# ROOT [${_ROOT}]"

# ARGUMENTS
export WORDSIZE=${1}
export BUILD_TYPE=${2}

# BUILD TYPE
if [[ "${BUILD_TYPE}" == "release" ]]; then export BUILD_TYPE=release; else export BUILD_TYPE=debug; fi
echo "# BUILD TYPE [${BUILD_TYPE}]"

# OS
if [[ -z ${OS} ]]; then export OS=Linux; fi

# WORDSIZE
if [[ "${WORDSIZE}" == "i686" ]]; then export WORDSIZE=i686; else export WORDSIZE=x86_64; fi
echo "# WORDSIZE [${WORDSIZE}]"

# CUSTOM ENVIRONMENT
export CUSTOM_ENV=${_ROOT}/scripts/myenv.sh
if [[ -e ${CUSTOM_ENV} ]]; then sh myenv.sh; fi

# IOLAND
if [[ -z ${IOLAND_ROOT} ]]; then export IOLAND_ROOT=/home/ioland/REF; fi

# CROSSTOOLS
if [[ -z ${CROSSTOOLS_ROOT} ]]; then export CROSSTOOLS_ROOT=/home/CrossTools; fi

# CMAKE
if [[ -z ${CMAKE_BIN} ]]; then export CMAKE_BIN=${IOLAND_ROOT}/COTS/CMAKE/V3.9.6/BINARIES/Linux/bin; fi
echo "# CMAKE BIN [${CMAKE_BIN}]"
export PATH=${CMAKE_BIN}:${PATH}
export LD_LIBRARY_PATH=${CMAKE_BIN}:${LD_LIBRARY_PATH}

# NINJA
if [[ -z ${NINJA_BIN} ]]; then export NINJA_BIN=${IOLAND_ROOT}/COTS/NINJA/V1.8.2/BINARIES/Linux; fi
echo "# NINJA BIN [${NINJA_BIN}]"
export PATH=${NINJA_BIN}:${PATH}
export LD_LIBRARY_PATH=${NINJA_BIN}:${LD_LIBRARY_PATH}

# DOXYGEN
if [[ -z ${DOXYGEN_BIN} ]]; then export DOXYGEN_BIN=${IOLAND_ROOT}/COTS/DOXYGEN/V1.8.11/BINARIES/Linux/bin; fi
echo "# DOXYGEN BIN [${DOXYGEN_BIN}]"
export PATH=${DOXYGEN_BIN}:${PATH}
export LD_LIBRARY_PATH=${DOXYGEN_BIN}:${LD_LIBRARY_PATH}

# GRAPHVIZ
if [[ -z ${GRAPHVIZ_BIN} ]]; then export GRAPHVIZ_BIN=${IOLAND_ROOT}/COTS/GRAPHVIZ/graphviz-2.26.3-1/BINARIES/Linux/bin; fi
echo "# GRAPHVIZ BIN [${GRAPHVIZ_BIN}]"
export PATH=${GRAPHVIZ_BIN}:${PATH}
export LD_LIBRARY_PATH=${GRAPHVIZ_BIN}:${LD_LIBRARY_PATH}

# ASCIIDOCTOR
if [[ -z ${ASCIIDOCTOR_BIN} ]]; then export ASCIIDOCTOR_BIN=${IOLAND_ROOT}/COTS/ASCIIDOCTORSTACK/V2.0.0/install/linux/bin; fi
echo "# ASCIIDOCTOR BIN [${ASCIIDOCTOR_BIN}]"
export PATH=${ASCIIDOCTOR_BIN}:${PATH}
export LD_LIBRARY_PATH=${ASCIIDOCTOR_BIN}:${LD_LIBRARY_PATH}

# LCOV
if [[ -z ${LCOV_BIN} ]]; then export LCOV_BIN=${IOLAND_ROOT}/../REF/COTS/LCOV/V1.13.0/BINARIES/Linux64/bin; fi
echo "# LCOV BIN [${LCOV_BIN}]"
export PATH=${LCOV_BIN}:${PATH}
export LD_LIBRARY_PATH=${LCOV_BIN}:${LD_LIBRARY_PATH}

# LIBXML2
if [[ "${WORDSIZE}" == "i686" ]];
then
	if [[ -z ${LIBXML2_ROOT} ]]; then export LIBXML2_ROOT=${IOLAND_ROOT}/COTS/LIBXML2/V2.9.3-1/BINARIES/Linux32; fi
else
	if [[ -z ${LIBXML2_ROOT} ]]; then export LIBXML2_ROOT=${IOLAND_ROOT}/COTS/LIBXML2/V2.9.3-1/BINARIES/Linux64; fi
fi
if [[ -z ${LIBXML2_BIN} ]]; then export LIBXML2_BIN=${LIBXML2_ROOT}/bin; fi
if [[ -z ${LIBXML2_INC} ]]; then export LIBXML2_INC=${LIBXML2_ROOT}/include/libxml2; fi
if [[ -z ${LIBXML2_LIB} ]]; then export LIBXML2_LIB=${LIBXML2_ROOT}/lib; fi
echo "# LIBXML2 BIN [${LIBXML2_BIN}]"
echo "# LIBXML2 INC [${LIBXML2_INC}]"
echo "# LIBXML2 LIB [${LIBXML2_LIB}]"
export PATH=${LIBXML2_BIN}:${LIBXML2_INC}:${LIBXML2_LIB}:${PATH}
export LD_LIBRARY_PATH=${LIBXML2_BIN}:${LIBXML2_INC}:${LIBXML2_LIB}:${LD_LIBRARY_PATH}

# BUILD DIRECTORY
if [[ -z ${BUILD_PATH} ]]; then export BUILD_PATH=${_ROOT}/build/${OS}/${WORDSIZE}; fi
echo "# BUILD PATH [${BUILD_PATH}]"

# INSTALL DIRECTORY
if [[ -z ${INSTALL_PATH} ]]; then export INSTALL_PATH=${_ROOT}/install/${OS}/${WORDSIZE}; fi
echo "# INSTALL PATH [${INSTALL_PATH}]"
export CURRENT_PATH=$(cd)

export CTEST_OUTPUT_ON_FAILURE=1

echo "## ENVIRONMENT - DONE"

