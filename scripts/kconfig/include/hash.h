/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef KCONFIG_HASH_H
#define KCONFIG_HASH_H

#include <stdint.h>
#include <stddef.h>

/* Jenkins hash helpers (user space, 32-bit) */
static inline uint32_t __hash_32(uint32_t val)
{
	uint32_t hash = val;

	hash += ~(hash << 15);
	hash ^=  (hash >> 10);
	hash +=  (hash << 3);
	hash ^=  (hash >> 6);
	hash += ~(hash << 11);
	hash ^=  (hash >> 16);

	return hash;
}

static inline uint32_t hash_ptr(const void *ptr)
{
	uintptr_t val = (uintptr_t)ptr;
	return __hash_32((uint32_t)val);
}

static inline int hash_str(const char *s)
{
	uint32_t hash = 0;
	while (*s)
		hash = (hash * 33) ^ (unsigned char)*s++;
	return (int)hash;
}

static inline uint32_t hash_32_bits(uint32_t val, unsigned int bits)
{
	return __hash_32(val) >> (32 - bits);
}

static inline uint32_t hash_32(uint32_t val)
{
	return __hash_32(val);
}

#endif /* KCONFIG_HASH_H */
