@echo off
rem testhash.bat
rem Copyright 2025-2026 by Christopher Heng. All rights reserved.

rem Usage: testhash all
rem The only reason for the argument is to make sure we don't accidentally execute it.
rem Run this in a Command Prompt on Windows
rem REQUIRES: diff (from GNU diffutils)

setlocal
rem Make sure that diff is available
where /Q diff
if "%ERRORLEVEL%" NEQ "0" goto NeedDiff

rem So that running "testhash" alone displays the usage info.
if not "%1" == "all" goto ShowUsage

rem Make sure we are in the correct directory
if not exist "test-sub.bat" goto WrongDirectory
if not exist "test-create.bat" goto WrongDirectory

rem Make sure hash.exe has been built
if not exist "..\hash.exe" goto NeedToBuildFirst

rem Init the error log
if exist results\test-fails.log move results\test-fails.log results\test-fails.log.bak

rem Unset the environment variables that we rely on (just in case)
set HASH_EXTRA_ARGS=
set HASH_FILENAME_WITH_SPACES=

rem Run tests
call test-sub single-line-valid-checksum-file.msg file-1.txt sha512-1.txt
call test-sub triple-lines-valid-checksum-last.msg file-3.txt sha512-2.txt
call test-sub bad-fmt-but-works.msg file-5.txt sha256-bad-1.txt
call test-sub bad-fmt-fails.msg file-3.txt sha256-bad-1.txt
call test-sub hash-only.msg file-1.txt sha256-hashonly.txt

set HASH_EXTRA_ARGS=-a md5 -c abcdef
call test-sub bad-hash-given.msg file-1.txt
set HASH_EXTRA_ARGS=

set HASH_EXTRA_ARGS=-c 3e938ccf76200b05fc1c24f8dbf88688
call test-sub no-such-file.msg nosuchfile.txt
set HASH_EXTRA_ARGS=

set HASH_EXTRA_ARGS=-vc sha256:1dd87bdf28e806d84497eff89e43c9581cb42ae42fa4146f7b767811a88952bf
call test-sub ghprefix-verbose.msg file-2.txt
set HASH_EXTRA_ARGS=

set HASH_EXTRA_ARGS=-g
call test-sub basic-generate.msg file-1.txt
set HASH_EXTRA_ARGS=

set HASH_EXTRA_ARGS=-v
call test-sub single-line-valid-chk-verbose.msg file-1.txt sha512-1.txt
call test-sub triple-lines-valid-chk-last-verbose.msg file-3.txt sha512-2.txt
call test-sub bad-fmt-but-works-verbose.msg file-5.txt sha256-bad-1.txt
call test-sub bad-fmt-fails-verbose.msg file-3.txt sha256-bad-1.txt
call test-sub hash-only-verbose.msg file-1.txt sha256-hashonly.txt
call test-sub not-listed-backslash-verbose.msg file-1.txt md5-backslash.txt
call test-sub no-k-backslash.msg ..\tests\file-1.txt md5-backslash.txt
call test-sub not-listed-normal-verbose.msg file-2.txt sha512-1.txt
call test-sub no-final-newline-verbose.msg file-1.txt sha512-no-newline.txt
call test-sub no-final-nl-wrong-file.msg file-2.txt sha512-no-newline.txt
set HASH_EXTRA_ARGS=

set HASH_EXTRA_ARGS=-t gnu -v
call test-sub hash-only-force-gnu-verbose.msg file-1.txt sha256-hashonly.txt
call test-sub triple-last-force-gnu-verbose.msg file-3.txt sha512-2.txt
set HASH_EXTRA_ARGS=

set HASH_EXTRA_ARGS=-t deno -v
rem This "-t deno" causes hash to misidentify the checksum file as deno and the
rem algorithm as MD5. I allowed this to happen because this is a user error, and not a program fault.
rem If the user had not specified -t deno but let hash work on its own (auto mode), it would have
rem successfully identified the file correctly. (Basically it tests a specific code path.)
call test-sub hash-only-force-deno-verbose.msg file-1.txt sha256-hashonly.txt
rem In the same vein, the following is misidentified as SHA384 because of user error.
call test-sub triple-last-force-deno-verbose.msg file-3.txt sha512-2.txt
set HASH_EXTRA_ARGS=

set HASH_EXTRA_ARGS=-t hashonly -v
call test-sub hash-only-force-hashonly-verbose.msg file-1.txt sha256-hashonly.txt
rem The following is a GNU file, but the user sets -t hashonly. As a result, hash
rem is forced to take the first line's hash, since such files should only have a single hash,
rem giving a hash mismatch. (To test a specific code path.)
call test-sub triple-last-force-hashonly-verbose.msg file-3.txt sha512-2.txt
set HASH_EXTRA_ARGS=

set HASH_EXTRA_ARGS=-v -k
call test-sub check-backslash.msg ..\tests\file-1.txt md5-backslash.txt
set HASH_EXTRA_ARGS=

rem Handle filenames with spaces - cannot pass as argument to batch file (will not work)
set HASH_FILENAME_WITH_SPACES="file with spaces.txt"
set HASH_EXTRA_ARGS=-g
call test-sub gen-file-spaces.msg USE_ENV_FOR_FILENAME
set HASH_EXTRA_ARGS=-c 0cb055fdd05e04be5306d7706917e84d
call test-sub chk-file-spaces.msg USE_ENV_FOR_FILENAME
set HASH_EXTRA_ARGS=-v -f sha512-spaces.txt
call test-sub csfile-spaces-verbose.msg USE_ENV_FOR_FILENAME
set HASH_EXTRA_ARGS=
set HASH_FILENAME_WITH_SPACES=

rem Test -g -f with or without -o, with and without backslashes
call test-create called_from_testhash

rem Check if any of the tests failed.
if exist results\test-fails.log goto FailedTest
echo All tests PASSED.
goto EndScript

:FailedTest
echo *** One or more tests FAILED. See results\test-fails.log.
goto EndScript

:WrongDirectory
echo Error: the test directory must be the current directory. Aborting.
goto EndScript

:NeedToBuildFirst
echo Error: you need to build hash.exe first. Aborting.
goto EndScript

:NeedDiff
echo This batch file requires diff (from GNU diffutils).
goto EndScript

:ShowUsage
echo Usage: testhash all
goto EndScript

:EndScript
endlocal
