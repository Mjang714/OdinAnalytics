@echo off

SET "ROOT=%CD%"
SET "OdinBaseDir=%ROOT%"

SET "PreferredToolArchitecture=x64"
IF "%1"=="vs" (
    START /B devenv.exe "%ROOT%\OdinAnalytics.sln"
    GOTO:Build_Sucessful
)

IF "%~1"=="excel" (
    IF  "%2" == "" GOTO:Bad_Excel_Start_Up

    START excel.exe /x %OdinBaseDir%\builds\%PreferredToolArchitecture%\%~2\oxl.xll
    GOTO:Excel_Start_Up_Sucessful    
)

:Build_Sucessful
ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
ECHO *** ODIN ANALYTICS Local Build Setup Sucessful ***
ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
GOTO:Environment_Var_Check

:Excel_Start_Up_Sucessful
ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
ECHO #### Excel start with oxl.xll loaded in  ####
ECHO !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
GOTO:Environment_Var_Check


:Environment_Var_Check
ECHO Checking environment variables are set
ECHO OdinBaseDir=%OdinBaseDir%
GOTO:End

:Bad_Excel_Start_Up
ECHO ###########################################################################
ECHO !!!!! Please specify the build you want to use as the second argument !!!!!
ECHO ###########################################################################
GOTO:End

:End
ECHO ****   Done   ****