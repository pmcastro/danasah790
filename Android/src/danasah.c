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
Definimos la funcion principal o main del programa en dicha funcion normalmente vamos a estar esperando hasta que la GUI nos
envie el comando xboard o uci para empezar con el protocolo correspondiente o hasta que el usuario quiera entrar en modo
consola.
*/

/*Includes*/
#if defined (UNDER_CE)
	#include <windows.h>	/*Messagebox*/
#endif
#include <stdio.h>			/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <string.h>     	/*strcmp, strcpy, strcat*/
#include <stdlib.h>     	/*atoi*/
#include <time.h>
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* funcion principal donde se inicia la ejecucion del motor */
int main (int argc, char *argv[])
{
    char linea[256], s[256];
	int i;

	/*información del tamaño de algunos tipos y estructuras */
	/*printf("Int_Size = %d\n", sizeof(int));
	printf("Int16_t_Size = %d\n", sizeof(int16_t));
	printf("Short_Size = %d\n", sizeof(short));
	printf("Char_Size = %d\n", sizeof(char));
	printf("U64_Size = %d\n", sizeof(U64));
	printf("Movimiento_Size = %d\n", sizeof(movimiento));
	printf("TTSlot_Size = %d\n", sizeof(TType));*/

	/* si en lugar de utilizar un archivo de configuración para el protocolo xboard pasamos parametros cuando llamamos al motor,
	entonces tenemos que guardar dichos parametros */
    for(i=1; i<argc; i++) {
        if(!strcmp(argv[i], "hash")) {
            i++;
            if(i < argc) guarda_hash = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "ownbook")) {
            i++;
            if(i < argc) strcpy(guarda_ownbook, argv[i]);
        }
        else if(!strcmp(argv[i], "book")) {
            i++;
            if(i < argc) strcpy(guarda_libro, argv[i]);
        }
        else if(!strcmp(argv[i], "egbb_path")) {
            i++;
            if(i < argc) strcpy(guarda_egbb_path, argv[i]);
        }
        else if(!strcmp(argv[i], "egbb_men")) {
            i++;
            if(i < argc) guarda_egbb_men = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "egbb_cache_size")) {
            i++;
            if(i < argc) guarda_egbb_cache_size = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "egbb_load_type")) {
            i++;
            if(i < argc) guarda_egbb_load_type = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "use_egbb")) {
            i++;
            if(i < argc) guarda_use_egbb = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "egtb_path")) {
            i++;
            if(i < argc) strcpy(guarda_egtb_path, argv[i]);
        }
        else if(!strcmp(argv[i], "egtb_cache_size")) {
            i++;
            if(i < argc) guarda_egtb_cache_size = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "use_egtb")) {
            i++;
            if(i < argc) guarda_use_egtb = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "noise")) {
            i++;
            if(i < argc) guarda_noise = atoi(argv[i]);
		}
		/* la version limitada en fuerza del motor dispone de mas parametros */
#if defined (UNDER_ELO)
        else if(!strcmp(argv[i], "limitstrength")) {
            i++;
            if(i < argc) strcpy(guarda_limitstrength, argv[i]);
        }
        else if(!strcmp(argv[i], "elo")) {
            i++;
            if(i < argc) guarda_elo = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "elo_type")) {
            i++;
            if(i < argc) strcpy(guarda_elo_type, argv[i]);
        }
        else if(!strcmp(argv[i], "style")) {
            i++;
            if(i < argc) guarda_style = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "diff_engine")) {
            i++;
            if(i < argc) guarda_diff_engine = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "diff_computer_engine")) {
            i++;
            if(i < argc) guarda_diff_computer_engine = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "diff_human_computer")) {
            i++;
            if(i < argc) guarda_diff_human_computer = atoi(argv[i]);
        }
        else if(!strcmp(argv[i], "easylevel")) {
            i++;
            if(i < argc) guarda_easylevel = atoi(argv[i]);
        }
#endif
	}
	/* si hemos recibido mas de un argumento al llamar al motor tendremos que modificar mas tarde los valores del archivo de configuracion */
	if (i > 1) leer_configuracion = VERDADERO;

/* inicia el generador de numeros aleatorios */
#if defined (UNDER_CE)
	srand(GetTickCount());
#else
	srand((unsigned)time(NULL));
#endif

	/* entramos en un bucle esperando protocolo o consola */
    for (;;) {
#if defined (UNDER_CE)
        if (gets(linea) == NULL)						/* cierra el programa */
            return 0;
#else
        if (fgets(linea, 256, stdin) == NULL)			/* cierra el programa */
            return 0;
#endif

        sscanf (linea,"%s",s);							/* captura el comando */

		if (!strcmp(s, "quit")) {						/* sale del motor */
			break;
		}
		if (!strcmp(s, "xboard")) {
			protocolo = XBOARD;
			xboard();									/* comunicacion del motor con la GUI a traves del protocolo xboard */
			break;
		}
		if (!strcmp(s, "uci")) {
			protocolo = UCI;
			uci();										/* comunicacion del motor con la GUI a traves del protocolo uci */
			break;
		}
		if (!strcmp(s, "console")) {
			protocolo = XBOARD;
			consola();									/* inicia en modo consola sin interface grafica */
			break;
		}
	}
	/* liberamos la memoria total utilizada por el motor */
	Libera_memoria();

	/*Salimos*/
	return 0;
}

void Libera_memoria(void)
{
	/*Liberamos la memoria de las bitbases*/
    if (egbb_is_loaded) unload_bitbases();
	/*Liberamos la memoria de las tablebases*/
    if (egtb_is_loaded) unload_tablebases();
	/*Liberamos la memoria del libro de aperturas*/
	if (libro_cargado) close_book();
    /*Liberamos la memoria de las tablas hash*/
	free_hash();
}

/* el siguiente codigo es utilizado por la version para PocketPc, dicho codigo fue dado por los programadores de MobileThinker y
basicamente lo que hace es crear una dll del motor que es lo que necesita MobileThinker */
#if defined (UNDER_CE)
    TEngineIO *EngineIO;
    int GetCh(void)
    {
        char s[2];
        EngineIO->p_get_string (EngineIO->pIOInst, s, 1);
        return s[0];
    }
    int __cdecl PrintF(const char *fmt, ...)
    {
        int n;
        va_list args;
        va_start (args, fmt);
        n = EngineIO->p_vprint (EngineIO->pIOInst, fmt, args);
        va_end (args);
        return n;
    }
    BOOL WINAPI DllMain (HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
    {
        return TRUE;
    }
    #ifdef __cplusplus
    extern "C"
    #endif
    __declspec(dllexport) void __stdcall Run (TEngineIO *pEngineIO)
    {
        EngineIO = pEngineIO;

        /*puts ("DanaSah Started");
        MessageBox (NULL, STRING ("DanaSah Started\r\n"), _T("Engine"), MB_OK);*/

        main (EngineIO->argc, EngineIO->argv);
    }
#endif
