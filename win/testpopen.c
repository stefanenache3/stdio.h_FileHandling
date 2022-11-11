#include "so_stdio.h"
#include <string.h>
#include <fcntl.h>
#define _MAX_ARGS 10
#define READ_END 0
#define WRITE_END 1
#define DEFAULT_BUFF_SIZE 4096

#if defined(__linux__)
#include <sys/wait.h>
#include <unistd.h>
#endif
/*-------------------------------------*/
#define _RD 1
#define _WR 2
#define _DEF 3

#define _ERROR_STATE 1
#define _NORMAL_STATE 0
/*-------------------------------------*/

typedef struct _so_file
{
    int _fd;
    unsigned char buff[DEFAULT_BUFF_SIZE];
    int mode;
    int _state;
    int exitState;
    int _update;
    int _EOF;
    size_t f_offset;
    size_t buff_offset;
    size_t buff_size;
#if defined(_WIN32)
    PROCESS_INFORMATION _pi;
#endif
#if defined(__linux__)
    int _pid;
#endif
}_so_file;

enum Mode { _m_r, _m_r_w, _m_w, _m_w_r, _m_a, _m_a_r };
int getFileHandle(const char* filename, enum Mode m, int* update)
{
    int fd = -1;
    *update = 0;
#if defined(__linux__)
    switch (m)
    {
    case _m_r:
        fd = open(filename, O_RDONLY);
        break;
    case _m_r_w:
        fd = open(filename, O_RDWR);
        *update = 1;
        break;
    case _m_w:
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0664);
        break;
    case _m_w_r:
        fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0664);
        *update = 1;
        break;
    case _m_a:
        fd = open(filename, O_APPEND | O_CREAT | O_WRONLY, 0664);
        break;
    case _m_a_r:
        fd = open(filename, O_APPEND | O_CREAT | O_RDWR, 0664);
        *update = 1;
        break;
    default:
        break;
    }
#endif
#if defined(_WIN32)
    switch (m)
    {
    case _m_r:
        fd = (int)CreateFile(filename,
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        break;
    case _m_r_w:
        fd = CreateFile(filename,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        *update = 1;
        break;
    case _m_w:
        fd = CreateFile(filename, GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        break;
    case _m_w_r:
        fd = CreateFile(filename,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        *update = 1;
        break;
    case _m_a:
        fd = CreateFile(filename,
            FILE_APPEND_DATA,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        break;
    case _m_a_r:
        fd = CreateFile(filename,
            GENERIC_READ | FILE_APPEND_DATA,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);
        *update = 1;
        break;
    default:
        break;
    }
#endif
    return fd;
}

SO_FILE* so_fopen(const char* pathname, const char* mode)
{
    char ch = mode[0];
    char ch2;
    int len = strlen(mode);
    int fd = -1;
    enum Mode md;
    int update;
    if (len == 2)
        ch2 = mode[1];
    else if (len > 2)
        return NULL;

    switch (ch)
    {
    case 'r':
        if (len == 1)
            md = _m_r;
        else if (ch2 == '+')
            md = _m_r_w;
        else
            return NULL;
        break;

    case 'w':
        if (len == 1)
            md = _m_w;
        else if (ch2 == '+')
            md = _m_w_r;
        else
            return NULL;
        break;
    case 'a':
        if (len == 1)
            md = _m_a;
        else if (ch2 == '+')
            md = _m_a_r;
        else
            return NULL;
        break;
    default:
        return NULL;
        break;
    }

    fd = getFileHandle(pathname, md, &update);
    if (fd == -1)
        return NULL;

    SO_FILE* handle = (SO_FILE*)malloc(sizeof(SO_FILE));
    handle->buff_offset = 0;
    handle->buff_size = 0;
    handle->f_offset = 0;
    handle->_fd = fd;
    handle->mode = md;
    handle->_state = _DEF;
    handle->exitState = _NORMAL_STATE;
    handle->_update = update;
    handle->_EOF = 0;
    return handle;
}


int ReadFromFile(SO_FILE* stream)
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
    if (ret == FALSE)
    {
        // EROARE CITIRE
        return SO_EOF;
    }
    return bRead;

#endif
}

int so_fgetc(SO_FILE* stream)
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
            return -1;           //MODIFICAT
        }

        stream->buff_offset = 0;
        stream->buff_size = ret;


    }
    stream->f_offset++;
    stream->_state = _RD;
    return (int)stream->buff[stream->buff_offset++];
}


