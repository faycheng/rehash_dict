#ifndef DICT_H
#define DICT_H

#include <sys/types.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "murmurhash.h"

#define		DICT_INIT_SIZE		4

typedef struct DictEntry{  //key & val_void pointer...heap
	char *key;
	int value_type;
	union {
		void *val_void;
		float val_float;
		double val_double;
		uint8_t val_uint8;
		int8_t val_int8;
		uint16_t val_uint16;
		int16_t val_int16;
		uint32_t val_uint32;
		int32_t val_int32;
		uint64_t val_uint64;
		int64_t val_int64;
		bool val_bool;
	}value;
	struct DictEntry *next;
}DictEntry;

typedef struct DictFunction{
	uint32_t (*hash_function)(const char *key);
	void *(*value_dup)(DictEntry *entry, void *ptr);
	void (*value_free)(DictEntry *ptr);
	void (*private_data_free)(void *d);
}DictFunction;


typedef struct DictHt{
	DictEntry **table;
	uint32_t size;
	uint32_t size_mask;
	uint32_t used;
}DictHt;



//当词典节点特别大（超过2亿）时，迭代器和渐进哈希索引值范围是否存在溢出风险
typedef struct Dict{
	DictFunction *dict_function;
	void *private_data;	//必须是堆中数据
	DictHt hash_table[2];
	int32_t rehash_index;
	int32_t iterator;    //无用
}Dict;


typedef struct DictIterator{
	uint8_t table_id;
	uint32_t array_index;
	DictEntry *current;
	Dict *d;
}DictIterator;

static inline uint32_t hashFunction(const char *key)
{
	uint32_t mmh = 0;
	mmh = murmurhash(key, (uint32_t )strlen(key), 421);

	return mmh;
}

static inline uint32_t _dictHashFunction(Dict *d, const char *key)
{
	if (d->dict_function != NULL && d->dict_function->hash_function != NULL)
	{
		return d->dict_function->hash_function(key);
	}
	return hashFunction(key);
}

static inline void _dictFreeEntry(Dict *d, DictEntry *entry)
{
	if (entry != NULL && d->dict_function != NULL &&  d->dict_function->value_free != NULL)
	{
		d->dict_function->value_free(entry);  //
	}
	if (entry != NULL)
	{
		free(entry->key);
		free(entry);
	}

	entry = NULL;
}

static inline void _dictFreePrivateData(Dict *d)
{
	if (d->private_data != NULL && d->dict_function != NULL &&  d->dict_function->private_data_free != NULL)
	{
		d->dict_function->private_data_free(d);
	}
	else if (d->private_data != NULL)
	{
		free(d->private_data);
		d->private_data = NULL;
	}
}

static inline void dictSetValueVoid(Dict *d, DictEntry *entry, void *value)
{
	if (d->dict_function->value_dup != NULL)
	{
		d->dict_function->value_dup(entry, value);
	}
	else
	{
		entry->value.val_void = value;
	}
}

static inline void dictSetValueFloat(DictEntry *entry, const float value)
{
	entry->value.val_float  = value;
}

static inline void dictSetValueDouble(DictEntry *entry, const double value)
{
	entry->value.val_double  = value;
}

static inline void dictSetValueUint8(DictEntry *entry, const uint8_t value)
{
	entry->value.val_uint8  = value;
}

static inline void dictSetValueInt8(DictEntry *entry, const int8_t value)
{
	entry->value.val_int8  = value;
}

static inline void dictSetValueUint16(DictEntry *entry,  const uint16_t value)
{
	entry->value.val_uint16  = value;
}

static inline void dictSetValueInt16 (DictEntry *entry, const int16_t value)
{
	entry->value.val_int16  = value;
}

static inline void dictSetValueUint32(DictEntry *entry, const uint32_t value)
{
	entry->value.val_uint32  = value;
}
static inline void dictSetValueInt32(DictEntry *entry, const int32_t value)
{
	entry->value.val_int32  = value;
}

