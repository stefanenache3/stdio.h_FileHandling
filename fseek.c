#include "_so_file.h"

int so_fseek(SO_FILE *stream, long offset, int whence)
{
    int tell;

#if defined(__linux__)
    tell = lseek(stream->_fd, offset, whence);
    if (tell < SEEK_END)
        stream->_EOF = 0;
#endif

#if defined(_WIN32)
    tell = SetFilePointer(
        (HANDLE)stream->_fd,
        offset,
        NULL,
        whence);
    if (tell == INVALID_SET_FILE_POINTER)
        tell = -1; // JUST TO BE SURE :]
    else if (tell < FILE_END)
        stream->_EOF = 0;

#endif
    if (tell == -1)
    {
        // seek error
        return SO_EOF;
    }
    so_fflush(stream);
    stream->f_offset = tell;
}