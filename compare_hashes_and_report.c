/*
	compare_hashes_and_report.c
	Copyright 2021-2026 by Christopher Heng. All rights reserved.
*/

#include "config.h"
#include <stdio.h> // fprintf()
#include <stdlib.h> // free()
#include <string.h> // strlen()
#include "hash.h"

enum extra_info_enum { EXTRA_INFO_NONE, EXTRA_INFO_IS_HASH, EXTRA_INFO_IS_ERROR };

void compare_hashes_and_report ( char * filename, char * user_hash, FILE * input_from_certutil_fp )
{
	char * hash_name_for_display ;
	char * name_for_actual_hash ;
	enum extra_info_enum extra_info_available ;
	int matched ;

	matched = hashes_match( user_hash, input_from_certutil_fp );
	if (generate_hash) {
		// we ignore int matched here, since it will always be 0 (no match) when generating a hash
		if (file_with_hashes != NULL) {
			create_checksum_file( filename );
		}
		else { // just print the hash
			fprintf( stdout, "%s\n", actual_hash );
		}
	}
	else {
		extra_info_available = EXTRA_INFO_NONE ;
		if (matched) {
			fprintf( stdout, "%s: hashes match\n", program_name );
				// this is all the user wants to know; don't need to distract with irrelevant info like hash name, etc.
		}
		else {
			// hashes don't match. Need to give a fuller report so that the user knows whether it's a user error (eg
			// specifying wrong hash name, or invalid filename) or it's really a hash mismatch.

			// find out if actual_hash contains an error message or a hash (may be wrong, if error message is the
			// same length as one of the hashes. If it is an error message, name_for_actual_hash will be NULL.
			if (actual_hash != NULL) {
				name_for_actual_hash = get_hash_algorithm_from_len( strlen( actual_hash ) ) ;
			}
			else {
				name_for_actual_hash = NULL ;
			}

			if (name_for_actual_hash == NULL) {
				// if actual_hash is not a valid hash (eg ENOMEM when strdup(), or error message),
				// try to get the hash name from user_hash
				hash_name_for_display = get_hash_algorithm_from_len( strlen( user_hash ) );
			}
			else {
				hash_name_for_display = name_for_actual_hash ;
			}

			/*
				If actual_hash is a valid hash, it means we have a hash mismatch. So we want to print the actual hash.
				If actual_hash is not a valid hash, it means it's an error message, and we want to print that (though
				with a different preamble from us).
			*/
			if ((hash_name_for_display != NULL) && (name_for_actual_hash != NULL)) { // hash mismatch
				fprintf( stdout, "%s: %s hashes do not match.\n", program_name, hash_name_for_display );
				if (actual_hash != NULL) {
					extra_info_available = EXTRA_INFO_IS_HASH ;
				}
			}
			else {
				/*
					We reach here if
					(1) we can't find a valid name for the hash (impossible if it's a simple hash mismatch error,
					since at the very least, name_for_actual_hash should have a valid name even if the user's string
					has the wrong length);
					or
					(2) actual_hash contains an error message.
					That is,
						if ((hash_name_for_display == NULL) [implying also that name_for_actual_hash == NULL]
						|| (name_for_actual_hash == NULL)
				*/
				fprintf( stdout, "%s: CertUtil returned an error", program_name );
				if (!verbose) {
					fputs( "; use -v with the same command line for the full report.\n", stdout );
				}
				else {
					// already verbose; don't ask user to use -v
					fputs( ".\n", stdout );
				}
				if (actual_hash != NULL) {
					extra_info_available = EXTRA_INFO_IS_ERROR ;
				}
			}

			if (actual_hash != NULL) {
				if (extra_info_available == EXTRA_INFO_IS_ERROR) {
					fprintf( stdout, "%s: message from %s\n", program_name, actual_hash );
					// The above printf format string is not a mistake. It will result in a message like:
					// "hash: message from CertUtil: The system cannot find the file specified."
				}
				else if (extra_info_available == EXTRA_INFO_IS_HASH) {
					fprintf( stdout, "%s: actual hash is %s\n", program_name, actual_hash );
				}
				free( actual_hash ); // actual_hash was obtained in hashes_match() using strdup()
				actual_hash = NULL ;
			}
		}
	}
	return ;
}
