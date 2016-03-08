#include "dict.h"

Dict *dictCreate()
{
	Dict *d = NULL;
	d = (Dict *)calloc(1, sizeof(Dict));
	d->dict_function = NULL;
	d->private_data = NULL;
	d->rehash_index = -1;
	d->iterator = 0;

	DictEntry **entry_table = NULL;
	entry_table = (DictEntry **)calloc(DICT_INIT_SIZE, sizeof(DictEntry *));
	if (entry_table == NULL)
	{
		return NULL;
	}
	d->hash_table[0].table = entry_table;
	d->hash_table[1].table = NULL;

	d->hash_table[0].size = DICT_INIT_SIZE;
	d->hash_table[0].size_mask = d->hash_table[0].size - 1;
	d->hash_table[0].used = 0;

	d->hash_table[1].size = 0;
	d->hash_table[1].size_mask = 0;
	d->hash_table[1].used = 0;

	return d;
}



Dict *dictCreateWithArgs(const DictFunction *function, const void *private_data)
{
	Dict *d = NULL;
	d = (Dict *)calloc(1, sizeof(Dict));
	d->dict_function = function;
	d->private_data = private_data;
	d->rehash_index = -1;
	d->iterator = 0;

	DictEntry **entry_table = NULL;
	entry_table = (DictEntry **)calloc(DICT_INIT_SIZE, sizeof(DictEntry *));
	d->hash_table[0].table = entry_table;
	d->hash_table[1].table = NULL;

	d->hash_table[0].size = DICT_INIT_SIZE;
	d->hash_table[0].size_mask = d->hash_table[0].size - 1;
	d->hash_table[0].used = 0;

	d->hash_table[1].size = 0;
	d->hash_table[1].size_mask = 0;
	d->hash_table[1].used = 0;

	return d;
}



Dict *dictAddEntry(Dict *d, DictEntry *entry)
{
	if (dictIsRehashing(d) == true)
	{
		_dictRehash(d);// singal rehashl;
	}
	if (entry == NULL || entry->next != NULL)
	{
		return NULL;
	}
	if (dictExistKey(d, entry->key) != NULL)
	{
		return NULL;
	}
	if (dictIsRehashing(d) == false)
	{
		if (d->hash_table[0].size <= d->hash_table[0].used)
		{
			uint32_t expand_size = dictExpandSize(d->hash_table[0].used);
			if (_dictExpandRoomOfHt(d->hash_table, expand_size) == NULL)
			{
				return NULL;
			}
			else
			{
				d->rehash_index = 0;
			}
		}
		if (_dictHtInsertEntry(d, &(d->hash_table[0]), entry) == NULL)
		{
			return NULL;
		}
	}
	else
	{
		if (_dictHtInsertEntry(d, &(d->hash_table[1]), entry) == NULL)
		{
			return NULL;
		};
	}
	return d;
}


DictEntry *dictExistKey(const Dict *d, const char *key)
{
	if (dictIsRehashing(d) == true)
	{
		_dictRehash(d);// singal rehash;
	}
	if (dictIsRehashing(d) == true)
	{
		uint32_t index = 0;
		index = _dictHashFunction(d, key) & d->hash_table[0].size_mask;
		DictEntry *entry = d->hash_table[0].table[index];
		while(entry != NULL)
		{
			if (strcmp(key, entry->key) == 0)
			{
				return entry;
			}
			entry = entry->next;
		}

		index = _dictHashFunction(d, key) & d->hash_table[1].size_mask;
		entry = d->hash_table[1].table[index];
		while(entry != NULL)
		{
			if (strcmp(key, entry->key) == 0)
			{
				return entry;
			}
			entry = entry->next;
		}
	}
	else
	{
		uint32_t index = 0;
		index = _dictHashFunction(d, key) & d->hash_table[0].size_mask;
		DictEntry *entry = d->hash_table[0].table[index];
		while(entry != NULL)
		{
			if (strcmp(key, entry->key) == 0)
			{
				return entry;
			}
			entry = entry->next;
		}
	}
	return NULL; //tmp
}



