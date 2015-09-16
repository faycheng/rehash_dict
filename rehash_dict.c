#include "rehash_dict.h"
/*
typedef struct dictEntry{
    void *key;
    union {
        void *value;
        long *value_int;
        double *value_double;
    } val;
    struct dictEntry *next;
}dictEntry;

typedef struct dictType{
    unsigned int (*murmur_hash)(void *key);
    void *(*key_copy)(void *private_data, void *key);
    void *(*value_copy)(void *private_data, void *obj);  //obj ???
    int (*key_compare)(void *private_data, void *key1, void *key2);
    void (*key_destory)(void *private_data, void *key);
    void (*value_destory)(void *private_data, void *obj);
}dictType;

typedef struct dictHT{
    dictEntry **table;
    unsigned long size;
    unsigned long size_mask;
    unsigned long used;
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
 * */

dict *create_dic(void)
{
    dict *d = NULL;
    int count = 0;

    d = (dict *)calloc(1, sizeof(dict));

    d->hash_table[0].table = NULL;
    d->hash_table[0].size = 0;
    d->hash_table[0].size_mask = 0;
    d->hash_table[0].used = 0;
    d->hash_table[1].table = NULL;
    d->hash_table[1].size = 0;
    d->hash_table[1].size_mask = 0;
    d->hash_table[1].used = 0;

    d->rehash_index = -1;
    d->iterators = 0;
    d->hash_table[0].table = (dictEntry **)calloc(DICT_HT_INIT_SIZE, sizeof(dictEntry *));
    for (count = 0; count < DICT_HT_INIT_SIZE; count ++)
    {
	d->hash_table[0].table[count] = NULL;
    }
    d->hash_table[0].size = DICT_HT_INIT_SIZE;
    d->hash_table[0].size_mask = DICT_HT_INIT_SIZE - 1;
    return d;
}

