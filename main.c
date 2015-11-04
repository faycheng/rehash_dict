#include <stdio.h>
#include "rehash_dict.h"
#include <time.h>


int main(void)
{
    dict *d = NULL;
    d = create_dic();

    if (add_dict(d, "key_true", BOOL_TRUE) == false)
    {
	    printf("add dict failed.\n");
	    exit(0);
    }
    if (add_dict(d, "key_false", BOOL_FALSE) == false)
    {
	    printf("add dict failed.\n");
	    exit(0);
    }
    if (add_dict(d, "key_int", INTTYPE, 10240) == false)
    {
	    printf("add dict failed.\n");
	    exit(0);
    }
    if (add_dict(d, "key_double", DECIMALTYPE, 1.00023) == false)
    {
	    printf("add dict failed.\n");
	    exit(0);
    }
    if (add_dict(d, "key_string", STRINGTYPE, "hello world") == false)
    {
	    printf("add dict failed.\n");
	    exit(0);
    }
    print_dict(d);
    release_dict(d);




    return 0;
}

