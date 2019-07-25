/*
	< DanaSah is a winboard/xboard and uci chess engine. >
    Copyright (C) <2018>  <Pedro Castro Elgarresta>
	E-mail <pecastro@msn.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
Funciones para acceder a las bitbases. Gracias a Daniel Shawul. Para más información sobre esta funciones deberíamos recurrir a Scorpio.
Las bitbases funcionan en todos los sistemas salvo en android donde es necesario crear una compilacion no estatica que pueda hacer uso
del archivo .so correspondiente. Hay algunas partes del codigo que son dificilmente comentadas porque el codigo pertene a Daniel
*/

/*Includes*/
#include <stdio.h>
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* Ignoramos avisos del compilador al convertir de ISO C */
#if defined (UNDER_64BIT)
/*#if BOOST_COMP_GNUC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"*/
#endif

/* definicion de las piezas y colores para usar las bitbases de scorpio */
enum {_WHITE,_BLACK};
enum {_EMPTY,_WKING,_WQUEEN,_WROOK,_WBISHOP,_WKNIGHT,_WPAWN,
_BKING,_BQUEEN,_BROOK,_BBISHOP,_BKNIGHT,_BPAWN};

#define _NOTFOUND 99999

/* para el acceso a a las bitbases se usa una dll en los dispositivos Windows y un archivo so en unix */
#if defined (UNDER_CE) || defined (_WIN32)
    #ifndef WINDOWS
        #define WINDOWS
    #endif
    #include <windows.h>
    #undef CDECL
    #define CDECL __cdecl
#else
    #include <dlfcn.h>
    #include <string.h>
    #include <unistd.h>								/*getcwd*/
    #define CDECL
	#define HMODULE void*
	#define LoadLibrary(x) dlopen(x,RTLD_LAZY)
	#define FreeLibrary(x) dlclose(x)
	#define GetProcAddress dlsym
#endif

#if defined (UNDER_CE) || defined (_WIN32)
	#if defined (UNDER_64BIT)
	#define EGBB_NAME STRING("egbbdll64.dll")
	#else
	#define EGBB_NAME STRING("egbbdll.dll")
	#endif
#else
	#if defined (UNDER_64BIT)
	#define EGBB_NAME "egbbso64.so"
	#else
	#define EGBB_NAME "egbbso.so"
	#endif
#endif

typedef int (CDECL *PPROBE_EGBB) (int player, int* piece, int* square);
typedef void (CDECL *PLOAD_EGBB) (char* path,int cache_size,int load_options);

static PPROBE_EGBB probe_egbb;
static HMODULE hmod;

/* esta funcion inicia el proceso de carga en las bitbases y se utiliza en las funciones xboard() y uci() */
void load_bitbases(void)
{
	egbb_cache_size = (egbb_cache_size * 1024 *1024);
	egbb_is_loaded = LoadEgbbLibrary(egbb_path, egbb_cache_size);
}

