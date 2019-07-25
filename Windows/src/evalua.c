/*
	< DanaSah is a chess engine winboard. >
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
Funciones para evaluar una posición.
*/

/*Includes*/
#include <stdio.h>
#include <stdlib.h>
#if defined (UNDER_ELO)
	#include <string.h>
#endif
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* definimos algunos bonus y malus */
#define PEON_BLOQUEADO			   -20
#define PEON_PASADO_NOAISLADO		20

#define	ALFIL_ATRAPADO			  -160

#define	TORRE_COLUMNASEMIABIERTA	10
#define	TORRE_COLUMNAABIERTA		15
#define	TORRE_EN_SEPTIMA			20
#define TORRE_ATRAPADA			   -70

#define	DAMA_PREMATURA			   -10
#define DAMA_EN_SEPTIMA				10

#define DESARROLLO_LENTO			-5

#define REY_ENROCADO				40

/*tabla para evaluar posiciones según el lugar de las piezas*/
int peon_blanco[64] = {
  0,   0,   0,   0,   0,   0,   0,   0,
  5,  10,  15,  20,  20,  15,  10,   5,
  4,   8,  12,  16,  16,  12,   8,   4,
  3,   6,   9,  12,  12,   9,   6,   3,
  2,   4,   6,   8,   8,   6,   4,   2,
  1,   2,   3,   4,   4,   3,   2,   1,
  0,   0,   0,  -5,  -5,   0,   0,   0,
  0,   0,   0,   0,   0,   0,   0,   0
};

int peon_negro[64] = {
  0,   0,   0,   0,   0,   0,   0,   0,
  0,   0,   0,  -5,  -5,   0,   0,   0,
  1,   2,   3,   4,   4,   3,   2,   1,
  2,   4,   6,   8,   8,   6,   4,   2,
  3,   6,   9,  12,  12,   9,   6,   3,
  4,   8,  12,  16,  16,  12,   8,   4,
  5,  10,  15,  20,  20,  15,  10,   5,
  0,   0,   0,   0,   0,   0,   0,   0
};

int caballo_blanco_inicio[64] = {
-135, -25, -15, -10, -10, -15, -25,-135,
 -20, -10,   0,   5,   5,   0, -10, -20,
  -5,   5,  15,  20,  20,  15,   5,  -5,
  -5,   5,  15,  20,  20,  15,   5,  -5,
 -10,   0,  10,  15,  15,  10,   0, -10,
 -20, -10,   0,   5,   5,   0, -10, -20,
 -35, -25, -15, -10, -10, -15, -25, -35,
 -50, -40, -30, -25, -25, -30, -40, -50
};

int caballo_negro_inicio[64] = {
 -50, -40, -30, -25, -25, -30, -40, -50,
 -35, -25, -15, -10, -10, -15, -25, -35,
 -20, -10,   0,   5,   5,   0, -10, -20,
 -10,   0,  10,  15,  15,  10,   0, -10,
  -5,   5,  15,  20,  20,  15,   5,  -5,
  -5,   5,  15,  20,  20,  15,   5,  -5,
 -20, -10,   0,   5,   5,   0, -10, -20,
-135, -25, -15, -10, -10, -15, -25,-135
};

int caballo_final[64] = {
 -40, -30, -20, -15, -15, -20, -30, -40,
 -30, -20, -10,  -5,  -5, -10, -20, -30,
 -20, -10,   0,   5,   5,   0, -10, -20,
 -15,  -5,   5,  10,  10,   5,  -5, -15,
 -15,  -5,   5,  10,  10,   5,  -5, -15,
 -20, -10,   0,   5,   5,   0, -10, -20,
 -30, -20, -10,  -5,  -5, -10, -20, -30,
 -40, -30, -20, -15, -15, -20, -30, -40
};

int alfil_blanco_inicio[64] = {
  -8,  -8,  -6,  -4,  -4,  -6,  -8,  -8,
  -8,   0,  -2,   0,   0,  -2,   0,  -8,
  -6,  -2,   4,   2,   2,   4,  -2,  -6,
  -4,   0,   2,   8,   8,   2,   0,  -4,
  -4,   0,   2,   8,   8,   2,   0,  -4,
  -6,  -2,   4,   2,   2,   4,  -2,  -6,
  -8,   0,  -2,   0,   0,  -2,   0,  -8,
 -18, -18, -16, -14, -14, -16, -18, -18
};

int alfil_negro_inicio[64] = {
 -18, -18, -16, -14, -14, -16, -18, -18,
  -8,   0,  -2,   0,   0,  -2,   0,  -8,
  -6,  -2,   4,   2,   2,   4,  -2,  -6,
  -4,   0,   2,   8,   8,   2,   0,  -4,
  -4,   0,   2,   8,   8,   2,   0,  -4,
  -6,  -2,   4,   2,   2,   4,  -2,  -6,
  -8,   0,  -2,   0,   0,  -2,   0,  -8,
  -8,  -8,  -6,  -4,  -4,  -6,  -8,  -8
};

int alfil_final[64] = {
 -18, -12,  -9,  -6,  -6,  -9, -12, -18,
 -12,  -6,  -3,   0,   0,  -3,  -6, -12,
  -9,  -3,   0,   3,   3,   0,  -3,  -9,
  -6,   0,   3,   6,   6,   3,   0,  -6,
  -6,   0,   3,   6,   6,   3,   0,  -6,
  -9,  -3,   0,   3,   3,   0,  -3,  -9,
 -12,  -6,  -3,   0,   0,  -3,  -6, -12,
 -18, -12,  -9,  -6,  -6,  -9, -12, -18
};

int torre_inicio[64] = {
  -6,  -3,   0,   3,   3,   0,  -3,  -6,
  -6,  -3,   0,   3,   3,   0,  -3,  -6,
  -6,  -3,   0,   3,   3,   0,  -3,  -6,
  -6,  -3,   0,   3,   3,   0,  -3,  -6,
  -6,  -3,   0,   3,   3,   0,  -3,  -6,
  -6,  -3,   0,   3,   3,   0,  -3,  -6,
  -6,  -3,   0,   3,   3,   0,  -3,  -6,
  -6,  -3,   0,   3,   3,   0,  -3,  -6
};

int dama_blanca_inicio[64] = {
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
  -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5
};

int dama_negra_inicio[64] = {
  -5,  -5,  -5,  -5,  -5,  -5,  -5,  -5,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0
};

int dama_final[64] = {
 -24, -16, -12,  -8,  -8, -12, -16, -24,
 -16,  -8,  -4,   0,   0,  -4,  -8, -16,
 -12,  -4,   0,   4,   4,   0,  -4, -12,
  -8,   0,   4,   8,   8,   4,   0,  -8,
  -8,   0,   4,   8,   8,   4,   0,  -8,
 -12,  -4,   0,   4,   4,   0,  -4, -12,
 -16,  -8,  -4,   0,   0,  -4,  -8, -16,
 -24, -16, -12,  -8,  -8, -12, -16, -24
};

int rey_blanco_inicio[64] = {
 -70, -60, -80, -90, -90, -80, -60, -70,
 -70, -60, -80, -90, -90, -80, -60, -70,
 -50, -40, -60, -80, -80, -60, -40, -50,
 -30, -20, -40, -60, -60, -40, -20, -30,
 -10,   0, -20, -40, -40, -20,   0, -10,
  10,  20,   0, -20, -20,   0,  20,  10,
  30,  40,  20,   0,   0,  20,  40,  30,
  40,  50,  30,  10,  10,  30,  50,  40
};

int rey_negro_inicio[64] = {
  40,  50,  30,  10,  10,  30,  50,  40,
  30,  40,  20,   0,   0,  20,  40,  30,
  10,  20,   0, -20, -20,   0,  20,  10,
 -10,   0, -20, -40, -40, -20,   0, -10,
 -30, -20, -40, -60, -60, -40, -20, -30,
 -50, -40, -60, -80, -80, -60, -40, -50,
 -70, -60, -80, -90, -90, -80, -60, -70,
 -70, -60, -80, -90, -90, -80, -60, -70
};

int rey_final[64] = {
 -72, -48, -36, -24, -24, -36, -48, -72,
 -48, -24, -12,   0,   0, -12, -24, -48,
 -36, -12,   0,  12,  12,   0, -12, -36,
 -24,   0,  12,  24,  24,  12,   0, -24,
 -24,   0,  12,  24,  24,  12,   0, -24,
 -36, -12,   0,  12,  12,   0, -12, -36,
 -48, -24, -12,   0,   0, -12, -24, -48,
 -72, -48, -36, -24, -24, -36, -48, -72
};

/*vector de simetría, utilizada para no repetir tablas de evaluación para blanco-negro*/
int flip[64] = {
 56,  57,  58,  59,  60,  61,  62,  63,
 48,  49,  50,  51,  52,  53,  54,  55,
 40,  41,  42,  43,  44,  45,  46,  47,
 32,  33,  34,  35,  36,  37,  38,  39,
 24,  25,  26,  27,  28,  29,  30,  31,
 16,  17,  18,  19,  20,  21,  22,  23,
  8,   9,  10,  11,  12,  13,  14,  15,
  0,   1,   2,   3,   4,   5,   6,   7
};

/*una casilla del tablero puede ser blanca o negra, con esto sabemos si los alfiles
contrarios corren por la misma diagonal*/
int	colorcasilla[64] = {
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0,
	0, 1, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 0, 1, 0, 1, 0
};

int malus_peones_doblados[64] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	8, 8, 8,10,10, 8, 8, 8,
	8, 8, 8,10,10, 8, 8, 8,
	8, 8, 8,10,10, 8, 8, 8,
	8, 8, 8,10,10, 8, 8, 8,
	8, 8, 8,10,10, 8, 8, 8,
	8, 8, 8,10,10, 8, 8, 8,
	0, 0, 0, 0, 0, 0, 0, 0
};

int malus_peones_aislados[64] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	8, 8, 8,10,10, 8, 8, 8,
	8, 8, 8,10,10, 8, 8, 8,
	8, 8, 8,10,10, 8, 8, 8,
	8, 8, 8,10,10, 8, 8, 8,
	8, 8, 8,10,10, 8, 8, 8,
	8, 8, 8,10,10, 8, 8, 8,
	0, 0, 0, 0, 0, 0, 0, 0
};

int malus_peones_retrasados[64] = {
	0, 0, 0, 0, 0, 0, 0, 0,
    6, 8, 8,10,10, 8, 8, 6,
    6, 8, 8,10,10, 8, 8, 6,
    6, 8, 8,10,10, 8, 8, 6,
    6, 8, 8,10,10, 8, 8, 6,
    6, 8, 8,10,10, 8, 8, 6,
    6, 8, 8,10,10, 8, 8, 6,
	0, 0, 0, 0, 0, 0, 0, 0
};

/*bonus utilizado para peones pasados*/
int bonus_fila_peones[] = { 0, 0, 0, 1, 3, 6, 10, 0 };

/*bonus por si dos peones de columnas seguidas están en la misma fila*/
int bonus_duo[64] = {
	0, 0, 0, 0, 0, 0, 0, 0,
   10,10,10,10,10,10,10,10,
	6, 6, 6, 6, 6, 6, 6, 6,
	3, 3, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2,
	2, 2, 2, 2, 2, 2, 2, 2,
	0, 0, 0, 0, 0, 0, 0, 0
};

/*los valores de esta tabla los sumamos al contador para comprobar la seguridad del rey durante la fase inicial y medio
juego, como se puede ver penaliza el que el rey esté situado en el centro o en el campo contrario */
int sr_contador[64] = {
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4,
	4, 3, 3, 3, 3, 3, 3, 4,
	2, 1, 1, 1, 1, 1, 1, 2,
	2, 0, 0, 0, 0, 0, 0, 2
};

/* tabla para penalizar si el rey esta atacado */
int seguridad_rey[100] = {
    0,  0,   1,   2,   3,   5,   7,   9,  12,  15,
  18,  22,  26,  30,  35,  39,  44,  50,  56,  62,
  68,  75,  82,  85,  89,  97, 105, 113, 122, 131,
 140, 150, 169, 180, 191, 202, 213, 225, 237, 248,
 260, 272, 283, 295, 307, 319, 330, 342, 354, 366,
 377, 389, 401, 412, 424, 436, 448, 459, 471, 483,
 494, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500
};

static const int PAREJA_ALFILES[] = { 50, 50, 50, 50, 50, 45, 45, 40, 40,
                                       35, 35, 30, 30, 30, 30, 30, 30 };	/*bonificación*/
int PAREJA_CABALLOS = 8;	    /*penalización*/
int PAREJA_TORRES = 16;		    /*penalización*/

