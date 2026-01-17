/*
	skip_hash_prefix.c
	Copyright 2026 by Christopher Heng. All rights reserved.
*/

#include "config.h"
#include <stdio.h>	// fprintf()
#include <string.h> // strlen()
#include "hash.h"

#if defined(__clang__)
// warning about unsafe pointer arithmetic when I checked for ':' and when s is adjusted past that spot
#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif

#define	GITHUB_PREFIX	"sha256:"
#define	PREFIX_LEN		(sizeof(GITHUB_PREFIX) - 1)

/*
	Github automatically generates the SHA256 hashes of files distributed
	on its releases page. When you click their copy icon, it actually
	copies to the clipboard the hash with "sha256:" prefixed to it.
	For example, the following line was pasted from one of those hashes:
	sha256:853dbc39933600db040a4e35841d8ca1e2602f66107cbf2890f680745aac84e1

	This function takes the user supplied string, checks for this
	prefix, and returns a pointer to the start of the actual hash,
	skipping past the prefix.

	At present, the function only looks for the ":" at the spot where it should
	appear in the string. Since hashes are hexadecimal numbers, any colon at that
	location means that all preceding characters (and the colon itself) do not belong
	to the hash. This gives a bit of flexibility, allowing us to detect other
	hashes besides SHA256 should such arise (provided of course they follow the
	patterm "??????:"), without my having to modify this function.
*/
char * skip_hash_prefix ( char * hash_string )
{
	size_t len ;
	char * s ;

	s = hash_string ;
	len = strlen( hash_string );
	if (len > PREFIX_LEN) {
		if (*(hash_string+PREFIX_LEN-1) == ':') {
			if (verbose) {
				fprintf( stdout, "%s: skipping past the initial prefix for the hash\n", program_name );
			}
			s = hash_string + PREFIX_LEN ;
		}
	}
	return s ;
}
