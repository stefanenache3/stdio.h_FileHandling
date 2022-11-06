#include "_so_file.h"

int so_feof(SO_FILE *stream)
{
    return stream->_EOF;
}