/*adjustamos el valor de la pieza según el número de peones como hace CPW, basado en las ideas de Larry*/
int adjuste_caballo[9] = { -20, -16, -12, -8, -4, 0,  4,  8, 12};
int adjuste_torre[9] =   {  15,  12,   9,  6,  3, 0, -3, -6, -9};

int alfilmalo[9] = { 0, 0, -30, -45, -60, -75, -90, -105, -120 };

/*valores para la movilidad en el medio juego y final*/
int mov_caballo[9] = {-10, -4, 2, 8, 14, 18, 22, 24, 25};
int mov_alfil[16] = {-20, -10, 0, 10, 20, 30, 38, 44, 48, 52, 54, 57, 58, 59, 60, 60};
int mov_torre[16] = {-12, -8, -4, 0, 4, 8, 11, 14, 16, 18, 19, 20, 21, 22, 23, 24};
int mov_torre_final[16] = {-20, -12, -4, 4, 12, 20, 28, 36, 44, 50, 54, 56, 57, 58, 59, 60};
int mov_dama[32] = {-10, -8, -6, -4, -2, 0, 2, 4, 6, 8, 10, 12, 13, 14, 15, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16};
int mov_dama_final[32] = {-20, -15, -10, -5, 0, 5, 10, 15, 19, 23, 27, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};

/*tropismo, valora que las piezas estén cerca del ley contrario, el alfil no es tan importante porque puede atacar desde lejos*/
int caballo_tropismo[9] = {0, 2, 2, 2, 0, 0, 0, 0, 0};
int torre_tropismo[9] = {0, 2, 2, 1, 0, 0, 0, 0, 0};
int dama_tropismo[9] = {0, 3, 2, 2, 1, 0, 0, 0, 0};

/*casillas débiles que son interesantes sobre todo para los caballos*/
int outpost[64] = {
	  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  4,  5,  5,  4,  0,  0,
	  0,  2,  5, 10, 10,  5,  2,  0,
	  0,  2,  5, 10, 10,  5,  2,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0
};

int atacantesb, atacantesn, ev_sr_b, ev_sr_n;

/* Si esta definido Rebel como 1 entonces danasah utiliza la evaluacion de una version temprana de Rebel (Gideon), esta evaluacion
es utilizada solo con el proposito de verificaciones y no usada en la version final de danasah. La evaluacion de danasah es el doble
de rapida que la de Gideon ya que en este caso no es necesario transformar el tablero de danasah al formato de rebel donde se pierden
muchos nodos por segundo, con esta doble (a veces triple) velocidad la evaluacion de danasah supera ligeramente a la de Gideon.
Para poder compilar esta version con evaluacion de Gideon necesitamos descargar de la pagina de Ed el proyecto EFS y colocar en
la carpeta del codigo fuente de danasah la carpeta source y el fichero attack.BIN debe acompañar al motor. */
#define REBEL	0

#if REBEL
#include "rebel/defines.c"
#include "rebel/tables.c"
#include "rebel/pst.c"
#include "rebel/vars.c"
#include "rebel/attack.c"
#include "rebel/eval1.c"
#include "rebel/eval2.c"
#include "rebel/init.c"
#include "rebel/quadrant.c"
#endif

/*convertimos el número de piezas a valor de piezas, en centipeones*/
void Calcula_material(void)
{
	piezasblanco = VALOR_CABALLO*caballosblancos + VALOR_ALFIL*alfilesblancos + VALOR_TORRE*torresblancas + VALOR_DAMA*damasblancas;
	piezasnegro = VALOR_CABALLO*caballosnegros + VALOR_ALFIL*alfilesnegros + VALOR_TORRE*torresnegras + VALOR_DAMA*damasnegras;
	peonesblanco = VALOR_PEON*peonesblancos;
	peonesnegro = VALOR_PEON*peonesnegros;
}

/*valor total para las blancas o para las negras en centipeones*/
static void Evalua_material(void)
{
	puntosblanco = piezasblanco + peonesblanco;
	puntosnegro = piezasnegro + peonesnegro;
}

/*situamos la posición de los peones para evaluar su estructura*/
void Busca_peones(void)
{
    int i,c;

	/*para facilitar los cálculos suponemos que hay 10 columnas*/
	for (i = 0; i < 10; i++) {
		fila_peon[BLANCO][i] = 0;
		fila_peon[NEGRO][i] = 7;
	}

	cuenta_piezas = 0;	/*aprovechamos para evaluar el número de piezas totales, nos permitirá hacer más rápido luego los ciclos*/

   	for (i = 0; i < 64; i++) {
		if (color[i] == VACIO)
			continue;
		lista_piezas[cuenta_piezas++] = i;	    /*semilista de piezas*/
		if (pieza[i] == PEON) {
			if (color[i] == BLANCO) {
				c = COLUMNA(i) + 1;
				if (fila_peon[BLANCO][c] < FILA(i))
					fila_peon[BLANCO][c] = FILA(i);
			}
			else  {
				c = COLUMNA(i) + 1;
				if (fila_peon[NEGRO][c] > FILA(i))
					fila_peon[NEGRO][c] = FILA(i);
			}
		}
	}
}

static int IsSqAttackedByPawn(int sq) {
	int n;

    if (turno == BLANCO) {
        n = mailbox[mailbox64[sq] - 11];
        if (n != -1 && pieza[n] == PEON && color[n] == NEGRO) return 1;
        n = mailbox[mailbox64[sq] - 9];
        if (n != -1 && pieza[n] == PEON && color[n] == NEGRO) return 1;
    }
    else {
        n = mailbox[mailbox64[sq] + 9];
        if (n != -1 && pieza[n] == PEON && color[n] == BLANCO) return 1;
        n = mailbox[mailbox64[sq] + 11];
        if (n != -1 && pieza[n] == PEON && color[n] == BLANCO) return 1;
    }
	return 0;
}

/*calcula el número de posiciones posibles para cada pieza (movilidad)*/
int Movilidad_Caballo(int pos)
{
	int n, i, m = 0, r = 0, rey_atacado = 0;

	if (color[pos] == BLANCO) r = rn;
	else if (color[pos] == NEGRO) r = rb;

	for (i = 0; i < 8; i++) {
		n = mailbox[mailbox64[pos]+offset[CABALLO][i]];
		if (n != -1 && color[n] != turno) {
            if (!IsSqAttackedByPawn(n)) m++;
            if (distancia[n][r] <= 3) rey_atacado = 1;
		}
	}
	if (rey_atacado) {
        if (r == rn && ev_sr_n) atacantesb += 2;
        else if (r == rb && ev_sr_b) atacantesn += 2;
	}
	return m;
}

int Movilidad_Alfil(int pos)
{
	int i, ndir, sq_o, n, m = 0, r = 0, rey_atacado = 0;

	if (color[pos] == BLANCO) r = rn;
	else if (color[pos] == NEGRO) r = rb;

	for (i = 0; i < 4; i++) {
		ndir = offset[ALFIL][i];
		sq_o = mailbox64[pos]+ndir;
		n = mailbox[sq_o];
		if (n == -1) continue;
		do  {
			if (color[n] == VACIO) {
                m++;
                if (distancia[n][r] <= 2) rey_atacado = 1;
			}
			else if (color[n] != color[pos]) {
                m++;
                if (distancia[n][r] <= 2) rey_atacado = 1;
                break;
			}
			else {
                break;
			}
			sq_o += ndir;
			n = mailbox[sq_o];
		} while (n != -1);
 	}
 	if (rey_atacado) {
        if (r == rn && ev_sr_n) atacantesb += 2;
        else if (r == rb && ev_sr_b) atacantesn += 2;
 	}
	return m;
}

int Movilidad_Torre(int pos)
{
	int i, ndir, sq_o, n, m = 0, r = 0, rey_atacado = 0, d = 2;

	if (color[pos] == BLANCO) r = rn;
	else if (color[pos] == NEGRO) r = rb;

	for (i = 0; i < 4; i++) {
		ndir = offset[TORRE][i];
		sq_o = mailbox64[pos]+ndir;
		n = mailbox[sq_o];
		if (n == -1) continue;
		do  {
			if (color[n] == VACIO) {
                m++;
                if (distancia[n][r] <= 2) rey_atacado = 1;
			}
			else if (color[n] != color[pos]) {
                m++;
                if (distancia[n][r] <= 2) {
                    rey_atacado = 1;
                    if (distancia[n][r] == 0) d = 0;
                }
                break;
			}
			else {
                break;
			}
			sq_o += ndir;
			n = mailbox[sq_o];
		} while (n != -1);
 	}
 	if (rey_atacado) {
        if (d == 0) {
            if (r == rn && ev_sr_n) atacantesb += 5;
            else if (r == rb && ev_sr_b) atacantesn += 5;
        }
        else {
            if (r == rn && ev_sr_n) atacantesb += 3;
            else if (r == rb && ev_sr_b) atacantesn += 3;
        }
 	}
	return m;
}

int Movilidad_Dama(int pos)
{
	int i, ndir, sq_o, n, m = 0, r = 0, rey_atacado = 0, d = 2;

	if (color[pos] == BLANCO) r = rn;
	else if (color[pos] == NEGRO) r = rb;

	for (i = 0; i < 8; i++) {
		ndir = offset[DAMA][i];
		sq_o = mailbox64[pos]+ndir;
		n = mailbox[sq_o];
		if (n == -1) continue;
		do  {
			if (color[n] == VACIO) {
                m++;
                if (distancia[n][r] <= 2) rey_atacado = 1;
			}
			else if (color[n] != color[pos]) {
                m++;
                if (distancia[n][r] <= 2) {
                    rey_atacado = 1;
                    if (distancia[n][r] == 0) d = 0;
                }
                break;
			}
			else {
                break;
			}
			sq_o += ndir;
			n = mailbox[sq_o];
		} while (n != -1);
 	}
 	if (rey_atacado) {
        if (d == 0) {
            if (r == rn && ev_sr_n) atacantesb += 11;
            else if (r == rb && ev_sr_b) atacantesn += 11;
        }
        else {
            if (r == rn && ev_sr_n) atacantesb += 5;
            else if (r == rb && ev_sr_b) atacantesn += 5;
        }
 	}
	return m;
}

static void Cambios_mediojuego(void)
{
	int temp1, temp2;

	temp1 = piezasblanco - piezasnegro;
	temp2 = alfilesblancos + caballosblancos - alfilesnegros - caballosnegros;

	if (temp1 >= VALOR_TORRE) puntosblanco += 150;			            /*mayor contra peones*/
	else if (temp1 <= -VALOR_TORRE) puntosnegro += 150;
	else if (temp1 >= VALOR_CABALLO) puntosblanco += 75;		        /*menor contra peones*/
	else if (temp1 <= -VALOR_CABALLO) puntosnegro += 75;
	else if (temp2 >= 2) puntosblanco += 50;		                    /*tres menores contra dama o dos torres*/
	else if (temp2 <= -2) puntosnegro += 50;		                    /*dos menores contra torre*/
	else if (temp1 >= (VALOR_TORRE-VALOR_ALFIL)) puntosblanco += 25;	/*menor contra torre*/
	else if (temp1 <= -(VALOR_TORRE-VALOR_ALFIL)) puntosnegro += 25;

	if (damasblancas && !damasnegras) puntosblanco += 25;	            /*bonus por tener dama*/
	else if(damasnegras && !damasblancas) puntosnegro += 25;
}

static void Cambios_final(void)
{
	int temp1, temp2;

	temp1 = piezasblanco - piezasnegro;
	temp2 = alfilesblancos + caballosblancos - alfilesnegros - caballosnegros;

	if (temp1 >= VALOR_CABALLO) puntosblanco += 25;			            /*menor contra peones*/
	else if (temp1 <= -VALOR_CABALLO) puntosnegro += 25;
	else if (temp2 >= 3) puntosblanco += 25;		                    /*tres menores contra dama o dos torres*/
	else if (temp2 <= -3) puntosnegro += 25;

	if (damasblancas && !damasnegras) puntosblanco += 25;	            /*bonus por tener dama*/
	else if(damasnegras && !damasblancas) puntosnegro += 25;
}

static void Evalua_peones_bloqueados(void)
{
	/*penaliza peones d2 y e2, d7 y e7 bloqueados*/
	if (pieza[51] == PEON && color[51] == BLANCO && color[43] != VACIO)
		puntosblanco += PEON_BLOQUEADO;
	if (pieza[52] == PEON && color[52] == BLANCO && color[44] != VACIO)
		puntosblanco += PEON_BLOQUEADO;
	if (pieza[11] == PEON && color[11] == NEGRO && color[19] != VACIO)
		puntosnegro += PEON_BLOQUEADO;
	if (pieza[12] == PEON && color[12] == NEGRO && color[20] != VACIO)
		puntosnegro += PEON_BLOQUEADO;
}

