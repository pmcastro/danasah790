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
Función uci(), sustituye a la función principal si recibimos el parámetro uci, gracias a esta funcion podemos hacer jugar a DanaSah
con interfaces de usuario o GUIS que manejen dicho protocolo, como Fritz GUI, Shredder GUI, Arena, etc.
Para comprender dicho protocolo ver:
https://ucichessengine.wordpress.com/2011/03/16/description-of-uci-protocol/
*/

/*Includes*/
#include <stdio.h>		/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <string.h>
#include <stdlib.h>		/*malloc, free, exit ...*/
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"
#include "gtb/gtb-probe.h"

char *SavePointer;
int num_moves;					/* cuenta el numero de movimientos enviados por los comandos starposition o fen */

/* gracias al comando ucinewgame vamos a poder saber si en cada movimiento el motor necesita gestionar toda la lista de movimientos recbiddos
con el comando position o fen o por si el contrario vale solo con realizar los 2 ultimos movimientos recibidos (nuestro y adversario), con
esto ganamos algo de tiempo, esto es algo que siempre ha sido muy discutido en la gestion del protocolo uci */
int ucinewgame_command = 1;		/* permite saber si hemos recibido el comando ucinewgame, puesto inicialmente a 1 porque Fritz no envía el comando en la primera partida */
int gui_has_ucinewgame = 1;		/* permite controlar si la GUI envía el comando ucinewgame con cada juego o no, por defecto pensamos que si */
int change_options_hash = 1;
int change_options_book = 1;
int change_options_bitbases = 1;
int change_options_tablebases = 1;
#if defined (UNDER_ELO)
int change_options_niveles = 1;
#endif

void ParseGoCommand();						/* trata el comando recibido go */
void ParsePositionCommand();				/* trata el comando recibido position */
void ParseSetOptionCommand(char *Line);		/* trata el comando recibido setoption */
int StringToBool(const char *String);		/* comprueba si la cadena recibida es true o false */


