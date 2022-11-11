.PHONY:clean

build: libso_stdio.so

libso_stdio.so: so_stdio.o
	gcc -shared so_stdio.o -o libso_stdio.so
so_stdio.o: so_stdio.c
	gcc -c -fPIC so_stdio.c -o so_stdio.o

clean:
	rm *.o *.so
