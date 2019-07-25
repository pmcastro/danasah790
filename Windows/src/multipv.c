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

/*Includes*/
#include <stdio.h>									/* prinft, scanf, sprinft, sscanf, fgets, fflush... */
#include <string.h>     							/* menset, strcmp, strcat... */
#include <stdlib.h>     							/* abs */
#include <math.h>									/* pow */
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"
#if defined (UNDER_CE) || defined (_WIN32)
	#include <windows.h>        					/* Sleep */
#else
	#include <unistd.h>
	#define Sleep(x) usleep((x)*1000)
#endif

/* Esta funcion nos va a devolver el mejor movimiento y el posible a ponderar, desde ella se llamara a la funcion alpha-beta o equivalente */
void multiMotorPiensa(movimiento *mejormovimiento, movimiento *pondermovimiento)
{
	int	i, j, m, v, x, y, z, alpha, beta, contar_legales;
	int tiempo_usado;
	int l;

	/* iniciamos el mejor movimiento y el de ponderar con ninguno */
	*mejormovimiento = no_move;
	*pondermovimiento = no_move;

	/* calculamos el tiempo inicial de partida gracias a la hora actual del sistema */
	tiempo_inicio = get_ms();
    para_busqueda = FALSO;              /* variable para saber cuando debemos abandonar la busqueda */
    actualizar_info = FALSO;			/* no actualizamos informacion para la GUI hasta despues de 1 segundo (uci) */

    nodos = 0;					        /* lleva la cuenta del numero de nodos */

	/* el motor tiene que comprobar si tiene que salir de la busqueda, una comprobacion por cada nodo es algo costoso, asi que trataremos
	de hacer esta comprobacion cada mas nodos si es posible, cknodos aqui esta puesto en hexadecimal */
#if defined (UNDER_ELO)
	/* en la version limitada de elo la comprobacion es mas amenudo para evitar perdidas por tiempo */
	if (material<FINAL)
		cknodos =  0x1;
	else
		cknodos =  0x10;
#else
	/* para el resto de versiones parece que estos numeros son ok para no perder en el tiempo, incluso para android */
	if (material<FINAL)
		cknodos =  0x400;
	else
		cknodos =  0x4000;
#endif

#if defined (UNDER_ELO)
	/* solo en la version limitada en fuerza si jugamos con un determinado elo o con nivel facil tenemos que limitar el numero de nodos
	por segundo que hace el motor (nps), la formula mas o menos tiene en cuenta que cada vez que duplicamos el numero de nodos vamos a
	ganar un ply de profundidad y un elo entorno a los 100 puntos */
	if (elo >= 2100) nps_elo = pow(2,((double)elo)/100.0) / 100;
	else if (elo >= 1400) nps_elo = (pow(2,((double)elo)/100.0) / 100) * ((2200 - elo) / 100);
	else nps_elo = 1000;
#endif

	/* Generamos la lista de movimientos legales */
	contar_legales = Generar_root_legales();
	/*printf("El número de movimientos legales es: %d\n", root_legales);*/

	/* El número de mejores líneas no puede ser mayor que el numero de movimientos legales */
	if (contar_legales < multipv)
        multipv = contar_legales;

    for (x = 1; x <= multipv; ++x) {
        MultiPVs[x].profundidad = 0;
        MultiPVs[x].valor = 0;
        MultiPVs[x].longitud = 0;
        memset(MultiPVs[x].pv, 0, sizeof(MultiPVs[x].pv));
    }

	root_jaque = EstaEnJaque(turno);

	/* vamos a llamar a la funcion RAIZ de una forma interactiva, primero buscamos completo la profundidad 1, luego la 2, etc, esto tiene
	2 ventajas, si el tiempo se termina al menos tenemos una busqueda completa en una profundidad y ademas cada nueva profundidad podemos
	resolverla mas rapido ya que se produce una mejor ordenación de la variante principal */

	for (i = 1; i <= max_profundidad;) {

		li = i;	                				/* guardamos profundidad por si tenemos que utilizarla fuera de esta funcion */

		if (i > 1) {
            if (i <= 7) Ordena_PVs(multipv);
            Generar_root_legales();
            Ordena_root_lista(root_contar);
            if (i <= 7) {
                for (y = 1; y <= multipv; ++y) {
                    multiprint_pvs(MultiPVs[y].profundidad, MultiPVs[y].valor, MultiPVs[y].longitud, y);
                }
            }
		}

        /* calculamos multiples pv */
        for (m = 1; m <= multipv;) {
            lm = m;

            /* reseteamos la lista de movimientos a excluir */
            if (m == 1) {
                for (j = 0; j < MAX_MULTIPV; ++j)
                    excluir[j] = no_move;
            }

            ply = 0;					                        			/* comenzamos en el ply 0 (otros empiezan en el 1) */

            material = piezasblanco+piezasnegro+peonesblanco+peonesnegro;	/* calcula el material todal de ambos lados */
            fail_low = 0; fail_high = 0; fail = NO_FAIL;					/* lleva cuantas veces nos hemos salido de la ventana en la busqueda */
            egtb_hits = 0;
            egbb_hits = 0;

            hacer_lazy_eval = 1;											/* realizamos lazy eval en la evaluacion */
            evalua_suma = 0;
            evalua_cuenta = 1;

            /* reseteamos la variante principal */
            memset(pv, 0, sizeof(pv));
            memset(Historia, maxhistoria, sizeof(Historia));
            memset(matekiller, 0, sizeof(matekiller));
            memset(killer1, 0, sizeof(killer1));
            memset(killer2, 0, sizeof(killer2));
            maxhistoria = 0;

            /* guardamos el material que hay en este momento para utilizarlo luego en la evaluacion */
            old_piezasblanco = piezasblanco;
            old_piezasnegro = piezasnegro;
            old_peonesblanco = peonesblanco;
            old_peonesnegro = peonesnegro;

            sigue_Pv = VERDADERO;					/* inicialmente hacemos que siga la variante principal */
            alpha = -MATE;
            beta = MATE;

            /* llamamos a la funcion RAIZ, la profundidad que vamos a utilizar sera fracional, PLY = 8,	las extensiones fraccionales deberian
            permitir un mejor control */
            valor[i] = multiRAIZ(alpha, beta, i * PLY);
            MultiPVs[m].profundidad = i;
            MultiPVs[m].valor = valor[i];
            v = pv_longuitud[ply];
            MultiPVs[m].longitud = v;
            for (z = 0; z <= v; ++z) {
                MultiPVs[m].pv[0][z] = pv[0][z];
            }

            /* Visualiza la variante principal al final de cada iteración */
            if (i > 7)  {
                Ordena_PVs(m);
                /*for (y = 1; y <= m; ++y) {
                    multiprint_pvs(MultiPVs[y].profundidad, MultiPVs[y].valor, MultiPVs[y].longitud, y);
                }*/
                for (y = /*m+*/1; y <= multipv; ++y) {
                    multiprint_pvs(MultiPVs[y].profundidad, MultiPVs[y].valor, MultiPVs[y].longitud, y);
                }
            }

            /* ya podemos incluir en la lista de excluir el movimiento realizado */
            excluir[m] = pv[0][0];

            /* sacamos el mejor movimiento y el movimiento a ponderar de la variante principal */
            if (m == 1) {
                l = pv_longuitud[0];
                if (l >= 1) {
                    *mejormovimiento = pv[0][0];
                    if (l >= 2) *pondermovimiento = pv[0][1];
                }
            }

            /* si la busqueda ha terminado al superar el tiempo, salimos de la funcion */
            if (para_busqueda == VERDADERO) {
                break;
            }

            /* enviamos informacion extra a la GUI en el protocolo uci si al menos ha pasado 1 segundo */
            tiempo_usado = get_ms() - tiempo_inicio;
            if (tiempo_usado > 1000) actualizar_info = VERDADERO;
            m++;
        }
        /* pasamos al siguiente nivel de profundidad */
        i++;
	}

	return;
}

