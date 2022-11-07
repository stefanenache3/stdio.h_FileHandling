#include "_so_file.h"

size_t so_fread(void *ptr, size_t size, size_t nmemb, SO_FILE *stream)
{

    size_t noRead = nmemb * size;
    size_t count = 0;
    int check;

    if(noRead==0)
        return 0;

    check = so_fgetc(stream); // in cazul in care s-a facut flush si buffer-ul este gol va fi umplut si putem verifica
                              //  daca buffer-ul nu se umple si dimensiunea lui este mai mica decat nr de caractere care trebuiesc citite se simplifica operatia

    if (check == SO_EOF)
        return 0;
      count++;
    if (stream->buff_size - stream->buff_offset >= noRead-count) // daca in buffer avem mai multi octeti ramasi decat numarul solicitat vom
    {                                                      // copia pur si simplu numarul solicitat de octeti
        memcpy((char *)ptr, stream->buff + stream->buff_offset-count, noRead);
        while (count < noRead)
        {
            check = so_fgetc(stream);
            count++;
        }

        return noRead;
    }


  
    if ((stream->buff_size - stream->buff_offset) < noRead - count)
    {
        if (stream->buff_size < DEFAULT_BUFF_SIZE) // daca dimensiunea buffer-ului este mai mica de cat cea DEFAULT inseamna ca nu ar mai fi
        {                                          // nevoie sa faca fetch din fisier, asadar se copiaza ce a ramas in buffer (posibil sa nu acopere necesarul)
            memcpy((char *)ptr, stream->buff + stream->buff_offset - count, stream->buff_size - stream->buff_offset + count);
            while (count < noRead)
            {
                so_fgetc(stream);
                count++;
            }

            return count;
        }
        else                                                  // in cazul acesta nu putem stii cate citiri se vor face asa ca ne folosim
        {                                                     // de un vector auxiliar in care putem copia tot ce citim prin fgetc si ulterior
            char *aux = (char *)calloc(noRead, sizeof(char)); // in vectorul dat ca parametru
            aux[0] = check;
            while (1)
            {
                if (count == noRead)
                    break;
                check = so_fgetc(stream);
                if (check == SO_EOF)
                    break;
                aux[count++] = check;
                count++;
            }

            memcpy((char *)ptr, aux, count);
            free(aux);
            aux = NULL;
            return count;
        }
    }
}