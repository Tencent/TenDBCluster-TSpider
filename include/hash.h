/* Copyright (c) 2000, 2010, Oracle and/or its affiliates.
   Copyright (c) 2011, 2013, Monty Program Ab.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/* Dynamic hashing of record with different key-length */

#ifndef _hash_h
#define _hash_h

#include "my_sys.h"                             /* DYNAMIC_ARRAY */

/*
  This forward declaration is used from C files where the real
  definition is included before.  Since C does not allow repeated
  typedef declarations, even when identical, the definition may not be
  repeated.
*/
#ifdef	__cplusplus
extern "C" {
#endif

/*
  Overhead to store an element in hash
  Can be used to approximate memory consumption for a hash
 */
#define HASH_OVERHEAD (sizeof(char*)*2)

/* flags for hash_init */
#define HASH_UNIQUE     1       /* hash_insert fails on duplicate key */
#define HASH_THREAD_SPECIFIC 2  /* Mark allocated memory THREAD_SPECIFIC */

typedef uint32 my_hash_value_type;
typedef uchar *(*my_hash_get_key)(const uchar *,size_t*,my_bool);
typedef my_hash_value_type (*my_hash_function)(CHARSET_INFO *,
                                               const uchar *, size_t);
typedef void (*my_hash_free_key)(void *);
typedef my_bool (*my_hash_walk_action)(void *,void *);

typedef struct st_hash {
  size_t key_offset,key_length;		/* Length of key if const length */
  size_t blength;                 /* Buffmax length, 2^n */
  ulong records;                  /* Number of elements */
  uint flags;                     /* 0 | HASH_UNIQUE | HASH_THREAD_SPECIFIC */
  DYNAMIC_ARRAY array;				    /* Place for all hash_keys in this single array */
  my_hash_get_key get_key;        /* get_key function, if null, use key_offset and key_length */
  my_hash_function hash_function; /* hash_function, default use my_hash_sort() */
  void (*free)(void *);           /* if no need, leave it as NULL */
  CHARSET_INFO *charset;          /* character set */
} HASH;

/* A search iterator state */
typedef uint HASH_SEARCH_STATE;

#define my_hash_init(A,B,C,D,E,F,G,H) my_hash_init2(A,0,B,C,D,E,F,0,G,H)
my_bool my_hash_init2(HASH *hash, uint growth_size, CHARSET_INFO *charset,
                      ulong default_array_elements, size_t key_offset,
                      size_t key_length, my_hash_get_key get_key,
                      my_hash_function hash_function,
                      void (*free_element)(void*),
                      uint flags);
void my_hash_free(HASH *tree);
void my_hash_reset(HASH *hash);
uchar *my_hash_element(HASH *hash, size_t idx);
uchar *my_hash_search(const HASH *info, const uchar *key, size_t length);
uchar *my_hash_search_using_hash_value(const HASH *info,
                                       my_hash_value_type hash_value,
                                       const uchar *key, size_t length);
my_hash_value_type my_hash_sort(CHARSET_INFO *cs,
                                const uchar *key, size_t length);
#define my_calc_hash(A, B, C) my_hash_sort((A)->charset, B, C)
uchar *my_hash_first(const HASH *info, const uchar *key, size_t length,
                     HASH_SEARCH_STATE *state);
uchar *my_hash_first_from_hash_value(const HASH *info,
                                     my_hash_value_type hash_value,
                                     const uchar *key,
                                     size_t length,
                                     HASH_SEARCH_STATE *state);
uchar *my_hash_next(const HASH *info, const uchar *key, size_t length,
                    HASH_SEARCH_STATE *state);
my_bool my_hash_insert(HASH *info, const uchar *data);
my_bool my_hash_delete(HASH *hash, uchar *record);
my_bool my_hash_update(HASH *hash, uchar *record, uchar *old_key,
                       size_t old_key_length);
void my_hash_replace(HASH *hash, HASH_SEARCH_STATE *state, uchar *new_row);
my_bool my_hash_check(HASH *hash); /* Only in debug library */
my_bool my_hash_iterate(HASH *hash, my_hash_walk_action action, void *argument);

#define my_hash_clear(H) bzero((char*) (H), sizeof(*(H)))
#define my_hash_inited(H) ((H)->blength != 0)
#define my_hash_init_opt(A,B,C,D,E,F,G,H) \
          (!my_hash_inited(A) && my_hash_init(A,B,C,D,E,F,G,H))

/* means all hash_data do something */
typedef void(*my_hash_delegate_func)(uchar * hash_data, void* maybe_unused_arg);
void
my_hash_delegate(HASH * hash, my_hash_delegate_func func, void* func_args);

/* means all hash_data do something */
typedef void(*my_hash_delegate_func_2args)(void* hash_data, void* maybe_unused_arg1, void *maybe_unsed_arg2);
void
my_hash_delegate_2args(HASH * hash, my_hash_delegate_func_2args func, void* func_args1, void* func_args2);

typedef void(*my_hash_delegate_func_nargs)(void *, void*, va_list);
void 
my_hash_delegate_nargs(HASH *hash, my_hash_delegate_func_nargs func, ...);

#ifdef	__cplusplus
}
#endif
#endif