Dict *dictDelEntry(Dict *d, const char  *key)
{
	if (dictIsRehashing(d) == true)
	{
		_dictRehash(d);
	}
	if (dictExistKey(d, key) == NULL)
	{
		return NULL;
	}
	if (dictIsRehashing(d) == false)
	{
		if (_dictHtDeleteEntry(d, &d->hash_table[0], key) == NULL)
		{
			return NULL;
		}
		return d;
	}
	else
	{
		if (_dictHtDeleteEntry(d, &d->hash_table[0], key) == NULL)
		{
			if (_dictHtDeleteEntry(d, &d->hash_table[1], key) == NULL)
			{
				return NULL;
			}
			return d;
		}
	}
}



DictEntry *dictSearchEntry(const Dict *d, const char *key)
{
	return dictExistKey(d, key);
}


DictHt *_dictExpandRoomOfHt(DictHt *ht, uint32_t size)
{
	if (ht[1].table == NULL)
	{
		DictEntry **entry_table = NULL;
		entry_table = (DictEntry **)calloc(size, sizeof(DictEntry *));
		if (entry_table == NULL)
		{
			return NULL;
		}
		ht[1].table = entry_table;
		ht[1].size = size;
		ht[1].size_mask = ht[1].size - 1;
		ht[1].used = 0;
		return ht;
	}
	else
	{
		return NULL;
	}

}

DictHt *_dictHtInsertEntry(Dict *d, DictHt *hash_table, DictEntry *entry)
{
	if (hash_table == NULL)
	{
		return NULL;
	}
	uint32_t index = 0;
	index = _dictHashFunction(d, entry->key) & hash_table->size_mask;
	if (hash_table->table[index] != NULL)
	{
		entry->next = hash_table->table[index];
	}
	else
	{
		entry->next = NULL; //rehash时第一个元素迁移需要手动设置next指针，否则会出现循环链表，导致无限遍历
	}
	hash_table->table[index] = entry;
	hash_table->used ++;
	return hash_table;
}

DictHt*_dictHtDeleteEntry(Dict *d, DictHt *hash_table, const char *key)
{
	uint32_t entry_index = _dictHashFunction(d, key) & hash_table->size_mask;
	DictEntry *entry_prev = NULL;
	DictEntry *entry_current = NULL;
	entry_current = hash_table->table[entry_index];
	while(entry_current != NULL)
	{
		if (strcmp(entry_current->key, key) == 0)
		{
			if (entry_current == hash_table->table[entry_index])
			{
				hash_table->table[entry_index] = entry_current->next;
			}
			else
			{
				entry_prev->next = entry_current->next;
			}
			_dictFreeEntry(d, entry_current);
			hash_table->used--;
			return d;
		}
		else
		{
			entry_prev = entry_current;
			entry_current = entry_current->next;
		}
	}
	return  NULL;
}

DictHt *dictReductRoomOfHt(DictHt*ht, uint32_t size)
{
	if (ht->size <= size)
	{
		return NULL;
	}
	ht->table = (DictEntry **)calloc(size, sizeof(DictEntry *));
	if (ht->table == NULL)
	{
		return NULL;
	}
	return  ht;
}


void _dictRehash(Dict *d)
{
	if (d->rehash_index == -1)
	{
		return ;
	}
	while(d->rehash_index < d->hash_table[0].size)
	{
		if (d->hash_table[0].table[d->rehash_index] != NULL)
		{
			DictEntry *entry = NULL;
			entry = d->hash_table[0].table[d->rehash_index];
			do{
				DictEntry *next = entry->next;  //rehash时第一个元素迁移next指针会丢失，需要提前记录，避免链表后续元素迁移失败
				if (_dictHtInsertEntry(d, &d->hash_table[1], entry) != NULL)
				{
					d->hash_table[0].used --;
				}
				entry = next;
			}while(entry);
			d->hash_table[0].table[d->rehash_index] = NULL;
			d->rehash_index++;
			return ;
		}
		d->rehash_index++;
	}
	d->rehash_index = -1;
	free(d->hash_table[0].table);
	d->hash_table[0].table = d->hash_table[1].table;

	d->hash_table[0].size = d->hash_table[1].size;
	d->hash_table[0].size_mask = d->hash_table[0].size - 1;
	d->hash_table[0].used = d->hash_table[1].used;

	d->hash_table[1].table = NULL;
	d->hash_table[1].size = 0;
	d->hash_table[1].size_mask = 0;
	d->hash_table[1].used = 0;
}


