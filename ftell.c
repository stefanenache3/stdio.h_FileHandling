#include "_so_file.h"

long so_ftell(SO_FILE *stream)
{
    return stream->f_offset;
}