#ifndef REHASH_DICT_H
#define REHASH_DICT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdint.h>
#include "dict.h"
#include "macro_definition.h"
#include "string_manipulation.h"
#include "murmurhash.h"

#define DICT_HT_INIT_SIZE		4
#define MMHASH_SEED		1024
#define BOOL_FALSE			0
#define BOOL_TRUE			1
#define INTTYPE				2
#define DECIMALTYPE			3
#define STRINGTYPE			4
#define OBJECTTYPE			5


#define dict Dict
#define dictHt DictHt
#define dictEntry DictEntry
#define num_value val_uint64
#define decimal_value val_double
#define string_value val_void

void value_free_custom(dictEntry *ptr);
void private_data_free_custom(void *d);

static inline dict *createDict(void){
	DictFunction *function =  (DictFunction *)calloc(1, sizeof(DictFunction));
	function->private_data_free = private_data_free_custom;
	function->value_free = value_free_custom;
	Dict *d = dictCreateWithArgs(function, NULL);
	return d;
}

static inline bool existKey(dict *d, char *key)
{
	if (dictExistKey(d, key) == NULL)
		return false;
	return true;
}

bool addDict(dict *d, char *key, int type, ...);


static inline bool releaseDict(dict *d)
{
	dictRelease(d);
	return true;
}


static inline dictEntry *fetchDictentry(dict *d, char *key)
{
	return dictSearchEntry(d, key);
}



















#endif // REHASH_DICT_H








