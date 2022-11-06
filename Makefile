CC := gcc
CFLAGS := -Wall -fPIC -c
SH := -shared
objs := fopen.o fgetc.o fputc.o fread.o fflush.o fwrite.o

.PHONY:clean
#----------OLD VERSION----------------
main: main.c libso_stdio.so
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
	$(CC) main.c -o main -lso_stdio -L .

libso_stdio.so: fopen.o fgetc.o fputc.o fread.o fflush.o
	$(CC) $(SH) $(objs) -o libso_stdio.so

fopen.o : fopen.c
	$(CC) $(CFLAGS) fopen.c -o fopen.o

fgetc.o: fgetc.c
	$(CC) $(CFLAGS) fgetc.c -o fgetc.o

fputc.o: fputc.c
	$(CC) $(CFLAGS) fputc.c -o fputc.o

fread.o: fread.c
	$(CC) $(CFLAGS) fread.c -o fread.o

fflush.o: fflush.c
	$(CC) $(CFLAGS) fflush.c -o fflush.o

fwrite.o: fwrite.c
	$(CC) $(CFLAGS) fwrite.c -o fwrite.o
clean: 
	rm *.o *.so main

