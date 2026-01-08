@echo off
rem test-create.bat
rem Copyright 2025-2026 by Christopher Heng. All rights reserved.

rem Do not run this file directly. It's meant to be called by testhash.bat.
rem IMPORTANT: you *must* have diff (from GNU diffutils) available on your path.

setlocal
if "%1" == "all" goto CalledDirectly
if not "%1" == "called_from_testhash" goto CalledDirectly

set HASH_CMD_1=..\hash -o -a MD5 -g -f results\genchksumfile.md5 file-1.txt
set HASH_CMD_2=..\hash -a MD5 -g -f results\genchksumfile.md5 ..\tests\file-2.txt

%HASH_CMD_1% > results\gen-2-line-ckf.msg 2>&1
%HASH_CMD_2% >> results\gen-2-line-ckf.msg 2>&1
diff expected\gen-2-line-ckf.msg results\gen-2-line-ckf.msg
if errorlevel 1 goto FailedTest
diff --binary expected\genchksumfile.md5 results\genchksumfile.md5
if errorlevel 1 goto FailedTest

echo PASSED: 1st line: %HASH_CMD_1%
echo .       2nd line: %HASH_CMD_2%
del results\gen-2-line-ckf.msg > nul 2>&1
del results\genchksumfile.md5 > nul 2>&1
goto EndTest

:FailedTest
echo FAILED: 1st line: %HASH_CMD_1%
echo .       2nd line: %HASH_CMD_2%
>> results\test-fails-log.txt echo FAILED: 1st line: %HASH_CMD_1%
>> results\test-fails-log.txt echo .       2nd line: %HASH_CMD_2%
goto EndTest

:CalledDirectly
echo Do not call this batch file directly. Run testhash.bat instead.
goto EndTest

:EndTest
endlocal
