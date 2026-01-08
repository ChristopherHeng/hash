/*
	getopt.h
	Copyright 1996-2023 by Christopher Heng. All rights reserved.

	Only use this implementation of getopt() as a last resort.
	It lacks the features found in the modern getopt()s on Linux and *BSDs.
	Most notably, it expects all options to precede non-option arguments on the
	command line. See getopt.c for the complete list.
*/

#if !defined(GETOPT_H_INCLUDED)
#define GETOPT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

/* global variables for getopt() */
extern char * optarg;	/* argument if option has one */
extern int opterr;		/* 0 = don't print err msg, 1 = print */
extern int optind;		/* next argument to process */
extern int optopt ;		/* current option */
extern int optreset ;	/* set by user to 1 if you want to reset getopt() */

/* function declarations */
extern int getopt( int argc, char * const * argv, const char * optlist );

#ifdef __cplusplus
}
#endif

#endif /* GETOPT_H_INCLUDED */
