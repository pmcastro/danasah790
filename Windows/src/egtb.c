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
Funciones para acceder a las tablebases de Gaviota. Gracias a Miguel Ballicora. Para mas información sobre estas funciones deberíamos
recurrir a la pagina de Gaviota.
*/

/*Includes*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"
#include "gtb/gtb-probe.h"


const char ** paths;										/* paths where files will be searched */

/* esta funcion inicia el proceso de carga de las tablebases y se utiliza en las funciones xboard() y uci() */
void load_tablebases()
{
	/*--------------------------------------*\
	|   Initialization info to be provided
	\*--------------------------------------*/

	int verbosity = 1;										/* initialization 0 = non-verbose, 1 = verbose */
	int	scheme = tb_CP4;									/* compression scheme to be used */
	egtb_cache_size = (egtb_cache_size * 1024 *1024);		/* 32 MiB in this example */
	/* 	wdl_fraction:
		fraction, over 128, that will be dedicated to wdl information.
		In other words, 96 means 3/4 of the cache will be dedicated to
		win-draw-loss info, and 1/4 dedicated to distance to mate
		information.
	*/
	int wdl_fraction = 96;

	char *initinfo;											/* NULL if verbosity=0, initialization info if verbosity=1*/

	/*--------------------------------------*\
	|   Initialization:
	|   Include something like this at
	|   the beginning of the program.
	\*--------------------------------------*/

	/* the number of paths that can be added is only limited by memory */
	paths = tbpaths_init();
	if (NULL == paths) printf ("Error here... %d\n",__LINE__);
	paths = tbpaths_add (paths, egtb_path);
	if (NULL == paths) printf ("Error here... %d\n",__LINE__);

	/* init probing code, indexes, paths, etc. */
	initinfo = tb_init (verbosity, scheme, paths);

	/* init cache */
	tbcache_init(egtb_cache_size, wdl_fraction);

	tbstats_reset();

	/* information to be output for the user, or to be saved in logs etc.*/
	if (initinfo != NULL)
		printf ("%s",initinfo);

/*
	|	Just to show, It is possible to know what TBs are installed.
	|	But, I can only find out after tb_init or tb_restart
	\*----------------------------------------------------------------------------------------*/

	egtb_men = 2;

	{
		unsigned av = tb_availability();

		if (0 != (av& 1)) printf ("Some 3-pc TBs available\n"); else printf ("No 3-pc TBs available\n");
		if (0 != (av& 2)) {
			printf ("3-pc TBs complete\n");
			egtb_men = 3;
		}
		if (0 != (av& 4)) printf ("Some 4-pc TBs available\n"); else printf ("No 4-pc TBs available\n");
		if (0 != (av& 8)) {
			printf ("4-pc TBs complete\n");
			egtb_men = 4;
		}
		if (0 != (av&16)) printf ("Some 5-pc TBs available\n"); else printf ("No 5-pc TBs available\n");
		if (0 != (av&32)) {
			printf ("5-pc TBs complete\n");
			egtb_men = 5;
		}
		/*printf("egtb_men: %d", egtb_men);
		printf ("\n");*/
	}
	if (egtb_men > 2) egtb_is_loaded = VERDADERO;
}

void reset_tablebases()
{
	int verbosity = 1;										/* initialization 0 = non-verbose, 1 = verbose */
	int	scheme = tb_CP4;									/* compression scheme to be used */
	egtb_cache_size = (egtb_cache_size * 1024 *1024);
	int wdl_fraction = 96;
	char *initinfo;											/* NULL if verbosity=0, initialization info if verbosity=1*/

	/* cleanup old paths */
	paths = tbpaths_done(paths);

	/* init new paths */
	paths = tbpaths_init();
	paths = tbpaths_add (paths, egtb_path);

	/* restart */
	initinfo = tb_restart (verbosity, scheme, paths);
	tbcache_restart(egtb_cache_size, wdl_fraction);

	/* information to be output for the user, or to be saved in logs etc.*/
	if (initinfo != NULL)
		printf ("%s",initinfo);

	return;
}

void unload_tablebases()
{
	/*--------------------------------------*\
	|
	|	Clean up at the end of the program
	|
	\*--------------------------------------*/

	tbcache_done();
	tb_done();
	paths = tbpaths_done(paths);
	egtb_is_loaded = FALSO;
	return;
}

