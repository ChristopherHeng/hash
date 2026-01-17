/*
	create_checksum_file.c
	Copyright 2025-2026 by Christopher Heng. All rights reserved.
*/

#include "config.h"
#include <stdio.h>	// fopen(), fprintf(), stderr
#include <stdlib.h>	// exit(), EXIT_FAILURE
#include "hash.h"

/*
	Note that this function calls exit() directly on failure.
*/
void create_checksum_file( char * filename )
{
	static FILE * fp = NULL ;
	char * filename_for_output ;
	int has_backslashes ;
	int retval_from_fprintf ;

	if (fp == NULL) {
		// only open the file if it hasn't already been opened
		fp = fopen( file_with_hashes, (overwrite_existing_checksum_file ? "wb" : "ab") ) ;
		if (fp == NULL) {
			fflush( stdout ); // just in case we're in verbose mode and stuff was printed to stdout earlier
			fprintf( stderr, "%s: unable to open checksum file %s\n", program_name, file_with_hashes );
			exit( EXIT_FAILURE );
		}
	}
	files_left-- ;

	filename_for_output = escape_filename( filename, &has_backslashes );

	if (has_backslashes) {
		retval_from_fprintf = fprintf( fp, "\\%s *%s\n", actual_hash, filename_for_output );
	}
	else {
		retval_from_fprintf = fprintf( fp, "%s *%s\n", actual_hash, filename_for_output );
	}

	if (retval_from_fprintf < 0) {
		fflush( stdout );
		fprintf( stderr, "%s: error writing checksum file %s\n", program_name, file_with_hashes );
		exit( EXIT_FAILURE );
	}

	free( filename_for_output );
	if (files_left == 0) {
		fclose( fp );
		fp = NULL ; // defensive (to help in debugging if need be)
	}
	return ;
}
