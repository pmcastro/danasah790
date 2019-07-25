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

/* Funciones para tener un static exchange evaluator */

/*Includes*/
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* comprueba si una casilla de un determinado color es defendida por un peon */
static int IsSqProtectedByPawn(int sq) {
	int n;
	if (color[sq] == NEGRO) {
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

/* comprueba si una casilla de un determinado color es defendida por un caballo */
static int IsSqProtectedByKnight(int sq) {
	int i, n, cturno;
	cturno = 1-turno;
	for (i = 0; i < 8; i++) {
		n = mailbox[mailbox64[sq]+offset[CABALLO][i]];
		if (n != -1 && pieza[n] == CABALLO && color[n] == cturno) return 1;
	}
	return 0;
}

/* comprueba si una casilla de un determinado color es defendida por un alfil */
static int IsSqProtectedByBishop(int sq) {
	int i, n, sq_o, cturno;
	register int ndir;
	cturno = 1-turno;
	for (i = 0; i < 4; i++) {
		ndir = offset[ALFIL][i];
		sq_o = mailbox64[sq]+ndir;
		n = mailbox[sq_o];
		if (n == -1) continue;
		do  {
			if (pieza[n] == ALFIL && color[n] == cturno) return 1;
			if (color[n] != VACIO) break;
			sq_o += ndir;
			n = mailbox[sq_o];
		} while (n != -1);
	}
	return 0;
}

/* comprueba si una casilla de un determinado color es defendida por una torre */
static int IsSqProtectedByRook(int sq) {
	int i, n, sq_o, cturno;
	register int ndir;
	cturno = 1-turno;
	for (i = 0; i < 4; i++) {
		ndir = offset[TORRE][i];
		sq_o = mailbox64[sq]+ndir;
		n = mailbox[sq_o];
		if (n == -1) continue;
		do  {
			if (pieza[n] == TORRE && color[n] == cturno) return 1;
			if (color[n] != VACIO) break;
			sq_o += ndir;
			n = mailbox[sq_o];
		} while (n != -1);
	}
	return 0;
}

/* Nos permite saber que algunos movimientos son malas capturas sin necesidad de hacer un caro SEE, podemos utilizarlo para la
ordenacion de los movimientos y tambien para cortar en la quiesce los movimientos que son malas capturas, esta funcion simple
da al menos 10 puntos elo */
int malaCaptura(movimiento mcmov) {

	/* Capturas "de igual a igual o de menos a mas" (también AxC) son buenas por definicion */
	if (valor_pieza2[pieza[mcmov.a]] >= valor_pieza2[pieza[mcmov.de]])
		return 0;

	/* recaptura por peon ? */
	if (IsSqProtectedByPawn(mcmov.a))
		return 1;

	/* recaptura por caballo o alfil ? */
	if (valor_pieza2[pieza[mcmov.de]] == TORRE && valor_pieza2[pieza[mcmov.a]] == PEON
		&& (IsSqProtectedByKnight(mcmov.a) || IsSqProtectedByBishop(mcmov.a)))
			return 1;
	if (valor_pieza2[pieza[mcmov.de]] == DAMA && valor_pieza2[pieza[mcmov.a]] <= TORRE
		&& (IsSqProtectedByKnight(mcmov.a) || IsSqProtectedByBishop(mcmov.a)))
			return 1;

	/* recaptura por torre ? */
	if (valor_pieza2[pieza[mcmov.de]] == DAMA && valor_pieza2[pieza[mcmov.a]] <= ALFIL
		&& (IsSqProtectedByRook(mcmov.a)))
			return 1;

	return 0;
}

/* Esta funcion calcula informacion de ataque-defensa para una casilla y forma parte del see */
/* No tiene ataques o defensas por rayos x */
void calcula_atacantes(int casilla) {
	int i, n, ca_orig;
	register int ndir;
	int numB = 0, numN = 0;

	/* ponemos a 0 algunas variables */
	for (i = 0; i < 16; i++){
		atacantes[BLANCO][i].casilla = atacantes[NEGRO][i].casilla = 0;
		atacantes[BLANCO][i].deslizante = atacantes[NEGRO][i].deslizante = 0;
		atacantes[BLANCO][i].direccion = atacantes[NEGRO][i].direccion = 0;
	}

	/* movimientos estilo alfil */
	for (i = 0; i < 4; i++) {
		ndir = offset[ALFIL][i];
		ca_orig = mailbox64[casilla]+ndir;
		n = mailbox[ca_orig];
		if (n == -1) continue;
		/* ataques por peones */
		if (pieza[n] == PEON) {
            if (color[n] == BLANCO && i >= 2) {
                atacantes[BLANCO][numB].casilla = n;
                atacantes[BLANCO][numB].deslizante = 1;
                atacantes[BLANCO][numB].direccion = ndir;
                numB++;
            }
            else if (color[n] == NEGRO && i <= 1) {
                atacantes[NEGRO][numN].casilla = n;
                atacantes[NEGRO][numN].deslizante = 1;
                atacantes[NEGRO][numN].direccion = ndir;
                numN++;
            }
		}
		/* el rey puede atacar solo desde una casilla contigua */
		else if (pieza[n] == REY) {
            if (color[n] == BLANCO) {
                atacantes[BLANCO][numB].casilla = n;
                numB++;
            }
            else {
                atacantes[NEGRO][numN].casilla = n;
                numN++;
            }
		}
		/* ataques por alfil o dama en diagonal considerando que pueden atacar a distancia siempre que las casillas intermedias esten vacias */
		else {
			do {
				if (pieza[n] == ALFIL || pieza[n] == DAMA) {
                    if (color[n] == BLANCO) {
                        atacantes[BLANCO][numB].casilla = n;
                        atacantes[BLANCO][numB].deslizante = 1;
                        atacantes[BLANCO][numB].direccion = ndir;
                        numB++;
                        break;
                    }
                    else {
                        atacantes[NEGRO][numN].casilla = n;
                        atacantes[NEGRO][numN].deslizante = 1;
                        atacantes[NEGRO][numN].direccion = ndir;
                        numN++;
                        break;
                    }
				}
				else if (color[n] == !VACIO) break;
				ca_orig += ndir;
				n = mailbox[ca_orig];
			} while (n != -1);
		}
	}

	/* movimientos estilo caballo */
	for (i = 0; i < 8; i++) {
		n = mailbox[mailbox64[casilla]+offset[CABALLO][i]];
		if (n == -1) continue;
		if (pieza[n] == CABALLO) {
            if (color[n] == BLANCO) {
                atacantes[BLANCO][numB].casilla = n;
                numB++;
            }
            else {
                atacantes[NEGRO][numN].casilla = n;
                numN++;
            }
		}
	}

	/* movimientos estilo torre */
	for (i = 0; i < 4; i++) {
		ndir = offset[TORRE][i];
		ca_orig = mailbox64[casilla]+ndir;
		n = mailbox[ca_orig];
		if (n == -1) continue;
		/* el rey puede atacar solo desde una casilla contigua */
		if (pieza[n] == REY) {
            if (color[n] == BLANCO) {
                atacantes[BLANCO][numB].casilla = n;
                numB++;
            }
            else {
                atacantes[NEGRO][numN].casilla = n;
                numN++;
            }
		}
		/* ataques por torre o dama horizontales o verticales considerando que pueden atacar a distancia */
		else {
			do {
				if (pieza[n] == TORRE || pieza[n] == DAMA) {
                    if (color[n] == BLANCO) {
                        atacantes[BLANCO][numB].casilla = n;
                        atacantes[BLANCO][numB].deslizante = 2;
                        atacantes[BLANCO][numB].direccion = ndir;
                        numB++;
                        break;
                    }
                    else {
                        atacantes[NEGRO][numN].casilla = n;
                        atacantes[NEGRO][numN].deslizante = 2;
                        atacantes[NEGRO][numN].direccion = ndir;
                        numN++;
                        break;
                    }
				}
				else if (color[n] == !VACIO) break;
				ca_orig += ndir;
				n = mailbox[ca_orig];
			} while (n != -1);
		}
	}

	num_atacantes[BLANCO] = numB;
	num_atacantes[NEGRO] = numN;
}

/* SEE Static Exchange Evaluator, podriamos utilizarlo para la ordenacion de los movimientos aunque en danasah es tan lenta que en
lugar de beneficiar perjudica, por eso no es usada salvo para evitar una explosion de la quiesce */

/* Otra version del SEE Static Exchange Evaluator con una pequeña modificacion respecto a la anterior y es que en este caso no se calcula
el valor exacto del see sino que basta con saber si sera menor que cero o no, de esta forma podria ser un poquito mas rapida que la funcion
anterior. Dicha funcion la utilizariamos durante la quiesce para cortar los movimientos con see menor que cero y en danasah es discutible el
elo conseguido, yo diria que entre 0 y 5 puntos, muy poco comparado a otros motores */
int see(int turno2, int casilla, int de, int rapido) {
	int i, n, d;
	int valor_del_menor, indice_del_menor, casilla_del_menor, valor_temp;
	int valorsee[32];
	int origpieza, origcolor;
	int ganancia;
	int ca_orig;

	/* salida rapida */
	if (rapido && valor_pieza2[pieza[casilla]] >= valor_pieza2[pieza[de]]) return 0;

	/* eliminamos el capturador original del tablero */
	origpieza = pieza[de];
	origcolor = color[de];
	pieza[de] = VACIO;
	color[de] = VACIO;

	/* calcula todos los atacantes para la casilla */
	calcula_atacantes(casilla);

	/* ¿es una captura libre? */
	if (num_atacantes[1-turno2] == 0) {
		pieza[de] = origpieza;
		color[de] = origcolor;
		return valor_pieza2[pieza[casilla]];
	}

    /* inicia valorsee y d */
	for (i = 0; i < 32; i++) valorsee[i] = 0;
	d = 1;

	/* iniciamos la primera recaptura */
	valorsee[0] = valor_pieza2[pieza[casilla]];
	ganancia = valor_pieza2[origpieza];
	turno2 ^= 1;

	/* continamos mientras haya atacantes */
	while(num_atacantes[turno2])
	{
		valor_del_menor = valor_pieza2[REY];
		indice_del_menor = 0;
		casilla_del_menor = 0;

		/* Buscamos el atacante de menor valor */
		for(i = 0; i < num_atacantes[turno2]; i++) {
			valor_temp = valor_pieza2[pieza[atacantes[turno2][i].casilla]];
			if (valor_temp < valor_del_menor) {
				valor_del_menor = valor_temp;
				indice_del_menor = i;
			}
		}

		/* sumamos piezas que atacan y defienden mediante rayos x */
		if (atacantes[turno2][indice_del_menor].deslizante == 1){
			int ndir = atacantes[turno2][indice_del_menor].direccion;
			int principio = atacantes[turno2][indice_del_menor].casilla;
			ca_orig = mailbox64[principio]+ndir;
			n = mailbox[ca_orig];
			while (n != -1) {
				if (pieza[n] == ALFIL || pieza[n] == DAMA) {
                    if (color[n] == BLANCO) {
                        atacantes[BLANCO][num_atacantes[BLANCO]].casilla = n;
                        atacantes[BLANCO][num_atacantes[BLANCO]].deslizante = 1;
                        atacantes[BLANCO][num_atacantes[BLANCO]].direccion = ndir;
                        num_atacantes[BLANCO]++;
                        break;
                    }
                    else {
                        atacantes[NEGRO][num_atacantes[NEGRO]].casilla = n;
                        atacantes[NEGRO][num_atacantes[NEGRO]].deslizante = 1;
                        atacantes[NEGRO][num_atacantes[NEGRO]].direccion = ndir;
                        num_atacantes[NEGRO]++;
                        break;
                    }
				}
				else if (color[n] == !VACIO) break;
				ca_orig += ndir;
				n = mailbox[ca_orig];
			}
		}
		else if (atacantes[turno2][indice_del_menor].deslizante == 2){
			int ndir = atacantes[turno2][indice_del_menor].direccion;
			int principio = atacantes[turno2][indice_del_menor].casilla;
			ca_orig = mailbox64[principio]+ndir;
			n = mailbox[ca_orig];
			while (n != -1) {
				if (pieza[n] == TORRE || pieza[n] == DAMA) {
                    if (color[n] == BLANCO) {
                        atacantes[BLANCO][num_atacantes[BLANCO]].casilla = n;
                        atacantes[BLANCO][num_atacantes[BLANCO]].deslizante = 2;
                        atacantes[BLANCO][num_atacantes[BLANCO]].direccion = ndir;
                        num_atacantes[BLANCO]++;
                        break;
                    }
                    else {
                        atacantes[NEGRO][num_atacantes[NEGRO]].casilla = n;
                        atacantes[NEGRO][num_atacantes[NEGRO]].deslizante = 2;
                        atacantes[NEGRO][num_atacantes[NEGRO]].direccion = ndir;
                        num_atacantes[NEGRO]++;
                        break;
                    }
				}
				else if (color[n] == !VACIO) break;
				ca_orig += ndir;
				n = mailbox[ca_orig];
			}
		}
		/* Intercambiamos la posicion de la ficha que hemos utilizado, con la que hemos comparado, para no volver a utilizarla */
		casilla_del_menor = atacantes[turno2][indice_del_menor].casilla;
		atacantes[turno2][indice_del_menor] = atacantes[turno2][--num_atacantes[turno2]];
		atacantes[turno2][num_atacantes[turno2]].casilla = casilla_del_menor;

		valorsee[d] =  ganancia - valorsee[d-1];

		/* ahora es donde comprobamos si podemos salir de la funcion sabiendo que see sera menor que cero */
		if (rapido && -MIN(-valorsee[d], valorsee[d-1]) < 0) break;

		/* cambiamos el turno */
		d++;
		ganancia = valor_del_menor;
		turno2 ^= 1;
	}

	while(--d)
		valorsee[d-1] = MIN(-valorsee[d], valorsee[d-1]);

	/* restauramos capturador */
	pieza[de] = origpieza;
	color[de] = origcolor;

	return (valorsee[0]);
}
