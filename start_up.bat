@echo off

SET "ROOT=%CD%"
SET "OdinBaseDir=%ROOT%"
SET "ODIN_MAGIC_ENUM_ROOT=%ROOT%\packages\magic_enum.0.8.2"
SET "ODIN_BOOST_ROOT=%ROOT%\packages\boost.1.82.0"
CALL:check_env_var "PreferredToolArchitecture" "x64"
CALL:check_env_var "DevEnv" "Debug"
SET "PATH=%ROOT%\builds\intermediate\%PreferredToolArchitecture%\%DevEnv%;%ROOT%\builds\%PreferredToolArchitecture%\%DevEnv%;%PATH%"

IF "%1"=="vs" (
    START /B devenv.exe "%ROOT%\OdinAnalytics.sln"
    GOTO:Build_Sucessful
)

IF "%~1"=="excel" (
    IF  "%2" == "" GOTO:Bad_Excel_Start_Up

    START excel.exe /x %OdinBaseDir%\builds\%PreferredToolArchitecture%\%~2\oxl.xll
    GOTO:Excel_Start_Up_Sucessful    
)

GOTO:Environment_Var_Check

:check_env_var
    IF NOT DEFINED %~1 (
        SET "%~1=%~2"
        ECHO Setting %~1 to %~2 since it was not already set
    )
EXIT /B 0

ECHO Argument given was not a valid argument
GOTO :End

:Build_Sucessful
ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
ECHO ****** ODIN ANALYTICS Local Build Setup Sucessful ******
ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
GOTO:Environment_Var_Check

:Excel_Start_Up_Sucessful
ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
ECHO ######## Excel start with oxl.xll loaded in  ########
ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
GOTO:Environment_Var_Check


:Environment_Var_Check
ECHO Checking environment variables are set
ECHO OdinBaseDir=%OdinBaseDir%
ECHO PreferredToolArchitecture=%PreferredToolArchitecture%
ECHO DevEnv=%DevEnv%
GOTO:End

:Bad_Excel_Start_Up
ECHO #############################################################################################
ECHO !!!!! Please specify the build (Debug or Release) you want to use as the second argument !!!!
ECHO #############################################################################################
GOTO:End


:End
ECHO ****   Done   ****