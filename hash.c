/*
	hash.c
	Copyright 2020-2026 by Christopher Heng. All rights reserved.
	see show_usage.c for usage information.
*/

#include "config.h"
#include <stdio.h> // fprintf()
#include <stdlib.h> // EXIT_SUCCESS, EXIT_FAILURE
#include <string.h> // strcmp()
#if defined(_MSC_VER) // VS and Clang (VS version)
#include "lib/getopt.h"
#else
#include <unistd.h> // getopt(), optind, opterr, optarg
#endif
#include "hash.h"

#if defined(__clang__)
// disable warning about using argv[optind] in calling process_file()
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

char * program_name = "hash" ;
char * version_string = "9.0.0" ;
char * copyright_start_years = "2020-2026" ;

int chksum_file_type = CHKSUM_FILE_AUTODETECT ; // -t
int files_left ;
int generate_hash ; // -g
int keep_path_prefix ; // -k
int overwrite_existing_checksum_file ; // -o
int verbose ; // -v
char * actual_hash ;
char * file_with_hashes ; // the argument to -f
char * name_to_match ; // the name specified for -n

/*
	WARNING: this function has multiple exit points, via "return"
*/
int main ( int argc, char ** argv )
{
	int option ;
	char * user_algorithm ;
	char * user_hash ;
	int option_invoked ;

	user_algorithm = NULL ;
	user_hash = NULL ;
	option_invoked = 0 ;

	opterr = 0 ;
	while ((option = getopt( argc, argv, "a:c:f:ghkn:ot:v" )) != -1) {
		option_invoked = 1 ;
		switch ( option ) {
			case 'a':
				user_algorithm = optarg ;
				break ;
			case 'c':
				user_hash = optarg ;
				break ;
			case 'f':
				file_with_hashes = optarg ;
				break ;
			case 'g':
				generate_hash = 1 ;
				break ;
			case 'h':
				show_usage();
				return EXIT_SUCCESS ;
				// break ;
			case 'k':
				keep_path_prefix = 1 ;
				break ;
			case 'n':
				name_to_match = optarg ;
				break ;
			case 'o':
				overwrite_existing_checksum_file = 1 ;
				break ;
			case 't':
				if (!strcmp( "gnu", optarg )) {
					chksum_file_type = CHKSUM_FILE_GNU ;
				}
				else if (!strcmp( "hashonly", optarg )) {
					chksum_file_type = CHKSUM_FILE_HASHONLY ;
				}
				else if (!strcmp( "deno", optarg )) {
					chksum_file_type = CHKSUM_FILE_DENO ;
				}
				else {
					fprintf( stderr, "%s: unknown checksum file type: %s\n", program_name, optarg );
					return EXIT_FAILURE ;
				}
				break ;
			case 'v':
				verbose = 1;
				break ;
			default:
				fprintf( stderr, "%s: Option -%c is either invalid or missing an argument.\n", program_name, optopt );
				return EXIT_FAILURE ;
				// break ;
		}
	}

	if (optind == argc) { // reached the end of the command line: no filename specified
		if (option_invoked) {
			fprintf( stderr, "%s: missing filename; type hash -h for usage information\n", program_name );
		}
		else {
			show_usage();
		}
	}
	else { // filename is specified
		files_left = argc - optind ;
		if (files_left > 1) { // multiple filenames specified
			if (!(generate_hash && (file_with_hashes != NULL))) { // not -g -f, which is the only option combo to permit multiple filenames
				fprintf( stderr, "%s: too many filenames (or you have an unquoted filename containing spaces)\n", program_name );
				return EXIT_FAILURE ;
			}
		}
		if ((user_hash == NULL) && (file_with_hashes == NULL) && (generate_hash == 0)) {
			fprintf( stderr, "%s: Either -c <checksum>, -f <checksums_file> or -g is required.\n", program_name );
			return EXIT_FAILURE ;
		}
		if ((user_hash != NULL) && (file_with_hashes != NULL)) {
			fprintf( stderr, "%s: Do not specify both -c <checksum> and -f <checksums_file>; use one or the other.\n",
				program_name );
			return EXIT_FAILURE ;
		}
		if ((user_hash != NULL) && generate_hash) {
			fprintf( stderr, "%s: contradictory options used: -c <checksum> and -g\n", program_name );
			return EXIT_FAILURE ;
		}
		if (user_hash != NULL) {
			user_hash = skip_hash_prefix( user_hash );
		}
		do {
			process_file( argv[optind], user_algorithm, user_hash );
			optind++ ;
		} while (optind < argc);
	}
	return EXIT_SUCCESS ;
}
