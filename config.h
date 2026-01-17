/*
	config.h
	Copyright 2023-2026 by Christopher Heng. All rights reserved.
*/

#pragma once

#if defined(_MSC_VER) // VS and Clang (VS version)

	// note that some of these are needed even on the VS version of Clang
	// due to what VS put in their headers (eg, stdio.h, vcruntime.h). And of course, the
	// macros for the POSIX standard functions are needed since we're using VS' C library.

// disable warnings about using POSIX-standard function names
#define _CRT_NONSTDC_NO_WARNINGS

// disable warnings about using certain C standard functions (like fopen(), strcpy(), etc)
#define _CRT_SECURE_NO_WARNINGS

// 'symbol' is not defined as a preprocessor macro, replacing with '0' for 'directives'
// occurs when including <winioctl.h> (which is included by <windows.h>) with /Wall (VS 2022, 2026)
// (And yes, we include <windows.h>; for MAX_PATH definition)
#pragma warning(disable: 4668)

// disable warnings about fprintf() not being inlined
#pragma warning(disable: 4710)

// disable warning that compiler will insert Spectre mitigation... if /Qspectre is specified
#pragma warning( disable: 5045 )

// Map POSIX-standard names to Visual Studio's non-standard (implementation-dependant) names.
// Note: strdup() is C23 standard, which VS does not yet support (KIV remove that macro if/when they do).
#define	popen(a,b) _popen(a,b)
#define pclose(a) _pclose(a)
#define strcasecmp(a,b)	_stricmp(a,b)
#define	strncasecmp(a,b,c) _strnicmp(a,b,c)
#define	strdup(a) _strdup(a)

#endif // defined(_MSC_VER)

#if defined(__clang__) // Clang only

// since I explicitly requested C17, disable warnings about using C features that require C99 or C11
#pragma clang diagnostic ignored "-Wdeclaration-after-statement"
#pragma clang diagnostic ignored "-Wpre-c11-compat"

// disable warnings about including <windows.h> (per MSDN's capitalization) when the file on disk
// as installed by Visual Studio is Windows.h (notice the capital 'W' here)
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"

#endif
