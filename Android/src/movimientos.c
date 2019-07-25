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
Funciones para generar y realizar un movimiento, estan basadas en el generador de movimientos del programa FirstChess, se ha añadido
un generador de capturas y promociones para evitar el efecto horizonte y la generación de enroques y al paso.
*/

/*Includes*/
#include <string.h>
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* Generamos los movimientos ordenados para la PVS */
void Generar_Movimiento_PVS(int de, int a, int tipo, movimiento *plista, int *pcuenta)
{
	movimiento	m;

	/* de un movimiento guardamos la casilla inicial, final, tipo de movimiento y valor	de lo bueno que podria ser el movimiento para
	luego poder ordenarlo */
	m.de = de;
	m.a = a;
	m.tipo = tipo;
	m.valor = 0;

	/* hacemos que puntuen mas el movimiento matekiller, las promociones a dama, las capturas de ganancia e intercambios de piezsas iguales,
	movimientos killer, resto de promociones, capturas malas y demas movimientos por historia. Durante la busqueda se añaden al principio
	de la lista el movimiento que viene de la hash y el movimiento que hace seguir la variante principal de la anterior iteracion. */
	if (m.tipo >= PROMOCION_A_DAMA) {                                                   /* si hay promocion */
		if (m.tipo == PROMOCION_A_DAMA) m.valor = 4000000;	                            /* promocionamos a dama */
		else if (m.tipo == PROMOCION_A_CABALLO) m.valor = 2300000;						/* promocionamos a caballo */
		else if (m.tipo == PROMOCION_A_TORRE) m.valor = 2200000;						/* promocionamos a torre */
		else if (m.tipo == PROMOCION_A_ALFIL) m.valor = 2100000;						/* promocionamos a alfil */
	}
	else {                                                                              /* no hay promoción */
		if ((color[a] != VACIO)) {														/* es una captura? */
			if (malaCaptura(m))
				m.valor = 900000 + (pieza[a]*100) - (pieza[de]);						/* malas capturas por debajo de los movimientos killer */
			else
				m.valor = 3000000 + (pieza[a]*100) - (pieza[de]);						/* capturas ordenadas por MVV/LVA */
		}
		else {
			if ((m.de == matekiller[ply].de) && (m.a == matekiller[ply].a))             /* mate killer */
				m.valor = 5000000;
			else if ((m.de == killer1[ply].de) && (m.a == killer1[ply].a))				/* movimientos killer */
				m.valor = 2800000;
			else if (ply >= 2 && (m.de == killer1[ply-2].de) && (m.a == killer1[ply-2].a))
				m.valor = 2700000;
			else if ((m.de == killer2[ply].de) && (m.a == killer2[ply].a))
				m.valor = 2600000;
			else if (ply >= 2 && (m.de == killer2[ply-2].de) && (m.a == killer2[ply-2].a))
				m.valor = 2500000;
			else
				m.valor = Historia[de][a];                                             /* heuristica de la historia */
		}
	}

#if defined (UNDER_ELO)
	/* solo para la version limitada en fuerza */
	if (easylevel == 1) {							/* para el modo de movimiento aleatorio solo generamos movimientos legales */
		if (Hacer_Movimiento(m)) {
			plista[*pcuenta] = m;
			*pcuenta = *pcuenta + 1;
			Deshacer();
		}
	}
	else {
		plista[*pcuenta] = m;						/* para el resto generamos movimientos pseudo-legales */
		*pcuenta = *pcuenta + 1;
	}
#else
	/* para la version normal */
	plista[*pcuenta] = m;							/* generamos movimientos pseudo-legales que es mas rapido */
	*pcuenta = *pcuenta + 1;
#endif
}

/* genera los movimientos vamos a llamar normales, movimientos que no son de peon y por tanto promociones, enroques y al paso */
static void Generar_Movimiento_Normal_PVS(int de, int a, movimiento *plista, int *pcuenta)
{
    Generar_Movimiento_PVS(de, a, NORMAL, plista, pcuenta);
}

/* genera los movimientos de peon incluido las promociones */
static void Generar_Movimiento_PEON_PVS(int de, int a, movimiento *plista, int *pcuenta)
{
    if (a > 7 && a < 56) {
		Generar_Movimiento_PVS(de, a, NORMAL, plista, pcuenta);
	} else {
		Generar_Movimiento_PVS(de, a, PROMOCION_A_DAMA, plista, pcuenta);
        Generar_Movimiento_PVS(de, a, PROMOCION_A_TORRE, plista, pcuenta);
        Generar_Movimiento_PVS(de, a, PROMOCION_A_ALFIL, plista, pcuenta);
        Generar_Movimiento_PVS(de, a, PROMOCION_A_CABALLO, plista, pcuenta);
	}
}

/* genera los movimientos que avanzan 2 casillas, es un caso especial por posible comidas al paso */
static void Generar_Movimiento_PEON2_PVS(int de, int a, movimiento *plista, int *pcuenta)
{
	Generar_Movimiento_PVS(de, a, PEON_AVANZA_2, plista, pcuenta);
}

/* general los movmientos posibles de enroque */
static void Generar_Movimiento_Enroque_PVS(int de, int a, movimiento *plista, int *pcuenta)
{
	Generar_Movimiento_PVS(de, a, ENROQUE, plista, pcuenta);
}

