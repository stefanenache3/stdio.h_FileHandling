#include "so_stdio.h"
#include "_so_file.h"
#include <string.h>


int getFileHandle(const char* filename,enum Mode m, int* update)
{
    int fd=-1;
    *update=0;
    #if defined (__linux__)
        switch (m)
        {
        case _m_r:
            fd=open(filename,O_RDONLY);
            break;
        case _m_r_w:
            fd=open(filename,O_RDWR);
            *update=1;
            break;
        case _m_w:
            fd=open(filename,O_WRONLY | O_CREAT | O_TRUNC);
            break;
        case _m_w_r:
            fd=open(filename,O_RDWR | O_CREAT | O_TRUNC);
            *update=1;
            break;
        case _m_a:
            fd=open(filename,O_APPEND | O_CREAT | O_RDONLY);
            break;
        case _m_a_r:
            fd=open(filename,O_APPEND | O_CREAT | O_RDWR);
            *update=1;
            break;
        default:
            break;
        }
    #endif
    #if defined (_WIN32)
         switch (m)
        {
        case _m_r:
            fd=CreateFileA(
                filename,
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL 
            );
            break;
        case _m_r_w:
            fd=CreateFileA(
                filename,
                GENERIC_WRITE | GENERIC_READ,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL 
            );
            *update=1;
            break;
        case _m_w:
            fd=CreateFileA(
                filename,
                GENERIC_WRITE ,
                0,
                NULL,
                CREATE_NEW | TRUNCATE_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL 
            );
            break;
        case _m_w_r:
            fd=CreateFileA(
                filename,
                GENERIC_WRITE | GENERIC_READ ,
                0,
                NULL,
                CREATE_NEW | TRUNCATE_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL 
            );
            *update=1;
            break;
        case _m_a:
            fd=CreateFileA(
                filename,
                O_APPEND| GENERIC_WRITE,
                0,
                NULL,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL 
            );
            break;
        case _m_a_r:
            fd=CreateFileA(
                filename,
                O_APPEND | GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL 
            );
            *update=1;
            break;
        default:
            break;
        }
    #endif
    return fd;
}


FUNC_DECL_PREFIX SO_FILE *so_fopen(const char *pathname, const char *mode)
{
    char ch=mode[0];
    char ch2;
    int len=strlen(mode);
    int fd=-1;
    int md;
    int update;
    if(len==2)
        ch2=mode[1];
    else if(len>2)
        return NULL;
    
    switch (ch)
    {
    case 'r':
        if(len==1)
             md=_m_r;
        else if(ch2=='+') 
            md=_m_r_w;
        else
            return NULL;
        break;
    
    case 'w':
        if(len==1)
             md=_m_w;
        else if(ch2=='+')
                md=_m_w_r;
        else 
            return NULL;
        break;
    case 'a':
        if(len==1)
            md=_m_a;
        else if(ch2=='+')
            md=_m_a_r;
        else 
            return NULL;
        break;
    default:
        return NULL;
        break;
    }

    fd=getFileHandle(pathname,md,&update);
    if(fd==-1)
    return NULL;

    SO_FILE* handle=(SO_FILE*)malloc(sizeof(SO_FILE));
    handle->buff_offset=0;
    handle->buff_size=0;
    handle->f_offset=0;
    handle->_fd=fd;
    handle->mode=md;
    handle->_state=_DEF;
    handle->exitState=_NORMAL_STATE;
    handle->_update=update;
    handle->_EOF=0;
    return handle;
}