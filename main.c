#include <stdio.h>
#include "rehash_dict.h"

int main(void)
{
    dict *d = NULL;
    char *ss = "helloworld";
     char *s2 = "strtwo";
      char *s3 = "strthree";
    d = create_dic();
    add_dict(d, ss, 0, 1024);
    add_dict(d, s2, 1, 1024.124);
    add_dict(d, s3, 2, "hello dict");
    return 0;
}