/* genera los movmientos posibles de comer al paso */
static void Generar_Movimiento_Alpaso_PVS(int de, int a, movimiento *plista, int *pcuenta)
{
	Generar_Movimiento_PVS(de, a, AL_PASO, plista, pcuenta);
}

/* Genera los movimientos para la quiesce pero solo capturas y promocion a dama */
void Generar_Movimiento_Capturas(int de, int a, int tipo, movimiento *plista, int *pcuenta)
{
	movimiento	m;

	/* de un movimiento guardamos la casilla inicial, final, tipo de movimiento y valor */
	m.de = de;
	m.a = a;
	m.tipo = tipo;
	m.valor = 0;

	/* en la quiesce primero ordenamos el movimiento de promocion a dama y luego las capturas por MVV/LVA */
	if (m.tipo == PROMOCION_A_DAMA) m.valor = 4000000;			/* promocion a dama */
	else {
		if (malaCaptura(m))
			m.valor = -1;										/* marcamos las capturas que sabemos que son malas */
		else
			m.valor = 3000000 + (pieza[a]*100) - (pieza[de]);	/* ordenamos capturas por MVV/LVA */
	}
	plista[*pcuenta] = m;
	*pcuenta = *pcuenta + 1;
}

/* Generamos los movimientos normales de captura para la quiesce, no incluye las capturas de peon (ni promocion a dama) ni alpaso */
static void Generar_Movimiento_Normal_Capturas(int de, int a, movimiento *plista, int *pcuenta)
{
    Generar_Movimiento_Capturas(de, a, NORMAL, plista, pcuenta);
}

/* Generamos las capturas de peon para la quiesce */
static void Generar_Movimiento_PEON_Capturas(int de, int a, movimiento *plista, int *pcuenta)
{
    if (a > 7 && a < 56) {
		Generar_Movimiento_Capturas(de, a, NORMAL, plista, pcuenta);
	} else {
		/* en la quiesce solo comprobamos promociones a dama */
		Generar_Movimiento_Capturas(de, a, PROMOCION_A_DAMA, plista, pcuenta);
	}
}

/* Generamos las capturas alpaso para la quiesce */
static void Generar_Movimiento_Alpaso_Capturas(int de, int a, movimiento *plista, int *pcuenta)
{
	Generar_Movimiento_Capturas(de, a, AL_PASO, plista, pcuenta);
}

