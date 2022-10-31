#include "_so_file.h"

int so_fflush(SO_FILE *stream)
{
    if(stream->_state=_DEF)
        return 0;
    if(stream->buff==_RD)
        {
            stream->buff_offset=0;
            stream->buff_size=0;
            return 0;
        }

    int ret;

    ret=write(stream->_fd,stream->buff,stream->buff_size);
    if(ret==-1)
    {
        stream->exitState=_ERROR_STATE;
        return SO_EOF;
    }

    stream->buff_size=0;
    stream->buff_offset=0;
    
    return 0;
    
}