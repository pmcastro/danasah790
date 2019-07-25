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
Funciones que van a tratar algun tipo de informacion recibida de la GUI
*/

/*Includes*/
#include <stdio.h>						/* prinft, scanf, sprinft, sscanf, fgets... */
#if defined (_WIN32)
    #ifndef WINDOWS
        #define WINDOWS
    #endif
    #include <windows.h>				/* bioskey() */
#else
    #include <sys/time.h>
    #include <string.h>
    #include <unistd.h>
    struct timeval tv;
    struct timezone tz;
#endif
#include <stdlib.h>
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* la siguiente funcion nos va a permitir comprobar que tipo de movimiento nos llega, es utilizada para la siguiente función
"Comprobar_Movimiento()" */
int Comprobar_Tipo_Movimiento(int de, int a, char *s)
{
	int tipo = NINGUNO;		/* inicialmente no tenemos un tipo de movimiento */

	/* comprueba si nos llega un enroque en la variante normal del juego */
	if (strcmp (variante,"normal") == 0 && pieza[de] == REY) {
		if ((s[0] == 'e' && s[1] == '1') || (s[0] == 'e' && s[1] == '8')) {
			if ((s[2] == 'g' && s[3] == '1') || (s[2] == 'c' && s[3] == '1') || (s[2] == 'g' && s[3] == '8') || (s[2] == 'c' && s[3] == '8'))
				tipo = ENROQUE;
		}
	}
	/* si movemos un peon podriamos tener hasta 3 clases de tipo de movimiento */
	if (pieza[de] == PEON) {
		/* comprueba si es un movimiento al paso */
		if ((alpaso != -1) && (a == alpaso))
			tipo = AL_PASO;
		/* comprueba si es un movimiento de avance del peon 2 casillas */
		else if ((s[1] == '2' && s[3] == '4') || (s[1] == '7' && s[3] == '5'))
			tipo = PEON_AVANZA_2;
		/* comprueba si el movimiento es una promoción */
		else if (a < 8 || a > 55) {
			/* en el Pocket PC una promocion puede venir definida con el simbolo = (e8=Q) */
#if defined (UNDER_CE)
			if (s[4] == '=') {
				if (s[5] == 'q' || s[5] =='Q')
					tipo = PROMOCION_A_DAMA;
				else if (s[5] == 'r' || s[5] == 'R')
					tipo = PROMOCION_A_TORRE;
				else if (s[5] == 'b' || s[5] == 'B')
					tipo = PROMOCION_A_ALFIL;
				else if (s[5] == 'n' || s[5] == 'N')
					tipo = PROMOCION_A_CABALLO;
			}
			else {
				if (s[4] == 'q' || s[4] =='Q')
					tipo = PROMOCION_A_DAMA;
				else if (s[4] == 'r' || s[4] == 'R')
					tipo = PROMOCION_A_TORRE;
				else if (s[4] == 'b' || s[4] == 'B')
					tipo = PROMOCION_A_ALFIL;
				else if (s[4] == 'n' || s[4] == 'N')
					tipo = PROMOCION_A_CABALLO;
			}
			/* si no es un Pocket Pc */
#else
			if (s[4] == 'q' || s[4] =='Q')
				tipo = PROMOCION_A_DAMA;
			else if (s[4] == 'r' || s[4] == 'R')
				tipo = PROMOCION_A_TORRE;
			else if (s[4] == 'b' || s[4] == 'B')
				tipo = PROMOCION_A_ALFIL;
			else if (s[4] == 'n' || s[4] == 'N')
				tipo = PROMOCION_A_CABALLO;
#endif
		}
	}
	return tipo;
}