/* Generar todos los pseudo-movimientos y retornar el numero de ellos. No comprueba si nos quedamos en jaque. Esta es la funcion que es
llamada desde la busqueda */
int	Generar(movimiento *plista)
{
    int i, j, k, y,
        fila, columna,
		cuenta, total_piezas;

	/* vamos a calcular el numero de piezas totales para cada color */
	if (turno == BLANCO)
		total_piezas = peonesblancos + caballosblancos + alfilesblancos + torresblancas + damasblancas + 1;
	else
		total_piezas = peonesnegros + caballosnegros + alfilesnegros + torresnegras + damasnegras + 1;

	cuenta = 0;
	/* gracias a que sabemos el numero de piezas totales por color lo emplearemos para recorrer el tablero y en lugar de escanear las
	64 casillas buscamos hasta que alcanzamos el numero de piezas totales, es una pequeña optimizacion, mejor seria emplear una lista
	de piezas */
    for (i = 0, j = 1; j <= total_piezas; i++) {
        if (color[i] == turno) {
			j++;
            switch (pieza[i]) {
            case PEON:																	/* generamos movimientos para el peon */
                columna = COLUMNA(i);
                fila = FILA(i);
                if (turno == NEGRO) {													/* turno negro */
                    if (columna && color[i + 7] == BLANCO)
                        Generar_Movimiento_PEON_PVS(i, i + 7, plista, &cuenta);     	/* come en diagonal - izquierda */
                    if (columna < 7 && color[i + 9] == BLANCO)
                        Generar_Movimiento_PEON_PVS(i, i + 9, plista, &cuenta);	    	/*come en diagonal - derecha */
					if (color[i + 8] == VACIO) {
                        Generar_Movimiento_PEON_PVS(i, i + 8, plista, &cuenta);			/* avanza una casilla */
						if (fila == 1 && color[i + 16] == VACIO)
							Generar_Movimiento_PEON2_PVS(i, i + 16, plista, &cuenta);	/* avanza dos casillas */
					}
                } else {																/* turno blanco */
                    if (columna && color[i - 9] == NEGRO)
                        Generar_Movimiento_PEON_PVS(i, i - 9, plista, &cuenta);
                    if (columna < 7 && color[i - 7] == NEGRO)
                        Generar_Movimiento_PEON_PVS(i, i - 7, plista, &cuenta);
					if (color[i - 8] == VACIO) {
                        Generar_Movimiento_PEON_PVS(i, i - 8, plista, &cuenta);
						if (fila == 6 && color[i - 16] == VACIO)
							Generar_Movimiento_PEON2_PVS(i, i - 16, plista, &cuenta);
					}
                }
                break;

            case CABALLO:																/* generamos movimientos para el caballo */
                columna = COLUMNA(i);
                y = i - 6;
                if (y >= 0 && columna < 6 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i - 10;
                if (y >= 0 && columna > 1 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i - 15;
                if (y >= 0 && columna < 7 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i - 17;
                if (y >= 0 && columna > 0 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i + 6;
                if (y < 64 && columna > 1 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i + 10;
                if (y < 64 && columna < 6 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i + 15;
                if (y < 64 && columna > 0 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i + 17;
                if (y < 64 && columna < 7 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                break;

            case DAMA:         															/* genera movimientos para la dama = torre+alfil */
            case ALFIL:																	/* genera movimientos para el alfil */
                for (y = i - 9; y >= 0 && COLUMNA(y) != 7; y -= 9) {
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);			/* abajo izquierda */
                    if (color[y] != VACIO)
                        break;
                }
                for (y = i - 7; y >= 0 && COLUMNA(y) != 0; y -= 7) {
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);			/* abajo derecha */
                    if (color[y] != VACIO)
                        break;
                }
                for (y = i + 9; y < 64 && COLUMNA(y) != 0; y += 9) {
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);			/* arriba derecha */
                    if (color[y] != VACIO)
                        break;
                }
                for (y = i + 7; y < 64 && COLUMNA(y) != 7; y += 7) {
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);			/* arriba izquierda */
                    if (color[y] != VACIO)
                        break;
                }
                if (pieza[i] == ALFIL) break;													/* si la pieza es dama continua */
#if defined (UNDER_64BIT)
                /*__attribute__ ((fallthrough));*/
#endif
            case TORRE:																	/* genera movimientos para la torre */
                columna = COLUMNA(i);
                for (k = i - columna, y = i - 1; y >= k; y--) {
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);			/* izquierda */
                    if (color[y] != VACIO)
                        break;
                }
                for (k = i - columna + 7, y = i + 1; y <= k; y++) {
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);			/* derecha */
                    if (color[y] != VACIO)
                        break;
                }
                for (y = i - 8; y >= 0; y -= 8) {
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);			/* arriba */
                    if (color[y] != VACIO)
                        break;
                }
                for (y = i + 8; y < 64; y += 8) {
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);			/* abajo */
                    if (color[y] != VACIO)
                        break;
                }
                break;

            case REY:																	/* genera los movimientos para el rey */
                columna = COLUMNA(i);
                if (columna && color[i - 1] != turno)
                    Generar_Movimiento_Normal_PVS(i, i - 1, plista, &cuenta);   		/* izquierda */
                if (columna < 7 && color[i + 1] != turno)
                    Generar_Movimiento_Normal_PVS(i, i + 1, plista, &cuenta);   		/* derecha */
                if (i > 7 && color[i - 8] != turno)
                    Generar_Movimiento_Normal_PVS(i, i - 8, plista, &cuenta);   		/* arriba */
                if (i < 56 && color[i + 8] != turno)
                    Generar_Movimiento_Normal_PVS(i, i + 8, plista, &cuenta);   		/* abajo */
                if (columna && i > 7 && color[i - 9] != turno)
                    Generar_Movimiento_Normal_PVS(i, i - 9, plista, &cuenta);   		/* izquierda arriba */
                if (columna < 7 && i > 7 && color[i - 7] != turno)
                    Generar_Movimiento_Normal_PVS(i, i - 7, plista, &cuenta);   		/* derecha arriba */
                if (columna && i < 56 && color[i + 7] != turno)
                    Generar_Movimiento_Normal_PVS(i, i + 7, plista, &cuenta);   		/* izquierda abajo */
                if (columna < 7 && i < 56 && color[i + 9] != turno)
                    Generar_Movimiento_Normal_PVS(i, i + 9, plista, &cuenta);   		/* derecha abajo */
                break;

            default:
                break;
            }
        }
	}

	/* generamos los enroques */
	if (turno == BLANCO) {
		if (enroque & 1)
			Generar_Movimiento_Enroque_PVS(rb, 62, plista, &cuenta);					/* enroque blanco corto */
		if (enroque & 2)
			Generar_Movimiento_Enroque_PVS(rb, 58, plista, &cuenta);					/* enroque blanco largo */
	}
	if (turno == NEGRO) {
		if (enroque & 4)
			Generar_Movimiento_Enroque_PVS(rn, 6, plista, &cuenta);						/* enroque negro corto */
		if (enroque & 8)
			Generar_Movimiento_Enroque_PVS(rn, 2, plista, &cuenta);						/* enroque negro largo */
	}

	/* generamos al paso */
	if (alpaso != -1) {
		if (turno == BLANCO) {
			if ((COLUMNA(alpaso) != 0) && (color[alpaso + 7] == BLANCO) && (pieza[alpaso + 7] == PEON))
				Generar_Movimiento_Alpaso_PVS(alpaso + 7, alpaso, plista, &cuenta);
			if ((COLUMNA(alpaso) != 7) && (color[alpaso + 9] == BLANCO) && (pieza[alpaso + 9] == PEON))
				Generar_Movimiento_Alpaso_PVS(alpaso + 9, alpaso, plista, &cuenta);
		}
		else {
			if ((COLUMNA(alpaso) != 0) && (color[alpaso - 9] == NEGRO) && (pieza[alpaso - 9] == PEON))
				Generar_Movimiento_Alpaso_PVS(alpaso - 9, alpaso, plista, &cuenta);
			if ((COLUMNA(alpaso) != 7) && (color[alpaso - 7] == NEGRO) && (pieza[alpaso - 7] == PEON))
				Generar_Movimiento_Alpaso_PVS(alpaso - 7, alpaso, plista, &cuenta);
		}
	}
    return cuenta;																		/* devuelve el numero de movimiento semi-legales */
}

