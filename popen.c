#include "_so_file.h"
#define _MAX_ARGS 10
#define READ_END 0
#define WRITE_END 1
 int tokenizeCommand(char* command,char ***args)
 {  
    *args=(char**)malloc(_MAX_ARGS*sizeof(char*));
    if(args==NULL)
        {
            //allocation failed;
            return -1;
        }
    for(int i=0;i<10;i++)
        args[i]=NULL;
    
    (*args)[0]=strdup("sh");
    (*args)[1]=strdup("-c");

    int nArgs=2;

    char *p=NULL;
    p=strtok(command," ");
    if(p==NULL)
        return 2;
    nArgs++;

    while(p)
    {
        p=strtok(NULL," ");
        (*args)[nArgs-1]=strdup(p);
        nArgs++;
    }
    
    return nArgs;
 }
SO_FILE *so_popen(const char *command, const char *type)
{
    char**args=NULL;
    int nArgs=tokenizeCommand((char*)command,&args);
    if(nArgs<3)
        return NULL;
    int ret;
    if((type[0] != 'r' && type[0]!='w') || strlen(type)>1)
        return NULL;

    int pipe_fd[2]; 

    ret=pipe(pipe_fd);
    if(ret<0)
    {
        //error
        return NULL;
    }

    SO_FILE* f=(SO_FILE*)malloc(sizeof(SO_FILE));
    if(f==NULL)
        return NULL;

    /*-----------------------------------------*/
    f->buff_offset = 0;
    f->buff_size = 0;
    f->f_offset = 0;
    f->_state = _DEF;
    f->exitState = _NORMAL_STATE;
    f->_update = 0;
    f->_EOF = 0;
    /*-----------------------------------------*/

    int pid=fork();
    if(pid==0)
    {
        int fd;
        if(type[0]=='r')
        {
            close(pipe_fd[READ_END]);
            dup2(pipe_fd[WRITE_END],STDOUT_FILENO);
            close(pipe_fd[WRITE_END]);
            fd=STDOUT_FILENO;
        }
        if(type[0]=='w')
        {
            close(pipe_fd[WRITE_END]);
            dup2(pipe_fd[READ_END],STDIN_FILENO);
            close(pipe_fd[READ_END]);
            fd=STDIN_FILENO;
        }

        execvp(args[0],args);
    }
    if(pid>0)
    {
        if(type[0]=='r')
        {
            fcntl(pipe_fd[READ_END],F_SETFL,O_RDONLY);
            close(pipe_fd[WRITE_END]);
            f->mode=_RD;
            f->_fd=pipe_fd[READ_END];
        }

        if(type[0]=='w')
        {
           fcntl(pipe_fd[WRITE_END],F_SETFL,O_WRONLY);
           close(pipe_fd[READ_END]);
           f->mode=_WR;
           f->_fd=pipe_fd[WRITE_END];
        }
    }

    return f;
}