/*
	hashes_match.c
	Copyright 2020-2026 by Christopher Heng. All rights reserved.
*/

#include "config.h"
#include <stdio.h> // fgets(), fprintf(), fputs(), fflush()
#include <stdlib.h> // exit(), EXIT_FAILURE
#include <string.h> // strcasecmp(), strdup()
#include "hash.h"

// WARNING: BUFFER_SIZE must be more than the size of the largest hash text
#define BUFFER_SIZE 1024

/*
	hashes_match
	WARNING: that this function directly exit()s on error at multiple locations.

	Format of output from certutil (eg):
		SHA1 hash of sample.txt:
		ed89ee9e859963ce874ec5cb8553be8b98e9afff
		CertUtil: -hashfile command completed successfully.
	Theoretically, should check first word on first line to see if it matches user_hash. Else certutil
	will have encountered an error. Eg of an error message:
		CertUtil: -hashfile command FAILED: 0x80070002 (WIN32: 2 ERROR_FILE_NOT_FOUND)
		CertUtil: The system cannot find the file specified.
*/

int hashes_match( char * user_hash, FILE * input_from_certutil_fp )
{
	char buffer[BUFFER_SIZE] ; // so that we don't have to deal with malloc(), free()
	char * s ;
	char * t ;
	int matched ;

	do { // skip first line
		if ((s = fgets( buffer, sizeof(buffer), input_from_certutil_fp )) == NULL) {
			fflush( stdout );
			fprintf( stderr, "%s: unexpected end of input from CertUtil\n", program_name );
			exit( EXIT_FAILURE );
		}
		if (verbose) {
			fputs( buffer, stdout );
		}
		// find newline if any
		for ( ; *s != '\0'; s++) {
			if (*s == '\n')
				break ;
		}
	} while (*s != '\n') ;

	// get the line with the hash
	if ((s = fgets( buffer, sizeof(buffer), input_from_certutil_fp )) == NULL) {
		fflush( stdout );
		fprintf( stderr, "%s: Unexpected end of input from CertUtil\n", program_name );
		exit( EXIT_FAILURE );
	}
	if (verbose) {
		// have to do this before stripping the newline
		fputs( buffer, stdout );
	}
	// strip final newline
	t = s + strlen( s ) - 1;
	if (*t == '\n') {
		*t = '\0' ;
	}

	if (!generate_hash) {
		// do the comparison now before buffer is overwritten for verbose mode
		matched = (strcasecmp( buffer, user_hash ) ? 0 : 1) ;
	}
	else {
		matched = 0;
	}
	if (!matched) {
		// if there is no match, try to make a copy of the actual hash value (if possible) for display by caller
		actual_hash = strdup( buffer );
		// IMPORTANT: caller must check if actual_hash is not an error message before using it
		if (actual_hash == NULL) {	// insufficient memory allocating buffer to duplicate the hash
			fflush( stdout ); // flush earlier verbose messages if any
			fprintf( stderr, "%s: Either the hashes do not match or CertUtil returned an error.\n"
				"Unfortunately, we ran out of memory before we could determine what actually happened, so exiting now.\n",
				program_name ); // tell the user what happened before we die.
			exit( EXIT_FAILURE );
		}
	}

	// If verbose, read the rest of stdout and display it.
	if (verbose) {
		if (fgets( buffer, sizeof(buffer), input_from_certutil_fp ) != NULL) {
			fputs( buffer, stdout );
		}
	}

	return matched ;
}
