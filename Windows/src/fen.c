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
Funciones para manejar archivos del tipo fen
*/

/*Includes*/
#include <stdio.h>				/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <stdlib.h>
#include <string.h>				/*strncat ...*/
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* recibimos una posición en formato FEN desde la GUI (aqui no vamos a indicar como funciona una posicion FEN utiizar para ello la wikipedia)
 y configuramos las variables del motor acorde a ello, un ejemplo de posicion FEN, la posicion inicial es:
 rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - */
void SetBoard(char *string)
{
	char c;											/* guarda un caracter de la posicion */
	char r;											/* calcula la posicion del rey */
	int sq=0;										/* casilla correspondiente al caracter */
	int i=0;										/* posicion en el FEN */
	int j=0;										/* para controlar casillas vacias */
	/* tipo de pieza según el caracter que recibimos: tenemos que poder convertir un caracter recibido en un tipo de pieza, para ello
	tenemos que conocer como danasah ha definido cada una de sus piezas con un numero y conociendo la posicion de la letra en el alfabeto
	podremos transformarla con la siguiente tabla en una pieza (no se ha tenido en cuenta la letra ñ).
	 B(b) es bishop y esta definido con un 2, K(k) es king y definido con un 5, K(k) es knigth y esta definida con un 1,
	 P(p) es pawn y esta definido con 0, R(r) es rook y esta definida con 3 */
	int	tpieza[26] = {6,2,6,6,6,6,6,6,6,6,5,6,6,1,6,0,4,3,6,6,6,6,6,6,6,6};
	int ntb = 0, ntn = 0;

	/* numero de piezas iniciales */
	peonesblancos = peonesnegros = 0;
	caballosblancos = caballosnegros = 0;
	alfilesblancos = alfilesnegros = 0;
	torresblancas = torresnegras = 0;
	damasblancas = damasnegras = 0;

	/* Rellenamos la tabla de posiciones y colores */
	while (sq < 64) {								/* mientras no hayamos recorrido las 64 casillas del tablero */
		enroque_mascara[sq] = 15;					/* inicialmente configuramos los enroques con todas las posibilidades */
		c = string[i++];							/* obtenemos un caracter */
		if ('a'<c && c<'z') {						/* comprobamos si el caracter es una letra minuscula */
			color[sq] = NEGRO;						/* si es minuscula la pieza sera de color negra */
			pieza[sq] = tpieza[c - 'a'];			/* con la ayuda de la tabla sabremos a que pieza corresponde el caracter */
			switch (pieza[sq]) {					/* actualizamos el numero de piezas */
				case PEON:
					peonesnegros++;
					break;
				case CABALLO:
					caballosnegros++;
					break;
				case ALFIL:
					alfilesnegros++;
					break;
				case TORRE:							/* en el caso de las torres guardamos su posicion por motivos de enroque */
					ntn++;
					torresnegras++;
					if (ntn == 1) tn2 = sq;
					else if (ntn == 2) tn1 = sq;
					break;
				case DAMA:
					damasnegras++;
					break;
				case REY:
					rn = sq;						/* guardamos la posicion del rey que es util para no tener que buscarlo en el tablero */
					break;
                default:
                    break;
			}
			sq++;									/* pasamos a la siguiente casilla */
		}
		else if ('A'<c && c<'Z') {					/* si la letra es mayuscula */
			color[sq] = BLANCO;						/* la pieza sera blanca */
			pieza[sq] = tpieza[c - 'A'];
			switch (pieza[sq]) {
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
					ntb++;
					torresblancas++;
					if (ntb == 1) tb2 = sq;
					else if (ntb == 2) tb1 = sq;
					break;
				case DAMA:
					damasblancas++;
					break;
				case REY:
					rb = sq;
					break;
                default:
                    break;
			}
			sq++;
		}
		else if ('1'<=c && c<='8') {				/* si el caracter es un numero nos indica el numero de casillas que estaran vacias */
			for (j=0; j<(c - '0'); j++) {
				color[sq+j] = VACIO;
				pieza[sq+j] = VACIO;
			}
			sq += j;
		}
	}

	/* Color para el turno */
	/* en el protocolo xboard la cadena recibida con el FEN no tiene espacios, sin embargo durante el protocolo uci si se envia la cadena
	con los espacios segun la norma por lo que en dicho protocolo una vez obtenido el valor de las 64 casillas nos posicionamos correctamente
	para obtener el caracter para el turno */
	if (protocolo == UCI) c = string[i++];
	c = string[i++];
	if (c == 'w')		{ turno = BLANCO; }			/* caracter recibido w es turno para las blancas */
	else if (c == 'b')	{ turno = NEGRO; }			/* caracter recibido b es turno para las negras */

	motor = VACIO;									/* el motor esta parado hasta que se lo indique la GUI */

    /* Flag del enroque */
	if (protocolo == UCI) c = string[i++];
	if (strcmp (variante,"fischerandom") == 0) {						/* variante Chess960 */
		enroque = 0;
		c = string[i++];
		while ((c >= 'A' && c <= 'H') || (c >= 'a' && c <= 'h')) {		/* si nos envian un enroque tipo Shredder */
			if (c >= 'A' && c <= 'H') {									/* letra mayuscula corresponde a enronque blanco */
				r = 65 + COLUMNA(rb);									/* calculamos posicion del rey, 65 en ASCII es una A */
				if (c > r) {											/* si el caracter es mayor que el del rey sera un enroque corto */
					enroque = enroque + 1;
					tb1 = c - 'A' + 56;
				}
				if (c < r) {											/* si el caracter es menor que el del rey sera un enroque largo */
					enroque = enroque + 2;
					tb2 = c - 'A' + 56;
				}
			}
			if (c >= 'a' && c <= 'h') {									/* letra minuscula corresponde a enronque negro */
				r = 97 + COLUMNA(rn);									/* calculamos posicion del rey, 97 en ASCII es una a */
				if (c > r) {
					enroque = enroque + 4;
					tn1 = c - 'a';
				}
				if (c < r) {
					enroque = enroque + 8;
					tn2 = c - 'a';
				}
			}
			c = string[i++];
			castling_chess960_uci[0] = '\0';
			strcpy(castling_chess960_uci,"AHah");						/* en uci necesitamos saber que es un tipo de enroque Shredder */
		}
		while (c == 'K' || c == 'Q' || c == 'k' || c == 'q') {			/* si nos envian un enroque tipo Arena */
			if (c == 'K') {
				enroque = enroque + 1;
			}
			if (c == 'Q') {
				enroque = enroque + 2;
			}
			if (c == 'k') {
				enroque = enroque + 4;
			}
			if (c == 'q') {
				enroque = enroque + 8;
			}
			c = string[i++];
			castling_chess960_uci[0] = '\0';
			strcpy(castling_chess960_uci,"OO");							/* en uci necesitamos saber que es un tipo de enroque Arema */
		}
	}
	else {																/* variante normal */
		enroque = 0;
		c = string[i++];
		while (c == 'K' || c == 'Q' || c == 'k' || c == 'q') {
			if (c == 'K') {
				enroque = enroque + 1;
				tb1 = 63;
			}
			if (c == 'Q') {
				enroque = enroque + 2;
				tb2 = 56;
			}
			if (c == 'k') {
				enroque = enroque + 4;
				tn1 = 7;
			}
			if (c == 'q') {
				enroque = enroque + 8;
				tn2 = 0;
			}
			c = string[i++];
		}
	}

	/* mascaras para los enroques, si movemos el rey o una torre perderemos opciones de enroque, para calcular las opciones de
	enroque haremos una operacion AND entre las posibilidades actuales y las posibilidades que quedaran al hacer el movimiento */
	enroque_mascara[rb] = 12;
	enroque_mascara[rn] = 3;
	enroque_mascara[tb2] = 13;
	enroque_mascara[tb1] = 14;
	enroque_mascara[tn2] = 7;
	enroque_mascara[tn1] = 11;

	/* Flag de al paso: si recibimos una casilla tenemos que transformar dicha casilla en el numero que identifica danasah para dicha
	casilla, en caso de no recibir casilla iniciamos la variable alpaso con -1 */
	if (protocolo == UCI) c = string[i++];
	c = string[i++];
	if (c>='a' && c<='h') {
		alpaso = (c - 'a');
		c = string[i++];
		if (c>='1' && c<='8') alpaso += 8*(7-(c - '1'));
		else alpaso = -1;
	}
	else alpaso = -1;


	/* El ultimo bloque que recibimos corresponde a la regla de los 50 movimientos y el numero de jugadas realizadas, danasah no comprueba
	dichos numeros y los inicia a 0, no afecta a los juegos y en general tampoco a cuando se analizan posiciones ya que cuando esto se hace
	no es normal tener en cuenta estas reglas, pero lo correcto seria iniciar estas variables con lo enviado en la posicion FEN */
	regla50 = 0;
	njugadas = 0;

	/* ya hemos terminado de procesar el FEN a continuacion iniciamos algunas variables extras */
	if (protocolo == XBOARD) fuera_del_libro = 0;	/* nos permite saber si hemos salido del libro de aperturas */
	hacer_ponder = FALSO;							/* no podemos ponderar si es nuestro turno */

	Calcula_material();								/*calculamos el material en centipeones */

	/* inicializamos la historia */
	memset(Historia, 0, sizeof(Historia));

	/* calculamos las distancias entre casillas */
	for (i=0;i<64;i++) {
		for (j=0;j<64;j++) {
			distancia[i][j] = MAX(abs(FILA(j) - FILA(i)), abs(COLUMNA(j) - COLUMNA(i)));
		}
	}

	if (protocolo == XBOARD) clear_hash();			/* borramos las tablas hash, esto lo podriamos hacer fuera de esta funcion */
	poner_hash();									/* calculamos la llave hash para la posicion */
}