static void Evalua_alfiles_atrapados(void)
{
	/*alfiles encerrados en A2, A7, H2, H7, B1, B8, G1, G8*/
	if (((pieza[48] == ALFIL && color[48] == NEGRO) || (pieza[57] == ALFIL && color[57] == NEGRO))
		&& (pieza[41] == PEON && color[41] == BLANCO) && (pieza[50] == PEON && color[50] == BLANCO)) {
			puntosnegro += ALFIL_ATRAPADO;
	}
	if (((pieza[55] == ALFIL && color[55] == NEGRO) || (pieza[62] == ALFIL && color[62] == NEGRO))
		&& (pieza[46] == PEON && color[46] == BLANCO) && (pieza[53] == PEON && color[53] == BLANCO)) {
			puntosnegro += ALFIL_ATRAPADO;
	}
	if (((pieza[8] == ALFIL && color[8] == BLANCO) || (pieza[1] == ALFIL && color[1] == BLANCO))
		&& (pieza[17] == PEON && color[17] == NEGRO) && (pieza[10] == PEON && color[10] == NEGRO)) {
			puntosblanco += ALFIL_ATRAPADO;
	}
	if (((pieza[15] == ALFIL && color[15] == BLANCO) || (pieza[6] == ALFIL && color[6] == BLANCO))
		&& (pieza[22] == PEON && color[22] == NEGRO) && (pieza[13] == PEON && color[13] == NEGRO)) {
			puntosblanco += ALFIL_ATRAPADO;
	}
}


/* Evaluación completa */
int	Evaluar(int alpha, int beta)
{
#if REBEL
	int y,s,p;

	if (first_time==0) for (y=0; y<276; y++) bgboard[y]=bgstel[y];
	first_time++;
	for (y=A1; y<=H8; y++) if (bord[y] > RR) bord[y]=LL;

	char danasah_piece [] = { WP,WN,WL,WT,WD,WK,LL };			/* como estan definidas las piezas en danasah */
	char danasah_square [64] = {								/* como estan definidas las casillas en danasah */
	A8,B8,C8,D8,E8,F8,G8,H8,
	A7,B7,C7,D7,E7,F7,G7,H7,
	A6,B6,C6,D6,E6,F6,G6,H6,
	A5,B5,C5,D5,E5,F5,G5,H5,
	A4,B4,C4,D4,E4,F4,G4,H4,
	A3,B3,C3,D3,E3,F3,G3,H3,
	A2,B2,C2,D2,E2,F2,G2,H2,
	A1,B1,C1,D1,E1,F1,G1,H1 };

	kleur=1-turno;												/* convertimos de color a movimiento */
	for (y=0; y<=63; y++)
	 { s=danasah_square[y];										/* convertimos casillas al formato de REBEL */
	   p=pieza[y]; p=danasah_piece[p];							/* convertimos piezas blancas al formato de REBEL */
	   if (color[y]==0) p=p+6;									/* añadimos las piezas negras */
	   bord[s]=p; }

	init();
	evaluation();
	mp=(mp*100)/256;											/* en REBEL 1 peon es 256, para nosotros 100 */
	return mp;													/* termina la evaluacion de REBEL */
#endif

	/* Aqui es donde comienza verdaderamente la evaluacion de danasah */
	int i, mat, x, fase, dif_evalua;

    /*ETables, comprobamos la evaluación caché por si ya tenemos evaluada la posición*/
	x = probe_eval();
	if (x != -MATE)
		return x;

	/*material total*/
	material = piezasblanco+piezasnegro+peonesblanco+peonesnegro;
    Evalua_material();	        /*calcula los puntos totales (piezas + peones) para cada bando*/
    if (turno == BLANCO) mat = puntosblanco-puntosnegro;
    else mat = puntosnegro-puntosblanco;

    /* Lazy Eval
	si tenemos mucha ventaja o desventaja no hace falta seguir evaluando, cuanto
	menor es el valor que consideramos más rápido es el programa pero con riesgo de dejarnos
	algún movimiento super-bueno, son buenos valores entre 150-200 */
    if (hacer_lazy_eval) {
    	if (mat - lazy_eval >= beta) {
            return beta;
        }
    	if (mat + lazy_eval <= alpha) {
            return alpha;
    	}
	}

	/* inicia la seguridad del rey */
	ev_sr_b = 0, atacantesn = 0;
	if (damasnegras >= 1 && ((torresnegras+alfilesnegros+caballosnegros) >= 1)) ev_sr_b = 1;
	ev_sr_n = 0, atacantesb = 0;
	if (damasblancas >= 1 && ((torresblancas+alfilesblancos+caballosblancos) >= 1)) ev_sr_n = 1;

	if (material < FINAL) {	    /*si estamos en el final*/

        /* si no hay peones y material es menor que una torre no podemos ganar */
		if (!egtb_is_loaded && !egbb_is_loaded) {
            int matf;
            matf = puntosblanco-puntosnegro;
            if (peonesblanco == 0 && piezasblanco < 500 && matf > 0)
                return 0;
            if (peonesnegro == 0 && piezasnegro < 500 && matf < 0)
                return 0;
		}

		Busca_peones();

		Evalua_posicion_final();	/*evaluamos material, posición, estructuras de peones y no evaluamos seguridad del rey*/

		/*si cada bando solo tiene un alfil y peones es complicado ganar a veces*/
		if (alfilesblancos == 1 && alfilesnegros == 1) {
			if (!damasblancas && !damasnegras && !torresblancas && !torresnegras && !caballosblancos && !caballosnegros) {
				if (wbcolor != bbcolor) {	/*alfiles de distinto color*/
					if (turno == BLANCO)
						return ((puntosblanco-puntosnegro) * (peonesblancos + peonesnegros) / 16);
					else
						return ((puntosnegro-puntosblanco) * (peonesblancos + peonesnegros) / 16);
				}
				else {	/*alfiles del mismo color*/
					if (abs(peonesblancos - peonesnegros) <= 1) {
						if (turno == BLANCO)
							return ((puntosblanco-puntosnegro) * (peonesblancos + peonesnegros) / 8);
						else
							return ((puntosnegro-puntosblanco) * (peonesblancos + peonesnegros) / 8);
					}
				}
			}
		}

		/*alfiles malos*/
		if (alfilesblancos == 1 && peonesblancos >= 2) {
			npcolor = 0;
			for (i = 0; i < 64; i++) {
				if ((color[i] == BLANCO) && (pieza[i] == PEON)) {
					wpcolor = colorcasilla[i];
					if (wpcolor == wbcolor) npcolor++;
				}
			}
			if (npcolor >= 2) puntosblanco += alfilmalo[npcolor];
		}
		if (alfilesnegros == 1 && peonesnegros >= 2) {
			npcolor = 0;
			for (i = 0; i < 64; i++) {
				if ((color[i] == NEGRO) && (pieza[i] == PEON)) {
					bpcolor = colorcasilla[i];
					if (bpcolor == bbcolor) npcolor++;
				}
			}
			if (npcolor >= 2) puntosnegro += alfilmalo[npcolor];
		}

		/*pareja de alfiles en el final*/
		if (alfilesblancos == 2) {
			if ((piezasblanco == 2*VALOR_ALFIL) && (piezasnegro == 2*VALOR_CABALLO))
				puntosblanco += 0;
			else
			{
				puntosblanco += PAREJA_ALFILES[peonesblancos + peonesnegros];
				if (alfilesnegros == 0) {
					puntosblanco += 20;
					if (caballosnegros == 0)
						puntosblanco += 10;
				}
			}
		}
		if (alfilesnegros == 2) {
			if ((piezasblanco == 2*VALOR_CABALLO) && (piezasnegro == 2*VALOR_ALFIL))
				puntosnegro += 0;
			else
			{
				puntosnegro += PAREJA_ALFILES[peonesblancos + peonesnegros];
				if (alfilesblancos == 0) {
					puntosnegro += 20;
					if (caballosblancos == 0)
						puntosnegro += 10;
				}
			}
		}
		/*penalizamos tener la pareja de caballos y de torres*/
		if (caballosblancos == 2) puntosblanco -= PAREJA_CABALLOS;
		if (caballosnegros == 2) puntosnegro -= PAREJA_CABALLOS;
		if (torresblancas == 2) puntosblanco -= PAREJA_TORRES;
		if (torresnegras == 2) puntosnegro -= PAREJA_TORRES;

		/*penalizamos por falta de peones*/
		if (!peonesblancos) puntosblanco -= 32;
		if (!peonesnegros) puntosnegro -= 32;
		if (peonesblancos == 1) puntosblanco -= 16;
		if (peonesnegros == 1) puntosnegro -= 16;

		Evalua_alfiles_atrapados();		/*evaluamos si un alfil se queda atrapado*/

		Cambios_final();				/*intentamos bonificar algunos cambios ventajosos en el final*/

		/*Ajustamos evaluación algunos finales*/
		if (/*!egbb_is_loaded &&*/ reconoce_tablas() == 1) {
			puntosblanco /= 4;
			puntosnegro /= 4;
		}
		if (!damasblancas && !damasnegras) {
			if (torresblancas == 1  && torresnegras == 1) {
				/* KRBPKRB */
				if (peonesblancos == 1 && !peonesnegros) {
					if ((caballosblancos + alfilesblancos) <= 1 && (caballosnegros + alfilesnegros) == 1) {
						puntosblanco /= 2;
						puntosnegro /= 2;
					}
				}
				/* KRBKRBP */
				else if (!peonesblancos && peonesnegros == 1) {
					if ((caballosblancos + alfilesblancos) == 1 && (caballosnegros + alfilesnegros) <= 1) {
						puntosblanco /= 2;
						puntosnegro /= 2;
					}
				}
				/* KRBPKRBP */
				else if (peonesblancos == 1 && peonesnegros == 1) {
					if ((caballosblancos + alfilesblancos) <= 1 && (caballosnegros + alfilesnegros) <= 1) {
						puntosblanco /= 2;
						puntosnegro /= 2;
					}
				}
			}
		}
	}
	else {
		//material = puntosblanco+puntosnegro;
		Busca_peones();

		/* Calculamos la fase del juego como un valor entre 0 y 256 */
		fase = 24 - 4*(damasblancas+damasnegras) - 2*(torresblancas+torresnegras) - (alfilesblancos+alfilesnegros) - (caballosblancos+caballosnegros);
        if (fase < 0) fase = 0;
        fase = (fase * 256 + 12) / 24;

		puntosblanco_m = puntosblanco_f = puntosblanco;
		puntosnegro_m = puntosnegro_f = puntosnegro;

		Evalua_posicion_mediojuego_final();

		puntosblanco = puntosnegro = 0;
		Evalua_peones_bloqueados();
		Cambios_mediojuego();
		Evalua_seguridad_rey();
		puntosblanco_m += puntosblanco;
		puntosnegro_m += puntosnegro;

		if (alfilesblancos == 1 && peonesblancos >= 2) {
			npcolor = 0;
			for (i = 0; i < 64; i++) {
				if ((color[i] == BLANCO) && (pieza[i] == PEON)) {
					wpcolor = colorcasilla[i];
					if (wpcolor == wbcolor) npcolor++;
				}
			}
			if (npcolor >= 2) puntosblanco_f += alfilmalo[npcolor];
		}
		if (alfilesnegros == 1 && peonesnegros >= 2) {
			npcolor = 0;
			for (i = 0; i < 64; i++) {
				if ((color[i] == NEGRO) && (pieza[i] == PEON)) {
					bpcolor = colorcasilla[i];
					if (bpcolor == bbcolor) npcolor++;
				}
			}
			if (npcolor >= 2) puntosnegro_f += alfilmalo[npcolor];
		}

		puntosblanco = puntosnegro = 0;
		Cambios_final();
		puntosblanco_f += puntosblanco;
		puntosnegro_f += puntosnegro;

		/* Calculamos los puntos finales teniendo en cuenta la fase del juego */
		puntosblanco = ((puntosblanco_m * (256-fase)) + (puntosblanco_f * fase)) / 256;
		puntosnegro = ((puntosnegro_m * (256-fase)) + (puntosnegro_f * fase)) / 256;

		if (alfilesblancos == 2) {
			puntosblanco += PAREJA_ALFILES[peonesblancos + peonesnegros];
			if (alfilesnegros == 0) {
				puntosblanco += 20;
				if (caballosnegros == 0)
					puntosblanco += 10;
			}
		}
		if (alfilesnegros == 2) {
			puntosnegro += PAREJA_ALFILES[peonesblancos + peonesnegros];
			if (alfilesblancos == 0) {
				puntosnegro += 20;
				if (caballosblancos == 0)
					puntosnegro += 10;
			}
		}
		if (caballosblancos == 2) puntosblanco -= PAREJA_CABALLOS;
		if (caballosnegros == 2) puntosnegro -= PAREJA_CABALLOS;
		if (torresblancas == 2) puntosblanco -= PAREJA_TORRES;
		if (torresnegras == 2) puntosnegro -= PAREJA_TORRES;

		Evalua_alfiles_atrapados();
	}

	/*bonifica cambiar piezas cuando estamos en cabeza y cambiar peones cuando estamos detrás*/
#if defined (UNDER_ELO)
	if (strcmp(limitstrength, "true") == 0) {
		if (style == 1) {
			if (puntosblanco >= puntosnegro) {
				if (piezasnegro < old_piezasnegro)
					puntosblanco += 4 * (old_piezasnegro - piezasnegro) / 100;
				if (peonesblanco < old_peonesblanco)
					puntosnegro += 20;
			}
			else if (puntosnegro >= puntosblanco) {
				if (piezasblanco < old_piezasblanco)
					puntosnegro += 4 * (old_piezasblanco - piezasblanco) / 100;
				if (peonesnegro < old_peonesnegro)
					puntosblanco += 20;
			}
		}
		else if (style == 2) {
			if (puntosblanco >= puntosnegro) {
				if (piezasnegro < old_piezasnegro)
					puntosblanco += 2 * (old_piezasnegro - piezasnegro) / 100;
				if (peonesblanco < old_peonesblanco)
					puntosnegro += 10;
			}
			else if (puntosnegro > puntosblanco) {
				if (piezasblanco < old_piezasblanco)
					puntosnegro += 2 * (old_piezasblanco - piezasblanco) / 100;
				if (peonesnegro < old_peonesnegro)
					puntosblanco += 10;
			}
		}
		else if (style == 3) {
			if (puntosblanco > puntosnegro) {
				if (piezasnegro < old_piezasnegro)
					puntosblanco += (old_piezasnegro - piezasnegro) / 100;
				if (peonesblanco < old_peonesblanco)
					puntosnegro += 5;
			}
			else if (puntosnegro > puntosblanco) {
				if (piezasblanco < old_piezasblanco)
					puntosnegro += (old_piezasblanco - piezasblanco) / 100;
				if (peonesnegro < old_peonesnegro)
					puntosblanco += 5;
			}
		}
		else if (style == 4) {
			if (puntosblanco >= puntosnegro) {
				if (piezasnegro < old_piezasnegro)
					puntosblanco -= 2 * (old_piezasnegro - piezasnegro) / 100;
				if (peonesblanco < old_peonesblanco)
					puntosnegro -= 5;
			}
			else if (puntosnegro > puntosblanco) {
				if (piezasblanco < old_piezasblanco)
					puntosnegro -= 2 * (old_piezasblanco - piezasblanco) / 100;
				if (peonesnegro < old_peonesnegro)
					puntosblanco -= 5;
			}
		}
		else if (style == 5) {
			if (puntosblanco >= puntosnegro) {
				if (piezasnegro < old_piezasnegro)
					puntosblanco -= 4 * (old_piezasnegro - piezasnegro) / 100;
				if (peonesblanco < old_peonesblanco)
					puntosnegro -= 10;
			}
			else if (puntosnegro > puntosblanco) {
				if (piezasblanco < old_piezasblanco)
					puntosnegro -= 4 * (old_piezasblanco - piezasblanco) / 100;
				if (peonesnegro < old_peonesnegro)
					puntosblanco -= 10;
			}
		}
	}
#else
	if (puntosblanco > puntosnegro + 100) {
		if (piezasnegro < old_piezasnegro)
			puntosblanco += (old_piezasnegro - piezasnegro) / 20;
		if (peonesblanco < old_peonesblanco)
			puntosnegro += 10;
	}
	else if (puntosnegro > puntosblanco + 100) {
		if (piezasblanco < old_piezasblanco)
			puntosnegro += (old_piezasblanco - piezasblanco) / 20;
		if (peonesnegro < old_peonesnegro)
			puntosblanco += 10;
	}
#endif

	/*ajustamos el signo de la evaluación según el turno y bonificamos el tempo*/
	if (turno == BLANCO)
		x = puntosblanco+10-puntosnegro;
	else
		x = puntosnegro+10-puntosblanco;

	/*reducimos evaluación si se acerca la regla de los 50 movimientos*/
	if (regla50 > 70 && regla50 < 100) {
		int tope = 101 - regla50;
		x = x * tope / 30;
	}

#if defined (UNDER_ELO)
	if ((strcmp(limitstrength, "true") == 0) || easylevel > 1)
        x = x +(rand() % RandF - RandF / 2);
    if (x > 8999) x = 8999;
    if (x < -8999) x = -8999;
#endif

	/*ETables, cuando hacemos una evaluación completa entonces la almacenamos en la evaluación caché*/
	store_eval(x);

	/* Ajusta lazy_eval */
	dif_evalua = abs(x - mat);
    evalua_suma += dif_evalua;
    evalua_cuenta++;
    lazy_eval = evalua_suma / evalua_cuenta;
    lazy_eval += 16;
    //hacer_lazy_eval = 1;

	return x;
}

