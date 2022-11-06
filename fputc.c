#include "_so_file.h"

int so_fputc(int c, SO_FILE *stream)
{
    int ok = -1;
    if (stream->mode == _m_r)
    {
        // EROARE MOD RDONLY
        return SO_EOF;
    }

    if (stream->_update == 1 && stream->_state == _RD)
    {
        // seek first
        return SO_EOF;
    }

    if (stream->buff_offset == DEFAULT_BUFF_SIZE)
    {
        ok = so_fflush(stream);
        if (ok != 0)
        {
            stream->exitState = _ERROR_STATE;
            return SO_EOF;
        }
    }
    stream->f_offset++;
    stream->buff[stream->buff_size++] = c;
    stream->_state = _WR;
    return c;
}