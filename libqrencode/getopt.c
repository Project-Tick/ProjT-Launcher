/**
 * getopt.c - POSIX getopt for Windows
 *
 * This is a simple implementation of getopt and getopt_long for Windows.
 * Public domain / BSD-style license.
 */

#include <stdio.h>
#include <string.h>
#include "getopt.h"

char* optarg = NULL;
int optind	 = 1;
int opterr	 = 1;
int optopt	 = '?';

static int getopt_internal(int argc,
						   char* const argv[],
						   const char* optstring,
						   const struct option* longopts,
						   int* longindex,
						   int long_only)
{
	static int sp = 1;
	int opt;
	const char* p;

	optarg = NULL;

	if (optind >= argc || argv[optind] == NULL)
	{
		return -1;
	}

	if (argv[optind][0] != '-' || argv[optind][1] == '\0')
	{
		return -1;
	}

	if (argv[optind][1] == '-' && argv[optind][2] == '\0')
	{
		optind++;
		return -1;
	}

	/* Long option */
	if (longopts != NULL && (argv[optind][1] == '-' || (long_only && argv[optind][2] != '\0')))
	{
		const char* arg = argv[optind] + (argv[optind][1] == '-' ? 2 : 1);
		const char* eq	= strchr(arg, '=');
		size_t len		= eq ? (size_t)(eq - arg) : strlen(arg);
		int i;

		for (i = 0; longopts[i].name != NULL; i++)
		{
			if (strncmp(arg, longopts[i].name, len) == 0 && longopts[i].name[len] == '\0')
			{
				if (longindex)
				{
					*longindex = i;
				}

				if (longopts[i].has_arg == required_argument || longopts[i].has_arg == optional_argument)
				{
					if (eq)
					{
						optarg = (char*)(eq + 1);
					}
					else if (longopts[i].has_arg == required_argument)
					{
						optind++;
						if (optind >= argc)
						{
							if (opterr)
							{
								fprintf(stderr, "%s: option '--%s' requires an argument\n", argv[0], longopts[i].name);
							}
							optopt = longopts[i].val;
							optind++;
							return optstring[0] == ':' ? ':' : '?';
						}
						optarg = argv[optind];
					}
				}

				optind++;

				if (longopts[i].flag != NULL)
				{
					*longopts[i].flag = longopts[i].val;
					return 0;
				}
				return longopts[i].val;
			}
		}

		if (opterr)
		{
			fprintf(stderr, "%s: unrecognized option '%s'\n", argv[0], argv[optind]);
		}
		optind++;
		return '?';
	}

	/* Short option */
	opt = argv[optind][sp];
	p	= strchr(optstring, opt);

	if (p == NULL || opt == ':')
	{
		if (opterr)
		{
			fprintf(stderr, "%s: invalid option -- '%c'\n", argv[0], opt);
		}
		optopt = opt;
		if (argv[optind][++sp] == '\0')
		{
			optind++;
			sp = 1;
		}
		return '?';
	}

	if (p[1] == ':')
	{
		if (argv[optind][sp + 1] != '\0')
		{
			optarg = &argv[optind][sp + 1];
			optind++;
			sp = 1;
		}
		else if (p[2] == ':')
		{
			/* Optional argument not provided */
			optind++;
			sp = 1;
		}
		else
		{
			optind++;
			if (optind >= argc)
			{
				if (opterr)
				{
					fprintf(stderr, "%s: option requires an argument -- '%c'\n", argv[0], opt);
				}
				optopt = opt;
				sp	   = 1;
				return optstring[0] == ':' ? ':' : '?';
			}
			optarg = argv[optind];
			optind++;
			sp = 1;
		}
	}
	else
	{
		if (argv[optind][++sp] == '\0')
		{
			optind++;
			sp = 1;
		}
	}

	return opt;
}

int getopt(int argc, char* const argv[], const char* optstring)
{
	return getopt_internal(argc, argv, optstring, NULL, NULL, 0);
}

int getopt_long(int argc, char* const argv[], const char* optstring, const struct option* longopts, int* longindex)
{
	return getopt_internal(argc, argv, optstring, longopts, longindex, 0);
}
