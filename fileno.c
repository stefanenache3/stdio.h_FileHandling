#include "_so_file.h"

#if defined(__linux)
int so_fileno(SO_FILE *stream)
{
    return stream->_fd;
}
#endif

#if defined(_WIN32)
HANDLE so_fileno(SO_FILE *stream)
{
    return (HANDLE)stream->_fd;
}
#endif