/* si el motor juega con el protocolo uci entramos en esta funcion */
void uci() {
	char Line[10000];	        /* definimos el tamaño de la linea que vamos a recibir para los comandos que envía la GUI */

	/* En windows, linux o Android podemos intentar eliminar el problema de buffering */
	setbuf(stdout, NULL);

	printf("\n");

	/* Identificamos el nombre del motor y del autor */
	printf("id name %s\n", nombre);
	printf("id author Pedro Castro\n");

	/* las siguientes opciones son solo para la version limitada en fuerza */
#if defined (UNDER_ELO)
	printf("option name UCI_LimitStrength type check default true\n");
	printf("option name UCI_Elo type spin default 2000 min 0 max 2500\n");
	printf("option name Opponent type combo default human var human var computer var engine\n");
	printf("option name Easy Level type combo default Elo_Regulation var Elo_Regulation var Random_Move var Newcomer var Beginner var Weak_Player_Club var Average_Player_Club var Strong_Player_Club var Expert var Candidate_Master var FIDE_Master var International_Master\n");
	printf("option name Style type spin default 3 min 1 max 5\n");
	printf("option name Diff Engine type spin default 0 min -400 max 400\n");
	printf("option name Diff Computer-Engine type spin default 466 min 0 max 800\n");
	printf("option name Diff Human-Computer type spin default 70 min 0 max 100\n");
	printf("option name OwnBook type check default false\n");
#else
    printf("option name OwnBook type check default false\n");
#endif

	/* envíamos a la GUI las opciones que el motor maneja */
#if defined (UNDER_ARM)
    printf("option name Book File type string default /storage/sdcard1/book/book.bin\n");
	printf("option name Hash type spin default 64 min 8 max 1024\n");
	printf("option name EndGames Tablebases type combo default Gaviota var None var Gaviota\n");
	printf("option name GaviotaTbPath type string default /storage/sdcard1/gtb/\n");
	printf("option name GaviotaTbCache type spin default 16 min 16 max 128\n");
#else
	printf("option name Book File type string default ../book/book.bin\n");
	printf("option name Hash type spin default 256 min 8 max 1024\n");
	printf("option name EndGames Tablebases type combo default Gaviota-Scorpio var None var Gaviota var Scorpio var Gaviota-Scorpio\n");
	printf("option name Bitbases Path type string default ../egbb/\n");
    printf("option name Bitbases Men type spin default 4 min 4 max 6\n");
	printf("option name Bitbases Cache Size type spin default 4 min 4 max 128\n");
	printf("option name Bitbases Load type combo default LOAD_4MEN var LOAD_NONE var LOAD_4MEN var SMART_LOAD var LOAD_5MEN\n");
	printf("option name GaviotaTbPath type string default ../gtb/gtb4/\n");
	printf("option name GaviotaTbCache type spin default 16 min 16 max 128\n");
#endif

	printf("option name Ponder type check default true\n");
	printf("option name MultiPV type spin default 1 min 1 max 64\n");
	printf("option name UCI_AnalyseMode type check default false\n");
	printf("option name UCI_Chess960 type check default false\n");
	printf("option name GUI support 'Ucinewgame Command' type check default true\n");
	printf("option name Clear Hash type button\n");
	printf("option name Clear Tablebases Cache type button\n");

	printf("uciok\n");

#if defined (UNDER_ELO)
	printf("info Default Elo human : 2000\n");
#endif

	/* ahora entramos en el ciclo en el que el motor comprueba los comando que llegan de la GUI */
	for (;;) {

		if (fgets(Line, 10000, stdin)==NULL)
			return;

		/* Removemos el caracter de nueva linea */
		if (Line[strlen(Line)-1]=='\n')
			Line[strlen(Line)-1]='\0';

        /* Comprobamos el comando recibido */
		char *Command=strtok_r(Line, " ", &SavePointer);

		if (Command==NULL)
			return;

		if (!strcmp(Command, "uci")) {
			continue;
		}
		if (!strcmp(Command, "isready")) {
			if (change_options_hash == 1) {
                free_hash();
#if defined (UNDER_ARM)
                /* en Android ponemos el tamaño de las tablas hash inicialmente en 64 Mb */
                TTSize = (64 * 1024 * 1024) / sizeof(TType);
                ETSize = (8 * 1024 * 1024) / sizeof(EType);
#else
                /* en Windows y Linux usamos inicialmente 256 Mb para las tablas hash */
                TTSize = (256 * 1024 * 1024) / sizeof(TType);
                ETSize = (32 * 1024 * 1024) / sizeof(EType);
#endif
                alloc_hash();
                inicia_hash();
                clear_hash();
			}
			if (change_options_book == 1) {
                if (libro_cargado)
                    close_book();
				if (strcmp(ownbook, "true") == 0)
                    open_book();
			}
			if (change_options_bitbases == 1) {
                if (egbb_is_loaded)
                    unload_bitbases();
                if (use_egbb == VERDADERO)
                    load_bitbases();
			}
			if (change_options_tablebases == 1) {
			    if (use_egtb == VERDADERO) {
                    if (!egtb_is_loaded)
                        load_tablebases();
                    else
                        reset_tablebases();
			    }
			    else {
			        if (egtb_is_loaded)
                        unload_tablebases();
			    }
			}
#if defined (UNDER_ELO)
			if (change_options_niveles == 1) {
                Nivel();
			}
#endif
            change_options_hash = 0;
			change_options_book = 0;
			change_options_bitbases = 0;
			change_options_tablebases = 0;
#if defined (UNDER_ELO)
            change_options_niveles = 0;
#endif
			printf("readyok\n");
			continue;
		}
		if (!strcmp(Command, "quit")) {
			printf("info Bye\n");
			exit(0);
		}
		if (!strcmp(Command, "ucinewgame")) {
			ucinewgame_command = 1;		/* controlamos cuando la GUI nos envia el comando ucinewgame */
			clear_hash();				/* no es necesario borrar las hash con cada nuevo juego pero tampoco seguramente ganamos nada con no hacerlo */
			fuera_del_libro = 0;
			continue;
		}
		if (!strcmp(Command, "go")) {
			ParseGoCommand();
			continue;
		}
		if (!strcmp(Command, "position")) {
			ParsePositionCommand();
			ucinewgame_command = 0;		/* una vez que ya hemos realizado al menos 1 jugada en la partida reseteamos el comando ucinewgame */
			continue;
		}
		if (!strcmp(Command, "setoption")) {
			ParseSetOptionCommand(Line+strlen(Command)+1);
			continue;
		}
	}
}

