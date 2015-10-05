#include <stdio.h>
#include "rehash_dict.h"
#include <time.h>


int main(void)
{
    dict *d = NULL;
    long count = 0;
    int index = 0;
    d = create_dic();

   while(count < 2000)
   {
	   time_t time_seed;
	   time(&time_seed);
	   srand((unsigned)time_seed);
	   char *key  = NULL;
	   key = (char *)calloc(16, sizeof(char));
	   strcat(key, "key");
	   char *key_index =  NULL;
	   key_index = (char *)calloc(16, sizeof(char));
	   sprintf(key_index, "%ld", rand()%10);
	   strcat(key, key_index);
	   if (d->hash_table[0].used == 10)
	   {
		   printf("stop\n");
	   }
	   if (add_dict(d, key, STRINGTYPE, key) == false)
	   {
		   printf("add dict error.key:%s\n", key);
		   sleep(1);
		   continue;
	   }
	   printf("count:%ld\tsize:%d\t\t\tkey:%s\n", count++, d->hash_table[0].used, key);
	   for (index = 0; index < d->hash_table[0].size; index ++)
	   {
		   if (d->hash_table[0].table[index] != NULL)
		   {
			   dictEntry *tm = d->hash_table[0].table[index];
			   while(tm)
			   {
				   printf("######index:%d\tkey:%s\n", index, tm->key);
				   tm = tm->next;
			   }
		   }
	   }
	   free(key);
	   free(key_index);
	   key = NULL;
	   key_index = NULL;

	   sleep(1);
    }
   printf("key97:%s\n", fetch_dictEntry(d, "key97")->value.string_value);
   sleep(10);



    return 0;
}

