#include "_so_file.h"

int so_ferror(SO_FILE *stream)
{
    return stream->exitState;
}