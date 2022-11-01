#include "so_stdio.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#define DEFAULT_BUFF_SIZE 4096

/*-------------------------------------*/
#define _m_r 1
#define _m_r_w 2
#define _m_w 3 
#define _m_w_r 4
#define _m_a 5 
#define _m_ar 6

#define _RD 1
#define _WR 2
#define _DEF 3

#define _ERROR_STATE 300
#define _NORMAL_STATE 200
/*-------------------------------------*/

typedef struct _so_file
{
    int _fd;
    char buff[DEFAULT_BUFF_SIZE];
    int mode;
    int _state;
    int exitState;
    size_t f_offset;
    size_t buff_offset;
    size_t buff_size;

}_so_file;