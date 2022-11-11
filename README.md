# stdio.h_FileHandling

## Instalare
	windows
```bash
  git clone https://github.com/stefanenache3/stdio.h_FileHandling
  cd /win
  nmake build
```
	linux
	
```bash

  git clone https://github.com/stefanenache3/stdio.h_FileHandling
  cd /line
  make build
  
```
	clean:
```bash
	
	make clean

```

Implementarea proiectului respecta cerintele temei.

typedef struct _so_file
{
    int _fd;						: salvam descriptor/handle fisierului pentru care vrem sa facem operatiile
    unsigned char buff[DEFAULT_BUFF_SIZE];		
    int mode;						: dupa deschidere salvam modul in care a fost deschis fisierul
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


Membrul **_state** din structura este initial _DEF ( default, stare neutra).Dupa o operatie de citire starea va fi _RD, iar dupa una de scriere _WR.
Retinem de fiecare data starea pentru a stii secventa de operatii permise in modul update.

-**exitState** : setam valoarea pe unu de fiecare data cand intalnim o eroare
-**_EOF** : val pe 1 inseamna ca am ajuns la finalul fisierului

In **f_offset** retinem pozitia cursorului in fisier, iar in **buff_offset** indexul elementului curent din buffer.


Citire: citirea se face din buffer.Atunci cand bufferul este gol sau s-a atins dimensiunea maxima setata se face read() si se incearca umplerea buffer-ului.
Daca exista elemente din buffer care nu au fost citite acestea se retureaza fara a se mai face read.

In **so_fread** sunt tratate mai multe cazuri: 
	Initial se face un singur so_fgetc.Daca suntem in update si se incearca citirea in starea _WR aceasta returneaza EOF si se iese din functie.

	-daca in buffer sunt mai multe elemente decat se solicita pentru citire se copiaza direct in vectorul dat ca argument elementele
	-in caz contrar avem doua posibilitati:
		1.Dimensiunea totala a bufferului este mai mica decat cea maxima (DEFAULT_BUFF_SIZE), asta inseamna ca nu mai este nimic de citit in continuare din fisier si se copiaza
		ce a ramas in buffer.
		2.In cazul al doilea inseamna ca o sa se mai faca citiri din fisier si nu putem copia direct, asa ca ne folosim de vectorul auxiliar, pe care il completam si din care vom copia la final
		
		
In **so_fwrite** se apeleaza so_fputc in mod repetate pana se scriu toate elementele.Scrierea se face in buffer pana cand acesta se umple si se apeleaza fflush.

In **so_fflush** se trece in stare neutra, astfel o citire va putea fi urmata de o scriere sau invers (in modul update)
	-daca suntem in starea _WR: se scrie ce era in buffer in fisier si se reinitializeaza dimensinile acestuia
	-daca suntem in starea _RD : se reinitializeaza dimensiunile buffer-ului
