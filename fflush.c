#include "_so_file.h"

int WriteInFile(SO_FILE *stream)
{
    int bWr; // NUMARUL DE OCTETI SCRISI IN FISIER
    int ok;
#if defined(__linux__)
    bWr = write(stream->_fd, stream->buff, stream->buff_size);
    return bWr;
#endif
#if defined(_WIN32)
    ok = WriteFile(
        (HANDLE)stream->_fd,
        stream->buff,
        stream->buff_size,
        (LPDWORD)&bWr,
        NULL);
    if (ok)
        return bWr;
    else
        return SO_EOF;
#endif
}
int so_fflush(SO_FILE *stream)
{
    if (stream->_state == _DEF)
        return 0;
    if (stream->_state == _RD)
    {
        stream->buff_offset = 0;
        stream->buff_size = 0;
        stream->_state = _DEF;
        return 0;
    }

    int ret;

    ret = WriteInFile(stream);
    if (ret == -1)
    {
        stream->exitState = _ERROR_STATE;
        return SO_EOF;
    }

    stream->buff_size = 0;
    stream->buff_offset = 0;
 
    return 0;
}