#include <stdio.h>
#include "rehash_dict.h"
#include <time.h>

int main(void)
{
    dict *d = NULL;
    char *s1 = "key1";
    char *s2 = "key2";
    char *s3 = "key3";
    char *s4 = "key4";
    char *s5 = "key5";
    char *s6 = "key3";
    char *s7 = "key6";
    char *value = NULL;
    long count = 0;

   while (1)
   {
	    value = (char *)calloc(100, sizeof(char));
	    d = create_dic();
//	    add_dict(d, s1, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");
//	    add_dict(d, s2, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");

//	    add_dict(d, s4, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");
//	    add_dict(d, s5, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");
//	    add_dict(d, s6, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");
//	    add_dict(d, s7, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");
//	    add_dict(d, s4, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");
//	    add_dict(d, s5, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");
//	    add_dict(d, s6, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");
//	    add_dict(d, s4, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");
//	    add_dict(d, s5, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");
//	    add_dict(d, s6, 2, "12345678901234567890123456789012345678901234567890123456789012345678901234567890");
	    add_dict(d, s3, 1, 1.23456);
//	    if (fetch_dict_value(d, "key5", STRINGTYPE, value) == true)
//	    {
//		    printf("value:%s\n", value);
//	    }
	    free(value);
	    value = NULL;
	    printf("dic value:%lf\n", fetch_dictEntry(d, "key3")->value.decimal_value);

//	    if (replace_dict_value(d, "key5", STRINGTYPE, "hello redis dict") == true)
//	    {
//		    printf("replace value success.\n");
//	    }
//	    if(delete_dict_key(d, "key", STRINGTYPE) == true)
//	    {
//		    printf("delete key success.\n");
//	    }
//	    else
//	    {
//		    printf("delete key failed.\n");
//	    }
	    if(release_dict(d) == true)
	    {
		    printf("release dict  success.\n");
	    }
	    else
	    {
		    printf("release dict  failed.\n");
	    }
	    printf("count:%ld\n", count++);
	    break;
	    sleep(10000);
	    fflush(stdout);
    }



    return 0;
}

