/*
	process_file.c
	Copyright 2020-2026 by Christopher Heng. All rights reserved.
*/

#include "config.h"
#include <stdint.h> // SIZE_MAX (for size_t)
#include <stdio.h> // fprintf(), snprintf(), popen(), pclose(), fflush()
#include <stdlib.h> // malloc(), free(), exit(), EXIT_FAILURE
#include <string.h> // strlen()
#include "hash.h"

#define	COMMAND_LINE_FORMAT	"certutil -hashfile \"%s\" %s"

/*
	process_file
	WARNING: this function exit()s on error at multiple locations.
*/
void process_file( char * filename, char * user_algorithm, char * user_hash )
{
	char * filename_to_search ;
	char * real_algorithm ;
	char * shell_command_line ;
	char * hash_to_compare_with ;
	size_t len ;
	FILE * input_from_certutil_fp ;

	if ((file_with_hashes != NULL) && !generate_hash) {
		if (name_to_match == NULL) {
			if (!keep_path_prefix) {
				filename_to_search = strip_path_prefix( filename );
				if (*filename_to_search == '\0') {
					// if stripping dir and drive leads to an empty string, just use the entire path
					filename_to_search = filename ;
					if (verbose) {
						fprintf( stdout,
							"%s: searching for checksum for %s even though it does not seem to be a file\n",
							program_name, filename );
					}
				}
			}
			else {
				filename_to_search = filename ;
			}
		}
		else { // name_to_match != NULL, that is, user specified a name for -n <name to match>
			if (verbose) {
				fprintf( stdout, "%s: searching for checksum for %s\n", program_name, name_to_match );
			}
			filename_to_search = name_to_match ;
		}
		if ((real_algorithm = get_algorithm_and_hash_from_file( filename_to_search, &hash_to_compare_with ))
			== NULL) {
			// error message will have already been printed by get_algorithm_and_hash_from_file()
			exit( EXIT_FAILURE );
		}
		if (verbose) {
			fprintf( stdout, "%s: %s checksum obtained from checksums file was:\n%s\n", program_name, real_algorithm,
				hash_to_compare_with );
		}
		if (user_algorithm != NULL) {
			// override the algorithm determined by the internal routine
			real_algorithm = user_algorithm ;
			if (verbose) {
				fprintf( stdout, "%s: algorithm overridden by command line option to %s\n", program_name, user_algorithm );
			}
		}
	}
	else {
		if (generate_hash) {
			if (user_algorithm == NULL) {
				real_algorithm = "SHA512" ;
			}
			else {
				real_algorithm = user_algorithm ;
			}
		}
		else if ((real_algorithm = get_hash_algorithm( user_algorithm, user_hash )) == NULL) {
			// error message will have already been printed by get_hash_algorithm()
			exit( EXIT_FAILURE );
		}
		hash_to_compare_with = user_hash ;
	}

	fflush( stdout );	// just in case there's some verbose stuff we printed before
						// do it before popen() and also all the stderr fprintf()s
	len = 0 ;
	// len = (sizeof( COMMAND_LINE_FORMAT ) + 2) + strlen( filename ) + strlen( real_algorithm ) ;
		// the +2 is in case I forgot something
	if (safely_add_three_size_t_operands( &len, (sizeof( COMMAND_LINE_FORMAT ) + 2),
		strlen( filename ), strlen( real_algorithm ) )) {

		fprintf( stderr, "%s: The filename and/or algorithm name is too long.\n",
			program_name );
		exit( EXIT_FAILURE );

	}

	if ((shell_command_line = malloc( len )) == NULL) {
		fprintf( stderr, "%s: cannot allocate memory for command line\n", program_name );
		exit( EXIT_FAILURE );
	}

	snprintf( shell_command_line, len, COMMAND_LINE_FORMAT, filename, real_algorithm );
	if (verbose) {
		fprintf( stdout, "%s: %s\n", program_name, shell_command_line );
	}

	input_from_certutil_fp = popen( shell_command_line, "rt" );
	if (input_from_certutil_fp == NULL) {
		fprintf( stderr, "%s: cannot run CertUtil\n", program_name );
		exit( EXIT_FAILURE );
	}
	compare_hashes_and_report( filename, hash_to_compare_with, input_from_certutil_fp );
	pclose( input_from_certutil_fp );
	free( shell_command_line );

	return ;
}
