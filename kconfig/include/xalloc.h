/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef KCONFIG_XALLOC_H
#define KCONFIG_XALLOC_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static inline void *xmalloc(size_t size)
{
	void *p = malloc(size ? size : 1);
	if (!p) {
		fprintf(stderr, "kconfig: out of memory\n");
		exit(1);
	}
	return p;
}

static inline void *xrealloc(void *ptr, size_t size)
{
	void *p = realloc(ptr, size ? size : 1);
	if (!p) {
		fprintf(stderr, "kconfig: out of memory\n");
		exit(1);
	}
	return p;
}

static inline void *xcalloc(size_t nmemb, size_t size)
{
	void *p = calloc(nmemb ? nmemb : 1, size ? size : 1);
	if (!p) {
		fprintf(stderr, "kconfig: out of memory\n");
		exit(1);
	}
	return p;
}

static inline char *xstrdup(const char *s)
{
	if (!s)
		return NULL;
	return (char *)memcpy(xmalloc(strlen(s) + 1), s, strlen(s) + 1);
}

static inline char *xstrndup(const char *s, size_t n)
{
	size_t len = 0;
	char *out;

	if (!s)
		return NULL;
	while (len < n && s[len] != '\0')
		len++;
	out = (char *)xmalloc(len + 1);
	memcpy(out, s, len);
	out[len] = '\0';
	return out;
}

#endif /* KCONFIG_XALLOC_H */
