#include "_so_file.h"
#include <stdio.h>

int main()
{   
    SO_FILE* fptr;
    fptr=so_fopen("test.txt","r+");
    if(fptr==NULL)
    {
        fprintf(stderr,"COULDNT OPEN THE FILE\n");
    }
    else
        printf("File opened successfuly\n");

    char ch;
    /*--------------------------------------------*/
    ch=so_fgetc(fptr);
    printf("%c",ch);
    ch=so_fgetc(fptr);
    printf("%c",ch);
    ch=so_fgetc(fptr);
    printf("%c",ch);
    /*--------------------------------------------*/

    char buff[50];
    so_fread(buff,1,10,fptr);
    buff[10]='\0';
    printf("\n%s",buff);
    /*--------------------------------------------*/

    so_fseek(fptr,0,SEEK_SET);
    so_fread(buff,1,10,fptr);
    buff[10]='\0';
    printf("\n%s",buff);
    /*--------------------------------------------*/
    printf("\n%d",so_ftell(fptr));

                //SCRIERE DUPA CITIRE
    /*--------------------------------------------*/
    printf("\n\n");
    int check;
    so_fseek(fptr,0,SEEK_SET);          //SEEK FIRST

    memset(buff,0,50);
    strcpy(buff,"Scriere dupa citire");

    check=so_fwrite(buff,1,19,fptr);
    printf("Numar de octeti scrisi in fisier:%d\n",check);

    so_fflush(fptr);
                //CITIRE DUPA SCRIERE
    /*--------------------------------------------*/

    memset(buff,0,50);
    so_fread(buff,1,19,fptr);
    printf("%s\n",buff);

    memset(buff,0,50);
    so_fseek(fptr,0,SEEK_SET);
    so_fread(buff,1,19,fptr);
    printf("%s",buff);

    return 0;
}