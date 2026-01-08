/*
	show_usage.c
	Copyright 2020-2026 by Christopher Heng. All rights reserved.
*/

#include "config.h"
#include <stdio.h> // fprintf()
#include "hash.h"

void show_usage( void )
{
	fprintf( stdout,
			"Hash version %s - Copyright %s by Christopher Heng. All rights reserved.\n"
			"Compares the checksum provided on the command line or in a file with the actual checksum of a file.\n"
			"Usage:\n"
			"Method 1: hash -c <checksum> [-a <algorithm>] [-v] <file>\n"
			"Method 2: hash -f <chksum_file> [-a <algorithm>] [-n <name_to_use>] [-t <chksum_file_type>] [-k] [-v] <file>\n"
			"Method 3: hash -g [-a <algorithm>] [-f <chksum_file>] [-o] [-v] <file>\n"
			"Method 4: hash [-h]\n"
			"<file>\t\t\tThe name of the file to check.\n"
			"-a algorithm\t\tOne of {MD2|MD4|MD5|SHA1|SHA256|SHA384|SHA512}. You only need to specify\n"
			"\t\t\tthis for MD2 or MD4. In all other cases, hash will auto-detect it.\n"
			"-c <checksum>\t\tThe checksum value to compare against.\n"
			"-f <chksum_file>\tThe checksums file to read (without -g) or write to (with -g).\n"
			"-g\t\t\tGenerate hash (default: SHA512).\n"
			"-h\t\t\tDisplay this usage screen and exit immediately.\n"
			"-k\t\t\tKeep path prefix; ie, don't strip drive and dir from <file>.\n"
			"-n <name_to_match>\tUse the checksum for <name_to_match> instead of <file> (useful if\n"
			"\t\t\tyou have renamed the file).\n"
			"-o\t\t\tOverwrite existing checksum file instead of appending to it.\n"
			"-t <chksum_file_type>\tOne of {gnu|hashonly|deno}. Force hash to assume the specified checksum file\n"
			"\t\t\tformat. Default is to auto-detect. Use this to override if hash gets it wrong.\n"
			"-v\t\t\t(verbose) Show detailed messages, including the full output from Certutil.\n",
			version_string, copyright_start_years );
}
