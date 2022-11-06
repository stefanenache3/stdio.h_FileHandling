#include "so_stdio.h"
#include <fcntl.h>
#include <string.h>

#define DEFAULT_BUFF_SIZE 4096

/*-------------------------------------*/
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
    int _update;
    int _EOF;
    size_t f_offset;
    size_t buff_offset;
    size_t buff_size;

}_so_file;

enum Mode{_m_r,_m_r_w,_m_w,_m_w_r,_m_a,_m_a_r};