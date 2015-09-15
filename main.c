#include <stdio.h>
#include "rehash_dict.h"

int main(void)
{
    dict *d = NULL;
    char *ss = "helloworld";
    d = create_dic();
    add_dict(d, (void *)ss, 1024);
    return 0;
}

