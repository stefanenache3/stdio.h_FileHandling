#include "_so_file.h"

int ReadFromFile(SO_FILE *stream)
{
    int ret;
#if defined(__linux__)

    ret = read(stream->_fd, stream->buff, DEFAULT_BUFF_SIZE);
    return ret;
#endif

#if defined(_WIN32)
    int bRead;
    ret = ReadFile(
        (HANDLE)stream->_fd,
        stream->buff,
        DEFAULT_BUFF_SIZE,
        (LPDWORD)&bRead,
        NULL);
    if (ret == 0)
    {
        // EROARE CITIRE
        return SO_EOF;
    }
    return bRead;

#endif
}


int so_fgetc(SO_FILE *stream)
{
    int ret;

    if (stream->_EOF) // am ajuns la finalul fisierului
        return SO_EOF;

    if (stream->_update && stream->_state == _WR) // verificam modul doar in cazul update, la modurile simple eroarea va fi intoarsa de functia open
    {

        if (stream->buff_size)
        {
            // eroare, trebuie sa se faca fflush inainte
            return SO_EOF;
        }
    }
    if (stream->buff_size == 0 || stream->buff_offset == stream->buff_size)
    {

        ret = ReadFromFile(stream);
        if (ret == -1)
        {
            stream->exitState = _ERROR_STATE;
            return SO_EOF;
        }
        else if (ret == 0)
        {
            stream->_EOF = 1; // nu avem eroare dar se citesc 0 octeti => am ajuns la sfarsitul fisierului
            return SO_EOF;
        }

        stream->buff_offset = 0;
        stream->buff_size = ret;
      
    }
    stream->f_offset++;
    stream->_state = _RD;
    return (int)stream->buff[stream->buff_offset++];
}
