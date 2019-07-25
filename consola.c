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
Función consola, no usa una interface grafica. Este modo permite a un usuario realizar una prueba de velocidad del motor
llamada bench o un test donde analiza 25 posiciones y que ayuda a generar un ejecutable mas rapido.
*/

#include <stdio.h>		/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <string.h>     /*strcmp, strcpy, strcat*/
#include <stdlib.h>     /*atoi*/
#include <inttypes.h>
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* el numero de nodos puede ser muy grande, necesitamos definirlo al menos con 32 bits, revisar esto */

#if defined (_WIN32)
	#if defined (UNDER_64BIT)
	#define PRI_SIZET PRIu64
	#else
	#define PRI_SIZET PRIu32
	#endif
#else
	#define PRI_SIZET "zu"
#endif

void consola(void)
{
	char linea[256], s[256];
	int	depth, i;
#if defined (UNDER_64BIT)
	U64 nodes;
#else
	U32 nodes;
#endif

	/* damos algo de informacion del motor y autor */
	printf("# %s is a chess engine with xboard and uci protocol.\n", nombre);
	printf("# Copyright (C) <%s>  <Pedro Castro Elgarresta> \n", fecha);
	printf("# Basque Contry, Spain \n");
	printf("# Web Page: http://sites.google.com/site/danasah/ \n");
	printf("# \n");
	printf("# \"help\" displays a list of commands.\n");
	printf("\n");

	/* a continuacion leemos el archivo de configuracion danacfg.txt si esta disponible, esta parte se comporta de forma parecida
	al protocolo xboard, en el archivo xboard.c podemos ver mas comentarios */
	leer_inifile();
	/* si hemos pasado parametros al lanzar el ejecutable tenemos que cambiar el valor de dichos parametros */
	if (leer_configuracion)
		Analiza_parametros();

#if defined (UNDER_ELO)
	/* en la version limitada de fuerza modificaremos la fuerza en funcion del oponente */
    if (!strcmp(limitstrength, "true")) Nivel();
#endif
	/* tablas hash */
	alloc_hash();	/*reservamos memoria para las tablas hash*/
	inicia_hash();	/*inicia las tablas hash*/

	/*entramos en un cliclo esperando un comando del usuario*/
    for (;;) {
#if defined (UNDER_CE)
        if (gets(linea) == NULL)						/*cierra el programa*/
            return;
#else
        printf("danasah> ");
        if (fgets(linea, 256, stdin) == NULL)			/*cierra el programa*/
            return;
#endif

        sscanf (linea,"%s",s);

		/* sale del programa */
		if (!strcmp(s, "quit")) {
			break;
		}
		/* perft: comprueba el correcto funcionamiento del generador de movimientos llamando a una funcion llamada perft situada en el
		archivo test.c */
		if (!strcmp(s, "perft")) {
			sscanf(linea,"%*s %d", &depth);
			variante[0] = '\0';
			strcpy(variante,"normal");
			printf("# Variante = %s\n", variante);
			strcpy(fenstring, "");
			strcat(fenstring, STARTPOS);
			SetBoard(fenstring);
			printf("# Performance Test\n");
			printf("# Ply\tSeconds\tNodes\n");
			for (i = 0; i <= depth; i++) {
				tiempo_inicio = get_ms();
				nodes = perft(i);
				tiempo_total = get_ms()-tiempo_inicio;
				printf("# %d:\t%d\t%" PRI_SIZET "\n",i,tiempo_total/1000, nodes);
			}
			printf("\n");
			continue;
		}
		/* bench: comprueba la velocidad del equipo llamando a una funcion llamada bench del archivo test.c */
		if (!strcmp(s, "bench")) {
			bench();
			printf("\n");
			continue;
		}
		/* test: reaaliza un test para generar un "profile guided optimization" */
		if (!strcmp(s, "test")) {
			Test();
			printf("\n");
			continue;
		}
		/* ayuda en pantalla */
		if (!strcmp(s, "help")) {
			printf("\n");
			printf("# perft number - Total nodes account for a given depth from the position Start\n");
			printf("# test - Test 25 positions 20 s, used to build a profile guided optimization\n");
			printf("# bench - Run the built-in benchmark\n");
			printf("# quit - Exit the program\n");
			printf("\n");
			continue;
		}
	}

	return;
}
