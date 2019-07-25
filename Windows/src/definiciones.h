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

/* Definiciones y tipos que vamos a utilizar a lo largo de todo el programa */

/* definimos las 6 piezas, quizás hubiera sido mejor definir piezas blancas y negras por
separado ya que luego sería menos costoso hacer comparaciones, una en lugar de 2 */
#define	PEON    0
#define	CABALLO 1
#define	ALFIL	2
#define	TORRE	3
#define	DAMA	4
#define	REY		5
#define	VACIO   6								/* VACIO es color de pieza y tipo de pieza */

/* colores de las piezas y turnos */
#define	BLANCO  1
#define	NEGRO   0

/* muy util para guardar informacion cuando hay 2 posibilidades */
#define VERDADERO	1
#define FALSO		0

/* valores de las piezas en centipeones */
#define	VALOR_PEON		100
#define	VALOR_CABALLO   325
#define	VALOR_ALFIL		325
#define	VALOR_TORRE     500
#define	VALOR_DAMA		975
#define	VALOR_REY		9999					/* este valor no llego a utilizarlo */
#define	VALOR_VACIO		0						/* logicamente si la casilla esta vacia no cuenta en el material */

#define	MATE            9999					/* un mate vale un rey */

#define GANADA			7000

#define MAX_PROFUNDIDAD	100						/* numero de plys maximos que pensara el motor */
#define QS_LIMITE		-7						/* limitada profundidad en busca del horizonte para que no haya una explosion en el arbol de variantes */
#define STACK			4096					/* máximo numero de movimientos que se generan en el arbol y que guardamos */
#define PLY				2						/* utilizado para extensiones fraccionales */

#define MAX_MULTIPV     64

#define COLUMNA(x)	(x & 7)						/* permite calcular el numero de columna a partir del numero de casilla */
#define FILA(y)		(y >> 3)					/* permite calcular el numero de fila a partir del numero de casilla */

/* tipos de movimientos, es una informacion que forma parte del movimiento */
#define NINGUNO					0
#define NORMAL					1
#define ENROQUE					2
#define AL_PASO					3
#define PEON_AVANZA_2			4
#define PROMOCION_A_DAMA		5
#define PROMOCION_A_TORRE		6
#define PROMOCION_A_ALFIL		7
#define PROMOCION_A_CABALLO		8

/* TTables */
#define hasfNULL	0
#define hasfEXACT	1
#define hasfALPHA	2
#define hasfBETA	3
#define valUNKNOWN	32767

/* Fail aspiration */
#define NO_FAIL     0
#define LOW         1
#define HIGH        2

/* diferentes formas de cargar las bitbases de scorpio en memoria */
#define LOAD_NONE	0
#define LOAD_4MEN	1
#define SMART_LOAD	2
#define LOAD_5MEN	3

/* diferentes formas de cargar las tablebases de Gaviota */
#define DTM_hard	0
#define DTM_soft	1
#define WDL_hard	2
#define WDL_soft	3

/* actuaremos de diferente forma durante la busqueda si es nuestro turno, el del contrario o solo queremos analizar la posicion */
#define BUSCANDO	1
#define PONDERANDO	2
#define ANALIZANDO	3

/* Protocolo */
#define XBOARD		1
#define UCI			2

/* fases del juego según la cantidad de material */
/*#define APERTURA	((2*VALOR_DAMA)+(4*VALOR_TORRE)+(4*VALOR_ALFIL)+(4*VALOR_CABALLO)+(16*VALOR_PEON))*/
#define MEDIOJUEGO	((VALOR_DAMA)+(2*VALOR_TORRE)+(2*VALOR_ALFIL)+(2*VALOR_CABALLO)+(8*VALOR_PEON))
#define FINAL		((4*VALOR_TORRE)+(VALOR_ALFIL)+(VALOR_CABALLO)+(4*VALOR_PEON))

/* definimos la posicion inicial del ajedrez mediante una posicion FEN, aqui deberia integrar las 2 opciones en 1 y dejar solo INIPOS que
es el formato normal, todavia tengo la otra porque inicialmente la funcion que configura el tablero no comprobaba espacios vacios entre las
diferentes partes del FEN */
#define STARTPOS		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNRwKQkq-"
#define INIPOS			"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -"

/* definimos las funciones maximo y minimo */
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

/* definimos los tipos para numeros enteros de 8, 16, 32 y 64 bits segun el tipo de compilador */
typedef unsigned char U8;
typedef unsigned short U16;
#if defined (_MSC_VER)
    typedef unsigned __int64 U64;
    typedef unsigned __int32 U32;
#elif defined (__GNUC__)
    #include <stdint.h>
    #include <inttypes.h>
    typedef uint64_t U64;
    typedef uint32_t U32;
#else
    typedef unsigned long long U64;
    typedef unsigned long U32;
#endif

#ifdef _MSC_VER
#  define U64(u) (u##ui64)
#else
#  define U64(u) (u##ULL)
#endif

/* un movimiento tiene asociado una casilla de inicio, una final, el tipo de movimiento que es (normal, al paso,
enroque, promoción, etc) y un valor para la ordenación de mejor a peor en la lista */
/* esta estructura con 4 enteros ocupa 16 bytes, sería recomendable usar un byte (char) por cada uno y utilizar
una union para guardar el valor, de esta forma solo nos ocuparia 3 bytes, cosa importante para las hash */
typedef struct estructura_movimiento {
    int valor;
    int  de,
		 a,
         tipo;
}	movimiento;

/* una jugada tiene las siguientes características; movimiento realizado, pieza capturada por si tenemos que
deshacer el movimiento, enroque, al paso, regla de los 50 movimientos y asociamos una llave unica a la posición
que nos permitirá reconer fácilmente repeticiones */
typedef struct estrutura_jugada {
    movimiento	m;
    int	captura,
		enroque,
		alpaso,
		regla50;
	U64 hash;
}	jugada;

/* estructura para una tabla hash optimizada para ocupar 16 bytes (8+2+2+1+1+1+1) */
typedef struct {
	U64 Hash;
	short Value;
	short Depth;
	char Type;
	char mov_de;
	char mov_a;
	char mov_tipo;
} TType;

/* algo parecido a lo anterior hacemos durante la evaluacion, guardaremos informacion de la evaluacion para cada posicion, eval_cache
y si volvemos a examinar dicha posicion no necesitaremos volver a realizar la evaluacion ya que dispondremos de la informacion previa
guardada que consiste de una llave unica para la posicion y el valor de la evaluacion */
typedef struct {
	U64 Hash;
	int Bound;
} EType;

/* este es un tipo utilizado para el static exchange evaluator (see) */
typedef struct {
	int deslizante;
	int direccion;
	int casilla;
} tipo_see;

/* book */
typedef struct {
    U64 key;
    U16 move;
    U16 weight;
    U32 learn;
} entry_t;

typedef struct {
    int profundidad;
    int valor;
    int longitud;
    movimiento pv[MAX_PROFUNDIDAD+1][MAX_PROFUNDIDAD+1];
} MultiPV;

/* utilizado para la versión pocketpc */
#if defined(_UNICODE) || defined(UNICODE)
    #define STRING(s_) L##s_
#else
    #define STRING(s_) s_
#endif
