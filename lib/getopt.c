/*
	getopt.c
	Copyright 1996-2025 by Christopher Heng. All rights reserved.

	This code is released under the terms of the GNU General Public
	License Version 3. You should have received a copy of the GNU
	General Public License along with this program; if not, write to the
	Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
	USA.

	Important:

	* You should always use the getopt() provided by your compiler if
	one is available, since those versions behave as they are supposed to.
	They have additional functionality and are undoubtedly much better
	tested. I wrote this in the 1990s so that I could use getopt() in
	my programs when compiled with MSDOS and Windows development systems,
	since those typically did not have getopt() in their C library.
	Nowadays, in 2024, probably only 2 Windows C development systems remain
	in widespread use, and one of them, MinGW-w64, has always had
	getopt(), leaving only Visual Studio without one.

	* Unlike the GNU getopt() implementation, my version expects all
	options to appear before the first non-option argument. The
	moment a non-option argument is encountered, getopt() regards
	the list of options as ended. As I said, I originally wrote
	this decades ago, when such a behaviour was the norm (and I think
	it's still the norm on *BSD systems). I did not bother to update
	it to the more end-user-friendly way of doing things.

	* To reset getopt(), set optreset to 1. You will also need to reset
	optind to 1, or whatever other number you need (even 0, since this
	implementation doesn't check whether optind is changed by the caller).

	* optlist only supports a list of option characters, or option characters
	followed by a single ':' character to denote an option with an argument.
	It does not support a leading ':' the way modern GNU and OpenBSD does.
	(In those implementations, a leading ':' causes ':' to be returned
	when an option that is supposed to have an argument is missing an argument.)
	My implementation always returns '?' on error.

	* In general, this implementation has none of the additional features provided
	by the modern implementations found in GNU and BSD based systems.

	See also the documentation preceding the function itself.
*/

#if defined(_MSC_VER)
// disable warnings about fprintf() not being inlined, new in VS 17.7 when compiled with /Wall
#pragma warning(disable: 4710)
#endif

#include <stdio.h>	/* fprintf() */
#include <string.h>	/* strchr() */
#include "getopt.h"

/* option switch character */
#define OPT_SW		'-'

/* return values for errors */
#define	OPT_BADOPT		((int)'?')

/* global variables */
char * optarg ;		/* argument to option */
int opterr = 1 ;	/* 0 = don't print error msg, 1 = print */
int optind = 1 ;	/* next argument to parse */
int optopt ;		/* the current option */
int optreset ;		/* 0 = continue processing, 1 = reset to start */

/*
	getopt

	getopt() parses the command line given in the vector list,
	and returns the option character (if any) and the option
	argument in optarg.

	argc, argv corresponds to the versions passed to main().
	optlist is the list of valid options. Options which must
	have arguments have a ':' suffixed to the option character
	in optlist. Needless to say, ':' cannot be a valid option
	character.

	This function uses various globals: optarg, opterr, optind,
	optopt, optreset to govern its actions.

	optarg		Contains the argument to the option if the option is
				specified to have an argument.
	opterr		If this is set to 1, an error message will
				be printed on stderr for bad options or missing arguments
				to options. The default is to print the error message.
	optind		Contains the next argv item which to be parsed. Defaults
				to 1 to skip over the program name in the standard
				argv[] passed to main().
	optopt		Always contains the option character which is returned on
				a valid option or which caused an error.
	optreset	Set this to 1 if you need to call getopt() after using
				it to parse a different argv list. You should also set
				optind to point to the first item in argv[] to parse.

	The option "--" will terminate the list of options. The character ':' can
	never be an option. A solitary "-" will also terminate the list of options.
	According to the getopt manual page in BSD, at the time I checked it, this
	appears to be the behaviour on System V too.

	Returns:
		-1		No more options to parse.
		'?'		An option character was encountered which was not in the list of valid options.
				or an option was supposed to have an argument but was found without one.
		Otherwise, the option character is returned.
*/
int getopt ( int argc, char * const * argv, const char * optlist )
{
	static char		nullstring[] = "" ;
	static char *	curptr = nullstring ;
	char *			s ;

	if (optreset) { /* the caller forced a reset */
		curptr = nullstring ;
		optreset = 0 ;
	}

	if (*curptr == '\0') {
		/* either start of new arg || first time we're called || caller wants us to treat this as the first time */

		/* get next (or first arg) */
		if ((optind >= argc) || /* no more args */
			(*(curptr = argv[optind]) != OPT_SW)) { /* a non-option arg */
			/* we're done here */
			curptr = nullstring ;	/* reset */
			return -1;
		}
		/* got to set curptr since we could have got here by optind < argc prior to curptr being set */
		curptr = argv[optind] ;
		/* check if caller flagged the end of options */
		if (*++curptr == '\0' ||	/* solitary '-' */
			(*curptr == OPT_SW)) {	/* "--" */
			/* we're done here */
			optind++;	/* point to next argument */
			curptr = nullstring ;	/* reset */
			return -1;
		}
	}
	/* by the time we get here, we have skipped over the option */
	/* switch character */
	optopt = *curptr ;

	if (optopt == ':' ||	/* need to catch this manually, since optlist may have ':' to flag an optarg */
		(s = strchr( optlist, optopt )) == NULL) { /* no such option */
		if (opterr) {
			fprintf( stderr, "Unknown option: -%c.\n", optopt );
		}
		/* skip erroneous option character */
		if (*++curptr == '\0') {			/* end of argument */
			optind++ ;					/* go to next */
			/* curptr is already pointing to a null string */
		}
		return OPT_BADOPT ;
	}
	if (*++s == ':') {	/* argument expected */
		/* point to next argument; always done, so we might as well do it before we test */
		optind++ ;
		/* is the argument at the end of current argument? */
		if (*++curptr != '\0') {	/* yep */
			optarg = curptr ;
			curptr = nullstring ;
			/* optind is already pointing to next argument for next round */
		}
		else if (optind < argc)	{ /* optarg is in next argument */
			optarg = argv[optind++] ;
			/*
				since we use the next arg for our optarg, we needed to
				point optind to the argument after that for next round
				curptr already pointing to null byte
			*/
		}
		else {	/* optarg is not at end of current arg nor are there any more args */
			if (opterr) {
				fprintf( stderr, "Option -%c requires an argument.\n", optopt );
			}
			return OPT_BADOPT ;
			/* optind is already pointing to next (nonexistant) argument */
			/* curptr is already pointing to null byte */
		}
	}
	else {	/* optarg is not expected */
		if (*++curptr == '\0') {	/* end of current argument */
			optind++ ;	/* skip to next for next round */
		}
		optarg = NULL;			/* just in case */
	}
	return optopt ;
}
