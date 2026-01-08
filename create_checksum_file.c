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
	char * filename_for_output ;
	int has_backslashes ;
	FILE * fp ;
	int retval_from_fprintf ;

	fp = fopen( file_with_hashes, (overwrite_existing_checksum_file ? "wb" : "ab") ) ;
		// need to open in binary mode ("b") because the GNU coreutils expect lines to
		// end with \n and not \r\n

	if (fp == NULL) {
		fflush( stdout ); // just in case we're in verbose mode and stuff was printed to stdout earlier
		fprintf( stderr, "%s: unable to open checksum file %s\n", program_name, file_with_hashes );
		exit( EXIT_FAILURE );
	}

	filename_for_output = escape_filename( filename, &has_backslashes );

	if (has_backslashes) {
		retval_from_fprintf = fprintf( fp, "\\%s *%s\n", actual_hash, filename_for_output );
	}
	else {
		retval_from_fprintf = fprintf( fp, "%s *%s\n", actual_hash, filename_for_output );
	}

	if (retval_from_fprintf < 0) {
		fprintf( stderr, "%s: error writing checksum file %s\n", program_name, file_with_hashes );
		exit( EXIT_FAILURE );
	}

	free( filename_for_output );
	fclose( fp );
	return ;
}
