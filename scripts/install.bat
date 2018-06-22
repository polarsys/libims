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
 
:: File: install.bat
:: Brief: Install project using CMake

@echo off

echo # INSTALL

set ENABLE_DOC=0
set ENABLE_PACKAGE=0
set CLEAN=0
set ENABLE_STEP=0
set ENABLE_INSTRUMENTATION=0
set ENABLE_TEST=0

:loop
set ARG=%1
if not "%ARG%"=="" (
	if "%ARG%"=="-h" (
		echo # Help
		goto :usage
	) else if "%ARG%"=="-d" (
		echo # Build documentation
		set ENABLE_DOC=1
		shift
		goto :loop
	) else if "%ARG%"=="-c" (
		echo # Clean before
		set CLEAN=1
		shift
		goto :loop
	) else if "%ARG%"=="-p" (
		echo # Package
		set ENABLE_PACKAGE=1
		shift
		goto :loop
	) else if "%ARG%"=="-s" (
		echo # Step by step
		set ENABLE_STEP=1
		shift
		goto :loop
	) else if "%ARG%"=="-i" (
		echo # Enable instrumentation
		set ENABLE_INSTRUMENTATION=1
		shift
		goto :loop
    ) else if "%ARG%"=="-t" (
		echo # Build tests
		set ENABLE_TEST=1
		shift
		goto :loop
	) else if "%ARG:~0,1%"=="-" (
		echo # Invalid option [%ARG%]
		goto :usage
	)
)

:: Environment
if "%ENVIRONMENT_SETUP%"=="" (
    call %~dp0env.bat %1 %2
)

:: Directories
echo # DIRECTORIES
set DIRECTORIES=%BUILD_PATH% ^
%BUILD_PATH%\doc ^
%BUILD_PATH%\Windows ^
%BUILD_PATH%\Windows\i686 ^
%BUILD_PATH%\Windows\x86_64 ^
%INSTALL_PATH% ^
%INSTALL_PATH%\doc ^
%INSTALL_PATH%\Windows ^
%INSTALL_PATH%\Windows\i686 ^
%INSTALL_PATH%\Windows\x86_64 

for %%d in (%DIRECTORIES%) do (
	if not exist "%%d" (
		echo # CREATE [%%d]
		mkdir %%d
	) else (
		echo # EXISTS [%%d]
	)
)

:: Custom environment
echo # CUSTOM ENVIRONMENT
set ENV_CUSTOM=%~dp0myenv.bat
if not exist "%ENV_CUSTOM%" (
	echo # CREATE [%ENV_CUSTOM%]
	echo :: Custom environment>%ENV_CUSTOM%
	echo.>>%ENV_CUSTOM%
	echo @echo off>>%ENV_CUSTOM%
	echo.>>%ENV_CUSTOM%
	echo echo # CUSTOM ENVIRONMENT - BEGIN>>%ENV_CUSTOM%
	echo.>>%ENV_CUSTOM%
	echo echo # CUSTOM ENVIRONMENT - DONE>>%ENV_CUSTOM%
) else (
	echo # EXISTS [%ENV_CUSTOM%]
)

:: Clean
if "%CLEAN%"=="1" (
	echo ## CLEAN
	rmdir /s /q %BUILD_PATH%
	mkdir %BUILD_PATH%
)

:: Build Path
cd /d %BUILD_PATH%

:: Configure
echo ## CMAKE CONFIGURE
set arg=-DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
        -DCMAKE_INSTALL_PREFIX=%INSTALL_PATH% ^
        -DWORDSIZE=%WORDSIZE% ^
        -DENABLE_DOC=%ENABLE_DOC% ^
        -DENABLE_TEST=%ENABLE_TEST% ^
        -DENABLE_INSTRUMENTATION=%ENABLE_INSTRUMENTATION% 

if "%ENABLE_STEP%"=="1" (pause)
cmake.exe %_ROOT% -G"Ninja"	%arg% -Wno-dev

:: Build
echo ## CMAKE BUILD
if "%ENABLE_STEP%"=="1" (pause)
cmake.exe --build %BUILD_PATH% --target all --config %BUILD_TYPE%

:: Test
if "%ENABLE_TEST%"=="1" (
	echo ## CMAKE TEST
	if "%ENABLE_STEP%"=="1" (pause)
	cmake.exe --build %BUILD_PATH% --target test --config %BUILD_TYPE%
)

:: Doc
if "%ENABLE_DOC%"=="1" (
	echo ## CMAKE DOC
	if "%ENABLE_STEP%"=="1" (pause)
	if exist "%ASCIIDOCTOR_BIN%" (
		echo ## Build asciidoctor target: OK
		cmake.exe --build %BUILD_PATH% --target asciidoctor --config %BUILD_TYPE%
	) else (
		echo ## Build asciidoctor target: KO
	)
	if exist "%DOXYGEN_BIN%" (
		echo ## Build doxygen target: OK
		cmake.exe --build %BUILD_PATH% --target doc --config %BUILD_TYPE%
	) else (
		echo ## Build doxygen target: KO
	)
)

:: Install
echo ## CMAKE INSTALL
if "%ENABLE_STEP%"=="1" (pause)
cmake.exe --build %BUILD_PATH% --target install --config %BUILD_TYPE%

:: Package
if "%ENABLE_PACKAGE%" == "1" (
	echo ## CMAKE PACKAGE
	if "%ENABLE_STEP%"=="1" (pause)
	cmake.exe --build %BUILD_PATH% --target package --config %BUILD_TYPE%
	cmake.exe --build %BUILD_PATH% --target package_source --config %BUILD_TYPE%
)

:: Return
cd /d %CURRENT_PATH%

:: Done
echo ## DONE
goto :eof

:usage
    set arg=usage: %0 [-h] [-c] [-d] [-t] [-p] [-i] [-s] wordsize build_type
	echo %arg%
	echo  -h         : Help
	echo  -c         : Clean before
	echo  -d         : Build documentation
	echo  -p         : Package
    echo  -t         : Build tests
	echo  -i         : Enable instrumentation
	echo  -s         : Enable step by step
	echo  wordsize   : Computer wordsize [[x86_64]^|i686]
	echo  build_type : Build type [[debug]^|release]
	exit /b 1
goto :eof
