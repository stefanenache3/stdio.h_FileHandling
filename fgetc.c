#include "_so_file.h"

int so_fgetc(SO_FILE *stream)
{   
    int ret;
    if(stream->buff_size == 0 || stream->buff_offset==stream->buff_size)
    {
        
        ret=read(stream->_fd,stream->buff,DEFAULT_BUFF_SIZE);
        if(ret==-1)
        {
        stream->exitState=_ERROR_STATE;
        return SO_EOF;
        }
        else if(ret==0)
        return SO_EOF;
    

        stream->buff_offset=0;
        stream->buff_size=ret;
        stream->f_offset+=ret;
    }

    else 
    stream->buff_offset++;

    stream->_state=_RD;

    return (int)stream->buff[stream->buff_offset];
}