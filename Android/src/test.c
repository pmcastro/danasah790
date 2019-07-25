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
Aquí tenemos funciones para comprobar el correcto funcionamiento del motor y poder generar un ejecutable optimizado.
*/

/*Includes*/
#include <stdio.h>
#include <string.h>
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* esta función nos permite comprobar el generador de movimientos contando el numero de nodos totales para una determinada profundidad */
U64 perft(int depth)
{
	movimiento lista[256];
	int contar = 0, i;
	U64 nodes = 0;

	/* si hemos alcanzado la máxima profundidad, retornamos */
	if (!depth) return 1;

	contar = Generar(lista);     /* generamos todos los movimientos para la posición actual */

	/* recorremos la lista de movimientos para la profundidad actual */
	for (i = 0; i < contar; ++i) {
		if (!Hacer_Movimiento(lista[i]))
			continue;

		/* vamos más profundo en el árbol recursivamente */
		nodes += perft(depth-1);

		/* Deshacemos el movimiento para ir al próximo */
		Deshacer();
	}
	return nodes;
}

/* esta funcion resuelve 25 posiciones (WAC) durante 20 segundos, se puede utilizar si vamos a realizar un
profile guided optimization del ejecutable */
void Test(void)
{
	movimiento mejormovimiento, pondermovimiento;

	tiempo_total = 20000;
	no_nueva_iteracion = tiempo_total;
	no_nuevo_movimiento = tiempo_total;
	limite_tiempo = tiempo_total;
	max_profundidad = MAX_PROFUNDIDAD;
	estado = BUSCANDO;

	printf("# 1:\n");
	SetBoard("5rk1/1p6/q2P3p/2p2rp1/p1nbQ3/P1N3BP/1PR1B1P1/4K3b--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 2:\n");
	SetBoard("4r1k1/1pq2p1p/2p2npb/2N5/1PPQn1P1/7P/6B1/B2R2K1b--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 3:\n");
	SetBoard("2q1r2k/5R1p/pp1B2pN/2p1P3/1n1b4/3P2Q1/1P4K1/8w--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 4:\n");
	SetBoard("6rk/7p/Q2R1p2/1p2n3/4b3/1P4NP/P1P2PPK/2q5b--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 5:\n");
	SetBoard("2k1r2r/ppn3pq/b7/5p2/5Qn1/2N1B1P1/1P3PB1/2RR2K1w--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 6:\n");
	SetBoard("2k4r/pp3ppp/2p2n2/5PBb/P3P3/2Nr3P/1PB2bP1/R3R2Kb--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 7:\n");
	SetBoard("r5k1/pq2pn2/3pN1pp/n2P4/1rp1PQ2/1P3PP1/PN4K1/2R4Rw--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 8:\n");
	SetBoard("3r1r2/p4pk1/bp2p1P1/4P2p/Pq1N1P2/2p4Q/2P3P1/3RK2Rw--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 9:\n");
	SetBoard("rr4k1/2q2ppp/p1b1p3/2b1pPPN/1p2P1R1/3B3Q/PPP4P/1K1R4w--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 10:\n");
	SetBoard("2r1qrk1/3n3p/b3pPp1/4P3/1pp1nBN1/pP4PQ/P1P2PK1/3RR3w--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 11:\n");
	SetBoard("4rrk1/1b3p1p/pp2p1p1/2qnN1Pn/3N1P2/2P4R/PPB1Q2P/5RK1w--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 12:\n");
	SetBoard("6k1/pp3p1p/3b3r/3p3q/P2n2p1/1P1Pp1P1/1N2P1QP/2R2RK1b--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 13:\n");
	SetBoard("4r3/pbrq1p1k/1p3Bpp/3pPn2/5PQ1/P6R/1P4PP/1B3RK1w--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 14:\n");
	SetBoard("r1r3k1/1p1b1nq1/6p1/pN1p4/P2Np2R/1P2P3/1Q3PP1/1K5Rw--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 15:\n");
	SetBoard("r2q1rk1/pb2Bppp/4p3/1p6/2pnP1n1/6N1/PPPN2PP/R1Q1RBK1b--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 16:\n");
	SetBoard("6k1/5qp1/pN5p/3pRp2/3QnP2/2r4P/r5P1/6RKb--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 17:\n");
	SetBoard("2b2rk1/pp2R1bp/1qpp2p1/6N1/2P2PP1/3Q1B2/PP4PK/8w--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 18:\n");
	SetBoard("1q3rnk/5p2/3p1P1p/p3n3/Np1r2P1/1P1B4/1PP4Q/1K1R3Rw--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 19:\n");
	SetBoard("3r3k/p5qp/bp2B1p1/n2P1p2/5N2/Q1R5/P4PPP/6K1w--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 20:\n");
	SetBoard("r4qk1/1b3rpp/1pn1p3/p1b5/2P5/2NQ1N2/PP2BPPP/3R1RK1b--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 21:\n");
	SetBoard("4k3/pp2pp2/4n1p1/3RP1P1/2r1N3/P1P5/1P2q3/1K5Qw--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 22:\n");
	SetBoard("5rk1/5pp1/3b4/1pp2qB1/4R2Q/1BPn4/1P3PPP/6K1b--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 23:\n");
	SetBoard("3r2k1/p1rq1pp1/1pR1p2p/8/Q2P4/4P3/P4PPP/2R3K1b--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 24:\n");
	SetBoard("1q4r1/3Q1Npk/p6p/1p5N/8/7P/Pn3PP1/6K1w--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	printf("# 25:\n");
	SetBoard("5k2/1pp2b1p/2p5/r3PPR1/pr6/2N5/1PP4P/2KR4w--");
	MotorPiensa(&mejormovimiento, &pondermovimiento);
	printf("\n");
	return;
}

/* esta función resuelve un mate en 7, nos sirve para comprobar la velocidad relativa del ordenador respecto a
uno determinado, tambien para comporar por ejemplo diferentes compilaciones del codigo con diferentes compiladores
u opciones del compilador */
void bench(void)
{
	int i, t;
	double nps[2], npst;
	movimiento mejormovimiento, pondermovimiento;

	tiempo_total = 1 << 25;
	no_nueva_iteracion = tiempo_total;
	no_nuevo_movimiento = tiempo_total;
	limite_tiempo = tiempo_total;
	max_profundidad = 20;
	estado = BUSCANDO;

	variante[0] = '\0';
	strcpy(variante,"normal");
	printf("# Variante = %s\n", variante);
	printf("# Searching Mate in 7... \n");
	printf("# r3rk2/ppq2pbQ/2p1b1p1/4p1B1/2P3P1/3P1B2/P3PPK1/1R5R w - - \n");
	printf("\n");

	/* la resolucion del problema se realiza 2 veces por seguridad y luego escogemos la mas rapida */
	for (i = 0; i < 2; ++i) {
		SetBoard("r3rk2/ppq2pbQ/2p1b1p1/4p1B1/2P3P1/3P1B2/P3PPK1/1R5Rw--");
		MotorPiensa(&mejormovimiento, &pondermovimiento);
		t = get_ms() - tiempo_inicio;
		printf("\n");
		printf("# Time: %d ms\n", t);
		printf("# Nodes: %d\n", nodos);
		printf("# \n");
		nps[i] = (double)nodos / (double)t;
		nps[i] *= 1000.0;
	}
	if (nps[0] > nps[1]) npst = nps[0];
	else npst = nps[1];
	/* Score: 1.000 = my AthlonII X2 250 */
	printf("# Best Nodes per second: %d (Score: %.3f)\n", (int)npst, (float)npst/753903.0);
	printf("# Score: 1.000 on AthlonII X2 250\n");
	printf("\n");
	printf("# Press Return key to continue "); getchar();
	return;
}