/* convertimos una posición de tablero de danasah a formato FEN sin tener en cuenta enroque, al paso y la regla de 50 movimientos
y jugadas hechas (epd), esto es util para hacer intercambios entre diferentes programas, por ejemplo gracias a esto podemos consultar
el libro de aperturas de Rebel indicando una posicion EPD, desgraciadamente no podemos hacer lo mismo con las bitbases de Scorpio y es
que dicha proceso de conversion seria lento (dansah --> EPD --> Scorpio) */
void Tablero_FEN(char *string)
{
	int x, y, l=0, i=0, sq;
	char row[8];
	char strep[4];

	strcpy(string,"");

	for (y=0;y<8;y++) {
		i=l=0;
		strcpy(row,"");
		for (x=0;x<8;x++) {
			sq = (y*8)+x;
			if (color[sq] == VACIO) l++;
			else {
				if (l>0) {
					row[i] = (char) (l+48);						/* 48 es el codigo ascii del 0 */
					i++;
				}
				l=0;
				if (color[sq] == BLANCO) {
					switch (pieza[sq]) {
						case PEON	 : row[i]='P';break;
						case CABALLO : row[i]='N';break;
						case ALFIL	 : row[i]='B';break;
						case TORRE	 : row[i]='R';break;
						case DAMA	 : row[i]='Q';break;
						case REY	 : row[i]='K';break;
						default: break;
					}
				}
				else {
					switch (pieza[sq]) {
						case PEON	 : row[i]='p';break;
						case CABALLO : row[i]='n';break;
						case ALFIL	 : row[i]='b';break;
						case TORRE	 : row[i]='r';break;
						case DAMA	 : row[i]='q';break;
						case REY	 : row[i]='k';break;
						default: break;
					}
				}
				i++;
			}
		}
		if (l>0) {
			row[i] = (char) (l+48);
			i++;
		}
		strncat(string,row,i);
		if (y<7) strcat(string,"/");
	}

	if (turno == BLANCO) strcat(string," w ");
	else strcat(string," b ");

	if (enroque != 0) {
		if (enroque & 1) strcat(string,"K");
		if (enroque & 2) strcat(string,"Q");
		if (enroque & 4) strcat(string,"k");
		if (enroque & 8) strcat(string,"q");
	}
	else
		strcat(string,"-");
	strcat(string," ");

	if (alpaso != -1) {
		sprintf(strep, "%c%d", COLUMNA(alpaso) + 'a', 8 - FILA(alpaso));
		strcat(string, strep);
	}
	else
		strcat(string,"-");
}

