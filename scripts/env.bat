:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Copyright (c) 2018 Airbus Operations S.A.S                                  ::
::                                                                             ::
:: This program and the accompanying materials are made available under the    ::
:: terms of the Eclipse Public License v. 2.0 which is available at            ::
:: http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License   ::
:: v. 1.0 which is available at                                                ::
:: http://www.eclipse.org/org/documents/edl-v10.php.                           ::
::                                                                             ::
:: SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause                            ::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
 
:: File: env.bat
:: Brief: Define global environment variables of the project

@echo off

echo ## ENVIRONMENT - BEGIN
set ENVIRONMENT_SETUP=1

:loop
set ARG=%1
if not "%ARG%"=="" (
	if "%ARG%"=="-h" (
		echo # Help
		goto :usage
	) else if "%ARG:~0,1%"=="-" (
		echo # Invalid option [%ARG%]
	)
)

:: ROOT
set _ROOT=%~dp0..
echo # ROOT [%_ROOT%]

:: ARGUMENTS
set WORDSIZE=%1
shift
set BUILD_TYPE=%1
shift

:: BUILD TYPE
if "%BUILD_TYPE%"=="release" (set BUILD_TYPE=release) else (set BUILD_TYPE=debug)
echo # BUILD TYPE [%BUILD_TYPE%]

:: CUSTOM ENVIRONMENT
set CUSTOM_ENV=%_ROOT%\scripts\myenv.bat
if exist %CUSTOM_ENV% (
	call %CUSTOM_ENV%
)

:: WORDSIZE
if not defined WORDSIZE (
	if "%WORDSIZE%"=="i686" (set WORDSIZE=i686) else (set WORDSIZE=x86_64)
)
echo # WORDSIZE [%WORDSIZE%]

:: IOLAND
if not defined IOLAND_ROOT (
	set IOLAND_ROOT=Y:
)
echo # IOLAND [%IOLAND_ROOT%]

:: CROSSTOOLS
if not defined CROSSTOOLS_ROOT (
	set CROSSTOOLS_ROOT=Z:
)
echo # CROSSTOOLS [%CROSSTOOLS_ROOT%]

:: MINGW
if "%WORDSIZE%"=="x86_64" goto MINGW_x86_64
if not defined MINGW_ROOT (set MINGW_ROOT=%IOLAND_ROOT%\REF\COTS\MinGW\V4.9.2\BINARIES\Win32)
goto MINGW_END
:MINGW_x86_64
if not defined MINGW_ROOT (set MINGW_ROOT=%IOLAND_ROOT%\REF\COTS\MinGW\V4.9.2\BINARIES\Win64)
:MINGW_END
if not defined MINGW_BIN (set MINGW_BIN=%MINGW_ROOT%\bin)
if not defined MINGW_INC (set MINGW_INC=%MINGW_ROOT%\include)
if not defined MINGW_LIB (set MINGW_LIB=%MINGW_ROOT%\lib)
echo # MINGW BIN [%MINGW_BIN%] INC [%MINGW_INC%] LIB [%MINGW_LIB%]
set PATH=%MINGW_BIN%;%MINGW_INC%;%MINGW_LIB%;%PATH%

:: CMAKE
if not defined CMAKE_BIN (set CMAKE_BIN=%IOLAND_ROOT%\REF\COTS\CMake\V3.9.6\BINARIES\Windows\bin)
echo # CMAKE BIN [%CMAKE_BIN%]
set PATH=%CMAKE_BIN%;%PATH%

:: NINJA
if not defined NINJA_BIN (set NINJA_BIN=%IOLAND_ROOT%\REF\COTS\NINJA\V1.8.2\BINARIES\Windows)
echo # NINJA BIN [%NINJA_BIN%]
set PATH=%NINJA_BIN%;%PATH%

:: DOXYGEN
if not defined DOXYGEN_BIN (set DOXYGEN_BIN=%IOLAND_ROOT%\REF\COTS\DOXYGEN\V1.8.14\BINARIES\Windows\bin)
echo # DOXYGEN BIN [%DOXYGEN_BIN%]
set PATH=%DOXYGEN_BIN%;%PATH%

:: GRAPHVIZ
if not defined GRAPHVIZ_BIN (set GRAPHVIZ_BIN=%IOLAND_ROOT%\REF\COTS\GRAPHVIZ\graphviz-2.26.3-1\BINARIES\Windows\bin)
echo # GRAPHVIZ BIN [%GRAPHVIZ_BIN%]
set PATH=%GRAPHVIZ_BIN%;%PATH%

:: ASCIIDOCTOR
if not defined ASCIIDOCTOR_BIN (set ASCIIDOCTOR_BIN=%IOLAND_ROOT%\REF\COTS\ASCIIDOCTORSTACK\V2.0.0\install\windows\bin)
echo # ASCIIDOCTOR BIN [%ASCIIDOCTOR_BIN%]
set PATH=%ASCIIDOCTOR_BIN%;%PATH%

:: LCOV
if not defined LCOV_BIN (set LCOV_BIN= )
echo # LCOV BIN [%LCOV_BIN%]
set PATH=%LCOV_BIN%;%PATH%

:: LIBXML2
if "%WORDSIZE%"=="x86_64" goto LIBXML2_x86_64
if not defined LIBXML2_ROOT (set LIBXML2_ROOT=%IOLAND_ROOT%\REF\COTS\LIBXML2\V2.9.3-1\BINARIES\Win32)
goto LIBXML2_END
:LIBXML2_x86_64
if not defined LIBXML2_ROOT (set LIBXML2_ROOT=%IOLAND_ROOT%\REF\COTS\LIBXML2\V2.9.3-1\BINARIES\Win64)
:LIBXML2_END
if not defined LIBXML2_BIN (set LIBXML2_BIN=%LIBXML2_ROOT%\bin)
if not defined LIBXML2_INC (set LIBXML2_INC=%LIBXML2_ROOT%\include)
if not defined LIBXML2_LIB (set LIBXML2_LIB=%LIBXML2_ROOT%\lib)
echo # LIBXML2 BIN [%LIBXML2_BIN%] INC [%LIBXML2_INC%] LIB [%LIBXML2_LIB%]
set PATH=%LIBXML2_BIN%;%LIBXML2_INC%;%LIBXML2_LIB%;%PATH%

:: BUILD DIRECTORY
if not defined BUILD_PATH (set BUILD_PATH=%_ROOT%\build\Windows\%WORDSIZE%)
echo # BUILD PATH [%BUILD_PATH%]

:: INSTALL DIRECTORY
if not defined INSTALL_PATH (set INSTALL_PATH=%_ROOT%\install\Windows\%WORDSIZE%)
echo # INSTALL PATH [%INSTALL_PATH%]

set CURRENT_PATH=%cd%

set CTEST_OUTPUT_ON_FAILURE=1

echo ## ENVIRONMENT - DONE
goto :eof

:usage
	echo usage: %0 [-h] wordsize build_type
	echo  -h         : Help
	echo  wordsize   : Computer wordsize [i686^|x86_64]
	echo  build_type : Build type [debug^|release]
	exit /b 1
goto :eof

