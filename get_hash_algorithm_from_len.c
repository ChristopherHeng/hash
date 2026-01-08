/*
	get_hash_algorithm_from_len.c
	Copyright 2020-2023 by Christopher Heng. All rights reserved.
*/

#include "hash.h"

/*
	Length of hashes:
	MD2 32
	MD4 32
	MD5 32
	SHA1 40
	SHA256 64
	SHA384 96
	SHA512 128

*/
char * get_hash_algorithm_from_len( size_t len )
{
	char * user_algorithm ;

	switch( len ) {
		case 32: // either MD2, MD4 or MD5, assume the most likely
			user_algorithm = "MD5" ;
			break ;
		case 40:
			user_algorithm = "SHA1" ;
			break ;
		case 64:
			user_algorithm = "SHA256" ;
			break ;
		case 96:
			user_algorithm = "SHA384" ;
			break ;
		case 128:
			user_algorithm = "SHA512" ;
			break ;
		default:
			user_algorithm = NULL ;
			break ;
	}
	return user_algorithm ;
}