/*penalización si no hay peón blanco del enroque o si se mueve, también penaliza si nos
atacan los peones contrarios, basado en TSCP*/
static int bpf(int f)
{
	int r = 0;

	if (fila_peon[BLANCO][f] == 6);     /*el peon no ha sido movido*/
	else if (fila_peon[BLANCO][f] == 5)
		r -= 10;                        /*el peon ha sido adelantado una casilla*/
	else if (fila_peon[BLANCO][f] != 0)
		r -= 20;                        /*el peon ha sido adelantado más de una casilla*/
	else
		r -= 25;                        /*no hay peon en esta columna*/

	if (fila_peon[NEGRO][f] == 7)
		r -= 15;                        /*no hay peon enemigo*/
	else if (fila_peon[NEGRO][f] == 5)
		r -= 10;                        /*el peon enemigo está en la tercera fila*/
	else if (fila_peon[NEGRO][f] == 4)
		r -= 5;                         /*el peon enemigo está en la cuarta fila*/

	return r;
}

/*comprueba la estructura de peones del enroque para el rey blanco*/
int rey_blanco(int sq)
{
	int r = 0;      /*el valor retornado*/
	int i;

	/*comprobamos si hay enroque largo o corto y así solo estudiamos las columnas correspondientes*/
	if (COLUMNA(sq) < 3) {
		r += bpf(1);
		r += bpf(2);
		r += bpf(3) / 2;    /*los peones de columnas c y f no son tan importantes para el enroque*/
	}
	else if (COLUMNA(sq) > 4) {
		r += bpf(8);
		r += bpf(7);
		r += bpf(6) / 2;
	}

	/*si hay una columna abierta cerca del rey penalizamos*/
	else {
		for (i = COLUMNA(sq); i <= COLUMNA(sq) + 2; ++i)
			if ((fila_peon[BLANCO][i] == 0) && (fila_peon[NEGRO][i] == 7))
				r -= 10;
	}

	/*solo comprobamos seguridad del rey si el contrario tiene suficiente material*/
	r *= piezasnegro;
	r /= (2*VALOR_TORRE + 2*VALOR_CABALLO + 2*VALOR_ALFIL + VALOR_DAMA);

	return r;
}

static int npf(int f)
{
	int r = 0;

	if (fila_peon[NEGRO][f] == 1);
	else if (fila_peon[NEGRO][f] == 2)
		r -= 10;
	else if (fila_peon[NEGRO][f] != 7)
		r -= 20;
	else
		r -= 25;

	if (fila_peon[BLANCO][f] == 0)
		r -= 15;
	else if (fila_peon[BLANCO][f] == 2)
		r -= 10;
	else if (fila_peon[BLANCO][f] == 3)
		r -= 5;

	return r;
}

int rey_negro(int sq)
{
	int r = 0;
	int i;

	if (COLUMNA(sq) < 3) {
		r += npf(1);
		r += npf(2);
		r += npf(3) / 2;
	}
	else if (COLUMNA(sq) > 4) {
		r += npf(8);
		r += npf(7);
		r += npf(6) / 2;
	}
	else {
		for (i = COLUMNA(sq); i <= COLUMNA(sq) + 2; ++i)
			if ((fila_peon[BLANCO][i] == 0) && (fila_peon[NEGRO][i] == 7))
				r -= 10;
	}
	r *= piezasblanco;
	r /= (2*VALOR_TORRE + 2*VALOR_CABALLO + 2*VALOR_ALFIL + VALOR_DAMA);
	return r;
}

/*estructura de peones blancos, bonifica peones pasados y penaliza los doblados, aislados y retrasados*/
void peones_blancos_mediojuego(int i, int *rm, int *rf)
{
	int m = 0, f = 0;      /*el valor para retornar con turno blanco*/
	int c;                  /*la columna del peon*/
	int pba = 0, pne = 0, pbd = 0, pna = 0, j;

	c = COLUMNA(i) + 1;
	aislado = FALSO;
	retrasado = FALSO;
	abierto = FALSO;

    f += 20;
	m += peon_blanco[i];
	f += peon_blanco[i];

	/*peon abierto*/
	if (fila_peon[NEGRO][c] > FILA(i))
		abierto = VERDADERO;
	/*peon doblado*/
	if (fila_peon[BLANCO][c] > FILA(i)) {
		m -= malus_peones_doblados[i];
		f -= malus_peones_doblados[i]*2;
	}
	/*peon aislado*/
	if ((fila_peon[BLANCO][c - 1] == 0) && (fila_peon[BLANCO][c + 1] == 0)) {
		m -= malus_peones_aislados[i];
		f -= malus_peones_aislados[i]*2;
		aislado = VERDADERO;
	}
	/*peon retrasado*/
	else if ((fila_peon[BLANCO][c - 1] < FILA(i)) && (fila_peon[BLANCO][c + 1] < FILA(i))) {
		if (FILA(i) != 6) {
			if (((fila_peon[BLANCO][c - 1] + 1) < FILA(i)) && ((fila_peon[BLANCO][c + 1] + 1) < FILA(i))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c - 1] + 1) == FILA(i)) || ((fila_peon[BLANCO][c + 1] + 1) == FILA(i))) {
				if (pieza[i-8] == PEON)
					retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c - 1] + 1) == FILA(i)) &&
				(((fila_peon[NEGRO][c + 1] + 1) == FILA(i)) || ((fila_peon[NEGRO][c + 1] + 2) == FILA(i)))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c + 1] + 1) == FILA(i)) &&
				(((fila_peon[NEGRO][c - 1] + 1) == FILA(i)) || ((fila_peon[NEGRO][c - 1] + 2) == FILA(i)))) {
				retrasado = VERDADERO;
			}
		}
		else {
			if (((fila_peon[BLANCO][c - 1] + 2) < FILA(i)) && ((fila_peon[BLANCO][c + 1] + 2) < FILA(i))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c - 1] + 2) == FILA(i)) || ((fila_peon[BLANCO][c + 1] + 2) == FILA(i))) {
				if ((pieza[i-8] == PEON) || (pieza[i-16] == PEON))
					retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c - 1] + 2) == FILA(i)) &&
				(((fila_peon[NEGRO][c + 1] + 1) == FILA(i)) || ((fila_peon[NEGRO][c + 1] + 2) == FILA(i)) ||
				((fila_peon[NEGRO][c + 1] + 3) == FILA(i)))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c + 1] + 2) == FILA(i)) &&
				(((fila_peon[NEGRO][c - 1] + 1) == FILA(i)) || ((fila_peon[NEGRO][c - 1] + 2) == FILA(i)) ||
				((fila_peon[NEGRO][c - 1] + 3) == FILA(i)))) {
				retrasado = VERDADERO;
			}
		}
		if (retrasado) {
			m -= malus_peones_retrasados[i];
			f -= malus_peones_retrasados[i];
		}
	}
	if (abierto) {
		/*peon abierto aislado*/
		if (aislado)
			m -= malus_peones_aislados[i];
		/*peon abierto retrasado*/
		if (retrasado)
			m -= malus_peones_retrasados[i];
		/*peon pasado*/
		if ((fila_peon[NEGRO][c - 1] >= FILA(i)) && (fila_peon[NEGRO][c + 1] >= FILA(i))) {
			m += 10 + 6 * bonus_fila_peones[7 - FILA(i)];
			f += 20 + (12 + 2 * distancia[i-8][rn] - (5 * distancia[i-8][rb] / 10)) *  bonus_fila_peones[7 - FILA(i)];
			if (!aislado) {
				m += PEON_PASADO_NOAISLADO;
				f += PEON_PASADO_NOAISLADO;
			}
			/*peon pasado libre*/
			if (color[i-8] == VACIO && !atacado((i-8), BLANCO))
				f += 6 * bonus_fila_peones[7 - FILA(i)];
			/*peon pasado no parable*/
			if (!damasnegras && !torresnegras && !alfilesnegros && !caballosnegros) {
				tmp = COLUMNA(i) - 1;
				if (turno == BLANCO) {
					if (distancia[rn][tmp] > (abs(FILA(tmp) - FILA(i))))
						f += 80 * bonus_fila_peones[7 - FILA(i)];
				}
				else {
					if ((distancia[rn][tmp]-1) > (abs(FILA(tmp) - FILA(i))))
						f += 80 * bonus_fila_peones[7 - FILA(i)];
				}
			}
			/*peon pasado soportado por una torre*/
			if (torresblancas > 0) {
				for (j = i + 8; j <= 63; j += 8) {
					if (pieza[j] == TORRE && color[j] == BLANCO)
						f += 70;
				}
			}
		}
		/*peon candidato*/
		else {
			if (COLUMNA(i) > 0 && COLUMNA(i) < 7) {
				if (pieza[i-9] == PEON && color [i-9] == NEGRO) pna++;
				if (pieza[i-7] == PEON && color [i-7] == NEGRO) pna++;
				if (pieza[i+7] == PEON && color [i+7] == BLANCO) pbd++;
				if (pieza[i+9] == PEON && color [i+9] == BLANCO) pbd++;
				if (pbd >= pna) {
					for (j = i - 9; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pne++;
					}
					for (j = i - 7; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pne++;
					}
					for (j = i - 1; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pba++;
					}
					for (j = i + 1; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pba++;
					}
					if (pba >= pne) {
						m += 5 + 5 * bonus_fila_peones[7 - FILA(i)];
						f += 10 + 10 * bonus_fila_peones[7 - FILA(i)];
					}
				}
			}
			else if (COLUMNA(i) == 0) {
				if (pieza[i-7] == PEON && color [i-7] == NEGRO) pna++;
				if (pieza[i+9] == PEON && color [i+9] == BLANCO) pbd++;
				if (pbd >= pna) {
					for (j = i - 7; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pne++;
					}
					for (j = i + 1; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pba++;
					}
					if (pba >= pne) {
						m += 5 + 5 * bonus_fila_peones[7 - FILA(i)];
						f += 10 + 10 * bonus_fila_peones[7 - FILA(i)];
					}
				}
			}
			else if (COLUMNA(i) == 7) {
				if (pieza[i-9] == PEON && color [i-9] == NEGRO) pna++;
				if (pieza[i+7] == PEON && color [i+7] == BLANCO) pbd++;
				if (pbd >= pna) {
					for (j = i - 9; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pne++;
					}
					for (j = i - 1; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pba++;
					}
					if (pba >= pne) {
						m += 5 + 5 * bonus_fila_peones[7 - FILA(i)];
						f += 10 + 10 * bonus_fila_peones[7 - FILA(i)];
					}
				}
			}
		}
	}
	/*peon duo*/
	if (!aislado) {
		if (fila_peon[BLANCO][c-1] == fila_peon[BLANCO][c]) {
			m += bonus_duo[i];
			f += bonus_duo[i];
		}
	}
	*rm = m;
	*rf = f;
	return;
}

