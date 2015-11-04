#ifndef REHASH_DICT_H
#define REHASH_DICT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include "murmurhash.h"

#define DICT_HT_INIT_SIZE		4
#define MMHASH_SEED		1024
#define BOOL_FALSE			0
#define BOOL_TRUE			1
#define INTTYPE				2
#define DECIMALTYPE			3
#define STRINGTYPE			4
#define OBJECTTYPE			5


typedef struct dictEntry{
    char *key;
    int value_type;
    union {
	bool *bool_value;
	char *string_value;
	long num_value;
	double decimal_value;
	void *object_value;
    }value;
    struct dictEntry *next;
}dictEntry;


typedef struct dictHT{
    dictEntry **table;
    int size;
    int size_mask;
    int used;
}dictHT;

typedef struct dict {
    dictHT hash_table[2];
    int rehash_index;
    int iterators;
}dict;

typedef struct dictIterator{
    dict *d;
    int table, index, safe;
    dictEntry *entry, *next_entry;
}dictIterator;

dict *create_dic(void);
bool add_dict(dict *d, char *key, int type, ...);
bool replace_dict_value(dict *d, char *key, int type, ...);
bool fetch_dict_value(dict *d, char *key, int type, ...);
bool delete_dict_key(dict *d, char *key, int type);
bool release_dict(dict *d);
bool single_rehash_dict(dict *d);
bool exist_key(dict *d, char *key);
dictEntry *fetch_dictEntry(dict *d, char *key);
int growth_size(int used);



















#endif // REHASH_DICT_H








