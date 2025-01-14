@echo off

chcp 437 > nul

set NINJA=0
set CYGWIN=nodosfilewarning
if exist %CFG_PROJECT%/project/%TARGET%/%TARGET% del %CFG_PROJECT%\project\%TARGET%\%TARGET%

set PROJECT_NAME=%CFG_PROJECT%

if "%CODEC%" == "1" (
    call :buildProject risc1_code
    call :buildProject codec
)

if "%CODEC_EX%" == "1" (
    call :buildProject codec_ex
)

if "%CODEC_IT9910%" == "1" (
    call :buildProject codec_it9910
)

if "%CODEC_EX_IT9910%" == "1" (
    call :buildProject codec_ex_it9910
)

if "%CODEC_IT9850%" == "1" (
    call :buildProject codec_it9850
)

if "%CODEC_EX_IT9850%" == "1" (
    call :buildProject codec_ex_it9850
)

if "%RISC_TEST%" == "1" (
    call :buildProject risc1_code
    call :buildProject risc2_code
    call :buildProject risc1_code_it9850
    call :buildProject risc2_code_it9850
)

call :buildProject %PROJECT_NAME%
goto :eof

rem ###########################################################################
rem The function to build project
rem ###########################################################################
:buildProject
SETLOCAL
set LOCAL_PROJECT_NAME=%1
set CFG_PROJECT=%LOCAL_PROJECT_NAME%
if not exist %LOCAL_PROJECT_NAME% mkdir %LOCAL_PROJECT_NAME%
pushd %LOCAL_PROJECT_NAME%

if "%NINJA%" == "0" (
    if exist CMakeCache.txt (
        cmake.exe -G"Unix Makefiles" "%CMAKE_SOURCE_DIR%"
    ) else (
        cmake.exe -G"Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE="%CFG_TOOLCHAIN_FILE%" "%CMAKE_SOURCE_DIR%"
    )
    if errorlevel 1 exit /b

    if "%MAKECLEAN%"=="1" (
        echo "Clean build..."
        make clean
    )

    if "%MAKEJOBS%"=="" (
        make -j 1 VERBOSE=%VERBOSE%
    ) else (
        make -j %MAKEJOBS% VERBOSE=%VERBOSE%
    )
) else (
    if "%VERBOSE%" == "1" (
        set VERBOSEOUT="-v"
    ) else (
        set VERBOSEOUT=""
    )

    if exist CMakeCache.txt (
        cmake.exe -G Ninja "%CMAKE_SOURCE_DIR%"
    ) else (
        cmake.exe -G Ninja -DCMAKE_TOOLCHAIN_FILE="%CFG_TOOLCHAIN_FILE%" "%CMAKE_SOURCE_DIR%"
    )
    if errorlevel 1 exit /b

    if "%MAKECLEAN%"=="1" (
        echo "Clean build..."
        ninja -t clean
    )

    if "%MAKEJOBS%"=="" (
        ninja -j 1 %VERBOSEOUT%
    ) else (
        ninja -j %MAKEJOBS% %VERBOSEOUT%
    )
)
popd

ENDLOCAL
