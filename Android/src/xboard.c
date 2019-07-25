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
Función xboard(), sustituye a la función principal si recibimos el parámetro xboard, gracias a esta funcion podremos jugar con el motor en
aquellos interfaces de usuario que soporten el protocolo Winboard/xboard como por ejemplo sobre Winboard GUI, Arena, Chessmaster GUI, etc.
Para comprender dicho protocolo se debe examinar la siguiente página:
http://home.hccnet.nl/h.g.muller/engine-intf.html
*/

/*Includes*/
#if defined (UNDER_CE)
	#include <windows.h>
#endif
#include <stdio.h>			/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <string.h>
#include <stdlib.h>			/*malloc, free, exit ...*/
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

void xboard(void) /*protocolo xboard*/
{
	char linea[256], comando[256], comando2[256];	/*para leer la información del GUI y saber que comando llega*/
	char args[4][64];								/*para el comando setboard*/
	int entrada = 0;                                /*recibe el número de movimientos para el control de tiempo*/
	char minutos[4];	                            /*recibe el número de minutos base para el control de tiempo*/
	int incremento;	                                /*recibe el número de segundos de incremento para el control de tiempo*/
	int contar;		                                /*numero de movimientos posibles en una posición*/
	movimiento lista[256];					        /*lista de movimientos posibles y mejormovimiento*/
	movimiento mejormovimiento, pondermovimiento;

/*En windows o linux podemos intentar eliminar el problema de buffering al enviar algo a xboard*/
#if defined (UNDER_CE)

#else
	setbuf(stdout, NULL);
#endif

	printf("feature done=0\n");		/* nos va a dar tiempo para iniciar el motor hasta que enviemos feature done=1 */

	/* configuramos la posición inicial, esto posiblemente no es necesario ya que lo haremos de nuevo cuando recibamos el comando new */
	variante[0] = '\0';
	strcpy(variante,"normal");
	strcpy(fenstring, "");
	strcat(fenstring, STARTPOS);
	SetBoard(fenstring);
	motor = NEGRO;

	/* proporcionamos información sobre el motor y autor, no se verá en la GUI, solo en algun tipo de archivo de depuracion o log */
	printf("# \n");
	printf("# %s is a chess engine with xboard and uci protocol.\n", nombre);
	printf("# Copyright (C) <%s>  <Pedro Castro> \n", fecha);
	printf("# Basque Contry, Spain \n");
	printf("# Web Page: http://sites.google.com/site/danasah/ \n");

	/* para poder configurar el protocolo xboard hacemos uso de un archivo de configuración llamado danacfg.txt */
	leer_inifile();
	/* otra posible opcion de configuracion es enviando los parametros como argumentos cuando lanzamos el motor, esto podria evitar tener
	un archivo de configuracion. Si hemos enviado alguno de estos parametros junto al ejecutable entonces modificaremos su valor, podemos
	ver como son capturados estos parametros si fuese el caso en la funcion main(). Para saber que guarda cada parametro se puede ver
	una informacion mas detallada en la funcion ParseSetOptionCommand() en el archivo uci.c */
	if (leer_configuracion)
		Analiza_parametros();

#if defined (UNDER_ELO)
    /* en la version limitada de fuerza modificaremos la fuerza en funcion del oponente */
	if (!strcmp(limitstrength, "true")) Nivel();
#endif
	/* tablas hash */
	alloc_hash();	/*reservamos memoria para las tablas hash*/
	inicia_hash();	/*inicia las tablas hash*/

	/* carga el libro de aperturas, bitbases y tablebases */
	if (strcmp(ownbook, "true") == 0) open_book();
	if (use_egbb == VERDADERO) load_bitbases();
	if (use_egtb == VERDADERO) load_tablebases();

	/* pasamos a un bucle esperando recibir un comando de la GUI o empezamos una busqueda si es nuestro turno o estamos ponderando */
	for (;;) {
/* En el Pocket Pc tenemos que hacer fflush después de cada línea de salida, no necesario en la version Windows o Linux */
#if defined (UNDER_CE)
		fflush(stdout);
#endif
		/* turno del programa */
        if (turno == motor) {
			estado = BUSCANDO;										/* ponemos el el flag de estado como buscando */
			MotorPiensa(&mejormovimiento, &pondermovimiento);		/* piensa en la mejor PV y envia informacion a la GUI */
			Hacer_Movimiento(mejormovimiento);	    				/* guarda el movimiento en la lista de jugadas */
			Enviar_Movimiento(mejormovimiento, pondermovimiento);   /* envia el movimiento a la GUI */
			contar = Generar(lista);		  	    				/* genera los movimientos del contrario para saber si damos mate */
			imprime_resultado(contar, lista);	    				/* si es final de partida imprime el resultado */
#if defined (UNDER_CE)
			/* en el PocketPc solo activamos la opcion de Ponder si tenemos suficiente tiempo ya que le cuesta al motor salir de dicho
			modo y podria perder algunas partidas por tiempo */
			if (max_tiempo > 5000)
				hacer_ponder = VERDADERO;
			else
				hacer_ponder = FALSO;
#else
			hacer_ponder = VERDADERO;
#endif
			continue;
        }
		else if (motor != VACIO) {
    		if (ponder && hacer_ponder) {		    					/* solo pondera si la GUI lo permite */
				estado = PONDERANDO;									/* ponemos el flag de estado como ponderando */
#if defined (UNDER_CE)
				if (fuera_del_libro)									/* en el PocketPC no ponderamos mientras estamos en el libro de aperturas */
					MotorPiensa(&mejormovimiento, &pondermovimiento);	/* piensa en la mejor PV mientras el motor pondera */
#else
				MotorPiensa(&mejormovimiento, &pondermovimiento);		/* piensa en la mejor PV mientras el motor pondera */
#endif
				hacer_ponder = FALSO;			    					/* deja de ponderar */
    			continue;
            }
        }
		/* comprobamos el comando que nos llega de la GUI */
#if defined (UNDER_CE)
		if (gets(linea) == NULL)
			return;
#else
		if (!fgets(linea, 256, stdin))
			return;
#endif
		if (linea[0] == '\n')
			continue;
		sscanf(linea, "%s", comando);

		/* si llega xboard simplemente continuamos */
		if (strcmp(comando, "xboard") == 0) {
			continue;
		}
		/* comprueba si tenemos protocolo winboard 2 y definimos algunas características */
		if (strcmp(comando,"protover") == 0) {
			printf("feature colors=0 draw=0 ics=1 myname=\"%s\" ping=1 setboard=1 sigint=0 sigterm=0 variants=\"normal,nocastle,fischerandom\"\n", nombre);
			printf("feature done=1\n");
			continue;
		}
		/* nos indica si las características definidas son aceptadas o no */
		if (strcmp(comando, "accepted") == 0) {
			/*printf("feature accepted\n");*/
			continue;
		}
		if (strcmp(comando, "rejected") == 0) {
			printf("feature rejected\n");
			continue;
		}
		/* comprueba si hay que empezar nueva partida */
		if (strcmp(comando, "new") == 0) {
			variante[0] = '\0';
			strcpy(variante,"normal");
			strcpy(fenstring, "");
			strcat(fenstring, STARTPOS);
			SetBoard(fenstring);
			motor = NEGRO;
			continue;
		}
		/* nos indica si jugamos una variante que no es la normal */
		if (strcmp(comando, "variant") == 0) {
			sscanf(linea, "variant %s", comando2);
			variante[0] = '\0';
			strcpy(variante,comando2);
			continue;
		}
		/* comando para cerrar el motor */
		if (strcmp(comando, "quit") == 0)
			return;
		/* comando que en GNUChess añadía un pequeño valor aleatorio a la evaluacion, nosotros no lo usamos */
		if (strcmp(comando, "random") == 0)
			continue;
		/* el motor deja de jugar */
		if (strcmp(comando, "force") == 0) {
			motor = VACIO;
			continue;
		}
		/* fuerza al motor a jugar */
		if (strcmp(comando, "go") == 0) {
			motor = turno;
			continue;
		}
		/* turno para las blancas, el motor juega negras */
		if (strcmp(comando, "white") == 0) {
			turno = BLANCO;
			motor = NEGRO;
			continue;
		}
		/* turno para las negras, el motor juega blancas */
		if (strcmp(comando, "black") == 0) {
			turno = NEGRO;
			motor = BLANCO;
			continue;
		}
		/* parametros para configurar el nivel de juego del programa	level 40 5 0 --> 40 movimientos 5 minutos incremento 0
		si el número de movimientos es 0 la partida es a finish */
		if (strcmp(comando, "level") == 0) {
			sscanf(linea, "level %d %s %d", &entrada, minutos, &incremento);
			/* no necesitamos capturar el valor de minutos ya que normalmente las GUIS envian cada vez que el motor tiene que pensar el
			tiempo que le queda y también el tiempo del oponente */
			control = mps = entrada;	    			/* nos interesa saber cuantos movimientos para el control */
			inc = incremento * 1000;					/* el incremento nos lo envian en segundos, internamente trabajamos en mseg */
			continue;
		}
		/* configura el nivel de juego indicando cuantos segundos por movimiento	st 10 --> 10 segundos por jugada */
		if (strcmp(comando, "st") == 0) {
			sscanf(linea, "st %d", &max_tiempo);
			tiempo_total = max_tiempo *= 1000;			/* maximo tiempo para buscar (internamente trabajamos con milisegundos) */
			max_profundidad = MAX_PROFUNDIDAD;			/* ponemos la profundida al maximo */
			/* en este modo de tiempo no gestionamos el tiempo para cambios de iteración o nuevos movimientos */
			no_nueva_iteracion = tiempo_total;
			no_nuevo_movimiento = tiempo_total;
			limite_tiempo = tiempo_total;
			continue;
		}
		/* configura el nivel de juego indicando la profundidad a jugar	sd 8 --> juega hasta profundidad 8 */
		if (strcmp(comando, "sd") == 0) {
			sscanf(linea, "sd %d", &max_profundidad);
			if (max_profundidad >  MAX_PROFUNDIDAD)
				max_profundidad =  MAX_PROFUNDIDAD;		/* controlamos que la profundida no es mayor que la que puede jugar el motor */
			tiempo_total = 1 << 25;						/* simulamos que tenemos tiempo infinito */
			/* en este modo de tiempo no gestionamos el tiempo para cambios de iteración o nuevos movimientos */
			no_nueva_iteracion = tiempo_total;
			no_nuevo_movimiento = tiempo_total;
			limite_tiempo = tiempo_total;
			continue;
		}
		/* recibimos de la GUI el tiempo que nos queda en centisegundos */
		if (strcmp(comando, "time") == 0) {
			sscanf(linea, "time %d", &max_tiempo);
			tiempo_total = max_tiempo *= 10;			/*pasamos a milisegundos que es como trabajamos internamente*/
			/* mantenemos un margen de seguridad en cada movimiento para evitar perdidas por tiempo en el movimiento anterior al control */
			if (tiempo_total > tiempo_seguridad)
				tiempo_total -= tiempo_seguridad;
			else
				tiempo_total /=2;
			/* con tiempos sin control de movimientos suponemos que quedan x movimientos */
			if (mps == 0) {
#if defined (UNDER_CE)
				if (njugadas < 40) quedan = 40;
				else if (njugadas < 80) quedan = 35;
				else if (njugadas < 120) quedan = 30;
				else quedan = 25;
#else
				quedan = 40;
#endif
			}
			/* si estamos jugando con tiempos de control necesitamos saber cuantos movimientos quedan para llegar al control */
			else {
				if (motor == BLANCO)
					quedan = (control-njugadas/2)-1;
				else
					quedan = (control-njugadas/2);
				while (quedan <= 0) {
					control += mps;
					quedan += mps;
				}
			}
			/* en tiempos con incremento si queda poco tiempo tratamos de utilizar menos y recuperarnos gracias al incremento para
			no peder en el tiempo */
			if (inc > 0 && tiempo_total < 5*inc) {
				tiempo_total = 75*inc/100;
				no_nueva_iteracion = tiempo_total/2;
				no_nuevo_movimiento = tiempo_total;
				limite_tiempo = tiempo_total;
			}
			/* gestion del tiempo segun el numero de movimiento que quedan para el control */
			else {
				no_nueva_iteracion = ((tiempo_total/2) / quedan) + inc;
				no_nuevo_movimiento = ((3*tiempo_total/2) / (quedan + (1/2))) + inc;
				limite_tiempo = ((10*tiempo_total) / (quedan + 9)) + inc;
			}
			/* esto no resuelve el problema de que en juegos sin incremento a finish el motor pueda perder en el tiempo y es que esto es
			dificilmente tratable ya que no sabemos de antemano el número de movimientos que se jugarán en la partida por lo que es complicado
			establecer una gestion optima, pero en cualquier caso el motor jugara posiblemente mas de 150 movimientos */
			continue;
		}
		/* tiempo del contrario, no lo utilizamos de momento en nuestro motor */
		if (strcmp(comando, "otim") == 0) {
			continue;
		}
		/* control para winboard, recibimos un ping y enviamos un pong */
		if (strcmp(comando,"ping") == 0){
			int n;
			sscanf(linea, "ping %d", &n);
			printf("pong %d\n",n);
			continue;
		}
		/* si recibimos un resultado de un juego el motor debe parar */
		if (strcmp(comando, "result") == 0) {
#if defined (UNDER_ELO)
			informa();
#endif
			motor = VACIO;
			continue;
		}
		if (strcmp(comando, "?") == 0) {
			motor = VACIO;
			continue;
		}
		if (strcmp(comando, ".") == 0) {
			continue;
		}
		if (strcmp(comando, "exit") == 0) {
			continue;
		}
		/* configuramos el tablero con una posición en formato FEN */
		if (strcmp(comando, "setboard") == 0) {
			strcpy(fenstring, "");
			sscanf(linea, "setboard %s %s %s %s", args[0],args[1],args[2],args[3]);
			strcat(fenstring, args[0]);
			strcat(fenstring, args[1]);
			strcat(fenstring, args[2]);
			strcat(fenstring, args[3]);
			SetBoard(fenstring);
			continue;
		}
		/* viejo comando edit de winboard para configurar una posición, no usado con el protocolo 2 */
		if (strcmp(comando, "edit") == 0) {
			continue;
		}
		/* damos una pista del mejor movimiento al usuario si nos lo pide */
		if (strcmp(comando, "hint") == 0) {
			char c;
			movimiento mh;
			tiempo_total = 5000;
			max_profundidad = MAX_PROFUNDIDAD;
			no_nueva_iteracion = tiempo_total;
			no_nuevo_movimiento = tiempo_total;
			limite_tiempo = tiempo_total;
			estado = BUSCANDO;
			MotorPiensa(&mejormovimiento, &pondermovimiento);
			mh = mejormovimiento;
			if (!(mh.de == 0 && mh.a == 0)) {
				/*letra a imprimir si hay una promoción*/
				switch (mh.tipo) {
					case PROMOCION_A_DAMA:
						c = 'q';
						break;
					case PROMOCION_A_TORRE:
						c = 'r';
						break;
					case PROMOCION_A_ALFIL:
						c = 'b';
						break;
					case PROMOCION_A_CABALLO:
						c = 'n';
						break;
					default:
						c = ' ';
				}
				printf("Hint: %c%d%c%d%c",
					'a' + COLUMNA(mh.de),
					8 - FILA(mh.de),
					'a' + COLUMNA(mh.a),
					8 - FILA(mh.a),
					c);
			}
			continue;
		}
		/* si el usuario selecciona Book en el menu, no usado */
		if (strcmp(comando, "bk") == 0) {
			continue;
		}
		/* deshacemos un movimiento */
		if (strcmp(comando, "undo") == 0) {
			if (njugadas == 0)
				continue;
			Deshacer();
			continue;
		}
		/* deshacemos los 2 últimos movimientos y continua el mismo color */
		if (strcmp(comando, "remove") == 0) {
			if (njugadas < 2)
				continue;
			Deshacer();
			Deshacer();
			continue;
		}
		/* ponder, pensar con el tiempo del contrario */
		if (strcmp(comando, "hard") == 0) {
			ponder = VERDADERO;
			continue;
		}
		/* no ponder */
		if (strcmp(comando, "easy") == 0) {
			ponder = FALSO;
			continue;
		}
		/* imprimimos el pensamiento */
		if (strcmp(comando, "post") == 0) {
			post = VERDADERO;
			continue;
		}
		/* no imprimir el pensamiento */
		if (strcmp(comando, "nopost") == 0) {
			post = FALSO;
			continue;
		}
		/* modo de analisis con la GUI */
		if (strcmp(comando, "analyze") == 0) {
			tiempo_total = 1 << 25;								/* simulamos tiempo infinito */
			max_profundidad = MAX_PROFUNDIDAD;					/* ponemos maxima profundidad */
			no_nueva_iteracion = tiempo_total;					/* no paramos la busqueda por tiempo */
			no_nuevo_movimiento = tiempo_total;
			limite_tiempo = tiempo_total;
			estado = ANALIZANDO;								/* ponemos el flag de analizando */
			MotorPiensa(&mejormovimiento, &pondermovimiento);	/* enviamos el mejor movimiento a la GUI */
			motor = VACIO;										/* paramos el motor */
			continue;
		}
		/* nombre del oponente */
		if (strcmp(comando, "name") == 0) {
			continue;
		}
		/* rating del oponente cuando estamos en modo ICS */
		if (strcmp(comando, "rating") == 0) {
			continue;
		}
		/* nombre del servidor de Internet donde jugamos */
		if (strcmp(comando, "ics") == 0) {
			sscanf(linea, "ics %s", comando2);
			servidor[0] = '\0';
			strcpy(servidor,comando2);
			continue;
		}
		/* el oponente es un motor */
		if (strcmp(comando, "computer") == 0) {
			continue;
		}
#if defined (UNDER_CE)
		if (strcmp(comando, "log") == 0) {
			continue;
		}
		if (strcmp(comando, "output") == 0) {
			continue;
		}
		if (strcmp(comando, "unrated") == 0) {
			continue;
		}
		if (strcmp(comando, "rated") == 0) {
			continue;
		}
#endif
		Comprobar_Movimiento(comando);			/* comprueba si llega un movimiento del usuario y es legal */
		contar = Generar(lista);		  	    /* genera los movimientos del contrario para saber si ha recibido mate */
		imprime_resultado(contar, lista);	    /* si es final de partida imprime el resultado */
	}
}

#if defined (UNDER_ELO)
/* esta funcion informa al finalizar el juego con la version limitada con que nivel ha jugado el motor */
void informa(void){
	if (strcmp (elo_type, "human") == 0) {
		#if defined (UNDER_CE)
		if (easylevel == 1) {
			sprintf(cadena, "DanaSah has played with Random move\n");
		}
		else if (easylevel >= 2){
			sprintf(cadena, "DanaSah has played with ELO human %d", elo + diff_human_computer + diff_computer_engine);
		}
		MessageBox(0, cadena, "Informacion", 0);
		#else
		if (easylevel == 1) {
			printf("tellall DanaSah has played with Random move\n");
		}
		else if (easylevel >= 2){
			printf("tellall DanaSah has played with ELO human %d\n", elo + diff_human_computer + diff_computer_engine);
		}
		#endif
	}
}
#endif
