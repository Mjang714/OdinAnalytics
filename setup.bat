@echo off

SET "ROOT=%CD%"
SET "OdinBaseDir=%ROOT%" 


SET "PreferredToolArchitecture=x64"
IF "%1"=="vs" (
    START /B devenv.exe "%ROOT%\OdinAnalytics.sln"
    GOTO:Build_Sucessful
)

:Build_Sucessful
ECHO !!!!
ECHO *** ODIN ANALYTICS Setup Sucessful ***
ECHO Checking environment variables are set
ECHO !!!!
ECHO ROOT=%ROOT%
ECHO OdinBaseDir=%OdinBaseDir%
