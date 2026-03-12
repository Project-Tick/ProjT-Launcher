/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef KCONFIG_HASHTABLE_H
#define KCONFIG_HASHTABLE_H

#include <stddef.h>
#include "list.h"
#include "hash.h"

#define HASH_SIZE(name) (sizeof(name) / sizeof((name)[0]))
#define HASH_BITS(name) (__builtin_ctzl(HASH_SIZE(name)))

#define DECLARE_HASHTABLE(name, bits) \
	struct hlist_head name[1U << (bits)]

#define DEFINE_HASHTABLE(name, bits) \
	struct hlist_head name[1U << (bits)] = { [0 ... ((1U << (bits)) - 1)] = { .first = NULL } }

#define HASHTABLE_DECLARE(name, size) \
	DECLARE_HASHTABLE(name, __builtin_ctzl(size))

#define HASHTABLE_DEFINE(name, size) \
	DEFINE_HASHTABLE(name, __builtin_ctzl(size))

static inline void __hash_init(struct hlist_head *ht, unsigned int bits)
{
	unsigned int i;
	for (i = 0; i < (1U << bits); i++)
		INIT_HLIST_HEAD(&ht[i]);
}

#define hash_init(hashtable) \
	__hash_init(hashtable, __builtin_ctzl(sizeof(hashtable) / sizeof(struct hlist_head)))

#define hash_add(hashtable, node, key) \
	hlist_add_head(node, &hashtable[hash_32_bits((uint32_t)(key), HASH_BITS(hashtable))])

#define hash_for_each_possible(name, obj, member, key) \
	hlist_for_each_entry(obj, &name[hash_32_bits((uint32_t)(key), HASH_BITS(name))], member)

#define hash_for_each(name, obj, member) \
	for (unsigned int __i = 0; __i < HASH_SIZE(name); __i++) \
		hlist_for_each_entry(obj, &name[__i], member)

#endif /* KCONFIG_HASHTABLE_H */