int WriteInFile(SO_FILE* stream)
{
    int bWr; // NUMARUL DE OCTETI SCRISI IN FISIER
    int ok;
    int count = 0;
#if defined(__linux__)
    while (count < stream->buff_size)
    {
        bWr = write(stream->_fd, stream->buff + count, stream->buff_size - count);
        if (bWr == -1)
        {
            stream->exitState = _ERROR_STATE;
            return -1;
        }

        count += bWr;
    }
    return count;
#endif
#if defined(_WIN32)
    while (count < stream->buff_size)
    {
        ok = WriteFile(
            (HANDLE)stream->_fd,
            stream->buff + count,
            stream->buff_size - count,
            (LPDWORD)&bWr,
            NULL);
        if (ok == FALSE)
        {
            stream->exitState = _ERROR_STATE;
            return SO_EOF;
        }
        count += bWr;
    }

    return count;
#endif
}
int so_fflush(SO_FILE* stream)
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


int so_fputc(int c, SO_FILE* stream)
{
    int ok = -1;
    if (stream->mode == _m_r)
    {
        // EROARE MOD RDONLY
        return SO_EOF;
    }

    if (stream->_update == 1 && stream->_state == _RD)
    {
        // seek first
        return SO_EOF;
    }

    if (stream->buff_size == DEFAULT_BUFF_SIZE)
    {
        ok = so_fflush(stream);
        if (ok != 0)
        {
            stream->exitState = _ERROR_STATE;
            return SO_EOF;
        }
    }
    stream->f_offset++;
    stream->buff[stream->buff_size++] = c;
    stream->_state = _WR;
    return c;
}


size_t so_fread(void* ptr, size_t size, size_t nmemb, SO_FILE* stream)
{

    size_t noRead = nmemb * size;
    size_t count = 0;
    int check;

    if (noRead == 0)
        return 0;

    check = so_fgetc(stream); // in cazul in care s-a facut flush si buffer-ul este gol va fi umplut si putem verifica
                              //  daca buffer-ul nu se umple si dimensiunea lui este mai mica decat nr de caractere care trebuiesc citite se simplifica operatia
    if (stream->_EOF)
        return 0;
    if (check == SO_EOF)
        return 0;
    count++;
    if (stream->buff_size - stream->buff_offset >= noRead - count) // daca in buffer avem mai multi octeti ramasi decat numarul solicitat vom
    {                                                      // copia pur si simplu numarul solicitat de octeti
        memcpy((unsigned char*)ptr, stream->buff + stream->buff_offset - count, noRead);
        while (count < noRead)
        {
            check = so_fgetc(stream);
            count++;
        }

        return noRead / size;
    }



    if ((stream->buff_size - stream->buff_offset) < noRead - count)
    {
        if (stream->buff_size < DEFAULT_BUFF_SIZE) // daca dimensiunea buffer-ului este mai mica de cat cea DEFAULT inseamna ca nu ar mai fi
        {                                          // nevoie sa faca fetch din fisier, asadar se copiaza ce a ramas in buffer (posibil sa nu acopere necesarul)
            memcpy((unsigned char*)ptr, stream->buff + stream->buff_offset - count, stream->buff_size - stream->buff_offset + count);
            while (count < noRead)
            {
                so_fgetc(stream);
                count++;
            }

            return count / size;
        }
        else                                                  // in cazul acesta nu putem stii cate citiri se vor face asa ca ne folosim
        {                                                     // de un vector auxiliar in care putem copia tot ce citim prin fgetc si ulterior
            unsigned char* aux = (unsigned char*)malloc(noRead); // in vectorul dat ca parametru
            memset(aux, 0, noRead);
            aux[0] = check;
            while (1)
            {

                if (count == noRead)
                    break;
                check = so_fgetc(stream);
                if (stream->_EOF)
                    break;
                if (check == SO_EOF)
                    break;
                aux[count] = check;
                count++;
            }
            memcpy((unsigned char*)ptr, aux, count);
            free(aux);
            aux = NULL;
            return count / size;
        }
    }
}

