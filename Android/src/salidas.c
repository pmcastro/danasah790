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
Funciones que envian informacion a la GUI
*/

/*Includes*/
#include <stdio.h>		/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <string.h>
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* muestra informacion para enviar a la GUI sobre el movimiento seleccionado, profundidad, puntuacion, tiempo empleado y nodos examinados,
esta infomacion es enviada para cada ply del pensamiento */
void print_pv(int depth, int score) {
	movimiento	mi;
	int i, j, tiempousado;
	char c;

	i = pv_longuitud[ply];								/* calcula la longuitud de la pv */
	//if (i == 0) return;									/* si la longuitud es cero no tenemos nada que enviar */

	/* si usamos el protocolo xboard y hemos examinado un numero de nodos superior a noise */
	if (nodos > noise && protocolo == XBOARD) {
		/* calculamos el tiempo usado en centisegundos que es lo que reconoce la GUI */
		tiempousado = (get_ms()- tiempo_inicio)/10;

		/* enviamos profundidad, puntos, tiempo (centisegundos), nodos */
		if (estado == PONDERANDO)
			printf("%d %d %d %d", depth, -score, tiempousado, nodos);
		else
			printf("%d %d %d %d", depth, score, tiempousado, nodos);

		/* imprimimos la mejor linea */
		for (j = 0; j < i; ++j) {
			mi = pv[0][j];								/* un movmiento de la variante principal */
			if (pv[0][j].a == 0 && pv[0][j].de == 0)	/* evita imprimir movimientos como a8a8 */
				break;

			/* si estamos jugando la variante Chess960 y es un enroque lo indicamos con O-O o O-O-O */
			if ((strcmp (variante,"fischerandom") == 0) && mi.tipo == ENROQUE) {
				if (mi.a == 62 || mi.a == 6) {
					printf(" O-O");
				}
				else if (mi.a == 58 || mi.a == 2) {
					printf(" O-O-O");
				}
				continue;
			}

			/* si el movimiento no es una promocion */
			if (mi.tipo < PROMOCION_A_DAMA) {
				printf(" %c%d%c%d",
					'a' + COLUMNA(mi.de),
					8 - FILA(mi.de),
					'a' + COLUMNA(mi.a),
					8 - FILA(mi.a));
			}
			/* con promocion */
			else {
		    	c = ' ';								/* letra a imprimir si hay una promocion */
				switch (mi.tipo) {
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
                    default:
                        break;
				}
				printf(" %c%d%c%d%c",
					'a' + COLUMNA(mi.de),
					8 - FILA(mi.de),
					'a' + COLUMNA(mi.a),
					8 - FILA(mi.a),
					c);
			}
		}

#if defined (UNDER_ELO)
		/* en la version limitada de fuerza del motor junto a la pv enviamos el elo con el que esta jugando el motor, esto ayuda a
		saber que estamos jugando con un elo limitado */
        if (strcmp(limitstrength, "true") == 0) {
			if (strcmp (elo_type, "engine") == 0)
				printf(" [%d ELO engine]", elo + diff_engine);
			else if (strcmp (elo_type, "computer") == 0)
				printf(" [%d ELO computer]", elo + diff_engine + diff_computer_engine);
			else
				printf(" [%d ELO human]", elo + diff_engine + diff_computer_engine + diff_human_computer);
		}
#endif

		printf("\n");								/* cuando terminamos con la linea de informacion debemos mandar un retorno de carro */
#if defined (UNDER_CE)
		fflush(stdout);								/* obligado por el protocolo xboard */
#endif
	}

	/* si usamos el protocolo uci */
	if (protocolo == UCI) {
		/* calculamos el tiempo usado en milisegundos que es lo que reconoce la GUI */
		tiempousado = (get_ms()- tiempo_inicio);

		/* enviamos profundidad, puntos, tiempo, nodos */
		printf("info depth %d time %d nodes %d ", depth, tiempousado, nodos);
		/* no calculamos nps para los primeros plys ya que no suele dar una buena media */
		if (tiempousado > 16)
			printf("nps %d ", (nodos / tiempousado * 1000));
		else
			printf("nps 0 ");
		/* ajustamos el score si tenemos un mate para que lo muestre asi la GUI */
		if (score > 9000 || score < -9000)
			printf("score mate %d ", (score>0) ? ((MATE-score)/2+1) : (-MATE+(MATE-score)/2));
		else
			printf("score cp %d ", score);
		/* indicamos si tenemos un lower o upper bound */
		if (fail != NO_FAIL) {
            if (fail == LOW)
                printf("upperbound ");
            else if (fail == HIGH)
                printf("lowerbound ");
		}

		printf("pv");

		/* imprimimos la mejor linea */
		for (j = 0; j < i; ++j) {
			mi = pv[0][j];
			if (pv[0][j].a == 0 && pv[0][j].de == 0)
				break;

			/* si estamos jugando la variante Chess960 y tenemos un enroque */
			if ((strcmp (variante,"fischerandom") == 0) && mi.tipo == ENROQUE) {
				if (strcmp (castling_chess960_uci,"OO") == 0) {		/* chess960 enroque para Arena */
					if (mi.a == 62 || mi.a == 6) {
						printf(" O-O");
					}
					else if (mi.a == 58 || mi.a == 2) {
						printf(" O-O-O");
					}
					continue;
				}
				else {												/* chess960 el rey captura su propia torre del enroque */
					if (color[mi.de] == BLANCO) {
						if (mi.a == 62) mi.a = tb1;
						else mi.a = tb2;
					}
					else {
						if (mi.a == 6) mi.a = tn1;
						else mi.a = tn2;
					}
					printf(" %c%d%c%d",
						'a' + COLUMNA(mi.de),
						8 - FILA(mi.de),
						'a' + COLUMNA(mi.a),
						8 - FILA(mi.a));
					continue;
				}
			}
			/* si tenemos un movimiento que no es una promocion */
			if (mi.tipo < PROMOCION_A_DAMA) {
				printf(" %c%d%c%d",
					'a' + COLUMNA(mi.de),
					8 - FILA(mi.de),
					'a' + COLUMNA(mi.a),
					8 - FILA(mi.a));
			}
			/* si tenemos promocion */
			else {
		    	c = ' ';											/* letra a imprimir si hay una promocion */
				switch (mi.tipo) {
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
                    default:
                        break;
				}
				printf(" %c%d%c%d%c",
					'a' + COLUMNA(mi.de),
					8 - FILA(mi.de),
					'a' + COLUMNA(mi.a),
					8 - FILA(mi.a),
					c);
			}
		}
		printf("\n");												/* cuando terminamos con la linea debemos mandar un retorno de carro */
	}
	return;
}