bool add_dict(dict *d, char *key, int type, ...)
{
    uint32_t key_index = 0;
    int count = 0;
    dictEntry *de = NULL;
    va_list value_ptr;
    char *string_value_ptr = NULL;

    va_start(value_ptr, type);
    if (d->rehash_index != -1)
    {
        single_rehash_dict(d);
    }
    if (key == NULL || strlen(key) == 0)
    {
	    printf("Enpty key.\n");
	    return false;
    }
    if (d->rehash_index == -1)
    {
	if(d->hash_table[0].size <= d->hash_table[0].used )
        {
	    d->hash_table[1].table = (dictEntry **)calloc(growth_size(d->hash_table[0].used ), sizeof(dictEntry *));
	    for (count = 0; count < growth_size(d->hash_table[0].used); count ++)
            {
		d->hash_table[1].table[count] = NULL;                           //pointer
            }
	    d->hash_table[1].size = growth_size(d->hash_table[0].used);
            d->hash_table[1].size_mask = d->hash_table[1].size - 1;
            d->hash_table[1].used = 0;
            d->rehash_index = 0;
	    switch (type) {
	    case INTTYPE:
	    {
		    add_dict(d, key, type, va_arg(value_ptr, long));
	    }
		    break;
	    case DECIMALTYPE:
	    {
		    add_dict(d, key, type, va_arg(value_ptr, double));
	    }
		    break;
	    case STRINGTYPE:
	    {
		    add_dict(d, key, type, va_arg(value_ptr, char *));
	    }
		    break;
	    case OBJECTTYPE:
	    {
		    add_dict(d, key, type, va_arg(value_ptr, void *));
	    }
		    break;
	    default:
	    {
		    printf("value type is error.\n");
		    return false;
	    }
		    break;
	    }
        }
        else
        {
            if (exist_key(d, key) == true)
            {
                printf("Exist a same key.\n");
                return false;
            }
            key_index = murmurhash(key, (uint32_t )strlen(key), MMHASH_SEED);
            key_index = key_index & d->hash_table[0].size_mask;
            de = (dictEntry *)calloc(1, sizeof(dictEntry));
	    de->key = (char *)calloc(strlen(key) + 1, sizeof(char));
	    memcpy(de->key, key, strlen(key));
	    switch (type) {
	    case INTTYPE:
	    {
		de->value.num_value = va_arg(value_ptr, long);
	    }
		break;
	    case DECIMALTYPE:
	    {
		de->value.decimal_value = va_arg(value_ptr, double);
	    }
		break;
	    case STRINGTYPE:
	    {
		    string_value_ptr = va_arg(value_ptr, char *);
		    de->value.string_value = (char *)calloc (strlen(string_value_ptr), sizeof(char));
		    memcpy(de->value.string_value, string_value_ptr, strlen(string_value_ptr));
	    }
		break;
	    case OBJECTTYPE:
	    {
		    de->value.object_value = va_arg(value_ptr, void *);
	    }
		break;
	    default:
	    {
		printf("value type is error.\n");
		return false;
	    }
		break;
	    }
	    de->next = d->hash_table[0].table[key_index];
	    d->hash_table[0].table[key_index] = de;
            d->hash_table[0].used++;
            de = NULL;
        }
    }
    else
    {
        if (exist_key(d, key) == true)
        {
            printf("Exist a same key.\n");
            return false;
        }
        key_index = murmurhash(key, (uint32_t )strlen(key), MMHASH_SEED);
        key_index = key_index & d->hash_table[1].size_mask;
        de = (dictEntry *)calloc(1, sizeof(dictEntry));
	de->key = (char *)calloc(strlen(key) + 1, sizeof(char));
	memcpy(de->key, key, strlen(key));
	switch (type) {
	case INTTYPE:
	{
	    de->value.num_value = va_arg(value_ptr, long);
	}
	    break;
	case DECIMALTYPE:
	{
	    de->value.decimal_value = va_arg(value_ptr, long double);
	}
	    break;
	case STRINGTYPE:
	{
		string_value_ptr = va_arg(value_ptr, char *);
		de->value.string_value = (char *)calloc (strlen(string_value_ptr), sizeof(char));
		memcpy(de->value.string_value, string_value_ptr, strlen(string_value_ptr));
	}
	    break;
	case OBJECTTYPE:
	{
	    de->value.object_value = va_arg(value_ptr, void *);
	    //can't delete value
	}
	    break;
	default:
	{
	    printf("value type is error.\n");
	    return false;
	}
	    break;
	}
	de->next = d->hash_table[1].table[key_index];
	d->hash_table[1].table[key_index] = de;
	d->hash_table[1].used++;
        de = NULL;
    }
    return true;
}

bool replace_dict_value(dict *d, char *key, int type, ...)
{
	dictEntry *head = NULL;
	va_list arg_ptr;
	uint32_t key_index = 0;
	char *string_value_ptr = NULL;

	if (exist_key(d, key) == false)
	{
		printf("Key is not exist.\n");
		return false;
	}
	va_start(arg_ptr, type);
	key_index = murmurhash(key, (uint32_t)strlen(key), MMHASH_SEED);
	key_index = key_index & d->hash_table[0].size_mask;
	head = d->hash_table[0].table[key_index];
	while(head)
	{
		if(strcmp(key, head->key) == 0)
		{
			switch (type) {
			case INTTYPE:
			{
				head->value.num_value =va_arg(arg_ptr, long) ;
				return true;
			}
			    break;
			case DECIMALTYPE:
			{
				head->value.decimal_value =va_arg(arg_ptr, double) ;
				return true;
			}
			    break;
			case STRINGTYPE:
			{
				string_value_ptr = va_arg(arg_ptr, char *);
				head->value.string_value = (char *)calloc (strlen(string_value_ptr), sizeof(char));
				memcpy(head->value.string_value, string_value_ptr, strlen(string_value_ptr));
				return true;
			}
			    break;
			case OBJECTTYPE:
			{
				head->value.object_value = va_arg(arg_ptr, void *);
				//can't delete value
				return true;
			}
			    break;
			default:
			{
			    printf("value type is error.\n");
			    return false;
			}
			    break;
			}
		}
		head = head->next;
	}
	if(d->rehash_index != -1)
	{
		key_index = murmurhash(key, (uint32_t)strlen(key), MMHASH_SEED);
		key_index = key_index & d->hash_table[1].size_mask;
		head = d->hash_table[1].table[key_index];
		while(head)
		{
			if(strcmp(key, head->key) == 0)
			{
				switch (type) {
				case INTTYPE:
				{
					head->value.num_value =va_arg(arg_ptr, long) ;
					return true;
				}
				    break;
				case DECIMALTYPE:
				{
					head->value.decimal_value =va_arg(arg_ptr, double) ;
					return true;
				}
				    break;
				case STRINGTYPE:
				{
					string_value_ptr = va_arg(arg_ptr, char *);
					head->value.string_value = (char *)calloc (strlen(string_value_ptr), sizeof(char));
					memcpy(head->value.string_value, string_value_ptr, strlen(string_value_ptr));
					return true;
				}
				    break;
				case OBJECTTYPE:
				{
					head->value.object_value = va_arg(arg_ptr, void *);
					//can't delete value
					return true;
				}
				    break;
				default:
				{
				    printf("value type is error.\n");
				    return false;
				}
				    break;
				}
			}
			head = head->next;
		}
	}
	return false;

}

