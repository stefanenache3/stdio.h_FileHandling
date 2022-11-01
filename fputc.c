#include "_so_file.h"

int so_fputc(int c, SO_FILE *stream)
{
    int ok=-1;
    stream->_state=_WR;
    if(stream->buff_offset==DEFAULT_BUFF_SIZE)
        {
            ok=so_fflush(stream);
            if(ok!=0)
            {
            stream->exitState=_ERROR_STATE;
            return SO_EOF;
            }

        }

    stream->buff[stream->buff_size++]=c;
    return c;
    
        
}