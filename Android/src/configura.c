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
Aquí tenemos una función que lee el archivo de configuración danasah.ini donde tenemos configurados
los libros de aperturas, el tamaño de las tablas hash y evaluación caché y la direccióny tamaño del
caché para las bitbases */

/*Includes*/
#include <stdio.h>		/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <string.h>
#include <stdlib.h>
#if defined (UNDER_CE) || defined (_WIN32)
    #include <windows.h>
    #include <io.h>
	#define CFG_NAME STRING("danacfg.txt")
#else
	#include <unistd.h>		/*getcwd*/
	#define CFG_NAME "danacfg.txt"
#endif
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

void leer_inifile(void)
{
/* sobre el PocketPc no usamos archivo de configuracion, establecemos por defecto un tamaño de 8 Mb para la hash */
#if defined (UNDER_CE)
	TTSize = (8 * 1024 * 1024) / sizeof(TType);
	ETSize = (1 * 1024 * 1024) / sizeof(EType);
	return;
/* para el resto de opciones esperamos comprobamos si existe el archivo danacfg.txt */
#else
	FILE *inifile;
	char linea[256], comandos[256];
	char temp1[32], temp2[64] = "", temp3[64] = "", temp4[64];
	int tamano1 = 0, tamano2 = 0, tamano3 = 0, tamano4 = 0, tamano5 = 0, tamano6 = 0, tamano13, tamano14, tamano15;
	char path[128];
#if defined (UNDER_ELO)
	char temp5[32] = "", temp6[32] = "";
	int tamano7 = 0, tamano8 = 0, tamano9 = 0, tamano10 = 0, tamano11 = 0, tamano12 = 0;
#endif

	/* buscamos el archivo de configuracion en la misma carpeta o directorio que el ejecutable */
	getcwd(path, 96);
	strcat(path, "/");
	strcat(path, CFG_NAME);

	/* si el archivo no es encontrado por si acaso configuramos las tablas hash con un tamaño no demasiado grande como 64 Mb */
	if ((inifile = fopen (path, "r")) == NULL) {
		printf("# Cfg file danacfg.txt no found.\n");
		tamano1= 32;
		tamano2= 4;
		TTSize = (tamano1 * 1024 * 1024) / sizeof(TType);
		ETSize = (tamano2 * 1024 * 1024) / sizeof(EType);
		/* mostrara en pantalla informacion de los parametros */
		imprime_configuracion(tamano1, tamano2);
		return;
	}
	/* si el archivo es encontrado entonces pasamos a configurar cada uno de los parametros encontrados, se puede ver una explicacion
	de cada parametro en la funcion ParseSetOptionCommand() del archivo uci.c */
	else {
		printf("# Cfg file danacfg.txt found.\n");

		fseek(inifile, 0, SEEK_SET);
		while (fgets(linea, 256, inifile)) {
			if (linea[0] != '#') {
				sscanf(linea, "%s", comandos);
				/* Transposition table */
				if (strcmp(comandos, "hash") == 0) {
					sscanf(linea, "hash %d", &tamano1);
					thash = tamano1;
					TTSize = (tamano1 * 1024 * 1024) / sizeof(TType);
					tamano2 = tamano1 / 8;
					ETSize = (tamano2 * 1024 * 1024) / sizeof(EType);
				}
				/* Libro de aperturas */
				if (strcmp(comandos, "ownbook") == 0) {
					sscanf(linea, "ownbook %s", temp1);
					strcpy(ownbook, temp1);
				}
				if (strcmp(comandos, "book") == 0) {
					sscanf(linea, "book %s", temp2);
					strcpy(libro, temp2);
				}
				/* Bitbases */
				if (strcmp(comandos, "egbb_path") == 0) {
					sscanf(linea, "egbb_path %s", temp3);
					strcpy(egbb_path, temp3);
				}
				if (strcmp(comandos, "egbb_men") == 0) {
					sscanf(linea, "egbb_men %d", &tamano3);
					egbb_men = tamano3;
				}
				if (strcmp(comandos, "egbb_cache_size") == 0) {
					sscanf(linea, "egbb_cache_size %d", &tamano4);
					egbb_cache_size = tamano4;
				}
				if (strcmp(comandos, "egbb_load_type") == 0) {
					sscanf(linea, "egbb_load_type %d", &tamano5);
					egbb_load_type = tamano5;
				}
				if (strcmp(comandos, "use_egbb") == 0) {
					sscanf(linea, "use_egbb %d", &tamano14);
					use_egbb = tamano14;
				}
				/* Tablebases */
				if (strcmp(comandos, "egtb_path") == 0) {
					sscanf(linea, "egtb_path %s", temp4);
					strcpy(egtb_path, temp4);
				}
				if (strcmp(comandos, "egtb_cache_size") == 0) {
					sscanf(linea, "egtb_cache_size %d", &tamano13);
					egtb_cache_size = tamano13;
				}
				if (strcmp(comandos, "use_egtb") == 0) {
					sscanf(linea, "use_egtb %d", &tamano15);
					use_egtb = tamano15;
				}
				/* Noise: numero de nodos buscados a partir de los cuales se ve informacion del motor, evita enviar muchas lineas con
				informacion a la GUI */
				if (strcmp(comandos, "noise") == 0) {
					sscanf(linea, "noise %d", &tamano6);
					noise = tamano6;
				}
				#if defined (UNDER_ELO)
				/*Regulación ELO y niveles*/
				if (strcmp(comandos, "limitstrength") == 0) {
					sscanf(linea, "limitstrength %s", temp5);
					strcpy(limitstrength, temp5);
				}
				if (strcmp(comandos, "elo") == 0) {
					sscanf(linea, "elo %d", &tamano7);
					elo = tamano7;
				}
				if (strcmp(comandos, "elo_type") == 0) {
					sscanf(linea, "elo_type %s", temp6);
					strcpy(elo_type, temp6);
				}
				if (strcmp(comandos, "easylevel") == 0) {
					sscanf(linea, "easylevel %d", &tamano9);
					easylevel = tamano9;
				}
				if (strcmp(comandos, "style") == 0) {
					sscanf(linea, "style %d", &tamano12);
					style = tamano12;
				}
				if (strcmp(comandos, "diff_engine") == 0) {
					sscanf(linea, "diff_engine %d", &tamano11);
					diff_engine = tamano11;
				}
				if (strcmp(comandos, "diff_computer_engine") == 0) {
					sscanf(linea, "diff_computer_engine %d", &tamano8);
					diff_computer_engine = tamano8;
				}
				if (strcmp(comandos, "diff_human_computer") == 0) {
					sscanf(linea, "diff_human_computer %d", &tamano10);
					diff_human_computer = tamano10;
				}
				#endif
			}
		}
		/* mostrara en pantalla informacion de los parametros */
		imprime_configuracion(tamano1, tamano2);
	}
	fclose(inifile);
	return;
#endif
}

