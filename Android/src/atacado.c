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
La función principal aqui va a ser comprobar si una casilla es atacada por una pieza y en base a ello comprobamos si el rey está en jaque,
la función atacado esta basado en las características de generación de movimientos de FirstChess, el generador más simple posible, el
codigo seria mas sencillo si usamos mailbox pero no tendriamos ventajas en cuanto a velocidad
*/

/*Includes*/
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* comprueba si el rey esta en jaque, tenemos guardadas la posición de los reyes rb (rey blanco) y rn ya que vamos actualizando su
posicion en las funciones hacer_movimiento() y deshacer() */
int	EstaEnJaque(int turno2)
{
	if (turno2 == BLANCO)
		return atacado(rb, BLANCO);
	else
		return atacado(rn, NEGRO);
}

/* comprueba si una casilla k es atacada por una casilla de color contraria al turno que toca, retorna 1 (VERDADERO) si es atacada y
un 0 (FALSO) si no lo es, esta función la vamos a utilizar para saber si el rey está en jaque y tambien nos permitira controlar la
seguridad del rey sabiendo si una casilla cercana al rey está siendo atacada */
int atacado(int k, int turno2)
{
    int i, n, cturno, h, y, columna;

	cturno = (BLANCO + NEGRO) - turno2;     			/* quien nos da jaque tiene el turno contrario */
    columna = COLUMNA(k);

	/* Chequea las diagonales para los ataques de dama, alfil, rey y peon, direcciones a considerar {-9,-7,7,9} */
    /* abajo derecha */
    y = k + 9;
    if (y < 64 && COLUMNA(y) != 0) {
        if (color[y] == cturno) {
            if (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno2 == NEGRO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y += 9; y < 64 && COLUMNA(y) != 0; y += 9) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /* abajo izquierda */
    y = k + 7;
    if (y < 64 && COLUMNA(y) != 7) {
        if (color[y] == cturno) {
            if (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno2 == NEGRO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y += 7; y < 64 && COLUMNA(y) != 7; y += 7) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /* arriba izquierda */
    y = k - 9;
    if (y >= 0 && COLUMNA(y) != 7) {
        if (color[y] == cturno) {
            if (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno2 == BLANCO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y -= 9; y >= 0 && COLUMNA(y) != 7; y -= 9) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /* arriba derecha */
    y = k - 7;
    if (y >= 0 && COLUMNA(y) != 0) {
        if (color[y] == cturno) {
            if (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno2 == BLANCO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y -= 7; y >= 0 && COLUMNA(y) != 0; y -= 7) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

	/* movimientos estilo caballo */
	for (i = 0; i < 8; i++) {
		n = mailbox[mailbox64[k]+offset[CABALLO][i]];
		if (n != -1 && pieza[n] == CABALLO && color[n] == cturno) return VERDADERO;
	}

	/* Chequea las lineas horizontales y verticales para los ataques de dama, torre y rey, direcciones a considerar {-8,-1,1,8} */
    /* abajo */
    y = k + 8;
    if (y < 64) {
    	/* comprobamos un ataque en la casilla contigua siguiendo la direccion +8 */
        if (color[y] == cturno && (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        /* la dama y torre pueden atacar con casillas vacias de por medio */
        if (pieza[y] == VACIO)
            for (y += 8; y < 64; y += 8) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                /* si hay por medio una pieza la dama y torre no podran atacar */
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /* izquierda */
    y = k - 1;
    h = k - columna;
    if (y >= h) {
        if (color[y] == cturno && (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y--; y >= h; y--) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /* derecha */
    y = k + 1;
    h = k - columna + 7;
    if (y <= h) {
        if (color[y] == cturno && (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y++; y <= h; y++) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /* arriba */
    y = k - 8;
    if (y >= 0) {
        if (color[y] == cturno && (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y -= 8; y >= 0; y -= 8) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

    return 0;
}

/* ahora vamos a tener una variante de la funcion atacado, en este caso es para utilizarla en la seguridad del rey y saber si una casilla
cercana al rey está defendida por las piezas del mismo color excepto por el propio rey */
int atacado_nr(int k, int turno2)
{
   int i, n, cturno, h, y, columna;

	cturno = (BLANCO + NEGRO) - turno2;     			/* quien nos da jaque tiene el turno contrario */
    columna = COLUMNA(k);

	/* Chequea las diagonales para los ataques de dama, alfil y peon */
    y = k + 9;
    if (y < 64 && COLUMNA(y) != 0) {
        if (color[y] == cturno) {
            if (pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno2 == NEGRO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y += 9; y < 64 && COLUMNA(y) != 0; y += 9) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

    y = k + 7;
    if (y < 64 && COLUMNA(y) != 7) {
        if (color[y] == cturno) {
            if (pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno2 == NEGRO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y += 7; y < 64 && COLUMNA(y) != 7; y += 7) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

    y = k - 9;
    if (y >= 0 && COLUMNA(y) != 7) {
        if (color[y] == cturno) {
            if (pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno2 == BLANCO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] ==	VACIO)
            for (y -= 9; y >= 0 && COLUMNA(y) != 7; y -= 9) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

    y = k - 7;
    if (y >= 0 && COLUMNA(y) != 0) {
        if (color[y] == cturno) {
            if (pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno2 == BLANCO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y -= 7; y >= 0 && COLUMNA(y) != 0; y -= 7) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

	/* movimientos estilo caballo */
	for (i = 0; i < 8; i++) {
		n = mailbox[mailbox64[k]+offset[CABALLO][i]];
		if (n != -1 && pieza[n] == CABALLO && color[n] == cturno) return VERDADERO;
	}

	/* Chequea las lineas horizontales y verticales para los ataques de dama, torre */
    y = k + 8;
    if (y < 64) {
        if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y += 8; y < 64; y += 8) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

    y = k - 1;
    h = k - columna;
    if (y >= h) {
        if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y--; y >= h; y--) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

    y = k + 1;
    h = k - columna + 7;
    if (y <= h) {
        if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y++; y <= h; y++) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

    y = k - 8;
    if (y >= 0) {
        if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y -= 8; y >= 0; y -= 8) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

    return 0;
}
