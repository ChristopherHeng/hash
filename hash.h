/*
	hash.h
	Copyright 2020-2026 by Christopher Heng. All rights reserved.
*/

#if !defined(HASH_H_INCLUDED)
#define	HASH_H_INCLUDED

#include <stdio.h> // FILE, size_t

// supported checksum file formats
#define CHKSUM_FILE_AUTODETECT	0
#define	CHKSUM_FILE_GNU			1	// https://www.gnu.org/software/coreutils/manual/coreutils.html#cksum-output-modes
#define	CHKSUM_FILE_HASHONLY	2	// file only has a single hash, no filename
#define	CHKSUM_FILE_DENO		4	// custom format used by Deno for https://github.com/denoland/deno/releases

extern char * program_name ;
extern char * version_string ;
extern char * copyright_start_years ;

extern int chksum_file_type ;
extern int generate_hash ;
extern int keep_path_prefix ;
extern int overwrite_existing_checksum_file ;
extern int verbose ;
extern char * actual_hash ;
extern char * file_with_hashes ;
extern char * name_to_match ;

extern void compare_hashes_and_report ( char * filename, char * user_hash, FILE * input_from_certutil_fp );
extern void create_checksum_file( char * filename );
extern char * escape_filename( char * filename, int * has_backslashes );
extern int hashes_match( char * user_hash, FILE * input_from_certutil_fp );
extern char * get_hash_algorithm( char * user_algorithm, char * user_hash );
extern char * get_hash_algorithm_from_len( size_t len );
extern char * get_algorithm_and_hash_from_file( char * target_filename, char ** checksum_to_compare_with );
extern void process_file( char * filename, char * user_algorithm, char * user_hash );
extern int safely_add_three_size_t_operands( size_t * result, size_t first, size_t second, size_t third );
extern void show_usage( void );
extern char * strip_path_prefix( char * filename );

#endif