/*la siguiente función va a comprobar si llega un movimiento del usuario y si este movimiento es legal o no*/
void Comprobar_Movimiento(char *s)
{
	int de, a, tipo, i;
	int contar;
	movimiento lista[256];

	/* comprobamos si recibimos un enroque corto en chess960 con el protocolo xboard y uci de Arena */
	if (strcmp (s,"O-O") == 0) {
		if (turno == BLANCO) {
			de = rb;
			a = 62;
		}
		else {
			de = rn;
			a = 6;
		}
		tipo = ENROQUE;
	}
	/* comprobamos si recibimos un enroque largo en chess960 con el protocolo xboard y uci de Arena */
	else if (strcmp (s,"O-O-O") == 0) {
		if (turno == BLANCO) {
			de = rb;
			a = 58;
		}
		else {
			de = rn;
			a = 2;
		}
		tipo = ENROQUE;
	}
	else {
	/* comprueba si recibimos un movimiento en coordenadas algebraicas */
		if (s[0] < 'a' || s[0] > 'h' ||
			s[1] < '0' || s[1] > '9' ||
			s[2] < 'a' || s[2] > 'h' ||
			s[3] < '0' || s[3] > '9') {
			printf("Error (unknown command): %s\n", s);     /* si no es un movimiento debe ser un comando desconocido */
			return;
			}

		de = s[0] - 'a';
		de += 8 * (8 - (s[1] - '0'));
		a = s[2] - 'a';
		a += 8 * (8 - (s[3] - '0'));
		tipo = NORMAL;

		/* comprobamos si recibimos un enroque en chess960 en el protoco uci (el rey captura la torre) */
		if (pieza[de] == REY && pieza[a] == TORRE) {
			if (color[de] == color[a]) {
				tipo = ENROQUE;
				if (turno == BLANCO) {
					if (a == tb1) a = 62;
					else if (a == tb2) a = 58;
				}
				else {
					if (a == tn1) a = 6;
					else if (a == tn2) a = 2;
				}
			}
		}

		/* comprueba si es un tipo de movimientos especial */
		if (Comprobar_Tipo_Movimiento(de, a, s) != NINGUNO)
			tipo = Comprobar_Tipo_Movimiento(de, a, s);
	}

	ply = 0;

    /* nosotros generamos una lista con todos los movimientos posibles */
	contar = Generar(lista);

    /* recorre todos los movimientos para ver si lo que nos ha llegado coincide con alguno de los movimientos generados y por tanto
	saber si lo que recibimos es un movimiento legal */
	for (i = 0; i < contar; i++) {
		if (lista[i].de == de && lista[i].a == a && lista[i].tipo == tipo) {
			if (Hacer_Movimiento(lista[i]))			/* hacemos el movimiento si está en nuestra lista */
				return;
			else {
				printf("Illegal move: %s\n", s);
				return;
			}
		}
	}
	printf("Illegal move: %s\n", s);	            /* si el movimiento no es legal se lo dice a la GUI */
	return;
}

/* la siguiente funcion permite salir de una busqueda si llega una interrupcion por parte de la GUI o permite saber cuando tenemos
que dejar de ponderar y empezar a pensar, esta función esta basada en el programa Olithink y practicamente todos los motores utilizan
la misma */
int bioskey(void)
{
#ifndef WINDOWS
	fd_set readfds;

	FD_ZERO (&readfds);
	FD_SET (fileno(stdin), &readfds);
	tv.tv_sec=0; tv.tv_usec=0;
	select(fileno(stdin)+1, &readfds, 0, 0, &tv);
	return (FD_ISSET(fileno(stdin), &readfds));
#else
    #if defined (UNDER_CE)
        return InputAvailable();
    #else
	    static int init = 0, pipe;
	    static HANDLE inh;
	    DWORD dw;

	    if (!init) {
		    init = 1;
		    inh = GetStdHandle(STD_INPUT_HANDLE);
		    pipe = !GetConsoleMode(inh, &dw);
		    if (!pipe) {
			    SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
			    FlushConsoleInputBuffer(inh);
		    }
	    }
	    if (pipe) {
		    if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) {
			    return 1;
		    }
		    return dw > 0;
	    } else {
		    GetNumberOfConsoleInputEvents(inh, &dw);
		    return dw > 1;
	    }
    #endif
#endif
}

/* funcion utilizada para saber que comando nos llega en caso de haber actuado la funcion bioskey() */
void readline(char *str, int n)
{
	char *ptr;

	if (fgets(str, n, stdin) == NULL)
		exit(0);
	if ((ptr = strchr(str, '\n')) != NULL)
		*ptr = '\0';
	return;
}

#ifndef HAVE_STRTOK_R
char *strtok_r(char *s, const char *sep, char **p)
{
    if (!s && !(s = *p))
        return NULL;

    s += strspn(s, sep);
    if (!*s)
        return *p = 0;
    *p = s + strcspn(s, sep);

    if (**p)
        *(*p)++ = 0;
    else
        *p = 0;
    return s;
}
#endif


/* con get_ms() obtenemos la hora actual en milisegundos desde el 1 de enero de 1970 y es la funcion que nos permitira controlar el
tiempo que utiliza el motor para hacer su movimiento haciendo una simple resta en la hora actual y la inicial */
int get_ms(void)
{
#ifndef WINDOWS
	gettimeofday (&tv, &tz);
	return(tv.tv_sec * 1000 + (tv.tv_usec / 1000));
#else
    return GetTickCount();
#endif
}
