/*
	escape_filename.c
	Copyright 2025-2026 by Christopher Heng. All rights reserved.
*/

#include "config.h"
#include <stdio.h> // fprintf(), stderr
#include <stdlib.h>	// size_t, malloc(), exit(), EXIT_FAILURE
#include <string.h> // strlen(), strdup()
#include "hash.h"

/*
	Note that this function calls exit() at multiple points.

	*has_backslashes will always be set to either 0 or 1 by this function.

	The pointer returned by this function points to a malloc()ed block
	and can be free()ed.
*/
char * escape_filename( char * filename, int * has_backslashes )
{
	size_t count ;
	size_t len ;
	char * s ;
	char * t ;
	char * adjusted_filename ;

	count = 0 ;

	for (s = filename; *s != '\0'; s++) {
		if (*s == '\\')
			count++ ;
	}
	if (count) {
		*has_backslashes = 1 ;
		len = 0 ;
		// len = strlen( filename) + count + 1;
		if (safely_add_three_size_t_operands( &len, strlen( filename ), count, 1 )) {
			fprintf( stderr, "%s: The filename is too long.\n", program_name );
			exit( EXIT_FAILURE );
		}
		if ((adjusted_filename = malloc( len )) == NULL) {
			fprintf( stderr, "%s: cannot allocate memory for filename\n", program_name );
			exit( EXIT_FAILURE );
		}
		s = filename ;
		t = adjusted_filename ;
		while (1) {
			*t = *s ;
			if (*s == '\\') {
				t++ ;
				*t = '\\' ;
			}
			if (*s == '\0')
				break ;
			s++ ;
			t++ ;
		}
	}
	else {
		*has_backslashes = 0 ;
		adjusted_filename = strdup( filename );
		if (adjusted_filename == NULL) {
			fprintf( stderr, "%s: cannot allocate memory for filename\n", program_name );
			exit( EXIT_FAILURE );
		}
	}
	return adjusted_filename ;
}