int probe_tablebases(int *score, int type)
{
	/*--------------------------------------*\
	|   Probing info to be provided
	\*--------------------------------------*/

	unsigned int  stm;				/* side to move */
	unsigned int  epsquare;			/* target square for an en passant capture */
	unsigned int  castling = 0;		/* castling availability, 0 => no castles */
	unsigned int  ws[17];			/* list of squares for white */
	unsigned int  bs[17];			/* list of squares for black */
	unsigned char wp[17];			/* what white pieces are on those squares */
	unsigned char bp[17];			/* what black pieces are on those squares */
	int i;
	int countw = 0;
	int countb = 0;

	/*--------------------------------------*\
	|   Probing info to be requested
	\*--------------------------------------*/

	/*char *initinfo;*/					/* NULL if verbosity=0, initialization info if verbosity=1*/
	int tb_available = 0;				/* 0 => FALSE, 1 => TRUE */
	unsigned info = tb_UNKNOWN;		/* default, no tbvalue */
	unsigned pliestomate;

/* danasah tambien usa un sistema de representacion diferente a Gaviota, en lugar de usar A8 = 0 y H1 = 63, Gaviota usa A1 = 0 y H8 = 63
con lo cual vamos a definir una funcion que nos permite pasar de un sistema a otro */
#define FLIP(sq) ((sq) ^ 0x38)

	stm = 1-turno;									/* 0 = white to move, 1 = black to move */
	if (alpaso == -1) epsquare = tb_NOSQUARE;		/* no ep available */
	else epsquare = FLIP(alpaso);
	if (enroque == 0) castling = tb_NOCASTLE;		/* no castling available */
	else {
		if (enroque & 1) castling |= tb_WOO;
		if (enroque & 2) castling |= tb_WOOO;
		if (enroque & 4) castling |= tb_BOO;
		if (enroque & 8) castling |= tb_BOOO;
	}

#define ADD_PIECE_WHITE(type) {\
	    wp[countw] = type;\
	    ws[countw] = FLIP(i);\
        countw++;\
};

#define ADD_PIECE_BLACK(type) {\
	    bp[countb] = type;\
	    bs[countb] = FLIP(i);\
        countb++;\
};

	for (i = 0; i < 64; ++i) {
		if (color[i] == VACIO)
			continue;
		if(color[i] == BLANCO) {
			switch (pieza[i]) {
				case PEON: ADD_PIECE_WHITE(tb_PAWN); break;
				case CABALLO: ADD_PIECE_WHITE(tb_KNIGHT); break;
				case ALFIL: ADD_PIECE_WHITE(tb_BISHOP); break;
				case TORRE: ADD_PIECE_WHITE(tb_ROOK); break;
				case DAMA: ADD_PIECE_WHITE(tb_QUEEN); break;
				case REY: ADD_PIECE_WHITE(tb_KING); break;
				default: break;
			}
		} else {
			switch(pieza[i]) {
				case PEON: ADD_PIECE_BLACK(tb_PAWN); break;
				case CABALLO: ADD_PIECE_BLACK(tb_KNIGHT); break;
				case ALFIL: ADD_PIECE_BLACK(tb_BISHOP); break;
				case TORRE: ADD_PIECE_BLACK(tb_ROOK); break;
				case DAMA: ADD_PIECE_BLACK(tb_QUEEN); break;
				case REY: ADD_PIECE_BLACK(tb_KING); break;
				default: break;
			}
		}
	}

	wp[countw] = tb_NOPIECE;
	ws[countw] = tb_NOSQUARE;
	bp[countb] = tb_NOPIECE;
	bs[countb] = tb_NOSQUARE;

	if (type == DTM_hard) tb_available = tb_probe_hard (stm, epsquare, castling, ws, bs, wp, bp, &info, &pliestomate);
	else if (type == DTM_soft) tb_available = tb_probe_soft (stm, epsquare, castling, ws, bs, wp, bp, &info, &pliestomate);
	else if (type == WDL_hard) tb_available = tb_probe_WDL_hard (stm, epsquare, castling, ws, bs, wp, bp, &info);
	else if (type == WDL_soft) tb_available = tb_probe_WDL_soft (stm, epsquare, castling, ws, bs, wp, bp, &info);

	if (tb_available) {
		egtb_hits++;
		if (type == WDL_hard || type == WDL_soft)
			pliestomate = MATE - GANADA;
		if (info == tb_DRAW)
			*score = 0;
		if (info == tb_WMATE && stm == tb_WHITE_TO_MOVE)
			*score = (MATE-pliestomate);
		else if (info == tb_WMATE && stm == tb_BLACK_TO_MOVE)
			*score = -(MATE-pliestomate);
		else if (info == tb_BMATE && stm == tb_BLACK_TO_MOVE)
			*score = (MATE-pliestomate);
		else if (info == tb_BMATE && stm == tb_WHITE_TO_MOVE)
			*score = -(MATE-pliestomate);
	}

    return tb_available;
}