/* esta es la funcion que muestra en pantalla información sobre el valor de los parametros en el protocolo xboard */
void imprime_configuracion(int tamano1, int tamano2) {
	printf("# Default or Cfg. File Values...\n");
	printf("# Table hash size: %d Mb.\n", tamano1);
	printf("# Eval cache size: %d Mb.\n", tamano2);
	#if defined (UNDER_ELO)
	printf("# LimitStrength: %s\n", limitstrength);
	printf("# Elo: %d\n", elo);
	printf("# Elo_Type: %s\n", elo_type);
	printf("# EasyLevel: %d\n", easylevel);
	printf("# Style: %d\n", style);
	printf("# Diff_engine: %d\n", diff_engine);
	printf("# Diff_computer_engine: %d\n", diff_computer_engine);
	printf("# Diff_human_computer: %d\n", diff_human_computer);
	#endif
	printf("# Ownbook: %s\n", ownbook);
	printf("# Book: %s\n", libro);
	printf("# Bitbases path: %s\n", egbb_path);
	printf("# Bitbases men: %d\n", egbb_men);
	printf("# Bitbases cache size: %d\n", egbb_cache_size);
	printf("# Bitbases load type: %d\n", egbb_load_type);
	printf("# Use Bitbases: %d\n", use_egbb);
	printf("# Tablebases path: %s\n", egtb_path);
	printf("# Tablebases cache size: %d\n", egtb_cache_size);
	printf("# Use Tablebases: %d\n", use_egtb);
	printf("# Noise for PV: %d\n", noise);
	printf("\n");
	return;
}