int peones_blancos_final(int i)
{
	int b = 0;      /*el valor para retornar con turno blanco*/
	int c;      /*la columna del peon*/
	int pba = 0, pne = 0, pbd = 0, pna = 0, j;

	c = COLUMNA(i) + 1;
	aislado = FALSO;
	retrasado = FALSO;
	abierto = FALSO;

	b += 20;

	/*peon abierto*/
	if (fila_peon[NEGRO][c] > FILA(i))
		abierto = VERDADERO;
	/*peon doblado*/
	if (fila_peon[BLANCO][c] > FILA(i))
		b -= malus_peones_doblados[i]*2;
	/*peon aislado*/
	if ((fila_peon[BLANCO][c - 1] == 0) && (fila_peon[BLANCO][c + 1] == 0)) {
		b -= malus_peones_aislados[i]*2;
		aislado = VERDADERO;
	}
	/*peon retrasado*/
	else if ((fila_peon[BLANCO][c - 1] < FILA(i)) && (fila_peon[BLANCO][c + 1] < FILA(i))) {
		if (FILA(i) != 6) {
			if (((fila_peon[BLANCO][c - 1] + 1) < FILA(i)) && ((fila_peon[BLANCO][c + 1] + 1) < FILA(i))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c - 1] + 1) == FILA(i)) || ((fila_peon[BLANCO][c + 1] + 1) == FILA(i))) {
				if (pieza[i-8] == PEON)
					retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c - 1] + 1) == FILA(i)) &&
				(((fila_peon[NEGRO][c + 1] + 1) == FILA(i)) || ((fila_peon[NEGRO][c + 1] + 2) == FILA(i)))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c + 1] + 1) == FILA(i)) &&
				(((fila_peon[NEGRO][c - 1] + 1) == FILA(i)) || ((fila_peon[NEGRO][c - 1] + 2) == FILA(i)))) {
				retrasado = VERDADERO;
			}
		}
		else {
			if (((fila_peon[BLANCO][c - 1] + 2) < FILA(i)) && ((fila_peon[BLANCO][c + 1] + 2) < FILA(i))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c - 1] + 2) == FILA(i)) || ((fila_peon[BLANCO][c + 1] + 2) == FILA(i))) {
				if ((pieza[i-8] == PEON) || (pieza[i-16] == PEON))
					retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c - 1] + 2) == FILA(i)) &&
				(((fila_peon[NEGRO][c + 1] + 1) == FILA(i)) || ((fila_peon[NEGRO][c + 1] + 2) == FILA(i)) ||
				((fila_peon[NEGRO][c + 1] + 3) == FILA(i)))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[BLANCO][c + 1] + 2) == FILA(i)) &&
				(((fila_peon[NEGRO][c - 1] + 1) == FILA(i)) || ((fila_peon[NEGRO][c - 1] + 2) == FILA(i)) ||
				((fila_peon[NEGRO][c - 1] + 3) == FILA(i)))) {
				retrasado = VERDADERO;
			}
		}
		if (retrasado)
			b -= malus_peones_retrasados[i];
	}
	if (abierto) {
		/*peon pasado*/
		if ((fila_peon[NEGRO][c - 1] >= FILA(i)) && (fila_peon[NEGRO][c + 1] >= FILA(i))) {
			b += 20 + (12 + 2 * distancia[i-8][rn] - (5 * distancia[i-8][rb] / 10)) *  bonus_fila_peones[7 - FILA(i)];
			if (!aislado)
				b += PEON_PASADO_NOAISLADO;
			/*peon pasado libre*/
			if (color[i-8] == VACIO && !atacado((i-8), BLANCO))
				b += 6 * bonus_fila_peones[7 - FILA(i)];
			/*peon pasado no parable*/
			if (!damasnegras && !torresnegras && !alfilesnegros && !caballosnegros) {
				tmp = COLUMNA(i) - 1;
				if (turno == BLANCO) {
					if (distancia[rn][tmp] > (abs(FILA(tmp) - FILA(i))))
						b += 80 * bonus_fila_peones[7 - FILA(i)];
				}
				else {
					if ((distancia[rn][tmp]-1) > (abs(FILA(tmp) - FILA(i))))
						b += 80 * bonus_fila_peones[7 - FILA(i)];
				}
			}
			/*peon pasado soportado por una torre*/
			if (torresblancas > 0) {
				for (j = i + 8; j <= 63; j += 8) {
					if (pieza[j] == TORRE && color[j] == BLANCO)
						b += 70;
				}
			}
		}
		/*peon candidato*/
		else {
			if (COLUMNA(i) > 0 && COLUMNA(i) < 7) {
				if (pieza[i-9] == PEON && color [i-9] == NEGRO) pna++;
				if (pieza[i-7] == PEON && color [i-7] == NEGRO) pna++;
				if (pieza[i+7] == PEON && color [i+7] == BLANCO) pbd++;
				if (pieza[i+9] == PEON && color [i+9] == BLANCO) pbd++;
				if (pbd >= pna) {
					for (j = i - 9; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pne++;
					}
					for (j = i - 7; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pne++;
					}
					for (j = i - 1; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pba++;
					}
					for (j = i + 1; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pba++;
					}
					if (pba >= pne)
						b += 10 + 10 * bonus_fila_peones[7 - FILA(i)];
				}
			}
			else if (COLUMNA(i) == 0) {
				if (pieza[i-7] == PEON && color [i-7] == NEGRO) pna++;
				if (pieza[i+9] == PEON && color [i+9] == BLANCO) pbd++;
				if (pbd >= pna) {
					for (j = i - 7; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pne++;
					}
					for (j = i + 1; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pba++;
					}
					if (pba >= pne)
						b += 10 + 10 * bonus_fila_peones[7 - FILA(i)];
				}
			}
			else if (COLUMNA(i) == 7) {
				if (pieza[i-9] == PEON && color [i-9] == NEGRO) pna++;
				if (pieza[i+7] == PEON && color [i+7] == BLANCO) pbd++;
				if (pbd >= pna) {
					for (j = i - 9; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pne++;
					}
					for (j = i - 1; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pba++;
					}
					if (pba >= pne)
						b += 10 + 10 * bonus_fila_peones[7 - FILA(i)];
				}
			}
		}
	}
	/*peon duo*/
	if (!aislado) {
		if (fila_peon[BLANCO][c-1] == fila_peon[BLANCO][c])
			b += bonus_duo[i];
	}
	return b;
}

