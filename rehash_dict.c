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
    return d;
}

bool add_dict(dict *d, char *key, void *value)
{
    uint32_t key_index = 0;
    int count = 0;
    dictEntry *de = NULL;
    if (d->rehash_index != -1)
    {
        single_rehash_dict(d);
    }
    if (d->rehash_index == -1)
    {
        if(d->hash_table[0].size <= d->hash_table[0].used + 1)
        {
            d->hash_table[1].table = (dict *)calloc(growth_size(d->hash_table[1].used + 1), sizeof(dict));
            for (count = 0; count < growth_size(d->hash_table[1].used + 1); count ++)
            {
                d->hash_table[1].table[count].next = NULL;
            }
            d->hash_table[1].size = growth_size(d->hash_table[1].used + 1);
            d->hash_table[1].size_mask = d->hash_table[1].size - 1;
            d->hash_table[1].used = 0;
            d->rehash_index = 0;
            add_dict(d, key, value);
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
            de->key = (void *)key;
            de->value = value;
            de->next = d->hash_table[0].table[key_index].next;
            d->hash_table[0].table[key_index].next = de;
            d->hash_table[0].used++;
            //free(de);                         ??????????????????
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
        de->key = (void *)key;
        de->value = value;
        de->next = d->hash_table[1].table[key_index].next;
        d->hash_table[1].table[key_index].next = de;
        d->hash_table[1].used++;
        //free(de);
        de = NULL;
    }
    return true;
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

bool exist_key(dict *d, char *key)
{
    uint32_t key_index = 0;
    dictEntry *head = NULL;

    key_index = murmurhash(key,  (uint32_t) strlen(key), MMHASH_SEED);
    key_index = key_index & d->hash_table[0].size_mask;
    head = d->hash_table[0].table[key_index].next;
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
        key_index = murmurhash(key,  (uint32_t) strlen(key), MMHASH_SEED);
        key_index = key_index & d->hash_table[1].size_mask;
        head = d->hash_table[0].table[key_index].next;
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
        head = d->hash_table[0].table[dict_entry_index].next;
        if (head == NULL)
        {
            continue;
        }
        else
        {
            key_index = murmurhash(head->key, (uint32_t)strlen(head->key), MMHASH_SEED);
            key_index = d->hash_table[1].size_mask & key_index;
            d->hash_table[1].table[key_index] = *head;
            d->hash_table[1].used++;
            d->hash_table[0].used--;
            d->rehash_index++;
            while(head->next)
            {
                tail = head->next;
                while(tail->next)
                {
                    tail = tail->next;
                }
                free(tail);
                tail = NULL;
            }

            break;
        }
    }
    if(d->rehash_index == d->hash_table[0].size)
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
