/* si cuando hemos llamado al motor le hemos enviado parametros lo tendremos que tener en cuenta cuando iniciamos el motor en modo consola
o modo xboard */
void Analiza_parametros(void)
{
    printf("\n");
	printf("# Command Line Values...\n");
	if (guarda_hash != 0) {
		TTSize = (guarda_hash * 1024 * 1024) / sizeof(TType);
		ETSize = (guarda_hash / 8 * 1024 * 1024) / sizeof(EType);
		printf("# Table hash size: %d Mb.\n", guarda_hash);
		printf("# Eval cache size: %d Mb.\n", guarda_hash/8);
	}
	if (strcmp(guarda_ownbook, "") > 0){
		strcpy(ownbook, guarda_ownbook);
		printf("# Ownbook: %s\n", ownbook);
	}
	if (strcmp(guarda_libro, "") > 0){
		strcpy(libro, guarda_libro);
		printf("# Opening book: %s\n", libro);
	}
	if (strcmp(guarda_egbb_path, "") > 0){
		strcpy(egbb_path, guarda_egbb_path);
		printf("# Bitbases path: %s\n", egbb_path);
	}
	if (guarda_egbb_men != 0) {
		egbb_men = guarda_egbb_men;
		printf("# Bitbases men: %d\n", egbb_men);
	}
	if (guarda_egbb_cache_size != 0) {
		egbb_cache_size = guarda_egbb_cache_size;
		printf("# Bitbases cache size: %d\n", egbb_cache_size);
	}
	if (guarda_egbb_load_type != 0) {
		egbb_load_type = guarda_egbb_load_type;
		printf("# Bitbases load type: %d\n", egbb_load_type);
	}
	if (guarda_use_egbb != 0) {
		use_egbb = guarda_use_egbb;
		printf("# Use Bitbases: %d\n", use_egbb);
	}
	if (strcmp(guarda_egtb_path, "") > 0){
		strcpy(egtb_path, guarda_egtb_path);
		printf("# Tablebases path: %s\n", egtb_path);
	}
	if (guarda_egtb_cache_size != 0) {
		egtb_cache_size = guarda_egtb_cache_size;
		printf("# Tablebases cache size: %d\n", egtb_cache_size);
	}
	if (guarda_use_egtb != 0) {
		use_egtb = guarda_use_egtb;
		printf("# Use Tablebases: %d\n", use_egtb);
	}
	if (guarda_noise != 0) {
		noise = guarda_noise;
		printf("# Noise for PV: %d\n", noise);
	}
#if defined (UNDER_ELO)
	if (strcmp(guarda_limitstrength, "") > 0){
		strcpy(limitstrength, guarda_limitstrength);
		printf("# LimitStrength: %s\n", limitstrength);
	}
	if (guarda_elo != 0) {
		elo = guarda_elo;
		printf("# Elo: %d\n", elo);
	}
	if (strcmp(guarda_elo_type, "") > 0){
		strcpy(elo_type, guarda_elo_type);
		printf("# Elo_Type: %s\n", elo_type);
	}
	if (guarda_style != 0) {
		style = guarda_style;
		printf("# Style: %d\n", style);
	}
	if (guarda_diff_engine != 0) {
		diff_engine = guarda_diff_engine;
		printf("# Diff_Engine: %d\n", diff_engine);
	}
	if (guarda_diff_computer_engine != 0) {
		diff_computer_engine = guarda_diff_computer_engine;
		printf("# Diff_Computer_Engine: %d\n", diff_computer_engine);
	}
	if (guarda_diff_human_computer != 0) {
		diff_human_computer = guarda_diff_human_computer;
		printf("# Diff_Human_Computer: %d\n", diff_human_computer);
	}
	if (guarda_easylevel != 0) {
		easylevel = guarda_easylevel;
		printf("# EasyLevel: %d\n", easylevel);
	}
	printf("\n");
#endif
}