bool fetch_dict_value(dict *d, char *key, int type, ...)
{
	dictEntry *head = NULL;
	va_list arg_ptr;
	uint32_t key_index = 0;
	long *long_value = 0;
	char *string_value = NULL;
	double *double_value = 0;
	void *object_value = NULL;

	if (exist_key(d, key) == false)
	{
		printf("Key is not exist.\n");
		return false;
	}
	va_start(arg_ptr, type);
	key_index = murmurhash(key, (uint32_t)strlen(key), MMHASH_SEED);
	key_index = key_index & d->hash_table[0].size_mask;
	head = d->hash_table[0].table[key_index];
	while(head)
	{
		if(strcmp(key, head->key) == 0)
		{
			switch (type) {
			case INTTYPE:
			{
				long_value = va_arg(arg_ptr, long *);
				*long_value = head->value.num_value;
				return true;
			}
			    break;
			case DECIMALTYPE:
			{
				double_value = va_arg(arg_ptr, double *);
				*double_value = head->value.decimal_value;
				return true;
			}
			    break;
			case STRINGTYPE:
			{
				string_value = va_arg(arg_ptr, char *);
				memcpy(string_value, head->value.string_value, strlen(head->value.string_value));
				return true;
			}
			    break;
			case OBJECTTYPE:
			{
				object_value = va_arg(arg_ptr, void *);
				memcpy(object_value, head->value.object_value, strlen(head->value.object_value));
				return true;
			}
			    break;
			default:
			{
			    printf("value type is error.\n");
			    return false;
			}
			    break;
			}
		}
		head = head->next;
	}
	if(d->rehash_index != -1)
	{
		key_index = murmurhash(key, (uint32_t)strlen(key), MMHASH_SEED);
		key_index = key_index & d->hash_table[1].size_mask;
		head = d->hash_table[1].table[key_index];
		while(head)
		{
			if(strcmp(key, head->key) == 0)
			{
				switch (type) {
				case INTTYPE:
				{
					long_value = va_arg(arg_ptr, long *);
					*long_value = head->value.num_value;
					return true;
				}
				    break;
				case DECIMALTYPE:
				{
					double_value = va_arg(arg_ptr, double *);
					*double_value = head->value.decimal_value;
					return true;
				}
				    break;
				case STRINGTYPE:
				{
					string_value = va_arg(arg_ptr, char *);
					memcpy(string_value, head->value.string_value, strlen(head->value.string_value));
					return true;
				}
				    break;
				case OBJECTTYPE:
				{
					object_value = va_arg(arg_ptr, void *);
					object_value = head->value.object_value;
					return true;       //can't delete d
				}
				    break;
				default:
				{
				    printf("value type is error.\n");
				    return false;
				}
				    break;
				}
			}
			head = head->next;
		}
	}
	return false;
}

int growth_size(int used)
{
    int size = 0;
    int base_number = 1;

    while(used * 2 > base_number )
    {
        base_number = base_number *2;
    }
    return base_number;
}

