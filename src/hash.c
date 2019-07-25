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
Funciones para manejar las tablas hash y evaluación cache. Quiero dar las gracias a Miguel Izquierdo (Popochin) por su ayuda.

Las tablas hash actuan durante la busqueda, en el momento de la busqueda cuando una posicion se visita, guardamos informacion de ella,
no sabemos el valor exacto en ese momento de la evaluacion, pero si que tenemos algo de informacion, vamos a poder cortar con las tablas
hash muchas ramas de los arboles ahorrando muchos nodos, vamos a conseguir con ellas de 1 a 2 plys mas en el medio juego y mas de 4 en
el final.
En el caso de la evaluacion cache, actuan en el momento que estamos evaluando una posicion, si hemos hecho una evaluacion completa guardamos
la llave de la posición y la evaluación, si volvemos a visitar esa posicion no tendremos que evaluarla sino retorna ya su valor, la evaluacion cache
no nos ahorra nodos pero si un poco de trabajo, en mi programa da aproximadamente un 5% de velocidad.
*/

/*Includes*/
#include <stdio.h>					/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <string.h>
#include <stdlib.h>					/*malloc, free ...*/
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

#define CLUSTER_SIZE 4

/* Aqui defino unas variables para saber el numero de veces que entramos en las tablas hash para almacenar informacio, el numero de veces
que luego probamos y el numero de veces que tenemos exito, el programa luego de momento no imprime estos resultados */
long TTProbes;
long TTHits;
long TTStores;

/* lo mismo para la evaluacion cache */
long ETProbes;
long ETHits;
long ETStores;

TType *TTable;
EType *ETable;

/* iniciamos las tablas con numeros aleatorios */
void inicia_hash(void)
{
	int i, j, k;

	for (i = 0; i < 2; ++i)
		for (j = 0; j < 6; ++j)
			for (k = 0; k < 64; ++k)
				hash_piezas[i][j][k] = Rand64();
	hash_turno[BLANCO] = Rand64();
	hash_turno[NEGRO] = Rand64();
	for (i = 0; i < 64; ++i)
		hash_alpaso[i] = Rand64();
	for (i = 0; i < 16; ++i)
		hash_enroque[i] = Rand64();
}

/* poner_hash() usa llaves de Zobrist para generar numeros unicos, para poder indentificar una posicion unica necesitamos
controlar ademas de la situacion de piezas en el tablero quien juega y las caracteristicas de enroque y al paso */
void poner_hash(void)
{
	int i;

	hash = 0;
	for (i = 0; i < 64; ++i)
		if (color[i] != VACIO)
			hash ^= hash_piezas[color[i]][pieza[i]][i];
	hash ^= hash_turno[turno];
	if (alpaso != -1)
		hash ^= hash_alpaso[alpaso];
	hash ^= hash_enroque[enroque];
}

/* borramos las tablas hash y evaluacion cache */
void clear_hash(void) {
	memset(TTable, 0, sizeof(TType) * TTSize);
	memset(ETable, 0, sizeof(EType) * ETSize);
}

void store_hash(int depth, char type, movimiento mov, int score) {
	U64 index = hash & (TTSize-1);
	TType *TThash;
    TType *TThash0 = &TTable[index ^ 0];
    TType *TThash1 = &TTable[index ^ 1];
    TType *TThash2 = &TTable[index ^ 2];
    TType *TThash3 = &TTable[index ^ 3];

    /* para un mate sumamos un ply (distancia al mate) y esto salvará algunos nodos en muchas posiciones */
    if (abs (score) > MATE-200) {
        if (score > 0)
            score += ply;
        else
            score -= ply;
    }

 	/* 1er bucket: guardamos si está vacia o si tiene la misma hash y mayor profundidad */
	if ((TThash0->Hash != hash) || (TThash0->Hash == hash && depth >= TThash0->Depth))
        TThash = TThash0;
    /* 2º bucket: guardamos si está vacia o si tiene la misma hash y profundidad suficiente */
	else if ((TThash1->Hash != hash) || (TThash1->Hash == hash && depth >= TThash1->Depth - 8))
	    TThash = TThash1;
	/* 3er y 4º bucket: guardamos en el slot que tiene menor profundidad */
	else {
        if ((TThash2->Hash != hash) || (TThash2->Depth <= TThash3->Depth))
            TThash = TThash2;
        else
            TThash = TThash3;
	}

    TThash->Hash = hash;
    TThash->Value = (short)score;
    TThash->Depth = (short)depth;
    TThash->Type = type;
    TThash->mov_de = (char)mov.de;
    TThash->mov_a = (char)mov.a;
    TThash->mov_tipo = (char)mov.tipo;

	return;
}

/* probamos si la posicion la tenemos almacenada en las tablas hash */
void probe_hash(int depth, int *hdepth, char *htype, movimiento *hmov, int *hscore, int *hpruning) {
	U64 index = hash & (TTSize-1);
	TType *TThash;
	int i;
	movimiento mov;

    /* incialmente la funcion devuelve ... */
    *hdepth = 0;
    *htype = hasfNULL;
    *hmov = no_move;
    *hscore = valUNKNOWN;
    *hpruning = 0;

	for (i=0; i<CLUSTER_SIZE; i++) {
        TThash = &TTable[index ^ (CLUSTER_SIZE - 1 - i)];
        /* si la posicion fue encontrada en las tablas */
        if (TThash->Hash == hash) {
            if ((int)TThash->Depth > *hdepth) {
                *hscore = (int)TThash->Value;
                /* eliminamos el ply introducido al guardar en caso de mate */
                if (abs (*hscore) > MATE-200) {
                    if (*hscore > 0)
                        *hscore -= ply;
                    else
                        *hscore += ply;
                }
                *hdepth = (int)TThash->Depth;
                *htype = TThash->Type;
                mov.de = (int)TThash->mov_de;
                mov.a = (int)TThash->mov_a;
                mov.tipo = (int)TThash->mov_tipo;
                mov.valor = 0;
                *hmov = mov;
                /* solo podremos cortar si la profundidad de las tablas es al menos la actual */
                if (*hdepth >= depth) {
                    *hpruning = 1;
                    return;
                }
            }
		}
	}
	return;
}

/* almacenamos informacion sobre la evaluacion de una posicion */
void store_eval(int score)
{
	U64 index;

	ETStores++;

	index = hash % ETSize;

	ETable[index].Hash = hash;
	ETable[index].Bound = score;

	return;
}

/* comprobamos si una posición ya la hemos evaluado antes */
int probe_eval(void)
{
	U64 index;

	ETProbes++;

	index = hash % ETSize;

	if (ETable[index].Hash == hash) {
		ETHits++;
		return (ETable[index].Bound);
	}
	return -MATE;
}

/* asignamos memoria para las tablas hash y evaluacion cache */
void alloc_hash(void) {
	TTable = (TType *) malloc(sizeof(TType) * TTSize);
	ETable = (EType *) malloc(sizeof(EType) * ETSize);

	if (TTable == NULL || ETable == NULL) {
		printf("# Out of memory allocating hash or evalcache.\n");
		exit(1);
	}

	return;
}

/* liberamos memoria de las tablas hash y de la evaluacion cache (cuando salimos del programa) */
void free_hash(void) {
	free(TTable);
	free(ETable);
	return;
}