void peones_negros_mediojuego(int i, int *rm, int *rf)
{
	int m = 0, f = 0;      /*el valor para retornar con turno blanco*/
	int c;                  /*la columna del peon*/
	int pna = 0, pbe = 0, pnd = 0, pba = 0, j;

	c = COLUMNA(i) + 1;
	aislado = FALSO;
	retrasado = FALSO;
	abierto = FALSO;

    f += 20;
	m += peon_negro[i];
	f += peon_negro[i];

	/*peon abierto*/
	if (fila_peon[BLANCO][c] < FILA(i))
		abierto = VERDADERO;
 	/*peon doblado*/
	if (fila_peon[NEGRO][c] < FILA(i)) {
		m -= malus_peones_doblados[i];
		f -= malus_peones_doblados[i]*2;
	}
	/*peon aislado*/
	if ((fila_peon[NEGRO][c - 1] == 7) && (fila_peon[NEGRO][c + 1] == 7)){
		m -= malus_peones_aislados[i];
		f -= malus_peones_aislados[i]*2;
		aislado = VERDADERO;
	}
	/*peon retrasado*/
	else if ((fila_peon[NEGRO][c - 1] > FILA(i)) && (fila_peon[NEGRO][c + 1] > FILA(i))) {
		if (FILA(i) != 1) {
			if (((fila_peon[NEGRO][c - 1] - 1) > FILA(i)) && ((fila_peon[NEGRO][c + 1] - 1) > FILA(i))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c - 1] - 1) == FILA(i)) || ((fila_peon[NEGRO][c + 1] - 1) == FILA(i))) {
				if (pieza[i+8] == PEON)
					retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c - 1] - 1) == FILA(i)) &&
				(((fila_peon[BLANCO][c + 1] - 1) == FILA(i)) || ((fila_peon[BLANCO][c + 1] - 2) == FILA(i)))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c + 1] - 1) == FILA(i)) &&
				(((fila_peon[BLANCO][c - 1] - 1) == FILA(i)) || ((fila_peon[BLANCO][c - 1] - 2) == FILA(i)))) {
				retrasado = VERDADERO;
			}
		}
		else {
			if (((fila_peon[NEGRO][c - 1] - 2) > FILA(i)) && ((fila_peon[NEGRO][c + 1] - 2) > FILA(i))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c - 1] - 2) == FILA(i)) || ((fila_peon[NEGRO][c + 1] - 2) == FILA(i))) {
				if ((pieza[i+8] == PEON) || (pieza[i+16] == PEON))
					retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c - 1] - 2) == FILA(i)) &&
				(((fila_peon[BLANCO][c + 1] - 1) == FILA(i)) || ((fila_peon[BLANCO][c + 1] - 2) == FILA(i)) ||
				((fila_peon[BLANCO][c + 1] - 3) == FILA(i)))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c + 1] - 2) == FILA(i)) &&
				(((fila_peon[BLANCO][c - 1] - 1) == FILA(i)) || ((fila_peon[BLANCO][c - 1] - 2) == FILA(i)) ||
				((fila_peon[BLANCO][c - 1] - 3) == FILA(i)))) {
				retrasado = VERDADERO;
			}
		}
		if (retrasado) {
			m -= malus_peones_retrasados[i];
			f -= malus_peones_retrasados[i];
		}
	}
	if (abierto) {
		/*peon abierto aislado*/
		if (aislado)
            m -= malus_peones_aislados[i];
		/*peon abierto retrasado*/
		if (retrasado)
            m -= malus_peones_retrasados[i];
		/*peon pasado*/
		if ((fila_peon[BLANCO][c - 1] <= FILA(i)) && (fila_peon[BLANCO][c + 1] <= FILA(i))) {
			m += 10 + 6 * bonus_fila_peones[FILA(i)];
			f += 20 + (12 + 2 * distancia[i+8][rb] - (5 * distancia[i+8][rn] / 10)) *  bonus_fila_peones[FILA(i)];
			if (!aislado) {
                m += PEON_PASADO_NOAISLADO;
				f += PEON_PASADO_NOAISLADO;
			}
			/*peon pasado libre*/
			if (color[i+8] == VACIO && !atacado((i+8), NEGRO))
				f += 6 * bonus_fila_peones[FILA(i)];
			/*peon pasado no parable*/
			if (!damasblancas && !torresblancas && !alfilesblancos && !caballosblancos) {
				tmp = 56 + COLUMNA(i) - 1;
				if (turno == NEGRO) {
					if (distancia[rb][tmp] > (abs(FILA(tmp) - FILA(i))))
						f += 80 * bonus_fila_peones[FILA(i)];
				}
				else {
					if ((distancia[rb][tmp]-1) > (abs(FILA(tmp) - FILA(i))))
						f += 80 * bonus_fila_peones[FILA(i)];
				}
			}
			/*peon pasado soportado por una torre*/
			if (torresnegras > 0) {
				for (j = i - 8; j >= 0; j -= 8) {
					if (pieza[j] == TORRE && color[j] == NEGRO)
						f += 70;
				}
			}
		}
		/*peon candidato*/
		else {
			if (COLUMNA(i) > 0 && COLUMNA(i) < 7) {
				if (pieza[i-9] == PEON && color [i-9] == NEGRO) pnd++;
				if (pieza[i-7] == PEON && color [i-7] == NEGRO) pnd++;
				if (pieza[i+7] == PEON && color [i+7] == BLANCO) pba++;
				if (pieza[i+9] == PEON && color [i+9] == BLANCO) pba++;
				if (pnd >= pba) {
					for (j = i + 9; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pbe++;
					}
					for (j = i + 7; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pbe++;
					}
					for (j = i - 1; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pna++;
					}
					for (j = i + 1; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pna++;
					}
					if (pna >= pbe) {
						m += 5 + 5 * bonus_fila_peones[FILA(i)];
						f += 10 + 10 * bonus_fila_peones[FILA(i)];
					}
				}
			}
			else if (COLUMNA(i) == 0) {
				if (pieza[i-7] == PEON && color [i-7] == NEGRO) pnd++;
				if (pieza[i+9] == PEON && color [i+9] == BLANCO) pba++;
				if (pnd >= pba) {
					for (j = i + 7; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pbe++;
					}
					for (j = i + 1; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pna++;
					}
					if (pna >= pbe) {
						m += 5 + 5 * bonus_fila_peones[FILA(i)];
						f += 10 + 10 * bonus_fila_peones[FILA(i)];
					}
				}
			}
			else if (COLUMNA(i) == 7) {
				if (pieza[i-9] == PEON && color [i-9] == NEGRO) pnd++;
				if (pieza[i+7] == PEON && color [i+7] == BLANCO) pba++;
				if (pnd >= pba) {
					for (j = i + 9; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pbe++;
					}
					for (j = i + 1; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pna++;
					}
					if (pna >= pbe) {
						m += 5 + 5 * bonus_fila_peones[FILA(i)];
						f += 10 + 10 * bonus_fila_peones[FILA(i)];
					}
				}
			}
		}
	}
	/*peon duo*/
	if (!aislado) {
		if (fila_peon[NEGRO][c-1] == fila_peon[NEGRO][c]) {
			m += bonus_duo[flip[i]];
			f += bonus_duo[flip[i]];
		}
	}
	*rm = m;
	*rf = f;
	return;
}

int peones_negros_final(int i)
{
	int n = 0;      /*el valor para retornar con turno blanco*/
	int c;      /*la columna del peon*/
	int pna = 0, pbe = 0, pnd = 0, pba = 0, j;

	c = COLUMNA(i) + 1;
	aislado = FALSO;
	retrasado = FALSO;
	abierto = FALSO;

	n += 20;

	/*peon abierto*/
	if (fila_peon[BLANCO][c] < FILA(i))
		abierto = VERDADERO;
 	/*peon doblado*/
	if (fila_peon[NEGRO][c] < FILA(i))
		n -= malus_peones_doblados[i]*2;
	/*peon aislado*/
	if ((fila_peon[NEGRO][c - 1] == 7) && (fila_peon[NEGRO][c + 1] == 7)){
		n -= malus_peones_aislados[i]*2;
		aislado = VERDADERO;
	}
	/*peon retrasado*/
	else if ((fila_peon[NEGRO][c - 1] > FILA(i)) && (fila_peon[NEGRO][c + 1] > FILA(i))) {
		if (FILA(i) != 1) {
			if (((fila_peon[NEGRO][c - 1] - 1) > FILA(i)) && ((fila_peon[NEGRO][c + 1] - 1) > FILA(i))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c - 1] - 1) == FILA(i)) || ((fila_peon[NEGRO][c + 1] - 1) == FILA(i))) {
				if (pieza[i+8] == PEON)
					retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c - 1] - 1) == FILA(i)) &&
				(((fila_peon[BLANCO][c + 1] - 1) == FILA(i)) || ((fila_peon[BLANCO][c + 1] - 2) == FILA(i)))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c + 1] - 1) == FILA(i)) &&
				(((fila_peon[BLANCO][c - 1] - 1) == FILA(i)) || ((fila_peon[BLANCO][c - 1] - 2) == FILA(i)))) {
				retrasado = VERDADERO;
			}
		}
		else {
			if (((fila_peon[NEGRO][c - 1] - 2) > FILA(i)) && ((fila_peon[NEGRO][c + 1] - 2) > FILA(i))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c - 1] - 2) == FILA(i)) || ((fila_peon[NEGRO][c + 1] - 2) == FILA(i))) {
				if ((pieza[i+8] == PEON) || (pieza[i+16] == PEON))
					retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c - 1] - 2) == FILA(i)) &&
				(((fila_peon[BLANCO][c + 1] - 1) == FILA(i)) || ((fila_peon[BLANCO][c + 1] - 2) == FILA(i)) ||
				((fila_peon[BLANCO][c + 1] - 3) == FILA(i)))) {
				retrasado = VERDADERO;
			}
			else if (((fila_peon[NEGRO][c + 1] - 2) == FILA(i)) &&
				(((fila_peon[BLANCO][c - 1] - 1) == FILA(i)) || ((fila_peon[BLANCO][c - 1] - 2) == FILA(i)) ||
				((fila_peon[BLANCO][c - 1] - 3) == FILA(i)))) {
				retrasado = VERDADERO;
			}
		}
		if (retrasado)
			n -= malus_peones_retrasados[i];
	}

	if (abierto) {
		/*peon pasado*/
		if ((fila_peon[BLANCO][c - 1] <= FILA(i)) && (fila_peon[BLANCO][c + 1] <= FILA(i))) {
			n += 20 + (12 + 2 * distancia[i+8][rb] - (5 * distancia[i+8][rn] / 10)) *  bonus_fila_peones[FILA(i)];
			if (!aislado)
				n += PEON_PASADO_NOAISLADO;
			/*peon pasado libre*/
			if (color[i+8] == VACIO && !atacado((i+8), NEGRO))
				n += 6 * bonus_fila_peones[FILA(i)];
			/*peon pasado no parable*/
			if (!damasblancas && !torresblancas && !alfilesblancos && !caballosblancos) {
				tmp = 56 + COLUMNA(i) - 1;
				if (turno == NEGRO) {
					if (distancia[rb][tmp] > (abs(FILA(tmp) - FILA(i))))
						n += 80 * bonus_fila_peones[FILA(i)];
				}
				else {
					if ((distancia[rb][tmp]-1) > (abs(FILA(tmp) - FILA(i))))
						n += 80 * bonus_fila_peones[FILA(i)];
				}
			}
			/*peon pasado soportado por una torre*/
			if (torresnegras > 0) {
				for (j = i - 8; j >= 0; j -= 8) {
					if (pieza[j] == TORRE && color[j] == NEGRO)
						n += 70;
				}
			}
		}
		/*peon candidato*/
		else {
			if (COLUMNA(i) > 0 && COLUMNA(i) < 7) {
				if (pieza[i-9] == PEON && color [i-9] == NEGRO) pnd++;
				if (pieza[i-7] == PEON && color [i-7] == NEGRO) pnd++;
				if (pieza[i+7] == PEON && color [i+7] == BLANCO) pba++;
				if (pieza[i+9] == PEON && color [i+9] == BLANCO) pba++;
				if (pnd >= pba) {
					for (j = i + 9; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pbe++;
					}
					for (j = i + 7; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pbe++;
					}
					for (j = i - 1; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pna++;
					}
					for (j = i + 1; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pna++;
					}
					if (pna >= pbe)
						n += 10 + 10 * bonus_fila_peones[FILA(i)];
				}
			}
			else if (COLUMNA(i) == 0) {
				if (pieza[i-7] == PEON && color [i-7] == NEGRO) pnd++;
				if (pieza[i+9] == PEON && color [i+9] == BLANCO) pba++;
				if (pnd >= pba) {
					for (j = i + 7; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pbe++;
					}
					for (j = i + 1; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pna++;
					}
					if (pna >= pbe)
						n += 10 + 10 * bonus_fila_peones[FILA(i)];
				}
			}
			else if (COLUMNA(i) == 7) {
				if (pieza[i-9] == PEON && color [i-9] == NEGRO) pnd++;
				if (pieza[i+7] == PEON && color [i+7] == BLANCO) pba++;
				if (pnd >= pba) {
					for (j = i + 9; j <= 63; j += 8) {
						if (pieza[j] == PEON && color[j] == BLANCO) pbe++;
					}
					for (j = i + 1; j >= 0; j -= 8) {
						if (pieza[j] == PEON && color[j] == NEGRO) pna++;
					}
					if (pna >= pbe)
						n += 10 + 10 * bonus_fila_peones[FILA(i)];
				}
			}
		}
	}
	/*peon duo*/
	if (!aislado) {
		if (fila_peon[NEGRO][c-1] == fila_peon[NEGRO][c])
			n += bonus_duo[flip[i]];
	}

	return n;
}