static inline void dictSetValueUint64(DictEntry *entry, const uint64_t value)
{
	entry->value.val_uint64  = value;
}

static inline void dictSetValueInt64(DictEntry *entry, const int64_t value)
{
	entry->value.val_int64  = value;
}

static inline bool dictIsRehashing(Dict *d)
{
	if (d->rehash_index == -1)
	{
		return false;
	}
	return true;
}

static inline bool dictIsReduce(Dict *d)
{
	if (((float)d->hash_table[0].used / (float)d->hash_table[0].size) <= 0.1 && dictIsRehashing(d) == false)
	{
		return true;
	}
	return false;
}

static inline uint32_t dictSize(Dict *d)
{
	return d->hash_table[0].size + d->hash_table[1].size;
}

static inline uint32_t dictUsed(Dict *d)
{
	return d->hash_table[0].used + d->hash_table[1].used;
}

static inline uint32_t dictExpandSize(uint32_t used)
{
	uint32_t newsize = 1;
	uint32_t double_used = used * 2;
	while (newsize < double_used)
	{
		newsize *= 2;
	}
	return newsize;
}


Dict *dictCreate();
Dict *dictCreateWithArgs(const DictFunction *function, const void *private_data);
DictEntry *dictExistKey(const Dict *d, const char *key);
Dict *dictAddEntry(Dict *d, DictEntry *entry);
DictHt *_dictHtInsertEntry(Dict *d, DictHt *hash_table, DictEntry *entry);
DictHt *_dictHtDeleteEntry(Dict *d, DictHt *hash_table, const char *key);
Dict *dictDelEntry(Dict *d, const char *key);
DictEntry *dictSearchEntry(const Dict *d, const char *key);
DictHt *_dictExpandRoomOfHt(DictHt *ht, uint32_t size);
DictHt *dictReductRoomOfHt(DictHt *ht, uint32_t size);
void _dictRehash(Dict *d);
void dictRelease(Dict *d);
void _dictHtRelease(Dict *d, DictHt *hash_table);




DictIterator *dictCreateIterator(Dict *d);
DictEntry *dictIteratorNext(DictIterator *iterator);
void dictDestoryIterator(DictIterator **iterator);




//dict *dictCreate(dictType *type, void *privDataPtr);
//int dictExpand(dict *d, unsigned long size);
//int dictAdd(dict *d, void *key, void *val);
//dictEntry *dictAddRaw(dict *d, void *key);
//int dictReplace(dict *d, void *key, void *val);
//dictEntry *dictReplaceRaw(dict *d, void *key);
//int dictDelete(dict *d, const void *key);
//int dictDeleteNoFree(dict *d, const void *key);
//void dictRelease(dict *d);
//dictEntry * dictFind(dict *d, const void *key);
//void *dictFetchValue(dict *d, const void *key);
//int dictResize(dict *d);
//dictIterator *dictGetIterator(dict *d);
//dictIterator *dictGetSafeIterator(dict *d);
//dictEntry *dictNext(dictIterator *iter);
//void dictReleaseIterator(dictIterator *iter);
//dictEntry *dictGetRandomKey(dict *d);
//int dictGetRandomKeys(dict *d, dictEntry **des, int count);
//void dictPrintStats(dict *d);
//unsigned int dictGenHashFunction(const void *key, int len);
//unsigned int dictGenCaseHashFunction(const unsigned char *buf, int len);
//void dictEmpty(dict *d, void(callback)(void*));
//void dictEnableResize(void);
//void dictDisableResize(void);
//int dictRehash(dict *d, int n);
//int dictRehashMilliseconds(dict *d, int ms);
//void dictSetHashFunctionSeed(unsigned int initval);
//unsigned int dictGetHashFunctionSeed(void);
//unsigned long dictScan(dict *d, unsigned long v, dictScanFunction *fn, void *privdata);













#endif // DICT_H