size_t so_fwrite(const void* ptr, size_t size, size_t nmemb, SO_FILE* stream)
{
    size_t noWrite = size * nmemb;
    size_t written = 0;
    int feed;

    for (int i = 0; i < noWrite; i++)
    {
        feed = so_fputc(((unsigned char*)ptr)[i], stream);

        if (feed == SO_EOF)
            return written;

        written++;
    }

    return written / size;
}


int so_fseek(SO_FILE* stream, long offset, int whence)
{
    int tell;
    so_fflush(stream);
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
    else stream->_EOF = 1;   //modif

#endif
    if (tell == -1)
    {
        // seek error
        return SO_EOF;
    }

    stream->f_offset = tell;
    return 0;
}

long so_ftell(SO_FILE* stream)
{
    return stream->f_offset;
}

#if defined(__linux)
int so_fileno(SO_FILE* stream)
{
    return stream->_fd;
}
#endif

#if defined(_WIN32)
HANDLE so_fileno(SO_FILE* stream)
{
    return (HANDLE)stream->_fd;
}
#endif

int so_fclose(SO_FILE* stream)
{
    if (stream == NULL)
        return -1;

    int ret;
    int ok;
    ok = so_fflush(stream);

#if defined(__linux__)
    ret = close(stream->_fd);
    free(stream);
    stream = NULL;
    if (ok != 0)
        return -1;
    return ret;
#endif

#if defined(_WIN32)
    ret = CloseHandle((HANDLE)stream->_fd);
    free(stream);
    stream = NULL;
    if (ret == 0)
        return SO_EOF;
    else
        return 0;
#endif


}

int so_feof(SO_FILE* stream)
{
    return stream->_EOF;
}

int so_ferror(SO_FILE* stream)
{
    return stream->exitState;
}



