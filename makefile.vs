# makefile.vs
# nmake makefile; for building hash with Visual Studio
# Copyright 2020-2026 by Christopher Heng. All rights reserved.

# To make hash.exe:
#	nmake -f makefile.vs all
# To remove all the generated files (equivalent of clobber in other makefiles)
#	nmake -f makefile.vs clean
# The following makes the binary distribution zip and accompanying SHA512 checksum file
#	nmake -f makefile.vs VERSION=9.0.0 dist
# The following runs the tests:
#	nmake -f makefile.vs test

# To make hash.exe with Clang (VS version) (to check for errors)
#	nmake -f makefile.vs CC=clang-cl all

# The following is used for generating the binary distribution zip; override on the nmake command line.
!ifndef VERSION
VERSION = 9.0.0
!endif

# programs
CC = cl
CP = copy
LD = link
PANDOC = pandoc
RM = del
TAR = tar
# pandoc (3rd party program that converts between formats) and tar (the bsdtar that comes with Windows)
# are only needed for the "dist" target. Note that the rules using these two have hardcoded command line options
# specific to them, so you can't simply swap the programs by altering the macros above. You need to change
# the rules for "dist" too. They are listed above mainly for documentation purposes, so that I can
# see at a glance all the programs used in the makefile.

# flags
CFLAGS = /nologo /c /O2 /DNDEBUG /std:c17 /MT /utf-8 /Wall /WX /I"$(LIBDIR)\\" /I"$(MAKEDIR)\\" /Fo"$(BINDIR)\\"
	# /nologo - don't display sign-on banner
	# /c - compile only
	# /O2 - creates fast code; short for /Og /Oi /Ot /Oy /Ob2 /GF /Gy
	# /D - define macro
	# /std:c17 - support ISO C17
	# /MT - multithreaded exe
	# /utf-8 - set source and execution character sets to UTF-8
	# /Wall - all warnings in /W4 + warnings that are not enabled by default
	# /WX - treat all warnings as errors
	# /I - include directory
	# /Fo - output directory
LDFLAGS = /NOLOGO /INCREMENTAL:NO /WX /OPT:REF /OPT:ICF /DEPENDENTLOADFLAG:0x800 /OUT:$(HASH) /SUBSYSTEM:CONSOLE setargv.obj
	# for details, see https://docs.microsoft.com/en-us/cpp/build/reference/linker-options?view=msvc-170
	# /OUT - output filename
	# /SUBSYSTEM - type of exe
	# /NOLOGO - don't display sign-on banner
	# /INCREMENTAL:NO - don't include padding and thunks to facilitate incremental linking
	# not specified because the linker should be able to infer from the obj files: /MACHINE:X64 - x64 target
	# /WX - treat linker warnings as errors
	# /OPT:REF - eliminate unreferenced functions and data
	# /OPT:ICF - perform identical COMDAT folding
	# /DEPENDENTLOADFLAG:0x800 - load statically linked imports from the %windows%\system32 directory only
	#   Note that this only works in Windows 10 version 1607 and later. Ignored in earlier versions of Windows.
	#   see documentation at https://learn.microsoft.com/en-us/cpp/build/reference/dependentloadflag?view=msvc-160
	#   and https://devblogs.microsoft.com/oldnewthing/20230328-00/?p=107978
	# setargv.obj - expand wildcards on the command line

# directories
BINDIR = .
DOCSDIR = .\docs
LIBDIR = .\lib

# files
HASH = hash.exe
HASH_BINARY_DIST = hash-$(VERSION)-x86-64.zip
HASH_BINARY_DIST_CHECKSUM = $(HASH_BINARY_DIST).sha512
OBJS =	hash.obj \
	compare_hashes_and_report.obj \
	create_checksum_file.obj \
	escape_filename.obj \
	get_algorithm_and_hash_from_file.obj \
	get_hash_algorithm.obj \
	get_hash_algorithm_from_len.obj \
	getopt.obj \
	hashes_match.obj \
	process_file.obj \
	show_usage.obj \
	safely_add_three_size_t_operands.obj \
	skip_hash_prefix.obj \
	strip_path_prefix.obj

# Implicit rules
.c{$(BINDIR)}.obj:
	$(CC) @<<
  $(CFLAGS) $<
<<

# targets
all: $(HASH)

dist: $(HASH_BINARY_DIST) $(HASH_BINARY_DIST_CHECKSUM)

test: $(HASH)
	cd tests
	testhash all

# note that "clean" deletes "hash-*.zip" and "hash-*.sha512" since at the time we "clean",
# we don't know the value of the VERSION macro when those files were created,
# and so don't know the exact name of those files.
clean:
	$(RM) $(OBJS)
	$(RM) $(HASH) readme.html COPYING.txt hash-*.zip hash-*.sha512

$(HASH_BINARY_DIST): $(HASH) readme.html COPYING.txt
	$(TAR) -a -c -f $(HASH_BINARY_DIST) $(HASH) readme.html COPYING.txt

$(HASH_BINARY_DIST_CHECKSUM): $(HASH_BINARY_DIST) $(HASH) readme.html COPYING.txt
	$(HASH) -go -f $(HASH_BINARY_DIST_CHECKSUM) $(HASH) readme.html COPYING.txt $(HASH_BINARY_DIST)

COPYING.txt: $(DOCSDIR)\COPYING.txt
	$(CP) %s .

$(HASH): $(OBJS)
	$(LD) @<<
  $(LDFLAGS) $(OBJS)
<<

compare_hashes_and_report.obj: compare_hashes_and_report.c hash.h config.h

create_checksum_file.obj: create_checksum_file.c hash.h config.h

escape_filename.obj: escape_filename.c hash.h config.h

get_algorithm_and_hash_from_file.obj: get_algorithm_and_hash_from_file.c hash.h config.h

get_hash_algorithm.obj: get_hash_algorithm.c hash.h config.h

get_hash_algorithm_from_len.obj: get_hash_algorithm_from_len.c hash.h config.h

getopt.obj: $(LIBDIR)\getopt.c $(LIBDIR)\getopt.h
	$(CC) $(CFLAGS) %s

hash.obj: hash.c hash.h config.h $(LIBDIR)\getopt.h

hashes_match.obj: hashes_match.c hash.h config.h

process_file.obj: process_file.c hash.h config.h

readme.html: $(DOCSDIR)\README.md $(DOCSDIR)\metadata.yml
	$(PANDOC) -f gfm -t html -o readme.html -s -V mainfont=Arial -V fontsize=16pt -V maxwidth=45em --metadata-file=$(DOCSDIR)\metadata.yml %s

safely_add_three_size_t_operands.obj: safely_add_three_size_t_operands.c hash.h

show_usage.obj: show_usage.c hash.h config.h

skip_hash_prefix.obj: skip_hash_prefix.c hash.h config.h

strip_path_prefix.obj: strip_path_prefix.c hash.h config.h
