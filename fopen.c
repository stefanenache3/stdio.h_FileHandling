#include "so_stdio.h"
#include "_so_file.h"
#include <string.h>
FUNC_DECL_PREFIX SO_FILE *so_fopen(const char *pathname, const char *mode)
{
    char ch=mode[0];
    char ch2;
    int len=strlen(mode);
    int fd=-1;
    int mode;

    if(len==2)
        ch2=mode[1];
    else if(len>2)
        return NULL;
    
    switch (ch)
    {
    case 'r':
        if(len==1)
           {
             fd=open(pathname,O_RDONLY);
             mode=_m_r;
           }
        else if(ch2=='+') 
        {
            fd=open(pathname,O_RDWR);
            mode=_m_r_w;
        }
        else
            return NULL;
        break;
    
    case 'w':
        if(len==1)
            {
                fd=open(pathname,O_WRONLY | O_CREAT | O_TRUNC);
                mode=_m_w;
            }
        else if(ch2=='+')
            {
                fd=open(pathname, O_RDWR | O_CREAT | O_TRUNC);
                mode=_m_w_r;
            }
        else 
            return NULL;
            break;
    case 'a':
        if(len==1)
        {
            fd=open(pathname, O_APPEND | O_CREAT | O_RDONLY);
            mode=_m_a;
        }
        else if(ch2=='+')
        {
            fd=open(pathname, O_APPEND | O_CREAT | O_RDWR);
            mode=_m_ar;
        }
        else 
            return NULL;
            break;
    default:
        return NULL;
        break;
    }


    SO_FILE* handle=(SO_FILE*)malloc(sizeof(SO_FILE));
    handle->buff_offset=0;
    handle->buff_size=0;
    handle->f_offset=0;
    handle->_fd=fd;
    handle->mode=mode;
    handle->_state=_DEF;
    handle->exitState=_NORMAL_STATE;

    return handle;
}
