@echo off
rem test-create.bat
rem Copyright 2025-2026 by Christopher Heng. All rights reserved.

rem Do not run this file directly. It's meant to be called by testhash.bat.
rem IMPORTANT: you *must* have diff (from GNU diffutils) available on your path.

rem Note: I'm only using MD5 because I have already set up .gitattributes to
rem treat MD5 files as having LF as EOL markers.

setlocal
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

:Test2
set HASH_CMD_1=..\hash -og -a MD5 -f results\genchksum4.md5 file-1.txt ..\tests\file-2.txt file-3.txt file-4.txt
%HASH_CMD_1% > results\gen-4-files-ckf.msg 2>&1
diff expected\gen-4-files-ckf.msg results\gen-4-files-ckf.msg
if errorlevel 1 goto FailedTest2
diff --binary expected\genchksum4.md5 results\genchksum4.md5
if errorlevel 1 goto FailedTest2
echo PASSED: %HASH_CMD_1%
del results\gen-4-files-ckf.msg > nul 2>&1
del results\genchksum4.md5 > nul 2>&1

:TestWild
set HASH_CMD_1=..\hash -og -a MD5 -f results\gen-wild.md5 file-*.txt "file with spaces.txt"
%HASH_CMD_1% > results\gen-wild.msg 2>&1
diff expected\gen-wild.msg results\gen-wild.msg
if errorlevel 1 goto FailedWild
diff --binary expected\gen-wild.md5 results\gen-wild.md5
if errorlevel 1 goto FailedWild
echo PASSED: %HASH_CMD_1%
del results\gen-wild.msg > nul 2>&1
del results\gen-wild.md5 > nul 2>&1
goto EndTest

:FailedTest
echo FAILED: 1st line: %HASH_CMD_1%
echo .       2nd line: %HASH_CMD_2%
>> results\test-fails-log.txt echo FAILED: 1st line: %HASH_CMD_1%
>> results\test-fails-log.txt echo .       2nd line: %HASH_CMD_2%
goto Test2

:Failedtest2
echo FAILED: %HASH_CMD_1%
>> results\test-fails-log.txt echo FAILED: %HASH_CMD_1%
goto TestWild

:FailedWild
echo FAILED: %HASH_CMD_1%
>> results\test-fails-log.txt echo FAILED: %HASH_CMD_1%
goto EndTest

:CalledDirectly
echo Do not call this batch file directly. Run testhash.bat instead.
goto EndTest

:EndTest
endlocal