void dictRelease(Dict *d)
{
	if (d == NULL || d->hash_table[0].table == NULL)
	{
		return ;
	}
	if (dictIsRehashing(d) == false)
	{
		_dictHtRelease(d, &d->hash_table[0]);//release hashtable[0]
	}
	else
	{
		_dictHtRelease(d, &d->hash_table[0]);
		_dictHtRelease(d, &d->hash_table[1]);//release hashtable[0] & hashtable[1];
	}
	if (d->dict_function != NULL)
	{
		free(d->dict_function);
	}
	_dictFreePrivateData(d);
	free(d);
}

void _dictHtRelease(Dict *d, DictHt*hash_table)
{
	uint32_t ht_index = 0;
	for (ht_index = 0; ht_index < hash_table->size; ht_index ++)
	{
		DictEntry *entry = hash_table->table[ht_index];
		DictEntry *next = NULL;
		while(entry != NULL)
		{
			next = entry->next;
			_dictFreeEntry(d, entry);
			entry = next;
		}
	}
	free(hash_table->table);
	hash_table->table = NULL;
}



static bool _dictTableIteratorInit(DictIterator *iterator, DictHt *table)
{
	uint32_t array_index = 0;
	for (; array_index < table->size; array_index++)
	{
		DictIterator *entry = table->table[array_index];
		if (entry != NULL)
		{
			iterator->current = entry;
			iterator->array_index = array_index;
			return true;
		}
	}
	return false;
}


DictIterator *dictCreateIterator(Dict *d)
{
	if (d == NULL || dictUsed(d) == 0)
		return NULL;
	DictIterator *iterator = NULL;
	iterator = (DictIterator *)calloc(1, sizeof(DictIterator));
	if (iterator == NULL)
	{
		return NULL;
	}
	iterator->d = d;
	iterator->current = NULL;

	if (d->hash_table[0].used != 0)
	{
		if (_dictTableIteratorInit(iterator, &(d->hash_table[0])) == false)
		{
			free(iterator);
			iterator = NULL;
			return NULL;
		}
		iterator->table_id = 0;
		return iterator;
	}
	if (d->hash_table[1].used != 0)
	{
		if (_dictTableIteratorInit(iterator, &(d->hash_table[1])) == false)
		{
			free(iterator);
			iterator = NULL;
			return NULL;
		}
		iterator->table_id = 1;
		return iterator;
	}

}


DictEntry *dictIteratorNext(DictIterator *iterator)
{
	if (iterator->current == NULL)
	{
		return NULL;
	}
	DictEntry *entry = iterator->current;
	if(iterator->current->next != NULL)
	{
		iterator->current = iterator->current->next;
		return entry;
	}
	if (iterator->table_id == 1)
	{
		uint32_t array_index = iterator->array_index + 1;
		for (; array_index < iterator->d->hash_table[1].size; array_index++)
		{
			DictEntry *entry_tmp = iterator->d->hash_table[1].table[array_index];
			if (entry_tmp != NULL)
			{
				iterator->current = entry_tmp;
				iterator->array_index = array_index;
				return entry;
			}
		}
		iterator->current = NULL;					//1表无数据
		iterator->array_index = iterator->d->hash_table[1].size;
		return entry;
	}
	uint32_t array_index = iterator->array_index + 1;
	for (; array_index < iterator->d->hash_table[0].size; array_index++)
	{
		DictEntry *entry_tmp = iterator->d->hash_table[0].table[array_index];
		if (entry_tmp != NULL)
		{
			iterator->current = entry_tmp;
			iterator->array_index = array_index;
			return entry;
		}
	}
	if (iterator->d->hash_table[1].used == 0)
	{
		iterator->current = NULL;					//0表遍历完，1表无数据
		iterator->array_index = iterator->d->hash_table[0].size;
		return entry;
	}
	else   //转换遍历的表
	{
		iterator->table_id = 1;
		uint32_t array_index_tmp = 0;
		for (; array_index_tmp < iterator->d->hash_table[1].size; array_index_tmp++)
		{
			DictEntry *entry_tmp = iterator->d->hash_table[1].table[array_index_tmp];
			if (entry_tmp != NULL)
			{
				iterator->current = entry_tmp;
				iterator->array_index = array_index_tmp;
				return entry;
			}
		}
		iterator->current = NULL;					//1表无数据
		iterator->array_index = iterator->d->hash_table[1].size;
		return entry;
	}
}

void dictDestoryIterator(DictIterator **iterator)
{
	free(*iterator);
	*iterator = NULL;
}










