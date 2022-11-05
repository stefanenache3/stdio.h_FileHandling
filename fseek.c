#include "_so_file.h"

int so_fseek(SO_FILE *stream, long offset, int whence)
{
    int tell;

    tell=lseek(stream->_fd,offset,whence);

    if(tell==-1)
    {
        //seek error
        return SO_EOF;
    }
    so_fflush(stream);
    stream->f_offset=tell;
}