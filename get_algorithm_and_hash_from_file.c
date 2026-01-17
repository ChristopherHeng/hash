/*
	get_algorithm_and_hash_from_file.c
	Copyright 2023-2026 by Christopher Heng. All rights reserved.
*/

#include "config.h"
#include <stdio.h> // fprintf(), fopen(), getc(), EOF, fclose(), fread(), fflush()
#include <stdlib.h>	// exit(), EXIT_FAILURE
#include <string.h> // strlen(), stricmp() or strcasecmp()
#include <windows.h> // MAX_PATH
#include "hash.h"

#if defined(__clang__)
// disable warning about my use of checksum[i] and filename[i] (unsafe pointer arithmetic)
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

#define	MAX_CHECKSUM_LENGTH	128		// SHA512 length
#define	CHECKSUM_BUFFER_LENGTH	(MAX_CHECKSUM_LENGTH + 4) // +1 for null byte, +3 extra, just in case
#define	DENO_CHKSUM_START_STRING	"\nAlgorithm : "	// string to match to see if it's a Deno checksum file
#define	DENO_CHKSUM_START_TILL_HASH	"\nAlgorithm : SHA256\nHash      : " // this is just to compute the length to skip

static char checksum[CHECKSUM_BUFFER_LENGTH]; // notice that this is a static buffer that is overwritten with each call

