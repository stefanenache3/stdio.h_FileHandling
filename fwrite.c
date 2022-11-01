#include "_so_file.h"

size_t so_fwrite(const void *ptr, size_t size, size_t nmemb, SO_FILE* stream)
{
    size_t noWrite=size*nmemb;
    size_t written=0;
    int feed;

    for(int i=0;i<noWrite;i++)
    {
        feed=so_fputc(((char*)ptr)[i],stream);

        if(feed==SO_EOF)
        return written;

        written++;

    }

    return written;


}