#include "_so_file.h"

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{   
    stream->_state=_RD;
    size_t noRead=nmemb*size; 
    size_t count=0;
    int check;

    if( stream->buff_size-stream->buff_offset >= noRead)
    {
        memcpy((char*)ptr,stream->buff+stream->buff_offset,noRead);
        while(count<noRead)
        {
            so_fgetc(stream);
            count++;
        }

        return noRead;
    }

    so_fgetc(stream);  //in cazul in care s-a facut flush si buffer-ul este gol va fi umplut si putem verifica 
                       // daca buffer-ul nu se umple si dimensiunea lui este mai mica decat nr de caractere care trebuiesc citite se simplifica operatia

    count++;
    if((stream->buff_size-stream->buff_offset)<noRead-count )
    {    
        if(stream->buff_size<DEFAULT_BUFF_SIZE)
        {
        memcpy((char*)ptr,stream->buff+stream->buff_offset-count,stream->buff_size-stream->buff_offset+count);
        while(count<noRead)
        {
            so_fgetc(stream);
            count++;
        }

        return stream->buff_size;
        }
        else
        {
            ///
        }
    }
    else
    {
        memcpy((char*)ptr,stream->buff+stream->buff_offset-count,noRead);
        return noRead;
    }
}