/*
	This function has various limitations:
	- it returns a pointer to a static string (the hash algorithm)
	- the checksum is saved in static storage that is overwritten with every call
	- filenames in the checksum file can be at most MAX_PATH length
	- it cannot skip improperly formatted lines and go onto the next line

	If the function returns NULL, *checksum_to_compare_with points to NULL, and an error message will have
	already been printed.
*/
char * get_algorithm_and_hash_from_file( char * target_filename, char ** checksum_to_compare_with )
{
	FILE * fp ;
	char * algorithm ;
	char filename[MAX_PATH + 4]; // +1 for null byte, +3 extra, just in case
	int c ;
	int i ;
	int found ;
	char temp_buf[ sizeof(DENO_CHKSUM_START_TILL_HASH) + 2 ];
	int actual_file_format ;
	int at_first_non_blank_line ;
	int line_with_backslashes ;
	char * user_choice_string ;
	int error_printed ;

	do { // outer dummy do loop

		found = 0 ;
		algorithm = NULL ;
		*checksum_to_compare_with = NULL ;
		actual_file_format = -1 ; // invalid value, for when we fail before even getting the hash
		error_printed = 0 ;

		if ((fp = fopen( file_with_hashes, "rt" )) == NULL) {
			fprintf( stderr, "%s: unable to open %s\n", program_name, file_with_hashes );
			error_printed = 1 ;
			break ;
		}

		if (chksum_file_type == CHKSUM_FILE_AUTODETECT) {
			do {
				memset( temp_buf, 0, sizeof(temp_buf) );

				_Static_assert( (sizeof(DENO_CHKSUM_START_STRING) < sizeof(temp_buf)), "temp_buf is too small" );
				if (fread( temp_buf, 1, (sizeof(DENO_CHKSUM_START_STRING) - 1), fp ) < (sizeof(DENO_CHKSUM_START_STRING) - 1)) {
					// did not even get enough chars to form the Deno start signature
					actual_file_format = CHKSUM_FILE_GNU ;	// mark it as the standard checksum file
															// and let the main part of this function handle the fail
					if (verbose) {
						fprintf( stdout, "%s: file is too small to be from the Deno repository;\n"
							"treating it as one created by GNU coreutils (or compatible) or a hash-only file\n", program_name );
					}
					break ;
				}
				if (!strcasecmp( temp_buf, DENO_CHKSUM_START_STRING )) {
					// this is in the format found on the Deno release repository
					actual_file_format = CHKSUM_FILE_DENO ;
					if (verbose) {
						fprintf( stdout, "%s: treating checksum file as one from the Deno repository\n", program_name );
					}
				}
				else {
					actual_file_format = CHKSUM_FILE_GNU ;
					if (verbose) {
						fprintf( stdout, "%s: checksum file is either one created by GNU coreutils (or compatible) or a hash-only file\n", program_name );
					}
				}
			} while (0);
			rewind( fp ); // pretend our sneak peak ahead never happened (simplifies things)

		}
		else {
			// no need to detect the file format; the user has specified it on the command line
			actual_file_format = chksum_file_type ;
			if (verbose) {
				switch( actual_file_format ) {
					case CHKSUM_FILE_GNU:
						user_choice_string = "one created by GNU coreutils or compatible" ;
						break ;
					case CHKSUM_FILE_HASHONLY:
						user_choice_string = "containing only the checksum" ;
						break ;
					case CHKSUM_FILE_DENO:
						user_choice_string = "one from the Deno repository" ;
						break ;
					default:
						fflush( stdout ); // make sure all verbose messages preceding this get printed first
						fprintf( stderr, "%s: internal error: unknown chksum_file_type value: %d\n", program_name, actual_file_format );
						exit( EXIT_FAILURE );
						// break ;
				}
				fprintf( stdout, "%s: forced by user to treat checksum file as %s\n", program_name, user_choice_string );

			}
		}

		if (actual_file_format == CHKSUM_FILE_DENO) {

			memset( temp_buf, 0, sizeof(temp_buf) );

			// skip till the start of the hash
			_Static_assert( (sizeof(DENO_CHKSUM_START_TILL_HASH) < sizeof(temp_buf)), "temp_buf is too small" );
			if (fread( temp_buf, 1, (sizeof(DENO_CHKSUM_START_TILL_HASH) - 1), fp ) < (sizeof(DENO_CHKSUM_START_TILL_HASH) - 1)) {
				fflush( stdout ); // make sure all verbose messages preceding this get printed first
				fprintf( stderr, "%s: unable to read from %s\n", program_name, file_with_hashes );
				error_printed = 1 ;
				break ;
			}
			// we're using temp_buf as a throwaway buffer so that we can skip content we're not interested in

		}

		at_first_non_blank_line = 1 ; // at first line
		checksum[0] = '\0' ;
		do {
			// inner read loop: read and process each line
			// first get the checksum
			c = 0 ;
			algorithm = NULL ;
			line_with_backslashes = 0 ;
			for (i = 0; i <= MAX_CHECKSUM_LENGTH; i++) {
				c = getc( fp );
				if ((i == 0) && (c == '\\') && (actual_file_format == CHKSUM_FILE_GNU)) {
					// first character on the line is a backslash; this line has escaped characters
					line_with_backslashes = 1 ;
					c = getc( fp ); // skip over char (don't count it), and get next char
				}
				if ((i == 0) && ((c == '\n') || (c == '\0'))) { // skip blank lines
					i = -1 ; // so that we restart the count at 0 at the "for" statement
					continue ;
				}
				if ((c == EOF) || (c == ' ') || (c == '\n') || (c == '\0')) {
					break ;
				}
				checksum[i] = (char) c ;
			}
			checksum[i] = '\0' ;
			if ((i == 0) && (c == EOF)) {
				// we reached EOF at the beginning of a line (and therefore no checksum found on this line)
				break ;
			}
			algorithm = get_hash_algorithm_from_len( (size_t) i ) ; // get_hash_algorithm_from_len() can handle i having incorrect values
			if (algorithm == NULL) {
				// we've got a checksum, but it doesn't match any algorithm we know
				fflush( stdout ); // make sure all verbose messages preceding this get printed first
				fprintf( stderr, "%s: checksum file was improperly formatted or unknown type of hash\n", program_name );
				error_printed = 1 ;
				break ;
			}
			if ((actual_file_format == CHKSUM_FILE_DENO) || (chksum_file_type == CHKSUM_FILE_HASHONLY)) {
				// for the Deno checksum files and the hash-only files, all we need is the hash; we already know the filename,
				// since each checksum file only contains the hash for one file.
				// In other words, this do loop is only executed once for such files, since there is only one line to process.
				*checksum_to_compare_with = checksum ;
				found = 1 ;
				break ;
			}

			// At this point, actual_file_format == GNU, although chksum_file_type may be CHKSUM_FILE_AUTODETECT (ie, user left it to us).
			if ((c == EOF) || (c == '\n') || (c == '\0')) {
				// there's no filename listed after the hash (else c will be ' ')
				if ((chksum_file_type == CHKSUM_FILE_AUTODETECT) && at_first_non_blank_line && !line_with_backslashes) {
					// no filename in the first line; treat as a file with only the hash
					if (verbose) {
						fprintf( stdout, "%s: no filename on the first line; assuming a hash-only checksum file\n",
							program_name );
					}
					*checksum_to_compare_with = checksum ;
					found = 1 ;
				}
				else {
					// else it's not a hash-only file; but it's still a GNU file, which means it's not properly formatted because
					// the filename is missing; in other words, we always break out of the loop
					// Note that this occurs either on the first line (if user has used -t gnu on a hashonly file); or on a line other
					// than the first (where the first line is properly formatted with a hash and filename) but the current one is missing
					// a filename; or it is a line that starts with a backslash, implying there should be a filename somewhere.
					fflush( stdout ); // make sure all verbose messages preceding this get printed first
					fprintf( stderr, "%s: missing filename in one of the lines in %s\n", program_name, file_with_hashes );
					algorithm = NULL ;
					error_printed = 1 ;
				}
				break ;
			}

			// Get and ignore next character; it will be '*' for binary, ' ' for text, officially. That said, for the Unix tools,
			// when running on Unix-type systems, there is no difference between them, and CertUtil always treats things as
			// binary, so for all intents and purposes, the character is irrelevant
			if ((c = getc( fp )) == EOF) {
				break ;
			}
			// now get the filename
			for (i = 0; i < MAX_PATH; i++) {
				c = getc( fp ) ;
				if (line_with_backslashes && c == '\\') {
					// skip first backslash; we don't care what's the next char though; we just
					// always accept whatever is next as-is. We don't handle '\' followed by a newline or
					// a carriage return (which are the only other 2 characters which are escaped by the
					// GNU utilities). I doubt those are valid filename characters on Windows file systems.
					c = getc( fp );
				}
				if (c == EOF) {
					filename[i] = '\0' ;
					break ;
				}
				if ((c == '\n') || (c == '\0')) { // since we open it with "r", the stdio lib will also convert \r\n to \n
					filename[i] = '\0' ;
					break ;
				}
				else {
					filename[i] = (char) c ;
				}
			}
			if (i == MAX_PATH) {
				fflush( stdout ); // make sure all verbose messages preceding this get printed first
				fprintf( stderr, "%s: Filename in %s is too long.\n", program_name, file_with_hashes );
				algorithm = NULL ;
				error_printed = 1 ;
				break ;
			}
			if (i == 0) { // didn't even manage to get a single char of filename
				if ((chksum_file_type == CHKSUM_FILE_AUTODETECT) && (at_first_non_blank_line)) {
					// assume this is a hash-only file with trailing spaces after the hash
					*checksum_to_compare_with = checksum ;
					found = 1 ;
					break ;
				}
				else {
					// no filename found next to the hash value
					fflush( stdout ); // make sure all verbose messages preceding this get printed first
					fprintf( stderr, "%s: %s is improperly formatted (missing filename next to checksum).\n", program_name, file_with_hashes );
					algorithm = NULL ;
					error_printed = 1 ;
					break ;
				}
			}
			if (verbose) {
				fprintf( stdout, "%s: comparing \"%s\" (from command line) with \"%s\" (from checksum file)\n",
					program_name, target_filename, filename );
			}
			if (!strcasecmp( filename, target_filename )) {
				*checksum_to_compare_with = checksum ;
				found = 1 ;
				break ;
			}
			at_first_non_blank_line = 0 ; // so that we know this is definitely not a hash-only file
		} while (c != EOF);

		if ((c == EOF) && !found) {
			algorithm = NULL ;
			if (!error_printed) {
				fflush( stdout ); // make sure all verbose messages preceding this get printed first
				fprintf( stderr, "%s: The specified file is not listed in the checksum file.\n", program_name );
			}
		}
	} while (0);

	fclose( fp );
	return algorithm ;
}