/* indica a la GUI el movimiento que debe realizar */
void Enviar_Movimiento(movimiento m, movimiento p)
{
	char c;

	/* si estamos en el protocolo xboard enviamos el movimiento anteponiendo la palabra move */
	if (protocolo == XBOARD) {
		/* si estamos jugando chess960 y es un enroque tenemos que enviar "move O-O" o "move O-O-O" */
		if ((strcmp (variante,"fischerandom") == 0) && m.tipo == ENROQUE) {
			if (m.a == 62 || m.a == 6)
				printf("move O-O\n");
			if (m.a == 58 || m.a == 2)
				printf("move O-O-O\n");
		}
		/* si estamos en un juego normal */
		else {
			/* si hay una promocion tenemos que enviar tambien la letra de la pieza a coronar */
			switch (m.tipo) {
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
#if defined (UNDER_CE)
			/* PocketPc */
			if (m.tipo < PROMOCION_A_DAMA)
				printf("move %c%d%c%d%c\n",
						'a' + COLUMNA(m.de),
						8 - FILA(m.de),
						'a' + COLUMNA(m.a),
						8 - FILA(m.a),
						c);
			else
				/* en el Pocket PC el movimiento que indica una promocion tiene un simbolo = (e8=q) */
				printf("move %c%d%c%d=%c\n",
						'a' + COLUMNA(m.de),
						8 - FILA(m.de),
						'a' + COLUMNA(m.a),
						8 - FILA(m.a),
						c);
#else
			/* resto */
			printf("move %c%d%c%d%c\n",
					'a' + COLUMNA(m.de),
					8 - FILA(m.de),
					'a' + COLUMNA(m.a),
					8 - FILA(m.a),
					c);
#endif
		}
#if defined (UNDER_CE)
		fflush(stdout);
#endif
	}
	/* si estamos en el protocolo uci enviamos el movimiento anteponiendo la palabra bestmove */
	else {
		/* si no tenemos un mejor movimiento para enviar */
		if ((m.de == 0) && (m.a == 0)) {
			printf("bestmove 0000\n");
			return;
		}
		/* si estamos jugando la variante Chess960 y es un enroque */
		if ((strcmp (variante,"fischerandom") == 0) && m.tipo == ENROQUE) {
			if (strcmp (castling_chess960_uci,"OO") == 0) {		/* chess960 enroque para Arena */
				if (m.a == 62 || m.a == 6) {
					printf("bestmove O-O");
				}
				else if (m.a == 58 || m.a == 2) {
					printf("bestmove O-O-O");
				}
			}
			else {												/* chess960 el rey captura su propia torre del enroque */
				if (color[m.de] == BLANCO) {
					if (m.a == 62) m.a = tb1;
					else m.a = tb2;
				}
				else {
					if (m.a == 6) m.a = tn1;
					else m.a = tn2;
				}
			printf("bestmove %c%d%c%d",
				'a' + COLUMNA(m.de),
				8 - FILA(m.de),
				'a' + COLUMNA(m.a),
				8 - FILA(m.a));
			}
		}
		/* jugando la variante normal */
		else if (m.tipo < PROMOCION_A_DAMA) {					/* no hay promocion */
			printf("bestmove %c%d%c%d",
				'a' + COLUMNA(m.de),
				8 - FILA(m.de),
				'a' + COLUMNA(m.a),
				8 - FILA(m.a));
				}
		else {													/* promocion */
		    c = ' ';
			switch (m.tipo) {
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
                default:
                    break;
			}
			printf("bestmove %c%d%c%d%c",
				'a' + COLUMNA(m.de),
				8 - FILA(m.de),
				'a' + COLUMNA(m.a),
				8 - FILA(m.a),
				c);
		}
		/* hemos terminado si no tenemos movimiento para ponderar */
		if (ponder == FALSO || ((p.de == 0) && (p.a == 0))) {
			printf("\n");
			return;
		}
		/* volvemos a repetir para ponder lo mismo que con bestmove */
		if ((strcmp (variante,"fischerandom") == 0) && p.tipo == ENROQUE) {
			if (strcmp (castling_chess960_uci,"OO") == 0) {
				if (p.a == 62 || p.a == 6) {
					printf(" ponder O-O");
				}
				else if (p.a == 58 || p.a == 2) {
					printf(" ponder O-O-O");
				}
			}
			else {												/* chess960 el rey captura su propia torre del enroque */
				if (color[p.de] == BLANCO) {
					if (p.a == 62) p.a = tb1;
					else p.a = tb2;
				}
				else {
					if (p.a == 6) p.a = tn1;
					else p.a = tn2;
				}
				printf(" ponder %c%d%c%d",
					'a' + COLUMNA(p.de),
					8 - FILA(p.de),
					'a' + COLUMNA(p.a),
					8 - FILA(p.a));
			}
		}
		else if (p.tipo < PROMOCION_A_DAMA) {
			printf(" ponder %c%d%c%d",
				'a' + COLUMNA(p.de),
				8 - FILA(p.de),
				'a' + COLUMNA(p.a),
				8 - FILA(p.a));
				}
		else {
		    c = ' ';
			switch (p.tipo) {
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
                default:
                    break;
			}
			printf(" ponder %c%d%c%d%c",
				'a' + COLUMNA(p.de),
				8 - FILA(p.de),
				'a' + COLUMNA(p.a),
				8 - FILA(p.a),
				c);
		}
		printf("\n");
	}
	return;
}

/* si es final de partida indica (solo en el modo xboard) el resultado de la misma y para el motor */
void imprime_resultado(int cuenta, movimiento *lista)
{
	int i;

	/* comprobamos si hay un movimiento legal */
	for (i = 0; i < cuenta; ++i)
		if (Hacer_Movimiento(lista[i])) {
			Deshacer();
			break;
		}
	if (i == cuenta) {                  	/* si hemos llegado al final de cuenta no hay movimientos entonces */
		if (EstaEnJaque(turno)) {
			if (turno == BLANCO){
				/* si toca mover a blancas y estan en jaque han perdido */
				printf("0-1 {Black mates}\n");
			}
			else {
				/* si toca mover a negras y están en jaque han perdido */
				printf("1-0 {White mates}\n");
			}
		}
		else {
			/* tablas porque quien mueve esta ahogado */
			printf("1/2-1/2 {Stalemate}\n");
		}
	}
	else if (repeticion() == 2) {
		/* tablas por repetición de 3 veces la misma posicion */
		printf("1/2-1/2 {Draw by repetition}\n");
	}
	else if (regla50 >= 100) {
		/* tablas por la regla de los 50 movimientos */
		printf("1/2-1/2 {Draw by 50 move rule}\n");
	}
	else if (reclama_tablas_material() == VERDADERO) {
		/* tablas por que no hay suficiente material para hacer mate */
		printf("1/2-1/2 {Draw by insufficient material}\n");
	}
	return;
}