bool delete_dict_key(dict *d, char *key, int type)
{
	dictEntry *head = NULL;
	dictEntry *pre = NULL;
	uint32_t key_index = 0;

	if (exist_key(d, key) == false)
	{
		printf("Key is not exist.\n");
		return false;
	}
	key_index = murmurhash(key, (uint32_t)strlen(key), MMHASH_SEED);
	key_index = key_index & d->hash_table[0].size_mask;
	pre = d->hash_table[0].table[key_index];
	head = pre->next;
	if (head == NULL && strcmp(key, pre->key) == 0)
	{
		switch (type) {
		case INTTYPE:
		{
			free(pre->key);
			pre->key = NULL;
			free(pre);
			pre = NULL;
			return true;
		}
		    break;
		case DECIMALTYPE:
		{
			free(pre->key);
			pre->key = NULL;
			free(pre);
			pre = NULL;
			return true;
		}
		    break;
		case STRINGTYPE:
		{
			free(pre->value.string_value);
			pre->value.string_value = NULL;
			free(pre->key);
			pre->key = NULL;
			free(pre);
			pre = NULL;
			return true;
		}
		    break;
		case OBJECTTYPE:
		{
			free(pre->value.object_value);
			pre->value.object_value = NULL;
			free(pre->key);
			pre->key = NULL;
			free(pre);
			pre = NULL;
			return true;
		}
		    break;
		default:
		{
		    printf("value type is error.\n");
		    return false;
		}
		    break;
		}
		return true;
	}
	else if (head == NULL && strcmp(key, pre->key) != 0)
	{
		printf("Key is not exist.\n");
		return false;
	}
	while(head)
	{
		if(strcmp(key, head->key) == 0)
		{
			switch (type) {
			case INTTYPE:
			{
				pre->next = head->next;
				free(head->key);
				head->key = NULL;
				free(head);
				head = NULL;
				return true;
			}
			    break;
			case DECIMALTYPE:
			{
				pre->next = head->next;
				free(head->key);
				head->key = NULL;
				free(head);
				head = NULL;
				return true;
			}
			    break;
			case STRINGTYPE:
			{
				pre->next = head->next;
				free(head->value.string_value);
				head->value.string_value = NULL;
				free(head->key);
				head->key = NULL;
				free(head);
				head = NULL;
				return true;
			}
			    break;
			case OBJECTTYPE:
			{
				pre->next = head->next;
				free(head->value.object_value);
				head->value.object_value = NULL;
				free(head->key);
				head->key = NULL;
				free(head);
				head = NULL;
				return true;
			}
			    break;
			default:
			{
			    printf("value type is error.\n");
			    return false;
			}
			    break;
			}
		}
		head = head->next;
	}
	if(d->rehash_index != -1)
	{
		key_index = murmurhash(key, (uint32_t)strlen(key), MMHASH_SEED);
		key_index = key_index & d->hash_table[1].size_mask;
		pre = d->hash_table[1].table[key_index];
		head = pre->next;
		if (head == NULL && strcmp(key, pre->key) == 0)
		{
			switch (type) {
			case INTTYPE:
			{
				free(pre->key);
				pre->key = NULL;
				free(pre);
				pre = NULL;
				return true;
			}
			    break;
			case DECIMALTYPE:
			{
				free(pre->key);
				pre->key = NULL;
				free(pre);
				pre = NULL;
				return true;
			}
			    break;
			case STRINGTYPE:
			{
				free(pre->key);
				pre->key = NULL;
				free(pre->value.string_value);
				pre->value.string_value = NULL;
				free(pre);
				pre = NULL;
				return true;
			}
			    break;
			case OBJECTTYPE:
			{
				free(pre->value.object_value);
				pre->value.object_value = NULL;
				free(pre->key);
				pre->key = NULL;
				free(pre);
				pre = NULL;
				return true;
			}
			    break;
			default:
			{
			    printf("value type is error.\n");
			    return false;
			}
			    break;
			}

			return true;
		}
		else if (head == NULL && strcmp(key, pre->key) != 0)
		{
			printf("Key is not exist.\n");
			return false;
		}
		while(head)
		{
			if(strcmp(key, head->key) == 0)
			{
				switch (type) {
				case INTTYPE:
				{
					pre->next = head->next;
					free(head->key);
					head->key = NULL;
					free(head);
					head = NULL;
					return true;
				}
				    break;
				case DECIMALTYPE:
				{
					pre->next = head->next;
					free(head->key);
					head->key = NULL;
					free(head);
					head = NULL;
					return true;
				}
				    break;
				case STRINGTYPE:
				{
					pre->next = head->next;
					free(head->value.string_value);
					head->value.string_value = NULL;
					free(head->key);
					head->key = NULL;
					free(head);
					head = NULL;
					return true;
				}
				    break;
				case OBJECTTYPE:
				{
					pre->next = head->next;
					free(head->value.object_value);
					head->value.object_value = NULL;
					free(head->key);
					head->key = NULL;
					free(head);
					head = NULL;
					return true;
				}
				    break;
				default:
				{
				    printf("value type is error.\n");
				    return false;
				}
				    break;
				}
			}
			head = head->next;
		}
	}
	return false;
}