/* Comprobamos las opciones del comando Go */
void ParseGoCommand() {
	char *Opc;										/* guarda el comando recibido despues de go */
	int tb = 0, tn = 0, ib = 0, in = 0;				/* almacena tiempos e incrementos */
	movimiento mejormovimiento, pondermovimiento;	/* almacena el mejor movimiento y sobre el que ponderar */
	quedan = 40;									/* si no hay número de movimientos para el control imaginamos que quedan 40 */
	hacer_ponder = FALSO;							/* no pondera si no recibimos el comando ponder */
	pdm = FALSO;									/* comprueba si recibimos el comando depth o movetime */

	while((Opc=strtok_r(NULL, " ", &SavePointer))!=NULL) {
		/* modo tiempo infinito o modo análisis, no diferenciamos entre los 2 modos */
		if (!strcmp(Opc, "infinite")) {
			tiempo_total = 1 << 25;									/* simulamos tiempo infinito */
			max_profundidad = MAX_PROFUNDIDAD;						/* ponemos maxima profundidad */
			no_nueva_iteracion = tiempo_total;						/* no paramos la busqueda por tiempo */
			no_nuevo_movimiento = tiempo_total;
			limite_tiempo = tiempo_total;
			estado = ANALIZANDO;									/* ponemos el flag de analizando */
			if (multipv > 1)
                multiMotorPiensa(&mejormovimiento, &pondermovimiento);	/* iniciamos la busqueda de la mejor PV enviando info a la GUI*/
			else
                MotorPiensa(&mejormovimiento, &pondermovimiento);		/* iniciamos la busqueda de la mejor PV enviando info a la GUI*/
			Enviar_Movimiento(mejormovimiento, pondermovimiento);	/* enviamos el mejor movimiento y si procede el movimiento a ponderar a la GUI */
			motor = VACIO;											/* paramos el motor */
			return;													/* podemos retornar, despues de infinite ya no vienen mas comandos */
		}
		/* opciones de tiempo e incremento para blancas y negras */
		if (!strcmp(Opc, "wtime")) {
			tb = atoi(strtok_r(NULL, " ", &SavePointer));
			continue;
		}
		if (!strcmp(Opc, "btime")) {
			tn = atoi(strtok_r(NULL, " ", &SavePointer));
			continue;
		}
		if (!strcmp(Opc, "winc")) {
			ib = atoi(strtok_r(NULL, " ", &SavePointer));
			continue;
		}
		if (!strcmp(Opc, "binc")) {
			in = atoi(strtok_r(NULL, " ", &SavePointer));
			continue;
		}
		/* numero de movimentos que faltan para el control de tiempo */
		if (!strcmp(Opc, "movestogo")) {
			quedan = atoi(strtok_r(NULL, " ", &SavePointer));
			quedan += 2;	/* añadimos 2 para no ajustar demasiado y que no haya problemas en los ultimos movimientos antes del control */
			continue;
		}
		/* modo profundidad fija */
		if (!strcmp(Opc, "depth")) {
			max_profundidad = atoi(strtok_r(NULL, " ", &SavePointer));
			if (max_profundidad >  MAX_PROFUNDIDAD)
				max_profundidad =  MAX_PROFUNDIDAD;		/* comprobamos que la profundida no exceda a la que puede jugar el programa */
			tiempo_total = 1 << 25;						/* en profundidad fija no miramos el tiempo */
			pdm = VERDADERO;
			continue;
		}
		/* modo tiempo fijo por movimiento */
		if (!strcmp(Opc, "movetime")) {
			tiempo_total = atoi(strtok_r(NULL, " ", &SavePointer));		/* tiempo maximo para el movimiento */
			max_profundidad = MAX_PROFUNDIDAD;							/* en este modo el programa puede jugar todo lo profundo que pueda */
			pdm = VERDADERO;
			continue;
		}
		/* si la GUI lo permite es tiempo de ponderar en este movimiento */
		if (!strcmp(Opc, "ponder")) {
			hacer_ponder = VERDADERO;
			continue;
		}
	}

	/* si hemos recibido el comando depth o movetime */
	if (pdm == VERDADERO) {
		inc = 0;		/* el juego es sin incremento */
		quedan = 1;		/* el tiempo indicado por la GUI es solo para un movimiento */
	}
	/* si estamos en un juego con control de tiempo o a finish */
	else {
		max_profundidad =  MAX_PROFUNDIDAD;					/* el programa puede jugar todo lo profundo que pueda */
		if (turno == BLANCO) {
			/* en cada movimiento quitamos un pequeño tiempo de seguridad, en juegos con control esto evitaria perder en el ultimo movimiento
			antes del control, si el tiempo que queda es incluso inferior al tiempo de seguridad en este caso no podemos dejar el tiempo a
			jugar como menor que cero, aqui probamos a jugar casi instantaneamente jugando la mitad de tiempo que queda */
			if (tb > tiempo_seguridad)
				tiempo_total = tb - tiempo_seguridad;
			else
				tiempo_total = tb/2;
			inc = ib;
		}
		else {
			if (tn > tiempo_seguridad)
				tiempo_total = tn - tiempo_seguridad;
			else
				tiempo_total = tn/2;
			inc = in;
		}
	}

	/* pondera si la GUI lo permite y recibimos el comando ponder */
	if (ponder && hacer_ponder) {
		/* previamente guardamos el tiempo, profunidad y movimientos para el control por si hay un ponderhit */
		guarda_profundidad = max_profundidad;
		guarda_tiempo = tiempo_total;
		guarda_incremento = inc;
		guarda_quedan = quedan;
		no_nueva_iteracion = tiempo_total;
		no_nuevo_movimiento = tiempo_total;
		limite_tiempo = tiempo_total;
		estado = PONDERANDO;										/* ponemos el flag de ponderando */
		MotorPiensa(&mejormovimiento, &pondermovimiento);			/* iniciamos la busqueda de la mejor PV desde el movimiento a ponderar enviando info a la GUI */
		Enviar_Movimiento(mejormovimiento, pondermovimiento);		/* enviamos el mejor movimiento y si procede el movimiento a ponderar a la GUI */
		if (estado == BUSCANDO) Hacer_Movimiento(mejormovimiento);	/* si estamos ponderando no es necesario guardar el movimiento en la lista de jugadas del motor */
		hacer_ponder = FALSO;										/* no permitimos ponderar hasta volver a recibir el comando go ponder */
		motor = VACIO;												/* paramos el motor */
		return;														/* podemos retornar */
	}
	/* si no recibimos el comando ponder hacemos una búsqueda con las opciones de tiempo del comando go */
	else {
		/* en tiempos con incremento si queda poco tiempo tratamos de utilizar menos y recuperarnos gracias al incremento para no peder
		 en el tiempo */
		if (pdm) {
			no_nueva_iteracion = tiempo_total;
			no_nuevo_movimiento = tiempo_total;
			limite_tiempo = tiempo_total;
		}
		else if (inc > 0 && tiempo_total < 5*inc) {
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
		dificilmente tratable ya que no sabemos de antemano el numero de movimientos que se jugaran en la partida por lo que es complicado
		establecer una gestion optima, pero en cualquier caso el motor jugara posiblemente mas de 150 movimientos */

		estado = BUSCANDO;											/* ponemos el flag de buscando */
		MotorPiensa(&mejormovimiento, &pondermovimiento);			/* iniciamos la busqueda de la mejor PV enviando info a la GUI */
		Enviar_Movimiento(mejormovimiento, pondermovimiento);		/* enviamos el mejor movimiento y si procede el movimiento a ponderar a la GUI */
		Hacer_Movimiento(mejormovimiento);							/* guardamos el movimiento en la lista de jugadas del motor */
		motor = VACIO;												/* paramos el motor */
		return;														/* podemos retornar */
	}
}

/* comprobamos las opciones del comando position */
void ParsePositionCommand() {
	char *Opc;
	num_moves = -1;

	Opc=strtok_r(NULL, " ", &SavePointer);
	if (Opc==NULL)
		return;

	/* El protocolo uci envia en cada movimiento al motor el comando postion startpos y luego la lista de jugadas realizadas desde la primera.
	Podemos tratar de evitar de realizar en cada movimiento la configuracion inicial y toda la lista de jugadas si sabemos que este
	startpos no corresponde a un nuevo juego ya que en este caso valdria solamente introducir en la lista de jugadas el ultimo realizado
	por el adversario. Por supuesto si nosotros sabemos que estamos recibiendo un comando ucinewgame o si la GUI no envia nunca este
	comando (creo que tambien en caso de ponder) entonces no podemos evitarlo y tenemos que realizar toda la secuencia. Esta forma de
	enviar en cada movimiento la lista completa es una de las cosas mas feas y criticadas del protocolo uci */
	if (!strcmp(Opc, "startpos")) {

		if (ucinewgame_command == 1 || ponder || !gui_has_ucinewgame) {
			strcpy(fenstring, "");
			strcat(fenstring, INIPOS);
			SetBoard(fenstring);			/* llamamos a la funcion que nos configura la posicion inicial mediante un fen inicial */
			variante[0] = '\0';
			strcpy(variante,"normal");
			motor = NEGRO;
		}

		Opc=strtok_r(NULL, " ", &SavePointer);
		if (Opc==NULL || strcmp(Opc, "moves"))
			return;
	}
	/* La GUI podría enviar también con el comando fen una posicion que no es la inicial del ajedrez normal y despues una lista de
	movimientos, en este caso en cada movimiento el motor configurara la posicion fen enviada por la GUI */
	else if (!strcmp(Opc, "fen")) {
		char FEN[1000];
		FEN[0]='\0';

		while((Opc=strtok_r(NULL, " ", &SavePointer))!=NULL) {
			if (!strcmp(Opc, "moves"))
				break;
			strcat(FEN, Opc);
			strcat(FEN, " ");
		}
		SetBoard(FEN);
	}
	/* si despues del comando position no recibimos el comamndo startpos o fen anulamos el comando*/
	else
		return;

	/* Ahora tenemos que gestionar la lista de movimientos recibidos después de las opciones startpos o fen */
	while((Opc=strtok_r(NULL, " ", &SavePointer))!=NULL) {
		num_moves++;
		/* si hemos recibido un comando ucinewgame, si la GUI no envia nunca dicho comando o si estamos ponderando necesitamos gestionar
		cada movimiento de la lista */
		if (ucinewgame_command == 1 || ponder || !gui_has_ucinewgame)
			Comprobar_Movimiento(Opc);			/* comprueba si llega un movimiento (legal) de la GUI y lo hace */
		/* en caso contrario nos podemos ahorrar realizar la configuracion incial y hacer toda la secuencia de movimientos, basta solo
		con hacer los movimientos que no estan en la lista de jugadas del motor */
		else {
			/* si la GUI esta enviando la opcion fen y lista de moves, el motor en este caso hara la lista completa ya que njugadas sera 0 */
			if (njugadas <= num_moves)
				Comprobar_Movimiento(Opc);
		}
	}
}

/* comprobamos las opciones recibidas del comando setoption si ha habido algun cambio de la GUI o el usuario */
void ParseSetOptionCommand(char *Line) {
	char *Name=strstr(Line, "name");
	char *Value=strstr(Line, "value");

	if (Name==NULL || Value==NULL)
		return;
	if (Name+2>=Value)
		return;

	Name+=strlen("name ");
	--Value;
	*Value='\0';
	Value+=strlen(" value ");

	/* cambia el tamaño de la hash si el usuario o GUI lo cambia */
	if (!strcmp(Name, "Hash"))	{
		free_hash();
		TTSize = (atoi(Value) * 1024 * 1024) / sizeof(TType);
		ETSize = (atoi(Value) / 8 * 1024 * 1024) / sizeof(EType);
		alloc_hash();
		inicia_hash();
		clear_hash();
		change_options_hash = 0;
	}
	/* borra las tablas hash tras pulsar el boton correspondiente en las opciones del motor */
	else if (!strcmp(Name, "Clear Hash")) {
		clear_hash();
	}
	/* activa los libros propios por defecto del motor si ownbook es true y los inicia*/
	else if (!strcmp(Name, "OwnBook")) {
		if (StringToBool(Value) == VERDADERO) {
			strcpy(ownbook, "");
			strcat(ownbook, "true");
		}
		else if (StringToBool(Value) == FALSO) {
			strcpy(ownbook, "");
			strcat(ownbook, "false");
		}
		change_options_book = 1;
	}
	/* cambia el libro de torneo, para ello previamente cierra los libros y los inicia de nuevo si el motor gestiona libros propios */
	else if(!strcmp(Name, "Book File")) {
		strcpy(libro, Value);
		change_options_book = 1;
	}
	/* activa las tablas de finales correspondientes */
    else if(!strcmp(Name, "EndGames Tablebases")) {
    	if (!strcmp(Value, "None")) {use_egbb = FALSO; use_egtb = FALSO;}
   		else if (!strcmp(Value, "Gaviota")) {use_egbb = FALSO; use_egtb = VERDADERO;}
   		else if (!strcmp(Value, "Scorpio")) {use_egbb = VERDADERO; use_egtb = FALSO;}
   		else if (!strcmp(Value, "Gaviota-Scorpio")) {use_egbb = VERDADERO; use_egtb = VERDADERO;}
   		change_options_bitbases = 1;
   		change_options_tablebases = 1;
	}
	/* cambia la ruta donde estan las bitbases Scorpio */
	else if(!strcmp(Name, "Bitbases Path")) {
		strcpy(egbb_path, Value);
		change_options_bitbases = 1;
	}
	/* cambia el numero de piezas que deben quedar en el tablero para que el motor consulte las bitbases */
    else if(!strcmp(Name, "Bitbases Men")) {
		egbb_men = atoi(Value);
		change_options_bitbases = 1;
	}
	/* cambia el tamaño de la cache de las bitbases */
	else if(!strcmp(Name, "Bitbases Cache Size")) {
		egbb_cache_size = atoi(Value);
		change_options_bitbases = 1;
	}
	/* cambia la forma de cargar en memoria las bitbases */
    else if(!strcmp(Name, "Bitbases Load")) {
    	if (!strcmp(Value, "LOAD_NONE")) egbb_load_type = 0;
   		else if (!strcmp(Value, "LOAD_4MEN")) egbb_load_type = 1;
   		else if (!strcmp(Value, "SMART_LOAD")) egbb_load_type = 2;
   		else if (!strcmp(Value, "LOAD_5MEN")) egbb_load_type = 3;
   		change_options_bitbases = 1;
	}
	/* cambia la ruta donde estan las tablebases de Gaviota */
	else if(!strcmp(Name, "GaviotaTbPath")) {
		strcpy(egtb_path, Value);
		change_options_tablebases = 1;
	}
	/* cambia el tamaño de la cache de las tablebases */
	else if(!strcmp(Name, "GaviotaTbCache")) {
		egtb_cache_size = atoi(Value);
		change_options_tablebases = 1;
	}
	/* borra el cache de las tablebases de Gaviota */
	else if (!strcmp(Name, "Clear Tablebases Cache")) {
		tbcache_flush();
	}
/* las siguientes opciones son solo para la version limitada en fuerza */
#if defined (UNDER_ELO)
	/* le indica a la GUI que el motor puede jugar con limitada fuerza */
	else if (!strcmp(Name, "UCI_LimitStrength")) {
		if (StringToBool(Value) == VERDADERO) {
			strcpy(limitstrength, "");
			strcat(limitstrength, "true");
		}
		else if (StringToBool(Value) == FALSO) {
			strcpy(limitstrength, "");
			strcat(limitstrength, "false");
			elo = 2500;
			strcpy(elo_type, "engine");
		}
		change_options_niveles = 1;
	}
	/* cambia el valor de los puntos elo con el que el motor juega si tiene activada la fuerza limitada */
    else if(!strcmp(Name, "UCI_Elo")) {
		elo = atoi(Value);
		if (strcmp(limitstrength, "true") == 0)
			change_options_niveles = 1;
	}
	/* indica que clase de oponente es: engine (motor), computer (maquina dedicada de ajedrez) o human (humano) */
	else if (!strcmp(Name, "Opponent")) {
		strcpy(elo_type, Value);
		if ((strcmp(limitstrength, "true") == 0) || easylevel > 1)
			change_options_niveles = 1;
	}
	/* permite seleccionar ademas de la regulacion elo un nivel con movimientos aleatorios o niveles sencillos con fuerza variable */
   else if (!strcmp(Name, "Easy Level")) {
   		if (!strcmp(Value, "Elo_Regulation")) easylevel = 0;
   		else if (!strcmp(Value, "Random_Move")) easylevel = 1;
   		else if (!strcmp(Value, "Newcomer")) easylevel = 2;
   		else if (!strcmp(Value, "Beginner")) easylevel = 3;
   		else if (!strcmp(Value, "Weak_Player_Club")) easylevel = 4;
   		else if (!strcmp(Value, "Average_Player_Club")) easylevel = 5;
   		else if (!strcmp(Value, "Strong_Player_Club")) easylevel = 6;
   		else if (!strcmp(Value, "Expert")) easylevel = 7;
   		else if (!strcmp(Value, "Candidate_Master")) easylevel = 8;
   		else if (!strcmp(Value, "FIDE_Master")) easylevel = 9;
   		else if (!strcmp(Value, "International_Master")) easylevel = 10;
		if (easylevel > 1)
			change_options_niveles = 1;
	}
	/* permite definir un estilo entre 1-5, de momento esta opcion casi no modifica el comportamiento del motor */
    else if(!strcmp(Name, "Style")) {
		style = atoi(Value);
	}
	/* permite regular el elo del motor a diferentes listas cuando el oponente esta puesto como engine */
    else if(!strcmp(Name, "Diff Engine")) {
		diff_engine = atoi(Value);
		if ((strcmp(limitstrength, "true") == 0) || easylevel > 1)
			change_options_niveles = 1;
	}
	/* permite regular el elo del motor a diferentes listas cuando el oponente esta puesto como computer */
    else if(!strcmp(Name, "Diff Computer-Engine")) {
		diff_computer_engine = atoi(Value);
		if ((strcmp(limitstrength, "true") == 0) || easylevel > 1)
			change_options_niveles = 1;
	}
	/* permite regular el elo del motor a diferentes listas cuando el oponente esta puesto como human */
	else if(!strcmp(Name, "Diff Human-Computer")) {
		diff_human_computer = atoi(Value);
		if ((strcmp(limitstrength, "true") == 0) || easylevel > 1)
			change_options_niveles = 1;
	}
#endif
	/* la GUI permite ponderar si recibimos Ponder true */
	else if (!strcmp(Name, "Ponder")) {
		if (StringToBool(Value) == VERDADERO)
			ponder = VERDADERO;
		else
			ponder = FALSO;
	}
	/* utilizamos el modo multipv o k mejores movimientos */
    else if(!strcmp(Name, "MultiPV")) {
		multipv = atoi(Value);
	}
	/* si marcamos la siguiente opcion el motor jugara la variante Chess960 */
	else if (!strcmp(Name, "UCI_Chess960"))	{
		if (StringToBool(Value) == VERDADERO) {
			variante[0] = '\0';
			strcpy(variante,"fischerandom");
		}
		else {
			variante[0] = '\0';
			strcpy(variante,"normal");
		}
	}
	/* marcamos esta opcion si sabemos que el GUI envia el comando ucinewgame en cada nuevo juego */
	else if (!strcmp(Name, "GUI support 'Ucinewgame Command'")) {
		if (StringToBool(Value) == VERDADERO)
			gui_has_ucinewgame = 1;
		else
			gui_has_ucinewgame = 0;
	}
}

/* comprueba si la cadena recibida es true o TRUE */
int StringToBool(const char *String) {
	if (!strcmp(String, "true") || !strcmp(String, "TRUE"))
		return VERDADERO;
	else
		return FALSO;
}

