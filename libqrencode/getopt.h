/**
 * getopt.h - POSIX getopt for Windows
 *
 * This is a simple implementation of getopt and getopt_long for Windows.
 * Public domain / BSD-style license.
 */

#ifndef GETOPT_H
#define GETOPT_H

#ifdef __cplusplus
extern "C"
{
#endif

	extern char* optarg;
	extern int optind, opterr, optopt;

#define no_argument		  0
#define required_argument 1
#define optional_argument 2

	struct option
	{
		const char* name;
		int has_arg;
		int* flag;
		int val;
	};

	int getopt(int argc, char* const argv[], const char* optstring);
	int getopt_long(int argc, char* const argv[], const char* optstring, const struct option* longopts, int* longindex);

#ifdef __cplusplus
}
#endif

#endif /* GETOPT_H */