bool release_dict(dict *d)
{
	int count = 0;
	dictEntry *head = NULL;
	dictEntry *tail = NULL;
	dictEntry *pre = NULL;

	for (count = 0; count < d->hash_table[0].size; count ++)
	{
		head = d->hash_table[0].table[count];
		while (head)
		{
			tail = head;
			while(tail->next)
			{
				if (tail->next->next == NULL)
				{
					pre = tail;
				}
				tail = tail->next;
			}
			if (head->next == NULL)
			{
				free(d->hash_table[0].table[count] );
				d->hash_table[0].table[count] = NULL;
				break;
			}
			pre->next = NULL;
			free(tail->key);
			tail->key = NULL;
			free(tail->value.string_value);
			tail->value.string_value = NULL;
			free(tail);
			tail = NULL;
		}
	}
	return true;
}

bool exist_key(dict *d, char *key)
{
    uint32_t key_index = 0;
    dictEntry *head = NULL;

    key_index = murmurhash(key,  (uint32_t) strlen(key), MMHASH_SEED);
    key_index = key_index & d->hash_table[0].size_mask;
    head = d->hash_table[0].table[key_index];
    while(head)
    {
        if(strcmp(key, head->key) == 0)
        {
            return true;
        }
        head = head->next;
    }
    if(d->rehash_index != -1)
    {
	head = NULL;
        key_index = murmurhash(key,  (uint32_t) strlen(key), MMHASH_SEED);
        key_index = key_index & d->hash_table[1].size_mask;
	head = d->hash_table[1].table[key_index];
        while(head)
        {
            if(strcmp(key, head->key) == 0)
            {
                return true;
            }
            head = head->next;
        }
    }
    return false;
}

bool single_rehash_dict(dict *d)
{
    dictEntry *head = NULL;
    dictEntry *tail = NULL;
    int dict_entry_index = 0;
    uint32_t key_index = 0;

    for (; dict_entry_index < d->hash_table[0].size; dict_entry_index++)
    {
	head = d->hash_table[0].table[dict_entry_index];
        if (head == NULL)
        {
            continue;
        }
        else
        {
            key_index = murmurhash(head->key, (uint32_t)strlen(head->key), MMHASH_SEED);
            key_index = d->hash_table[1].size_mask & key_index;
	    d->hash_table[1].table[key_index] = head;
            d->hash_table[1].used++;
            d->rehash_index++;
//            while(head->next)
//            {
//                tail = head->next;
//                while(tail->next)
//                {
//                    tail = tail->next;
//                }
//                free(tail);
//                tail = NULL;
//            }

            break;
        }
    }
    if(d->rehash_index == d->hash_table[0].used)
    {
        free(d->hash_table[0].table);
        d->hash_table[0].table = NULL;
        d->hash_table[0].table = d->hash_table[1].table;
        d->hash_table[0].size = d->hash_table[1].size;
        d->hash_table[0].size_mask = d->hash_table[1].size_mask;
        d->hash_table[0].used = d->hash_table[1].used;
        d->hash_table[1].table = NULL;
        d->hash_table[1].size = 0;
        d->hash_table[1].size_mask = 0;
        d->hash_table[1].used = 0;
        d->rehash_index = -1;
    }
    return true;
}
















