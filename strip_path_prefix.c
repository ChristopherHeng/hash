/*
	strip_path_prefix.c
	Copyright 2021-2026 by Christopher Heng. All rights reserved.
*/

#include "config.h"
#include <string.h> // strlen()
#include "hash.h"

#if defined(__clang__)
// warning about unsafe pointer arithmetic for pointing s to last char of string,
// as well as incrementing and decrementing it
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

// note that we assume that filename != NULL
char * strip_path_prefix( char * filename )
{
	size_t orig_len ;
	char * s ;

	orig_len = strlen( filename );
	if (orig_len < 1) {
		s = filename ;
	}
	else {
		s = filename + orig_len - 1 ; // point to last char of string
		while (s != filename) {
			if ((*s == '/') || (*s == '\\') || (*s == ':')) {
				s++ ;
				break ;
			}
			s-- ;
		}
	}
	return s ;
}