/* Generar solo movimientos de captura y promociones. Es básicamente una copia de Generar. Esta funcion es llamada desde la quiesce */
int	Generar_Capturas(movimiento *plista)
{
    int i, j, k, y,
        columna,
		cuenta, cturno, total_piezas;

	if (turno == BLANCO)
		total_piezas = peonesblancos + caballosblancos + alfilesblancos + torresblancas + damasblancas + 1;
	else
		total_piezas = peonesnegros + caballosnegros + alfilesnegros + torresnegras + damasnegras + 1;

	cturno = (BLANCO+NEGRO)-turno;
	cuenta = 0;
    for (i = 0, j = 1; j <= total_piezas; i++) {
        if (color[i] == turno) {
			j++;
            switch (pieza[i]) {
            case PEON:
                columna = COLUMNA(i);
                if (turno == NEGRO) {
                    if ((color[i + 8] == VACIO) && (i>=48))     						/* promocion a dama negra */
                        Generar_Movimiento_PEON_Capturas(i, i + 8, plista, &cuenta);
                    if (columna && color[i + 7] == BLANCO)
                        Generar_Movimiento_PEON_Capturas(i, i + 7, plista, &cuenta);
                    if (columna < 7 && color[i + 9] == BLANCO)
                        Generar_Movimiento_PEON_Capturas(i, i + 9, plista, &cuenta);
                } else
				if (turno == BLANCO) {
                    if ((color[i - 8] == VACIO) && (i<=15))								/* promocion a dama blanca */
                        Generar_Movimiento_PEON_Capturas(i, i - 8, plista, &cuenta);
                    if (columna && color[i - 9] == NEGRO)
                        Generar_Movimiento_PEON_Capturas(i, i - 9, plista, &cuenta);
                    if (columna < 7 && color[i - 7] == NEGRO)
                        Generar_Movimiento_PEON_Capturas(i, i - 7, plista, &cuenta);
                }
                break;

            case CABALLO:
                columna = COLUMNA(i);
                y = i - 6;
                if (y >= 0 && columna < 6 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i - 10;
                if (y >= 0 && columna > 1 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i - 15;
                if (y >= 0 && columna < 7 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i - 17;
                if (y >= 0 && columna > 0 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i + 6;
                if (y < 64 && columna > 1 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i + 10;
                if (y < 64 && columna < 6 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i + 15;
                if (y < 64 && columna > 0 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i + 17;
                if (y < 64 && columna < 7 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                break;

            case DAMA:
            case ALFIL:
                for (y = i - 9; y >= 0 && COLUMNA(y) != 7; y -= 9) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                for (y = i - 7; y >= 0 && COLUMNA(y) != 0; y -= 7) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                for (y = i + 9; y < 64 && COLUMNA(y) != 0; y += 9) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                for (y = i + 7; y < 64 && COLUMNA(y) != 7; y += 7) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                if (pieza[i] == ALFIL) break;
#if defined (UNDER_64BIT)
                /*__attribute__ ((fallthrough));*/
#endif

            case TORRE:
                columna = COLUMNA(i);
                for (k = i - columna, y = i - 1; y >= k; y--) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                for (k = i - columna + 7, y = i + 1; y <= k; y++) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                for (y = i - 8; y >= 0; y -= 8) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
						}
                }
                for (y = i + 8; y < 64; y += 8) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                break;

            case REY:
                columna = COLUMNA(i);
                if ((columna > 1) && (color[i - 1] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i - 1, plista, &cuenta);
                if ((columna < 7) && (color[i + 1] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i + 1, plista, &cuenta);
                if ((i > 7) && (color[i - 8] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i - 8, plista, &cuenta);
                if ((i < 56) && (color[i + 8] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i + 8, plista, &cuenta);
                if (columna > 1 && i > 7 && (color[i - 9] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i - 9, plista, &cuenta);
                if (columna < 7 && i > 7 && (color[i - 7] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i - 7, plista, &cuenta);
                if (columna > 1 && i < 56 && (color[i + 7] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i + 7, plista, &cuenta);
                if (columna < 7 && i < 56 && (color[i + 9] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i + 9, plista, &cuenta);
                break;

            default:
                break;
            }
        }
	}

	if (alpaso != -1) {
		if (turno == BLANCO) {
			if ((COLUMNA(alpaso) != 0) && (color[alpaso + 7] == BLANCO) && (pieza[alpaso + 7] == PEON))
				Generar_Movimiento_Alpaso_Capturas(alpaso + 7, alpaso, plista, &cuenta);
			if ((COLUMNA(alpaso) != 7) && (color[alpaso + 9] == BLANCO) && (pieza[alpaso + 9] == PEON))
				Generar_Movimiento_Alpaso_Capturas(alpaso + 9, alpaso, plista, &cuenta);
		}
		else {
			if ((COLUMNA(alpaso) != 0) && (color[alpaso - 9] == NEGRO) && (pieza[alpaso - 9] == PEON))
				Generar_Movimiento_Alpaso_Capturas(alpaso - 9, alpaso, plista, &cuenta);
			if ((COLUMNA(alpaso) != 7) && (color[alpaso - 7] == NEGRO) && (pieza[alpaso - 7] == PEON))
				Generar_Movimiento_Alpaso_Capturas(alpaso - 7, alpaso, plista, &cuenta);
		}
	}
    return cuenta;
}

/* esta funcion es la encargada de realizar un movimiento y guardarlo en la lista de jugadas */
int Hacer_Movimiento(movimiento m)
{
	int de = -1, a = -1, i, viejo_enroque, viejo_alpaso;

	/* tenemos que mover la torre en caso de que haya un enroque */
	if (m.tipo == ENROQUE) {
		/* si estamos en jaque no podemos enrocar */
		if (EstaEnJaque(turno))
			return FALSO;
		/* en el caso de jugar la variante normal del ajedrez */
		if (strcmp (variante,"normal") == 0) {
			switch (m.a) {
				case 62:																			/* enroque blanco corto */
					/* si al mover el rey hacia su casilla de enroque la casilla esta atacada u ocupada no podemos enrocar */
					if ((color[61] != VACIO) || (color[62] != VACIO) ||	(atacado (61, BLANCO)))
						return FALSO;
					/* la torre se desplaza */
					de = 63;
					a = 61;
					break;
				case 58:																			/* enroque blanco largo */
					if ((color[59] != VACIO) || (color[58] != VACIO) || (color[57] != VACIO) ||	(atacado (59, BLANCO)))
						return FALSO;
					de = 56;
					a = 59;
					break;
				case 6:																				/* enroque negro corto */
					if ((color[5] != VACIO) || (color[6] != VACIO) || (atacado (5, NEGRO)))
						return FALSO;
					de = 7;
					a = 5;
					break;
				case 2:																				/* enroque negro largo */
					if ((color[3] != VACIO) || (color[2] != VACIO) || (color[1] != VACIO) || (atacado (3, NEGRO)))
						return FALSO;
					de = 0;
					a = 3;
					break;
				default: 																			/* no debería llegar aqui */
					de = -1;
					a = -1;
					break;
			}
		}
		/* en el caso de la variante chess960 se complica un poco el enroque ya que el rey y las torres no tienen unas casillas fijas
		en la primera fila */
		else if (strcmp (variante,"fischerandom") == 0) {
			switch (m.a) {
				case 62:																			/* enroque blanco corto */
					/* el rey blanco debe terminar en la casilla 62, comprobamos si antes de enrocar el rey esta a la izquierda de esta
					casilla */
					if (rb < 62 ) {
						/* las casillas que atraviese el rey hasta llegar a la casilla 62 deberian estar vacias, la unica excepcion es
						que puede saltar por encima de la propia torre del enroque */
						for (i = rb+1; i <= 62; i++) {
							if (color[i] != VACIO) {
								if ((pieza[i] == TORRE) && (color[i] == BLANCO) && (i == tb1))
									continue;
								else
									return FALSO;
							}
						}
						/* tambien tenemos que comprobar que las casillas hasta la 62 no estan atacadas por el contrario */
						for (i = rb+1; i < 62; i++) {
							if (atacado (i, BLANCO))
								return FALSO;
						}
					}
					/* si el rey esta ya en la casilla 62 tenemos que comprobar que la casilla a su izquierda esta libre para la torre */
					else if (rb == 62) {
						if (color[61] != VACIO)
							return FALSO;
					}
					de = tb1;
					a = 61;
					break;
				case 58:																			/* enroque blanco largo */
					/* en este caso el rey blanco debe acabar en la casilla 58, comprobamos si inicialmente el rey blanco esta a la
					derecha de esta casilla */
					if (rb > 58) {
						/* si la torre esta en la casilla 56 su posicion final sera la 58, pero para su desplazamiento la casilla 57 debe
						estar vacia, el resto de desplazamiento de torres esta contemplado mas adelante */
						if ((tb2 == 56) && (color[57] != VACIO))
							return FALSO;
						/* las casillas hasta la 58 no deben estar ocupadas, la excepcion puede ser que podemos saltar la torre del enroque */
						for (i = rb-1; i >= 58; i--) {
							if (color[i] != VACIO) {
								if ((pieza[i] == TORRE) && (color[i] == BLANCO) && (i == tb2))
									continue;
								else
									return FALSO;
							}
						}
						/* las casillas hasta la 58 no deben ser atacadas por el contrario */
						for (i = rb-1; i > 58; i--) {
							if (atacado (i, BLANCO))
								return FALSO;
						}
					}
					/* si el rey ya esta en la casilla 58 */
					else if (rb == 58) {
						/* si la torre esta en la casilla 57 solo nos tenemos que preocupar que la casilla a la derecha del rey este libre */
						if (tb2 == 57) {
							if (color[59] != VACIO)
								return FALSO;
						}
						/* si la torre esta en la casilla 58 ademas de lo anterior tiene que cumplirse que la casilla 57 este vacia */
						else if (tb2 == 56) {
							if ((color[57] != VACIO) || (color[59] != VACIO))
								return FALSO;
						}
					}
					/* si el rey esta en la casilla 57, la casilla 58 debe estar libre para colocar el rey y la casilla 59 debe estar
					libre para colocar la torre */
					else if (rb == 57) {
						if ((color[58] != VACIO) || (color[59] != VACIO))
							return FALSO;
					}
					de = tb2;
					a = 59;
					break;
				case 6:																				/* enroque negro corto */
					if (rn < 6) {
						for (i = rn+1; i <= 6; i++) {
							if (color[i] != VACIO) {
								if ((pieza[i] == TORRE) && (color[i] == NEGRO) && (i == tn1))
									continue;
								else
									return FALSO;
							}
						}
						for (i = rn+1; i < 6; i++) {
							if (atacado (i, NEGRO))
								return FALSO;
						}
					}
					else if (rn == 6) {
						if(color[5] != VACIO)
							return FALSO;
					}
					de = tn1;
					a = 5;
					break;
				case 2:																				/* enroque negro largo */
					if (rn > 2) {
						if ((tn2 == 0) && (color[1] != VACIO))
							return FALSO;
						for (i = rn-1; i >= 2; i--) {
							if (color[i] != VACIO) {
								if ((pieza[i] == TORRE) && (color[i] == NEGRO) && (i == tn2))
									continue;
								else
									return FALSO;
							}
						}
						for (i = rn-1; i > 2; i--) {
							if (atacado (i, NEGRO))
								return FALSO;
						}
					}
					else if (rn == 2) {
						if (tn2 == 1) {
							if (color[3] != VACIO)
								return FALSO;
						}
						else if (tn2 == 0) {
							if ((color[1] != VACIO) || (color[3] != VACIO))
								return FALSO;
						}
					}
					else if (rn == 1) {
						if ((color[2] != VACIO) || (color[3] != VACIO))
							return FALSO;
					}
					de = tn2;
					a = 3;
					break;
				default: 																			/* no debería llegar aqui */
					de = -1;
					a = -1;
					break;
			}
		}
	}

	/*guardamos en la lista el movimiento, pieza capturada, enroques, al paso, la regla de los 50 movimientos y la llave */
    jugadas[njugadas].m = m;
	if (m.tipo == ENROQUE)
		jugadas[njugadas].captura = VACIO;
	else
		jugadas[njugadas].captura = pieza[m.a];
	jugadas[njugadas].enroque = viejo_enroque = enroque;
	jugadas[njugadas].alpaso = viejo_alpaso = alpaso;
	jugadas[njugadas].regla50 = regla50;
	jugadas[njugadas].hash = hash;

	/* eliminamos la pieza de la casilla inicial para la llave */
	hash ^= hash_piezas[turno][pieza[m.de]][m.de];

	/* si la pieza esta en su sitio no la debemos borrar, esto es pensando en el rey en chess960 */
	if (m.tipo == ENROQUE && m.a == m.de) {
		pieza[m.a] = pieza[m.de];
		color[m.a] = color[m.de];
		/* desactivamos los enroques correspondientes aunque no hayamos movido el rey */
		if (m.a == 62 || m.a == 58) enroque &= 12;
		if (m.a == 6 || m.a == 2) enroque &= 3;
	}
	/* en la variante normal del ajedrez lo que tenemos que hacer es eliminar la pieza de la casilla inicial y llevarla a la final */
	else {
		pieza[m.a] = pieza[m.de];
		color[m.a] = color[m.de];
		pieza[m.de] = VACIO;
		color[m.de] = VACIO;
		/* actualizar enroques despues de mover un rey o torre */
		enroque &= enroque_mascara[m.de] & enroque_mascara[m.a];
	}

	/* actualizar la posibilidad alpaso después de avanzar un peon 2 casillas */
	if (m.tipo == PEON_AVANZA_2) {
		if (turno == BLANCO) {
			alpaso = m.a + 8;
		}
		else {
			alpaso = m.a - 8;
		}
	}
	else
		alpaso = -1;

	/* actualizar la regla de los 50 movimientos si no ha habido captura o movimiento de peon */
	if ((jugadas[njugadas].captura != VACIO) || (pieza[m.a] == PEON))
		regla50 = 0;
	else
		regla50++;

	/* actualizamos el numero de piezas y la situacion de los reyes */
	if (turno == NEGRO) {																			/* turno negro */
		/* situacion nueva del rey si lo hemos movido */
		if (pieza[m.a] == REY)
			rn = m.a;
		/* si capturamos una pieza, la eliminamos de la cuenta */
		if (jugadas[njugadas].captura != VACIO) {
			/* tambien debemos eliminar la pieza capturada de la llave */
			hash ^= hash_piezas[BLANCO][jugadas[njugadas].captura][m.a];
			switch(jugadas[njugadas].captura) {
				case PEON:
					peonesblancos--;
					break;
				case CABALLO:
					caballosblancos--;
					break;
				case ALFIL:
					alfilesblancos--;
					break;
				case TORRE:
					torresblancas--;
					break;
				case DAMA:
					damasblancas--;
					break;
                default:
                    break;
			}
		}
	}
	else {																							/* turno blanco */
		if (pieza[m.a] == REY)
			rb = m.a;
		if (jugadas[njugadas].captura != VACIO) {
			hash ^= hash_piezas[NEGRO][jugadas[njugadas].captura][m.a];
			switch(jugadas[njugadas].captura) {
				case PEON:
					peonesnegros--;
					break;
				case CABALLO:
					caballosnegros--;
					break;
				case ALFIL:
					alfilesnegros--;
					break;
				case TORRE:
					torresnegras--;
					break;
				case DAMA:
					damasnegras--;
					break;
                default:
                    break;
			}
		}
	}

	/* si estamos haciendo un enroque debemos mover la torre a su sitio */
	if (m.tipo == ENROQUE) {
		if (a != de) {
			pieza[a] = TORRE;
			color[a] = color[de];
			if ((m.a == 62 && tb1 != 62) || (m.a == 6 && tn1 != 6) || (m.a == 58 && tb2 != 58) || (m.a == 2 && tn2 != 2)) {
				/* quitamos la torre del punto inicial y en la llave si corresponde */
				hash ^= hash_piezas[color[de]][TORRE][de];
				color[de] = VACIO;
				pieza[de] = VACIO;
				/* al mover la torre del enroque la ponemos en la llave */
				hash ^= hash_piezas[color[a]][TORRE][a];
			}
		}
	}

    /* si coronamos cambia el peon por pieza */
	if (m.tipo >= PROMOCION_A_DAMA) {
        switch (m.tipo) {
			case PROMOCION_A_DAMA:
				pieza[m.a] = DAMA;
				if (turno == BLANCO) {
					peonesblancos--;
					damasblancas++;
				}
				else {
					peonesnegros--;
					damasnegras++;
				}
				break;

			case PROMOCION_A_TORRE:
				pieza[m.a] = TORRE;
				if (turno == BLANCO) {
					peonesblancos--;
					torresblancas++;
				}
				else {
					peonesnegros--;
					torresnegras++;
				}
				break;

			case PROMOCION_A_ALFIL:
				pieza[m.a] = ALFIL;
				if (turno == BLANCO) {
					peonesblancos--;
					alfilesblancos++;
				}
				else {
					peonesnegros--;
					alfilesnegros++;
				}
				break;

			case PROMOCION_A_CABALLO:
				pieza[m.a] = CABALLO;
				if (turno == BLANCO) {
					peonesblancos--;
					caballosblancos++;
				}
				else {
					peonesnegros--;
					caballosnegros++;
				}
				break;

            default:
                break;
        }
    }

	/* borra el peon si estamos comiendo alpaso */
	if (m.tipo == AL_PASO) {
		/* turno blanco */
		if (turno == BLANCO) {
			color[m.a + 8] = VACIO;
			pieza[m.a + 8] = VACIO;
			peonesnegros--;
			/* eliminamos el peon que comemos al paso de la llave */
			hash ^= hash_piezas[NEGRO][PEON][m.a + 8];
		}
		/* turno negro */
		else {
			color[m.a - 8] = VACIO;
			pieza[m.a - 8] = VACIO;
			peonesblancos--;
			hash ^= hash_piezas[BLANCO][PEON][m.a - 8];
		}
	}

	/* ponemos la pieza de la casilla final en la llave */
	hash ^= hash_piezas[turno][pieza[m.a]][m.a];

	/* eliminamos de la hash el turno ya que vamos a cambiar el turno */
	hash ^= hash_turno[turno];

	ply++;												/* aumentamos el contador de plys */
    njugadas++; 										/* nos preparamos para guardar la siguiente jugada */
    turno = 1 - turno;    								/* da el turno al oponente */

	/* comprobamos si estamos en jaque despues de mover ya que si es asi no podemos hacer el movimiento */
	if (EstaEnJaque(1 - turno)) {
		Deshacer();
		return FALSO;
	}

	/* actualizamos la llave con el turno, alpaso y enroques */
	hash ^= hash_turno[turno];
	if (viejo_alpaso != -1)
		hash ^= hash_alpaso[viejo_alpaso];
	if (alpaso != -1)
		hash ^= hash_alpaso[alpaso];
	if (viejo_enroque != enroque) {
		hash ^= hash_enroque[viejo_enroque];
		hash ^= hash_enroque[enroque];
	}

	/* calculamos el material segun el numero de piezas, esto posiblemente lo podemos retrasar durante la busqueda */
	Calcula_material();
	return VERDADERO;
}

/* esta funcion permite deshacer un movimiento, es necesaria para recorrer el arbol de movimientos hacia delante y detras */
void Deshacer(void)
{
	movimiento	m;
	int de = -1, a = -1, td = FALSO;

	/* recuperamos el turno, el numero de jugadas de la lista, el ply, el movimiento, los derechos de enoque, alpaso, regla de 50
	movimientos y la llave hash de la posicion */
	turno = 1 - turno;
    njugadas--;
	ply--;
    m = jugadas[njugadas].m;
	enroque = jugadas[njugadas].enroque;
	alpaso = jugadas[njugadas].alpaso;
	regla50 = jugadas[njugadas].regla50;
	hash = jugadas[njugadas].hash;

	/* si habiamos realizado un enroque ahora tenemos que situar la torre en su posicion inicial */
	if (m.tipo == ENROQUE) {
		switch(m.a) {
			case 62:																				/* enroque blanco corto */
				de = 61;
				a = tb1;
				break;
			case 58:																				/* enroque blanco largo */
				de = 59;
				a = tb2;
				break;
			case 6:																					/* enroque negro corto */
				de = 5;
				a = tn1;
				break;
			case 2:																					/* enroque negro largo */
				de = 3;
				a = tn2;
				break;
			default:  																				/* no debería llegar aqui */
				de = -1;
				a = -1;
				break;
		}
		/*si estamos deshaciendo un enroque, primero probamos a llevar la torre a su sitio original siempre y cuando no vaya a colocarse
		donde todavia esta el rey*/
		if (a != de && a != 62 && a != 6 && a != 2 && a != 58) {
			pieza[a] = pieza[de];
			color[a] = color[de];
			color[de] = VACIO;
			pieza[de] = VACIO;
			td = VERDADERO;
		}
	}

	/* si estamos en la variante chess960 y la posicion del rey es la misma a la que tiene que regresar */
	if (m.tipo == ENROQUE && m.a == m.de) {
		pieza[m.de] = pieza[m.a];
		color[m.de] = color[m.a];
	}
	/* en la variante normal del ajedrez debemos regresar la pieza movida a su posicion y colocar la pieza capturada */
	else {
		pieza[m.de] = pieza[m.a];
		color[m.de] = turno;
		pieza[m.a] = jugadas[njugadas].captura;
		if (jugadas[njugadas].captura != VACIO)
			color[m.a] = (BLANCO + NEGRO) - turno;
		else
			color[m.a] = VACIO;
	}

	/* actualizamos la posicion de los reyes */
	if (pieza[m.de] == REY && color[m.de] == BLANCO)
		rb = m.de;
	else if (pieza[m.de] == REY && color[m.de] == NEGRO)
		rn = m.de;

	/* actualizamos el numero de piezas */
	if (color[m.a] == BLANCO) {
		switch(pieza[m.a]) {
			case PEON:
				peonesblancos++;
				break;
			case CABALLO:
				caballosblancos++;
				break;
			case ALFIL:
				alfilesblancos++;
				break;
			case TORRE:
				torresblancas++;
				break;
			case DAMA:
				damasblancas++;
				break;
            default:
                break;
		}
	}
	else if (color[m.a] == NEGRO) {
		switch(pieza[m.a]) {
			case PEON:
				peonesnegros++;
				break;
			case CABALLO:
				caballosnegros++;
				break;
			case ALFIL:
				alfilesnegros++;
				break;
			case TORRE:
				torresnegras++;
				break;
			case DAMA:
				damasnegras++;
				break;
            default:
                break;
		}
	}

	/* si deshacemos una promocion colocamos el peon en sitio y elminamos la dama */
	if (m.tipo >= PROMOCION_A_DAMA) {
        pieza[m.de] = PEON;
        switch (m.tipo) {
			case PROMOCION_A_DAMA:
				if (turno == BLANCO) {
					peonesblancos++;
					damasblancas--;
				}
				else {
					peonesnegros++;
					damasnegras--;
				}
				break;

			case PROMOCION_A_TORRE:
				if (turno == BLANCO) {
					peonesblancos++;
					torresblancas--;
				}
				else {
					peonesnegros++;
					torresnegras--;
				}
				break;

			case PROMOCION_A_ALFIL:
				if (turno == BLANCO) {
					peonesblancos++;
					alfilesblancos--;
				}
				else {
					peonesnegros++;
					alfilesnegros--;
				}
				break;

			case PROMOCION_A_CABALLO:
				if (turno == BLANCO) {
					peonesblancos++;
					caballosblancos--;
				}
				else {
					peonesnegros++;
					caballosnegros--;
				}
				break;
            default:
                break;
        }
	}

	/* colocamos la torre en su sitio si deshacemos el enroque y todavia no lo hemos hecho */
	if (m.tipo == ENROQUE) {
		if (!td && a != de) {
			pieza[a] = TORRE;
			color[a] = color[de];
			if (m.de != de) {
				color[de] = VACIO;
				pieza[de] = VACIO;
			}
		}
	}

	/* coloca el peon contrario en su sitio si deshacemos alpaso */
	if (m.tipo == AL_PASO) {
		if (turno == BLANCO) {
			color[m.a + 8] = NEGRO;
			pieza[m.a + 8] = PEON;
			peonesnegros++;
		}
		else {
			color[m.a - 8] = BLANCO;
			pieza[m.a - 8] = PEON;
			peonesblancos++;
		}
	}
	/* vuelve a recalcular el valor del material en funcion del numero de piezas */
	Calcula_material();
}

/* esta funcion retorna el numero de movimientos legales, no es util fundamentalmente para saber que el numero de movimientos legales
es 1 ya que en este caso el motor no necesita pensar que mover, además genera la lista de movimientos para el root */
int Generar_root_legales(void) {
	int i, legales = 0;
	movimiento lista[256];

	root_contar = Generar(lista);

	for (i = 0; i < root_contar; ++i) {
        root_lista[i] = lista[i];
		if (!Hacer_Movimiento(lista[i])) {
			continue;
		}
        ++legales;
 		Deshacer();
    }
	return legales;
}
