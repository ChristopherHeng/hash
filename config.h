/*
	config.h
	Copyright 2023-2026 by Christopher Heng. All rights reserved.
*/

#pragma once

#if defined(_MSC_VER) // VS and Clang (VS version)

	// note that some of these pragmas and macros are needed even on the VS version of Clang
	// due to what VS put in their headers (eg, stdio.h, vcruntime.h). And of course, the
	// macros for the POSIX standard functions are needed.

// disable warnings about using POSIX-standard function names
#define _CRT_NONSTDC_NO_WARNINGS

// disable warnings about using certain C standard functions (like fopen(), strcpy(), etc)
#define _CRT_SECURE_NO_WARNINGS

// 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
// occurs when including <winioctl.h> (which is included by <windows.h>) with /Wall (VS 2022, 2026)
#pragma warning(disable: 4668)

// disable warnings about fprintf() not being inlined
#pragma warning(disable: 4710)

// disable warning that compiler will insert Spectre mitigation... if /Qspectre is specified
#pragma warning( disable: 5045 )

// map POSIX-standard names to Visual Studio's non-standard (implementation-dependant) names
#define	popen(a,b) _popen(a,b)
#define pclose(a) _pclose(a)
#define strcasecmp(a,b)	_stricmp(a,b)
#define	strdup(a) _strdup(a)

#endif // defined(_MSC_VER)