SO_FILE* so_popen(const char* command, const char* type)
{
    int ret;
#if defined(__linux__)
    int status;
    if ((type[0] != 'r' && type[0] != 'w') || strlen(type) > 1)
        return NULL;

    int pipe_fd[2];

    ret = pipe(pipe_fd);
    if (ret < 0)
    {
        // error
        return NULL;
    }

    int pid = fork();
    if (pid == 0)
    {
        int fd;
        if (strcmp(type, "r") && strcmp(type, "w"))
            return NULL;

        if (type[0] == 'r')
        {
            close(pipe_fd[READ_END]);
            dup2(pipe_fd[WRITE_END], STDOUT_FILENO);
            //  close(pipe_fd[WRITE_END]);
        }
        if (type[0] == 'w')
        {
            close(pipe_fd[WRITE_END]);
            dup2(pipe_fd[READ_END], STDIN_FILENO);
            // close(pipe_fd[READ_END]);
        }

        // execlp("sh",cmd,NULL);

        execl("/bin/sh", "sh", "-c", command);
    }
    else if (pid > 0)
    {

        SO_FILE* f = (SO_FILE*)malloc(sizeof(SO_FILE));
        if (f == NULL)
            return NULL;

        /*-----------------------------------------*/
        f->buff_offset = 0;
        f->buff_size = 0;
        f->f_offset = 0;
        f->_state = _DEF;
        f->exitState = _NORMAL_STATE;
        f->_update = 0;
        f->_EOF = 0;
        f->_pid = pid;

        /*-----------------------------------------*/

        if (type[0] == 'r')
        {
            fcntl(pipe_fd[READ_END], F_SETFL, O_RDONLY);
            close(pipe_fd[WRITE_END]);
            f->mode = _RD;
            f->_fd = pipe_fd[READ_END];
        }

        if (type[0] == 'w')
        {
            fcntl(pipe_fd[WRITE_END], F_SETFL, O_WRONLY);
            close(pipe_fd[READ_END]);
            f->mode = _WR;
            f->_fd = pipe_fd[WRITE_END];
        }

        return f;
    }
    else
        return NULL;
#endif

#if defined(_WIN32)
    

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    BOOL bRes;
    CHAR* cmd[100];
    memset(cmd,0, 100);
    strcpy(cmd, "C:\\windows\\system32\\cmd.exe /c ");
    strcat(cmd, command);

    HANDLE rHandle = NULL;
    HANDLE wHandle=NULL;
    SECURITY_ATTRIBUTES pipeAttr;

    if ((type[0] != 'r' && type[0] != 'w') || strlen(type) > 1)
        return NULL;


    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    ZeroMemory(&pi, sizeof(pi));

    ZeroMemory(&pipeAttr, sizeof(pipeAttr));
    pipeAttr.nLength = sizeof(SECURITY_ATTRIBUTES);

    pipeAttr.bInheritHandle = TRUE;
    pipeAttr.lpSecurityDescriptor = NULL;

    ret = CreatePipe(&rHandle, &wHandle,&pipeAttr,0);
    if (ret == 0)
        return NULL;

    SO_FILE* stream = (SO_FILE*)malloc(sizeof(SO_FILE));
    if (stream == NULL)
        return NULL;
    stream->buff_offset = 0;
    stream->buff_size = 0;
    stream->_state = _DEF;
    stream->exitState = _NORMAL_STATE;
    stream->_update = 0;
    stream->_EOF = 0;
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    if (strcmp(type, "r") == 0) {
        stream->_fd = rHandle;
        stream->mode = _m_r;
        si.hStdOutput = wHandle;
        si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    }
    else if (strcmp(type, "w") == 0) {
        stream->_fd = wHandle;
        stream->mode = _m_w;
        si.hStdInput = rHandle;
        si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    }


    bRes = CreateProcessA(
            NULL,
            cmd,
            NULL,
            NULL,
            TRUE,
            0,
            NULL,
            NULL,
            &si,
            &pi
    );

    if (bRes == FALSE)
    {
        free(stream);
        stream = 0;
        return NULL;
    }

    stream->_pi = pi;

    return stream;
   
#endif
    return NULL;
}

int so_pclose(SO_FILE* stream)
{


    if (stream == NULL)
        return -1;
    int ret;
    int status;




    ret = so_fflush(stream);
    if (ret == -1)
    {
        free(stream);
        stream = NULL;
        return SO_EOF;
    }

#if defined(__linux__)
    int pid = stream->_pid;
    ret = close(stream->_fd);
    free(stream);
    stream = NULL;

    ret = waitpid(pid, &status, 0);
    if (ret < 0)
        return -1;
    return 0;
#endif
#if defined(_WIN32)
    PROCESS_INFORMATION pi = stream->_pi;
    HANDLE handle = stream->_fd;

    free(stream);
    CloseHandle(handle);

    ret = WaitForSingleObject(pi.hProcess, INFINITE);
    if (ret == WAIT_FAILED)
        return -1;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
    
#endif
    return 0;
}

#include <stdio.h>
int main()
{
    SO_FILE* fptr = so_popen("ipconfig", "r");
    char buff[64];
    memset(buff, 0, 64);
    so_fread(buff, 1, 40, fptr);
    printf("%s", buff);

    return 0;
}