#if defined (UNDER_ELO)
void Nivel(void)
{
	/*corregimos el diff_computer_engine*/
	if (elo < 700) diff_computer_engine = 0;
	else if (elo < 1900) diff_computer_engine = diff_computer_engine * (1 - ((1900-elo)/1200));

	/*elo*/
	if (easylevel == 0) {
		if (strcmp (elo_type, "engine") == 0) {
			elo = elo - diff_engine;
			if (protocolo == XBOARD)
                printf("# Elo engine: %d\n", elo + diff_engine);
            else
                printf("info Elo engine: %d\n", elo + diff_engine);
		}
		else if (strcmp (elo_type, "computer") == 0) {
			elo = elo - diff_engine - diff_computer_engine;
			if (protocolo == XBOARD)
                printf("# Elo computer: %d\n", elo + diff_engine + diff_computer_engine);
            else
                printf("info Elo computer: %d\n", elo + diff_engine + diff_computer_engine);
		}
		else if (strcmp (elo_type, "human") == 0) {
			elo = elo - diff_engine - diff_computer_engine - diff_human_computer;
			if (protocolo == XBOARD)
                printf("# Elo human: %d\n", elo + diff_engine + diff_computer_engine + diff_human_computer);
            else
                printf("info Elo human: %d\n", elo + diff_engine + diff_computer_engine + diff_human_computer);
		}
		printf("\n");
	}
	else if (easylevel == 1) {
        if (protocolo == XBOARD)
            printf("# Random move\n");
        else
            printf("info Random move\n");
		printf("\n");
	}
	else if (easylevel >= 2) {
		switch (easylevel) {
			case 2:
				elo = 500 + (rand() % 500);
				break;
			case 3:
				elo = 1000 + (rand() % 200);
				break;
			case 4:
				elo = 1200 + (rand() % 200);
				break;
			case 5:
				elo = 1400 + (rand() % 300);
				break;
			case 6:
				elo = 1700 + (rand() % 300);
				break;
			case 7:
				elo = 2000 + (rand() % 200);
				break;
			case 8:
				elo = 2200 + (rand() % 100);
				break;
			case 9:
				elo = 2300 + (rand() % 100);
				break;
			case 10:
				elo = 2400 + (rand() % 100);
				break;
			default:
				break;
		}
		if (strcmp (elo_type, "human") == 0) {
			elo = elo - diff_engine - diff_computer_engine - diff_human_computer;
			if (protocolo == XBOARD)
                printf("# Elo human set randomly: %d\n", elo + diff_engine + diff_computer_engine + diff_human_computer);
            else
                printf("info Elo human set randomly: %d\n", elo + diff_engine + diff_computer_engine + diff_human_computer);
		}
		else if (strcmp (elo_type, "computer") == 0) {
			elo = elo - diff_engine - diff_computer_engine;
			if (protocolo == XBOARD)
                printf("# Elo computer set randomly: %d\n", elo + diff_engine + diff_computer_engine);
            else
                printf("info Elo engine set randomly: %d\n", elo + diff_engine);
		}
		else {
			elo = elo - diff_engine;
			if (protocolo == XBOARD)
                printf("# Elo engine set randomly: %d\n", elo + diff_engine);
            else
                printf("info Elo engine set randomly: %d\n", elo + diff_engine);
		}
		printf("\n");
	}
	if (elo > 2500) elo = 2500;
	else if (elo < 0) elo = 0;
}
#endif