/* la siguiente funcion, que solo se aplica en el root (raiz) o ply = 0, es la encargada de comenzar la busqueda, es un algoritmo alphabeta,
una variante que se llama variante principal, esta basada en un algoritmo de Bruce Moreland. La busqueda en el ply 0 se comporta un poco
diferente que el resto de profundidades por lo que es interesante tener una funcion en exclusiva dedicado a ello y ademas nos permitira
en un futuro tener una ordenacion tambien diferente */
int	multiRAIZ(int alpha, int beta, int depth)
{
	int i, j, nuMoves = 0, de, a;
	movimiento mejor_movimiento = no_move;
	movimiento lista[256];	/*218 es el máximo número de combinaciones posibles para un movimiento*/

	int ext;
	int puntos = alpha;
	int mejor = - MATE;

	/*TTables*/
	int hdepth;
	char htype;
	movimiento hmov;
	int hpuntos;
	int hpruning;
	char hash_flag = hasfALPHA;

	/* incrementamos el numero de nodos visitados */
	nodos++;

	/* comprobamos las tablas hash solo para ordenar */
	probe_hash(depth+njugadas, &hdepth, &htype, &hmov, &hpuntos, &hpruning);

	pv_longuitud[ply] = ply;

	/* Generamos la lista de movimientos legales */
	Generar_root_legales();
	//OrdenaRootMayorMenor();

	/* recupera todos los movimientos para el root */
	for (i = 0; i < root_contar; ++i)
		lista[i] = root_lista[i];

	/* ordenamos primero el movimiento que viene de la hash y luego el de la variante principal */
	Ordena_hmove_multipv(root_contar, lista, hmov);

    /* recorremos todo el ciclo de movimientos */
    for (i = 0; i < root_contar; ++i) {

        OrdenaMovimiento(i, root_contar, lista);			/* ordena primero el movimiento con mayor valor */

        int continuar = 0;
        /*excluimos movimientos de la lista para tener varias pv */
        for (j = 1; j < lm; ++j) {
            if (excluir[j].de == lista[i].de && excluir[j].a == lista[i].a && excluir[j].tipo == lista[i].tipo) {
                continuar = 1;
                break;
            }
        }
        if (continuar == 1) continue;

        if (!Hacer_Movimiento(lista[i]))
            continue;
        nuMoves++;	    									/* numero de movimientos legales recorridos */

        /* mostramos valoracion extra por cada movimiento en el protocolo uci */
        if (protocolo == UCI && actualizar_info) {
            printf("info depth %d", li);
            printf(" currmove");
            printf(" %c%d%c%d",'a' + COLUMNA(lista[i].de),	8 - FILA(lista[i].de), 'a' + COLUMNA(lista[i].a), 8 - FILA(lista[i].a));
            printf(" currmovenumber %d\n", nuMoves);
        }

        /* si el movimiento lo preveemos complicado extenderemos la profundidad con hasta un PLY */
        ext = 0;
        material = piezasblanco+piezasnegro+peonesblanco+peonesnegro;
        /* damos jaque */
        if (EstaEnJaque(turno) && see(turno, lista[i].a, lista[i].de, 1) >= 0) ext += PLY;
        /* peon en septima */
        else if (material < MEDIOJUEGO && pieza[jugadas[njugadas-1].m.a] == PEON
            && (FILA(jugadas[njugadas-1].m.a) == 1 || FILA(jugadas[njugadas-1].m.a) == 6))
            ext += PLY;
        /*recaptura*/
        else if (jugadas[njugadas-1].captura != VACIO && jugadas[njugadas-2].captura != VACIO
            && jugadas[njugadas-1].m.a == jugadas[njugadas-2].m.a
            && abs(valor_pieza[jugadas[njugadas-1].captura] - valor_pieza[jugadas[njugadas-2].captura] <= VALOR_PEON/2))
            ext += PLY;

        /* para el primer movimiento hacemos una busqueda completa */
        if (nuMoves == 1) {
            puntos = -PVS(-beta, -alpha, depth+ext-PLY, VERDADERO);
        }
        else {
            /*probamos a hacer LMR si no hay extensiones, si hay un numero minimo de movimientos legales, si tenemos una determinada
            profundidad, si no es el movimiento de la tabla	hash, variante principal, matekiller, capturas de ganancia e iguales,
            promociones y killers y que su historia no sea elevada, si todo esto se cumple probamos a reducir la profundidad */
            if (nuMoves >= 3 && depth >= 2*PLY && !ext && !root_jaque && !sigue_Pv && alpha > (-MATE/2) && lista[i].valor < 1500000) {
                puntos = -PVS( -alpha-1, -alpha, depth-2*PLY, VERDADERO);
            }
            else
                puntos = alpha+1;								/* nos aseguramos que una busqueda completa es hecha */
            if (puntos > alpha) {
                puntos = -PVS(-alpha-1, -alpha, depth+ext-PLY, VERDADERO);
                if (para_busqueda == FALSO && puntos > alpha && puntos < beta)
                    puntos = -PVS(-beta, -alpha, depth+ext-PLY, VERDADERO);
            }
        }

        /* Deshacemos el movimiento */
        Deshacer();

        /* Salimos de la busqueda si se ha terminado el tiempo */
        if (para_busqueda == VERDADERO)
            return 0;

        if (puntos > mejor) {
            mejor = puntos;
            mejor_movimiento = lista[i];

            if (puntos > alpha) {
                alpha = puntos;
                hash_flag = hasfEXACT;
                Actualiza_pv(mejor_movimiento);	    			/* actualizamos la variante principal */
                /* actualizamos la historia si el movimiento mejora lo anterior */
                if (!root_jaque && !ext && pieza[mejor_movimiento.a] == VACIO && mejor_movimiento.tipo < PROMOCION_A_DAMA) {
                    Historia[mejor_movimiento.de][mejor_movimiento.a] += (depth/PLY) * (depth/PLY);
                    if (Historia[mejor_movimiento.de][mejor_movimiento.a] > maxhistoria)
                        maxhistoria = Historia[mejor_movimiento.de][mejor_movimiento.a];
                    if (maxhistoria > 2000000) {
                        maxhistoria /=2;
                        /*memset(Historia, maxhistoria, sizeof(Historia));*/
                        for (de = 0; de < 64; de++)
                            for (a = 0; a < 64; a++)
                                Historia[de][a] /= 2;
                    }
                }
                if (puntos >= beta) {
                    /* Los movimientos matekiller y killer son movimientos buenos, aprovechamos para guardar informacion de ellos */
                    if (puntos > (MATE-64))
                        matekiller[ply] = mejor_movimiento;		/*un movimiento matekiller, es un movimiento killer (asesino) que es un mate*/
                    else if (!root_jaque && pieza[mejor_movimiento.a] == VACIO && mejor_movimiento.tipo < PROMOCION_A_DAMA) {
                        /* guardamos dos movimientos killer que no sean movimientos de captura o promocion */
                        if ((mejor_movimiento.a != killer1[ply].a) || (mejor_movimiento.de != killer1[ply].de)) {
                            killer2[ply] = killer1[ply];
                            killer1[ply] = mejor_movimiento;
                        }
                    }
                    /* Almacenamos en las tablas hash un tipo lower */
                    store_hash(depth+njugadas, hasfBETA, mejor_movimiento, mejor);
                    return mejor;
                }
            }
                /* si el movimiento no mejora alpha lo tenemos en cuenta para la historia */
            else {
                if ((pieza[lista[i].a] == VACIO) && lista[i].tipo < PROMOCION_A_DAMA) {
                    Historia[lista[i].de][lista[i].a] -= (depth/PLY);
                }
            }
        }
	}

	/* si no hay movimiento legales */
	if (nuMoves == 0) {
		if (root_jaque) mejor = -MATE + ply; 			/* mate */
		else mejor = 0; 								/* ahogado */
	}

	/* Almacenamos en las tablas hash un tipo exact o upper segun corresponda */
	store_hash(depth+njugadas, hash_flag, mejor_movimiento, mejor);
	return mejor;
}

