#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "huffman.h"

void usage(const char* progname)
{
    printf("usage: %s command [args...]\n", progname);
    printf("\n");
    printf("commands:\n");
    printf("  stat        show char table of a file\n");
    printf("  encode      encode a file.\n");
    
}

int stat_file(const char* filename)
{
    chartab_t* tab = NULL;
    size_t i = 0;
    int ch_count = 0;
    
    FILE* fp = fopen(filename, "r");
    if (NULL == fp)
    {
        fprintf(stderr, "[ERROR] Can not open file '%s'\n", filename);
        return 1;
    }
    
    tab = chartab_read_from_file(fp);
    fclose(fp);
    
    if (NULL == tab || NULL == tab->items)
    {
        fprintf(stderr, "[ERROR] Failed to allocate chartab\n");
        return 2;
    }
    
    for (i = 0; i < tab->size; i++)
    {
        chartab_item_t* item = &tab->items[i];
        if (item->count > 0)
        {
            int is_print = 1;
            char c = (char) item->chval;
            
            ch_count += 1;
            if (!isprint(c))
            {
                is_print = 0;
                c = '.';
            }
            
            printf("CHAR %s'%c'(%d) - %d\n",
                   is_print ? "c" : "i", // Is printable.
                   c,                    // Character in string.
                   item->chval,          // Character value.
                   (int) item->count     // Character count.
            );
        }
    }
    
    printf("Total %d types of characters.\n", ch_count);
    chartab_free(tab);
    
    return 0;
}

int main(int argc, const char* argv[])
{
    if (argc <= 1)
    {
        usage(argv[0]);
        return 0;
    }
    
    if (!strcmp("stat", argv[1]))
    {
        if (argc >= 3)
            return stat_file(argv[2]);
        else
            usage(argv[0]);
    }
    
    return 0;
}
