/*
	get_hash_algorithm.c
	Copyright 2020-2026 by Christopher Heng. All rights reserved.
*/

#include "config.h"
#include <stdio.h> // fprintf(), NULL
#include <string.h> // strlen()
#include "hash.h"

char * get_hash_algorithm( char * user_algorithm, char * user_hash )
{
	size_t len ;
	if (user_hash == NULL) {
		// this should not happen; it should have been checked by the caller (or caller's caller)
		// We need to check it now because we call strlen() on user_hash without checking
		fprintf( stderr, "%s: internal error: NULL user_hash\n", program_name );
		return NULL ;
	}
	if (user_algorithm == NULL) {
		if ((user_algorithm = get_hash_algorithm_from_len( (len = strlen( user_hash )) )) == NULL) {
			fprintf( stderr, "%s: unknown type of hash\n", program_name );
		}
	}
	return user_algorithm ;
}