/* esta funcion carga las bitbases conociendo la ruta donde estan situadas y la cache de memoria que queremos utilizar */
int LoadEgbbLibrary(char* main_path, int cache_size) {
	PLOAD_EGBB load_egbb;
#if defined (UNDER_ARM)
    	/* buscamos la ruta completa del archivo so en Android, debe estar este archivo junto al ejecutable y cargado en memoria ? */
	    char path[128];
	    if (getcwd(path, 96) == NULL) {
			printf("Path not found!\n");
		}
        else {
            strcat(path, "/");
            strcat(path,EGBB_NAME);
	    }
#elif defined (UNDER_CE)
    	/* buscamos la ruta completa del archivo dll sobre el PocketPc, debe estar este archivo junto al ejecutable */
	    WCHAR path[128];
	    mbstowcs(path,main_path,strlen(main_path)+1);
	    wcscat(path,EGBB_NAME);
#else
		/* en Windows y Linux buscaremos el archivo dll o so en la misma carpeta que las bitbases */
	    char path[128];
	    size_t plen = strlen(main_path);
	    if (plen) {
	    	char terminador = main_path[plen-1];
	    	if (terminador != '/' && terminador != '\\') {
				if (strchr(main_path, '\\') != NULL)
					strcat(main_path, "\\");
				else
					strcat(main_path, "/");
			}
		}
	    strcpy(path,main_path);
	    strcat(path,EGBB_NAME);
#endif
	/* si las bitbases ya estaban cargadas liberamos la memoria */
	if (hmod) FreeLibrary(hmod);
	/* de momento danasah solo permite utilizar hasta 5 piezas en las bitbases, aunque ya existen bitbases de hasta 6 piezas */
	if ((hmod = LoadLibrary(path)) != 0) {
	    /* ISO C prohibe la conversión de objeto apuntador a un tipo de apuntador a función */
	    load_egbb = (PLOAD_EGBB) GetProcAddress(hmod,STRING("load_egbb_xmen"));
     	probe_egbb = (PPROBE_EGBB) GetProcAddress(hmod,STRING("probe_egbb_xmen"));
     	load_egbb(main_path, cache_size, egbb_load_type);
#if defined (UNDER_CE)
 		puts ("Bitbases loaded!");
		MessageBox (NULL, STRING ("Bitbases loaded!\r\n"), _T("Engine DanaSah"), MB_OK);
#endif
		return VERDADERO;
	} else {
		printf("EgbbProbe not Loaded!\n");
		return FALSO;
	}
}

/* esta funcion permite descargar las bitbases y liberar memoria */
void unload_bitbases(void)
{
        if (hmod == NULL)
            return;

         if (!FreeLibrary(hmod))
			printf("Can't unload bitbases");
         hmod = NULL;
         probe_egbb = NULL;
         egbb_is_loaded = FALSO;
}

/* esta funcion es la que probará a encontrar la mejor opcion, tenemos que indicar donde estan situadas las piezas del tablero,
hasta 5 */
int probe_bitbases(int *score) {

	int i, piece[9],square[9],count;
	int player;

	/* normalmente se usan valores de 1 ó 0 para indicar el jugador que mueve, danasah usa los valores al reves que scorpio de ahi el
	valor de player */
	player = 1-turno;
	count = 0;

/* danasah tambien usa un sistema de representacion diferente a scorpio, en lugar de usar A8 = 0 y H1 = 63, Scorpio usa A1 = 0 y H8 = 63
con lo cual vamos a definir una funcion que nos permite pasar de un sistema a otro */
#define FLIP(sq) ((sq) ^ 0x38)

#define ADD_PIECE(type) {\
	    piece[count] = type;\
	    square[count] = FLIP(i);\
        count++;\
};

	for (i = 0; i < 64; ++i) {
		if (color[i] == VACIO)
			continue;
		if(color[i] == BLANCO) {
			switch (pieza[i]) {
				case PEON: ADD_PIECE(_WPAWN); break;
				case CABALLO: ADD_PIECE(_WKNIGHT); break;
				case ALFIL: ADD_PIECE(_WBISHOP); break;
				case TORRE: ADD_PIECE(_WROOK); break;
				case DAMA: ADD_PIECE(_WQUEEN); break;
				case REY: ADD_PIECE(_WKING); break;
				default: break;
			}
		} else {
			switch(pieza[i]) {
				case PEON: ADD_PIECE(_BPAWN); break;
				case CABALLO: ADD_PIECE(_BKNIGHT); break;
				case ALFIL: ADD_PIECE(_BBISHOP); break;
				case TORRE: ADD_PIECE(_BROOK); break;
				case DAMA: ADD_PIECE(_BQUEEN); break;
				case REY:ADD_PIECE(_BKING); break;
				default: break;
			}
		}
	}
 	piece[count] = _EMPTY;
	if (alpaso == -1)
		 square[count] = _EMPTY;
	else
		square[count] = FLIP(alpaso);

	/* si tenemos 5 o menos piezas tratamos de obtener un resultado, si este resultado es un valor cercano a +50 significa que la posicion
	esta ganada, si es -50 que la posicion esta perdida y 0 que son tablas */
	*score = probe_egbb(player, piece, square);

	if(*score != _NOTFOUND) {
		return 1;
	}

    return 0;
}

