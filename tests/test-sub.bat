@echo off
rem test-sub.bat
rem Copyright 2025-2026 by Christopher Heng. All rights reserved.

rem Do not run this file directly. It's meant to be called by testhash.bat.
rem %1 is the filename to process
rem %2 (optional) is the checksum filename; if omitted, -f is not used

rem IMPORTANT: you *must* have diff (from GNU diffutils) available on your path

setlocal
if "%1" == "all" goto CalledDirectly
if "%1" == "" goto CalledDirectly
if "%2" == "" goto CalledDirectly
if "%2" == "USE_ENV_FOR_FILENAME" goto UseEnvForFilename
if "%3" == "" goto NoChecksumFile
goto HasChecksumFile

:UseEnvForFilename
set HASH_COMMAND_LINE=..\hash %HASH_EXTRA_ARGS% %HASH_FILENAME_WITH_SPACES%
goto RunHash

:HasChecksumFile
set HASH_COMMAND_LINE=..\hash %HASH_EXTRA_ARGS% -f %3 %2
goto RunHash

:NoChecksumFile
set HASH_COMMAND_LINE=..\hash %HASH_EXTRA_ARGS% %2
goto RunHash

:RunHash
%HASH_COMMAND_LINE% > results\%1 2>&1
diff expected\%1 results\%1
if errorlevel 1 goto FailedTest
goto PassedTest

:FailedTest
echo FAILED: %HASH_COMMAND_LINE%
>> results\test-fails-log.txt echo FAILED: %HASH_COMMAND_LINE%
goto EndTest

:PassedTest
echo PASSED: %HASH_COMMAND_LINE%
del results\%1 > nul 2>&1
goto EndTest

:CalledDirectly
echo Do not call this batch file directly. Run testhash.bat instead.
goto EndTest

:EndTest
endlocal