/*evaluación de la posición en el final, evalúa la estructura de peones pero no evalua la
seguridad del rey, la movilidad es también importante, torres en séptima y columnas abiertas*/
void Evalua_posicion_final(void)
{
	int i, j;

	for (j = 0; j < cuenta_piezas; j++) {   /*sumamos tablas de piezas*/
		i = lista_piezas[j];
		if (color[i] == VACIO) continue;
		if (color[i] == BLANCO) {
			switch (pieza[i]) {
				case PEON:
					puntosblanco += peon_blanco[i];
					puntosblanco += peones_blancos_final(i);	    /*estructura de peones*/
					break;
				case CABALLO:
					puntosblanco += caballo_final[i];
					puntosblanco += mov_caballo[Movilidad_Caballo(i)];
					puntosblanco += adjuste_caballo[peonesblancos];
					break;
				case ALFIL:
					puntosblanco += alfil_final[i];
					puntosblanco += mov_alfil[Movilidad_Alfil(i)];
					wbcolor = colorcasilla[i];
					break;
				case TORRE:
					puntosblanco += mov_torre_final[Movilidad_Torre(i)];
					if (FILA(i) == 1)	                            /*torre en séptima*/
						puntosblanco += TORRE_EN_SEPTIMA * 2;
					if (fila_peon[BLANCO][COLUMNA(i) + 1] == 0) {
						if (fila_peon[NEGRO][COLUMNA(i) + 1] == 7)
							puntosblanco += TORRE_COLUMNAABIERTA;   /*columna abierta*/
						else
							puntosblanco += TORRE_COLUMNASEMIABIERTA;	/*columna semiabierta*/
					}
					puntosblanco += adjuste_torre[peonesblancos];
					break;
				case DAMA:
					puntosblanco += dama_final[i];
					puntosblanco += mov_dama_final[Movilidad_Dama(i)];
					if (FILA(i) == 1)
						puntosblanco += DAMA_EN_SEPTIMA * 2;
					break;
				case REY:
					puntosblanco += rey_final[i];
					break;
                default:
                    break;
			}
        }
		else {
			switch (pieza[i]) {
				case PEON:
					puntosnegro += peon_negro[i];
					puntosnegro += peones_negros_final(i);
					break;
				case CABALLO:
					puntosnegro += caballo_final[i];
					puntosnegro += mov_caballo[Movilidad_Caballo(i)];
					puntosnegro += adjuste_caballo[peonesnegros];
					break;
				case ALFIL:
					puntosnegro += alfil_final[i];
					puntosnegro += mov_alfil[Movilidad_Alfil(i)];
					bbcolor = colorcasilla[i];
					break;
				case TORRE:
					puntosnegro += mov_torre_final[Movilidad_Torre(i)];
					if (FILA(i) == 6)
						puntosnegro += TORRE_EN_SEPTIMA * 2;
					if (fila_peon[NEGRO][COLUMNA(i) + 1] == 7) {
						if (fila_peon[BLANCO][COLUMNA(i) + 1] == 0)
							puntosnegro += TORRE_COLUMNAABIERTA;
						else
							puntosnegro += TORRE_COLUMNASEMIABIERTA;
					}
					puntosnegro += adjuste_torre[peonesnegros];
					break;
				case DAMA:
					puntosnegro += dama_final[i];
					puntosnegro += mov_dama_final[Movilidad_Dama(i)];
					if (FILA(i) == 6)
						puntosnegro += DAMA_EN_SEPTIMA * 2;
					break;
				case REY:
					puntosnegro += rey_final[i];
					break;
                default:
                    break;
			}
		}
	}
}

/*evaluación de la posición para la apertura y medio juego, evalua la estructura de peones
y la seguridad del rey, también evaluamos el desarrollo, tropismo, movilidad, outpost,
torres en séptima y en columanas abiertas, torres atrapadas y rey enrocado, los alfiles
y caballos atrapados los evaluamos aparte*/

void Evalua_posicion_mediojuego_final(void)
{
	int i, j, movilidad_cb, movilidad_ab, movilidad_tb, movilidad_db, movilidad_cn, movilidad_an, movilidad_tn, movilidad_dn;
    int rm, rf;

	for (j = 0; j < cuenta_piezas; j++) {   /*sumamos tablas de piezas*/
		i = lista_piezas[j];
		if (color[i] == VACIO) continue;
		if (color[i] == BLANCO) {
			switch (pieza[i]) {
				case PEON:
				    peones_blancos_mediojuego(i, &rm, &rf);
					puntosblanco_m += rm;
					if (i == 51 || i == 52)
						puntosblanco_m += DESARROLLO_LENTO * 2;
					puntosblanco_f += rf;
                    break;
				case CABALLO:
					movilidad_cb = mov_caballo[Movilidad_Caballo(i)];
					puntosblanco_m += caballo_blanco_inicio[i];
					puntosblanco_m += movilidad_cb;
					if (i == 57 || i == 62)
						puntosblanco_m += DESARROLLO_LENTO;
					if (outpost[i]) {
						if ((fila_peon[NEGRO][COLUMNA(i) + 2] == 7 || fila_peon[NEGRO][COLUMNA(i) + 2] >= FILA(i)) &&
							(fila_peon[NEGRO][COLUMNA(i)] == 7 || fila_peon[NEGRO][COLUMNA(i)] >= FILA(i))) {
								if (pieza[i+7] == PEON && color[i+7] == BLANCO) puntosblanco_m += outpost[i];
								if (pieza[i+9] == PEON && color[i+9] == BLANCO) puntosblanco_m += outpost[i];
						}
					}
					puntosblanco_m += caballo_tropismo[distancia[i][rn]];
					puntosblanco_m += adjuste_caballo[peonesblancos];
					puntosblanco_f += caballo_final[i];
					puntosblanco_f += movilidad_cb;
					puntosblanco_f += adjuste_caballo[peonesblancos];
					break;
				case ALFIL:
					movilidad_ab = mov_alfil[Movilidad_Alfil(i)];
					puntosblanco_m += alfil_blanco_inicio[i];
					puntosblanco_m += movilidad_ab;
					if (i == 58 || i == 61)
						puntosblanco_m += DESARROLLO_LENTO;
					puntosblanco_f += alfil_final[i];
					puntosblanco_f += movilidad_ab;
					wbcolor = colorcasilla[i];
					break;
				case TORRE:
					movilidad_tb = Movilidad_Torre(i);
					puntosblanco_m += torre_inicio[i];
					puntosblanco_m += mov_torre[movilidad_tb];
					if ((rb == 58 || rb == 57) && (i == 56 || i == 48 || i == 57))
						puntosblanco_m += TORRE_ATRAPADA;
					if ((rb == 61 || rb == 62) && (i == 63 || i == 55 || i == 62))
						puntosblanco_m += TORRE_ATRAPADA;
					puntosblanco_m += torre_tropismo[distancia[i][rn]];
					puntosblanco_f += mov_torre_final[movilidad_tb];
					if (FILA(i) == 1) {	                                /*torre en séptima*/
						puntosblanco_m += TORRE_EN_SEPTIMA;	            /*no valoramos tanto como en el final*/
						puntosblanco_f += TORRE_EN_SEPTIMA * 2;
					}
					if (fila_peon[BLANCO][COLUMNA(i) + 1] == 0) {
						if (fila_peon[NEGRO][COLUMNA(i) + 1] == 7) {
							puntosblanco_m += TORRE_COLUMNAABIERTA;     /*columna abierta*/
							puntosblanco_f += TORRE_COLUMNAABIERTA;
						}
						else {
							puntosblanco_m += TORRE_COLUMNASEMIABIERTA;	/*columna semiabierta*/
							puntosblanco_f += TORRE_COLUMNASEMIABIERTA;
						}
					}
					puntosblanco_m += adjuste_torre[peonesblancos];
					puntosblanco_f += adjuste_torre[peonesblancos];
					break;
				case DAMA:
					movilidad_db = Movilidad_Dama(i);
					puntosblanco_m += dama_blanca_inicio[i];
					puntosblanco_m += mov_dama[movilidad_db];
					if (i != 59 &&
						((color[57] == BLANCO && pieza[57] == CABALLO) ||
						(color[58] == BLANCO && pieza[58] == ALFIL) ||
							  (color[61] == BLANCO && pieza[61] == ALFIL) ||
							  (color[62] == BLANCO && pieza[62] == CABALLO)))
							puntosblanco_m += DAMA_PREMATURA;
					puntosblanco_m += dama_tropismo[distancia[i][rn]];
					puntosblanco_f += dama_final[i];
					puntosblanco_f += mov_dama_final[movilidad_db];
					if (FILA(i) == 1) {
						puntosblanco_m += DAMA_EN_SEPTIMA;
						puntosblanco_f += DAMA_EN_SEPTIMA * 2;
					}
					break;
				case REY:
					puntosblanco_m += rey_blanco_inicio[i];
					if ((COLUMNA(i) < 3 && (color[56] != BLANCO && pieza[56] != TORRE)) ||
						(COLUMNA(i) > 4 && (color[63] != BLANCO && pieza[63] != TORRE)))
						puntosblanco_m += REY_ENROCADO;
					puntosblanco_m += rey_blanco(i);
					puntosblanco_f += rey_final[i];
					break;
                default:
                    break;
			}
        }
		else {
			switch (pieza[i]) {
				case PEON:
				    peones_negros_mediojuego(i, &rm, &rf);
					puntosnegro_m += rm;
					if (i == 11 || i == 12)
						puntosnegro_m += DESARROLLO_LENTO * 2;
					puntosnegro_f += rf;
					break;
				case CABALLO:
					movilidad_cn = mov_caballo[Movilidad_Caballo(i)];
					puntosnegro_m += caballo_negro_inicio[i];
					puntosnegro_m += movilidad_cn;
					if (i == 1 || i == 6)
						puntosnegro_m += DESARROLLO_LENTO;
					if (outpost[i]) {
						if ((fila_peon[BLANCO][COLUMNA(i) + 2] == 7 || fila_peon[BLANCO][COLUMNA(i) + 2] <= FILA(i))&&
							(fila_peon[BLANCO][COLUMNA(i)] == 7 || fila_peon[BLANCO][COLUMNA(i)] <= FILA(i))) {
								if (pieza[i-7] == PEON && color[i-7] == NEGRO) puntosnegro_m += outpost[flip[i]];
								if (pieza[i-9] == PEON && color[i-9] == NEGRO) puntosnegro_m += outpost[flip[i]];
						}
					}
					puntosnegro_m += caballo_tropismo[distancia[i][rb]];
					puntosnegro_m += adjuste_caballo[peonesnegros];
					puntosnegro_f += caballo_final[i];
					puntosnegro_f += movilidad_cn;
					puntosnegro_f += adjuste_caballo[peonesnegros];
					break;
				case ALFIL:
					movilidad_an = mov_alfil[Movilidad_Alfil(i)];
					puntosnegro_m += alfil_negro_inicio[i];
					puntosnegro_m += movilidad_an;
					if (i == 2 || i == 5)
						puntosnegro_m += DESARROLLO_LENTO;
					puntosnegro_f += alfil_final[i];
					puntosnegro_f += movilidad_an;
					bbcolor = colorcasilla[i];
					break;
				case TORRE:
					movilidad_tn = Movilidad_Torre(i);
					puntosnegro_m += torre_inicio[i];
					puntosnegro_m +=  mov_torre[movilidad_tn];
					if ((rn == 2 || rn == 1) && (i == 0 || i == 8 || i == 1))
						puntosnegro_m += TORRE_ATRAPADA;
					if ((rn == 5 || rn == 6) && (i == 7 || i == 15 || i == 6))
						puntosnegro_m += TORRE_ATRAPADA;
					puntosnegro_m += torre_tropismo[distancia[i][rb]];
					puntosnegro_f += mov_torre_final[movilidad_tn];
					if (FILA(i) == 6) {
						puntosnegro_m += TORRE_EN_SEPTIMA;
						puntosnegro_f += TORRE_EN_SEPTIMA * 2;
					}
					if (fila_peon[NEGRO][COLUMNA(i) + 1] == 7) {
						if (fila_peon[BLANCO][COLUMNA(i) + 1] == 0) {
							puntosnegro_m += TORRE_COLUMNAABIERTA;
							puntosnegro_f += TORRE_COLUMNAABIERTA;
						}
						else {
							puntosnegro_m += TORRE_COLUMNASEMIABIERTA;
							puntosnegro_f += TORRE_COLUMNASEMIABIERTA;
						}
					}
					puntosnegro_m += adjuste_torre[peonesnegros];
					puntosnegro_f += adjuste_torre[peonesnegros];
					break;
				case DAMA:
					movilidad_dn = Movilidad_Dama(i);
					puntosnegro_m += dama_negra_inicio[i];
					puntosnegro_m += mov_dama[movilidad_dn];
					if (i != 3 &&
						((color[1] == NEGRO && pieza[1] == CABALLO) ||
						(color[2] == NEGRO && pieza[2] == ALFIL) ||
						(color[5] == NEGRO && pieza[5] == ALFIL) ||
						(color[6] == NEGRO && pieza[6] == CABALLO)))
						puntosnegro_m += DAMA_PREMATURA;
					puntosnegro_m += dama_tropismo[distancia[i][rb]];
					puntosnegro_f += dama_final[i];
					puntosnegro_f += mov_dama_final[movilidad_dn];
					if (FILA(i) == 6) {
						puntosnegro_m += DAMA_EN_SEPTIMA;
						puntosnegro_f += DAMA_EN_SEPTIMA * 2;
					}
					break;
				case REY:
					puntosnegro_m += rey_negro_inicio[i];
					if ((COLUMNA(i) < 3 && (color[0] != NEGRO && pieza[0] != TORRE)) ||
						(COLUMNA(i) > 4 && (color[7] != NEGRO && pieza[7] != TORRE)))
						puntosnegro_m += REY_ENROCADO;
					puntosnegro_m += rey_negro(i);
					puntosnegro_f += rey_final[i];
					break;
                default:
                    break;
			}
		}
	}
}