void multiprint_pvs(int depth, int score, int longuitud, int m) {
	movimiento	mi;
	int i, j, tiempousado;
	char c;

	i = longuitud;								/* calcula la longuitud de la pv */
	if (i == 0) return;							/* si la longuitud es cero no tenemos nada que enviar */

	/* si usamos el protocolo uci */
	if (protocolo == UCI) {
		/* calculamos el tiempo usado en milisegundos que es lo que reconoce la GUI */
		tiempousado = (get_ms()- tiempo_inicio);

		/* enviamos profundidad, puntos, tiempo, nodos */
		printf("info depth %d multipv %d time %d nodes %d ", depth, m, tiempousado, nodos);
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

		printf("pv");

		/* imprimimos la mejor linea */
		for (j = 0; j < i; ++j) {
			mi = MultiPVs[m].pv[0][j];
			if (MultiPVs[m].pv[0][j].a == 0 && MultiPVs[m].pv[0][j].de == 0)
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

void Ordena_PVs(int m1)
{
  int c, d, z;
  MultiPV t;

  for (c = 2; c < m1+1; c++)
  {
    for (d = 1; d < m1; d++)
    {
      if (MultiPVs[d].valor < MultiPVs[d+1].valor)
      {
        t.profundidad = MultiPVs[d].profundidad;
        t.valor = MultiPVs[d].valor;
        t.longitud = MultiPVs[d].longitud;
        for (z = 0; z <= t.longitud; ++z) {
            t.pv[0][z] = MultiPVs[d].pv[0][z];
        }
        MultiPVs[d].profundidad = MultiPVs[d+1].profundidad;
        MultiPVs[d].valor = MultiPVs[d+1].valor;
        MultiPVs[d].longitud = MultiPVs[d+1].longitud;
        for (z = 0; z <= MultiPVs[d].longitud; ++z) {
            MultiPVs[d].pv[0][z] = MultiPVs[d+1].pv[0][z];
        }
        MultiPVs[d+1].profundidad = t.profundidad;
        MultiPVs[d+1].valor = t.valor;
        MultiPVs[d+1].longitud = t.longitud;
        for (z = 0; z <= MultiPVs[d+1].longitud; ++z) {
            MultiPVs[d+1].pv[0][z] = t.pv[0][z];
        }
      }
    }
  }
}

void Ordena_hmove_multipv(int cuenta, movimiento *lista, movimiento hmove)
{
	int i;

	for(i = 0; i < cuenta; ++i) {
		if (hmove.de == lista[i].de && hmove.a == lista[i].a) {
			lista[i].valor = 100000000;
            break;
		}
	}
}

void Ordena_root_lista(int cuenta)
{
	int i, y;

	for(i = 0; i < cuenta; ++i) {
        for (y = 1; y <= multipv; ++y) {
            if (root_lista[i].de == MultiPVs[y].pv[0][0].de && root_lista[i].a == MultiPVs[y].pv[0][0].a) {
                root_lista[i].valor = (50000000 - (1000000*y));
            }
         }
	}
}

