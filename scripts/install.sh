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

# File: install.sh
# Brief: Install project using CMake

echo "## INSTALL"

PROGRAM=${0}
function usage {
    set usage="usage: ${0} [-h] [-c] [-d] [-t] [-p] [-s] [-i] wordsize build_type"
	echo ${usage}
	echo " -h         : Help"
	echo " -c         : Clean before"
	echo " -d         : Build documentation"
	echo " -t         : Build tests"
	echo " -p         : Package"
	echo " -s         : Enable step by step"
	echo " -i         : Enable instrumentation"
	echo " wordsize   : Computer wordsize [[x86_64]|i686]"
	echo " build_type : Build type [[debug]|release]"
	if [[ -z ${RES} ]]; then exit ${RES}; else exit 1; fi
}

ENABLE_DOC=0
ENABLE_TEST=0
ENABLE_PACKAGE=0
CLEAN=0
ENABLE_STEP=0
ENABLE_INSTRUMENTATION=0

while getopts ":hcdtpsie" opt
do
	case "${opt}" in
		h)
			echo "# Help"
			usage
			exit 0
			;;
		c)
			echo "# Clean before"
			CLEAN=1
			;;
		d)
			echo "# Build documentation"
			ENABLE_DOC=1
			;;
		t)
			echo "# Build tests"
			ENABLE_TEST=1
			;;
		p)
			echo "# Package"
			ENABLE_PACKAGE=1
			;;
		s)
			echo "# Enable step by step"
			ENABLE_STEP=1
			;;
		i)
			echo "# Enable instrumentation"
			ENABLE_INSTRUMENTATION=1
			;;
		*)
			echo "## Unknown option [${opt}]"
			;;
	esac
done

# Environment
if [[ -z "${ENVIRONMENT_SETUP}" ]]
then
	. $(dirname ${0})/env.sh ${@}
fi

# Preconfigure
DIRECTORIES=($(pwd)/../build 
$(pwd)/../build/doc 
$(pwd)/../build/Linux 
$(pwd)/../build/Linux/i686 
$(pwd)/../build/Linux/x86_64 
$(pwd)/../install 
$(pwd)/../install/doc 
$(pwd)/../install/Linux 
$(pwd)/../install/Linux/i686 
$(pwd)/../install/Linux/x86_64)
for d in ${DIRECTORIES[*]}
do
    DIRECTORY=${d}
    if [ ! -d ${DIRECTORY} ]
    then
        echo "# CREATE [${DIRECTORY}]"
        mkdir ${DIRECTORY}
    else
        echo "# EXISTS [${DIRECTORY}]"
    fi
done
ENV_CUSTOM=$(pwd)/myenv.sh
rm -rf ${ENV_CUSTOM}
if [[ ! -e ${ENV_CUSTOM} ]]
then
    echo "# CREATE [${ENV_CUSTOM}]"
    echo "#!/bin/bash" > ${ENV_CUSTOM}
    echo "" > ${ENV_CUSTOM}
    echo "# CUSTOM ENVIRONMENT" >> ${ENV_CUSTOM}
    echo "" >> ${ENV_CUSTOM}
    echo "echo \"# CUSTOM ENVIRONMENT - BEGIN\"" >> ${ENV_CUSTOM}
    echo "" >> ${ENV_CUSTOM}
    echo "echo \"# CUSTOM ENVIRONMENT - END\"" >> ${ENV_CUSTOM}
else
    echo "# EXISTS [${ENV_CUSTOM}]"
fi

# Clean
if [[ ${CLEAN} -eq 1 ]]
then
    echo "## Remove content from [${BUILD_PATH}]"
    rm -rf ${BUILD_PATH}/*
fi

# Build Path
cd ${BUILD_PATH}

# Configure
echo "## CMAKE CONFIGURE"
export arg="-DCMAKE_BUILD_TYPE=${BUILD_TYPE} 
            -DCMAKE_INSTALL_PREFIX=${INSTALL_PATH} 
            -DWORDSIZE=${WORDSIZE} 
            -DENABLE_DOC=${ENABLE_DOC} 
            -DENABLE_TEST=${ENABLE_TEST} 
            -DENABLE_INSTRUMENTATION=${ENABLE_INSTRUMENTATION}"
                      
if [[ "${ENABLE_STEP}" == "1" ]]; then read -p "Press any key to continue..."; fi
cmake ${_ROOT} -G"Ninja" ${arg} 

# Build
echo "## CMAKE BUILD"
if [[ "${ENABLE_STEP}" == "1" ]]; then read -p "Press any key to continue..."; fi
cmake --build ${BUILD_PATH} --target all --config ${BUILD_TYPE}

# Coverage
if [[ ${ENABLE_TEST} -eq 1 ]]
then
	if [[ $(cmake --build ${BUILD_PATH} --target help | grep coverage | wc -l) -eq 2 ]]
	then
		echo "## CMAKE COVERAGE"
		cmake --build ${BUILD_PATH} --target precoverage --config ${BUILD_TYPE}
	else
		echo "## CMAKE COVERAGE [FAILED] : Cannot find coverage targets. Missing lcov ?"
	fi
fi

# Test
if [[ ${ENABLE_TEST} -eq 1 ]]
then
	echo "## CMAKE TEST"
	if [[ "${ENABLE_STEP}" == "1" ]]; then read -p "Press any key to continue..."; fi
	cmake --build ${BUILD_PATH} --target test --config ${BUILD_TYPE}
	if [[ $? -eq 0 ]]
	then
		echo "## CMAKE TEST [FAILED]"
	fi
fi

# Coverage
if [[ ${ENABLE_TEST} -eq 1 ]]
then
	if [[ $(cmake --build ${BUILD_PATH} --target help | grep coverage | wc -l) -eq 2 ]]
	then
		echo "## CMAKE COVERAGE"
		cmake --build ${BUILD_PATH} --target postcoverage --config ${BUILD_TYPE}
	else
		echo "## CMAKE COVERAGE [FAILED] : Cannot find coverage targets. Missing lcov ?"
	fi
	
fi

# Doc
if [[ ${ENABLE_DOC} -eq 1 ]]
then
	echo "## CMAKE DOC"
	if [[ "${ENABLE_STEP}" == "1" ]]; then read -p "Press any key to continue..."; fi
	cmake --build ${BUILD_PATH} --target asciidoctor --config ${BUILD_TYPE}
	cmake --build ${BUILD_PATH} --target doc --config ${BUILD_TYPE}
fi

# Install
echo "## CMAKE INSTALL"
if [[ "${ENABLE_STEP}" == "1" ]]; then read -p "Press any key to continue..."; fi
cmake --build ${BUILD_PATH} --target install --config ${BUILD_TYPE}

# Package
if [[ ${ENABLE_PACKAGE} -eq 1 ]]
then
	echo "## CMAKE PACKAGE"
	if [[ "${ENABLE_STEP}" == "1" ]]; then read -p "Press any key to continue..."; fi
	cmake --build ${BUILD_PATH} --target package --config ${BUILD_TYPE}
	cmake --build ${BUILD_PATH} --target package_source --config ${BUILD_TYPE}
fi
							
# Current Path
cd ${CURRENT_PATH}

# Done
echo "## DONE"