void Evalua_seguridad_rey(void)
{
    int l;
	atacantesn += sr_contador[rb];
	atacantesb += sr_contador[flip[rn]];

    /* probamos a no hacer un examen complicado de la seguridad del rey si previamente sabemos que no hay muchas
    piecas atacando */
	if (ev_sr_b && atacantesn >= 7) {
		l = rb + 9;
		if (l <= 63 && COLUMNA(l) > COLUMNA(rb) && atacado(l, BLANCO)) {
			atacantesn += 2;
			if (!atacado_nr(l, NEGRO)) atacantesn++;
		}
		l = rb + 8;
		if (l <= 63 && atacado(l, BLANCO)) {
			atacantesn += 2;
			if (!atacado_nr(l, NEGRO)) atacantesn++;
		}
		l = rb + 7;
		if (l <= 63 && COLUMNA(l) < COLUMNA(rb) && atacado(l, BLANCO)) {
			atacantesn += 2;
			if (!atacado_nr(l, NEGRO)) atacantesn++;
		}
		l = rb - 1;
		if (FILA(l) == FILA(rb) && atacado(l, BLANCO)) {
			atacantesn += 2;
			if (!atacado_nr(l, NEGRO)) atacantesn++;
		}
		l = rb + 1;
		if (FILA(l) == FILA(rb) && atacado(l, BLANCO)) {
			atacantesn += 2;
			if (!atacado_nr(l, NEGRO)) atacantesn++;
		}
		l = rb - 7;
		if (l >= 0 && COLUMNA(l) > COLUMNA(rb)) {
			if (atacado(l, BLANCO)) {
				atacantesn += 2;
				if (color[l] != BLANCO) atacantesn += 3;
				else if (pieza[l] != PEON || pieza[l] != ALFIL) atacantesn++;
				if (!atacado_nr(l, NEGRO)) atacantesn++;
			}
			else if (color[l] != BLANCO) {
				atacantesn++;
			}
		}
		l = rb - 8;
		if (l >= 0) {
			if (atacado(l, BLANCO)) {
				atacantesn += 2;
				if (color[l] != BLANCO) atacantesn += 3;
				else if (pieza[l] != PEON || pieza[l] != ALFIL) atacantesn++;
				if (!atacado_nr(l, NEGRO)) atacantesn++;
			}
			else if (color[l] != BLANCO) {
				atacantesn++;
			}
		}
		l = rb - 9;
		if (l >= 0 && COLUMNA(l) < COLUMNA(rb)) {
			if (atacado(l, BLANCO)) {
				atacantesn += 2;
				if (color[l] != BLANCO) atacantesn += 3;
				else if (pieza[l] != PEON || pieza[l] != ALFIL) atacantesn++;
				if (!atacado_nr(l, NEGRO)) atacantesn++;
			}
			else if (color[l] != BLANCO) {
				atacantesn++;
			}
		}
		if (atacantesn > 99) atacantesn = 99;
		puntosnegro += seguridad_rey[atacantesn];
		/*if (atacado(rn, NEGRO))
			puntosblanco += 25;*/
	}
	else if (ev_sr_b) {
        atacantesn *= 2;
		puntosnegro += seguridad_rey[atacantesn];
	}

	if (ev_sr_n && atacantesb >= 7) {
		l = rn - 9;
		if (l >= 0 && COLUMNA(l) < COLUMNA(rn) && atacado(l, NEGRO)) {
			atacantesb += 2;
			if (!atacado_nr(l, BLANCO)) atacantesb++;
		}
		l = rn - 8;
		if (l >= 0 && atacado(l, NEGRO)) {
			atacantesb += 2;
			if (!atacado_nr(l, BLANCO)) atacantesb++;
		}
		l = rn - 7;
		if (l >= 0 && COLUMNA(l) > COLUMNA(rn) && atacado(l, NEGRO)) {
			atacantesb += 2;
			if (!atacado_nr(l, BLANCO)) atacantesb++;
		}
		l = rn - 1;
		if (FILA(l) == FILA(rn) && atacado(l, NEGRO)) {
			atacantesb += 2;
			if (!atacado_nr(l, BLANCO)) atacantesb++;
		}
		l = rn + 1;
		if (FILA(l) == FILA(rn) && atacado(l, NEGRO)) {
			atacantesb += 2;
			if (!atacado_nr(l, BLANCO)) atacantesb++;
		}
		l = rn + 7;
		if (l <= 63 && COLUMNA(l) < COLUMNA(rn)) {
			if (atacado(l, NEGRO)) {
				atacantesb += 2;
				if (color[l] != NEGRO) atacantesb += 3;
				else if (pieza[l] != PEON  || pieza[l] != ALFIL) atacantesb++;
				if (!atacado_nr(l, BLANCO)) atacantesb++;
			}
			else if (color[l] != NEGRO) {
				atacantesb++;
			}
		}
		l = rn + 8;
		if (l <= 63) {
			if (atacado(l, NEGRO)) {
				atacantesb += 2;
				if (color[l] != NEGRO) atacantesb += 3;
				else if (pieza[l] != PEON  || pieza[l] != ALFIL) atacantesb++;
				if (!atacado_nr(l, BLANCO)) atacantesb++;
			}
			else if (color[l] != NEGRO) {
				atacantesb++;
			}
		}
		l = rn + 9;
		if (l <= 63 && COLUMNA(l) > COLUMNA(rn)) {
			if (atacado(l, NEGRO)) {
				atacantesb += 2;
				if (color[l] != NEGRO) atacantesb += 3;
				else if (pieza[l] != PEON  || pieza[l] != ALFIL) atacantesb++;
				if (!atacado_nr(l, BLANCO)) atacantesb++;
			}
			else if (color[l] != NEGRO) {
				atacantesb++;
			}
		}
        if (atacantesb > 99) atacantesb = 99;
        puntosblanco += seguridad_rey[atacantesb];
        /*if (atacado(rb, BLANCO))	damos un bonus por dar jaque, parece ayudar en los mates*/
            /*puntosnegro += 25;*/
	}
	else if (ev_sr_n) {
        atacantesb *= 2;
        puntosblanco += seguridad_rey[atacantesb];
	}
}

int reconoce_tablas(void)
{ int blanco_total, negro_total, reconoce;

  reconoce = 0;
  blanco_total = damasblancas + torresblancas + alfilesblancos + caballosblancos + peonesblancos;
  negro_total = damasnegras + torresnegras + alfilesnegros + caballosnegros + peonesnegros;

  if (blanco_total == 1 && negro_total == 1) {
    if (peonesblancos == 1 && caballosnegros == 1) reconoce = 1; /*KPKN*/
    if (caballosblancos == 1 && peonesnegros == 1) reconoce = 1; /*KNKP*/
    if (peonesblancos == 1 && alfilesnegros == 1) reconoce = 1; /*KPKB*/
    if (alfilesblancos == 1 && peonesnegros == 1) reconoce = 1; /*KBKP*/
    if (alfilesblancos == 1 && caballosnegros == 1) reconoce = 1; /*KBKN*/
    if (caballosblancos == 1 && alfilesnegros == 1) reconoce = 1; /*KNKB*/
  }
  else if (blanco_total == 0 && negro_total == 2) {
    if (caballosnegros == 2) reconoce = 1;            /*KKNN*/
  }
  else if (blanco_total == 2 && negro_total == 0) {
    if (caballosblancos == 2) reconoce = 1;            /*KNNK*/
  }
  else if (blanco_total == 2 && negro_total == 1) {
    if (peonesblancos == 2 && caballosnegros == 1) reconoce = 1; /*KPPKN*/
    if (caballosblancos == 2 && caballosnegros == 1) reconoce = 1; /*KNNKN*/
    if (peonesblancos == 2 && alfilesnegros == 1) reconoce = 1; /*KPPKB*/
    if (caballosblancos == 2 && alfilesnegros == 1) reconoce = 1; /*KNNKB*/
    if (caballosblancos == 2 && torresnegras == 1) reconoce = 1; /*KNNKR*/
    if (alfilesblancos == 2 && torresnegras == 1) reconoce = 1; /*KBBKR*/
    if (alfilesblancos == 1 && caballosblancos == 1 && torresnegras == 1) reconoce = 1; /*KBNKR*/
  }
  else if (blanco_total == 1 && negro_total == 2) {
    if (caballosblancos == 1 && peonesnegros == 2) reconoce = 1; /*KNKPP*/
    if (caballosblancos == 1 && caballosnegros == 2) reconoce = 1; /*KNKNN*/
    if (alfilesblancos == 1 && peonesnegros == 2) reconoce = 1; /*KBKPP*/
    if (alfilesblancos == 1 && caballosnegros == 2) reconoce = 1; /*KBKNN*/
    if (torresblancas == 1 && caballosnegros == 2) reconoce = 1; /*KRKNN*/
    if (torresblancas == 1 && alfilesnegros == 2) reconoce = 1; /*KRKBB*/
    if (torresblancas == 1 && alfilesnegros == 1 && caballosnegros == 1) reconoce = 1; /*KRKBN*/
  }
  /*else if (blanco_total == 3 && negro_total == 1) {
    if (peonesblancos == 3 && caballosnegros == 1) reconoce = 1; // KPPPKN
    if (peonesblancos == 3 && alfilesnegros == 1) reconoce = 1; // KPPPKB
  }
  else if (blanco_total == 1 && negro_total == 3) {
    if (caballosblancos == 1 && peonesnegros == 3) reconoce = 1; // KNKPPP
    if (alfilesblancos == 1 && peonesnegros == 3) reconoce = 1; // KBKPPP
  }*/
  else if (blanco_total == 2 && negro_total == 2) {
    if (caballosblancos == 1 && peonesblancos == 1 && caballosnegros == 2) reconoce = 1;  /*KNPKNN*/
    if (caballosblancos == 2 && caballosnegros == 1 && peonesnegros == 1) reconoce = 1;  /*KNNKNP*/
    if (caballosblancos == 2 && alfilesnegros == 1 && peonesnegros == 1) reconoce = 1;  /*KNNKBP*/
    if (alfilesblancos == 1 && peonesblancos == 1 && caballosnegros == 2) reconoce = 1;  /*KBPKNN*/
  }
  else if (blanco_total == 3 && negro_total == 2) {
    if (caballosblancos == 1 && peonesblancos == 2 && caballosnegros == 2) reconoce = 1;  /*KNPPKNN*/
    if (alfilesblancos == 1 && peonesblancos == 2 && caballosnegros == 2) reconoce = 1;  /*KBPPKNN*/
  }
  else if (blanco_total == 2 && negro_total == 3) {
    if (caballosblancos == 2 && caballosnegros == 1 && peonesnegros == 2) reconoce = 1;  /*KNNKNPP*/
    if (caballosblancos == 2 && alfilesnegros == 1 && peonesnegros == 2) reconoce = 1;  /*KNNKBPP*/
  }
  /*else if (blanco_total == 4 && negro_total == 2) {
    if (caballosblancos == 1 && peonesblancos == 3 && caballosnegros == 2) reconoce = 1;  //KNPPPKNN
  }
  else if (blanco_total == 2 && negro_total == 4) {
    if (caballosblancos == 2 && caballosnegros == 1 && peonesnegros == 3) reconoce = 1;  //KNNKNPPP
  }*/
  return reconoce;
}

/*esta función nos va a permitir reclamar tablas por una insuficiencia de material*/
int reclama_tablas_material(void)
{
	int i;

	if (peonesblanco == 0 && peonesnegro == 0) {
		/*podemos reclamar tablas cuando ambos bandos tienen menos material que una torre*/
		if ((piezasblanco < VALOR_TORRE) && (piezasnegro < VALOR_TORRE)) {
			/*pero exceptuamos los siguientes casos*/
			/*si cada uno de los bandos tiene un caballo*/
			if ((piezasblanco == VALOR_CABALLO) && (piezasnegro == VALOR_CABALLO))
				return FALSO;
			/*si uno de los bandos tiene un alfil y el otro un caballo*/
			else if ((piezasblanco == VALOR_ALFIL) && (piezasnegro == VALOR_CABALLO))
				return FALSO;
			else if ((piezasblanco == VALOR_CABALLO) && (piezasnegro == VALOR_ALFIL))
				return FALSO;
			/*si cada bando tiene un alfil pero son de distinto color*/
			else if ((piezasblanco == VALOR_ALFIL) && (piezasnegro == VALOR_ALFIL)) {
   				for (i = 0; i < 64; i++) {
					if ((color[i] == BLANCO) && (pieza[i] == ALFIL))
						alfilblanco = colorcasilla[i];
					if ((color[i] == NEGRO) && (pieza[i] == ALFIL))
						alfilnegro = colorcasilla[i];
				}
				if (alfilblanco != alfilnegro)
					return FALSO;
				else
					return VERDADERO;
			}
			else
				return VERDADERO;
		}
		else
			return FALSO;
	}
	else
		return FALSO;
}
