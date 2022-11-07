#include "_so_file.h"

int so_fclose(SO_FILE *stream)
{
    if (stream == NULL)
        return -1;

    int ret;
    so_fflush(stream);

#if defined(__linux__)
    ret = close(stream->_fd);
    return ret;
#endif

#if defined(_WIN32)
    ret = CloseHandle((HANDLE)stream->_fd);
    if (ret == 0)
        return SO_EOF;
    else
        return 0;
#endif

    free(stream);
    stream = NULL;
}