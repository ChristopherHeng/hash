/*
	strip_path_prefix.c
	Copyright 2021-2026 by Christopher Heng. All rights reserved.
*/

#include <string.h> // strlen()
#include "hash.h"

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
