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
DanaSah desde la versión 6 accede a libros de apertura en formato Polyglot. Gracias a Michel Van den Bergh por
proporcionar codigo libre de uso para el acceso, por este motivo no sera comentado dicho codigo, mas informacion
en la pagina: http://hardy.uhasselt.be/Toga/book_format.html
El libro es iniciado en la funcion xboard() o uci() y se utilizara en la busqueda.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined (UNDER_CE) || defined (_WIN32)
	#include <windows.h>						/*perror*/
	#include <io.h>
#else
    #include <unistd.h>							/*getcwd*/
#endif
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

char *piece_names="pPnNbBrRqQkK";

char *promote_pieces=" nbrq";

FILE *file;

U64 *RandomPiece     =Random64;
U64 *RandomCastle    =Random64+768;
U64 *RandomEnPassant =Random64+772;
U64 *RandomTurn      =Random64+780;

entry_t entry_none = {
    0, 0, 0, 0
};

#define MAX_MOVES 100

void open_book()
{
	if (libro_cargado) close_book();

	/* buscamos el libro en la misma ruta que el motor */
#if defined (UNDER_CE)
	TCHAR BookPath[64], *pos;
	char fullPath1[128];
	GetModuleFileName(NULL, BookPath, 96);
	pos = wcsrchr(BookPath, '\\');
	*(pos + 1) = '\0';
	wcstombs(fullPath1, BookPath, 96);
	strcat(fullPath1, libro);
#elif defined (UNDER_ARM)
    char fullPath1[128];
    strcpy(fullPath1, libro);
#else
    char fullPath1[128];
    strcpy(fullPath1, libro);
	/*char BookPath[64];
	char fullPath1[128];
	getcwd(BookPath, 96);
	strcpy(fullPath1, BookPath);
	strcat(fullPath1, "/");
	strcat(fullPath1, libro);*/
#endif

	file = fopen(fullPath1, "rb");
   	if(file) {
   		libro_cargado = VERDADERO;
   		if (protocolo == XBOARD)
            printf("# Opening book loaded.\n");
        else
            printf("info Opening book loaded.\n");
	}
	else {
        libro_cargado = FALSO;
        if (protocolo == XBOARD)
            printf("# Opening book missing.\n");
        else
            printf("info Opening book missing.\n");
    }
}

void close_book()
{
	if (file) fclose(file);
	file = NULL;
	libro_cargado = FALSO;
}

U64 hash_book(char *fen){
    char board_s[72+1];
    char to_move_c;
    char castle_flags_s[4+1];
    char ep_square_s[2+1];
    char board[8][8];
    char c;
    int r,f,i,p_enc;
    unsigned p;
    U64 key=0;
    sscanf(fen,"%72s %c %4s %2s",
           board_s,
           &to_move_c,
           castle_flags_s,
           ep_square_s);
    r=7;
    f=0;
    p=0;
    while(1){
        if(p>=strlen(board_s)) break;
        c=board_s[p++];
        if(c=='/'){
            r--;
            f=0;
            continue;
        }
        if(('1'<=c)&&('8'>=c)){
            for(i=0;i<=c-'1';i++){
                board[f++][r]='-';
            }
            continue;
        }
        board[f++][r]=c;
    }
    for(f=0;f<=7;f++){
        for(r=0;r<=7;r++){
            c=board[f][r];
            if(c!='-'){
                p_enc=strchr(piece_names,c)-piece_names;
                key^=RandomPiece[64*p_enc+8*r+f];
            }
        }
    }
    p=0;
    while(1){
        if(p>=strlen(castle_flags_s)) break;
        c=castle_flags_s[p++];
        switch(c){
            case '-' :
                break;
            case 'K' :
                key^=RandomCastle[0];
                break;
            case 'Q' :
                key^=RandomCastle[1];
                break;
            case 'k' :
                key^=RandomCastle[2];
                break;
            case 'q' :
                key^=RandomCastle[3];
                break;
            default:
                break;
        }
    }
    if(ep_square_s[0]!='-'){
        f=ep_square_s[0]-'a';
        if(to_move_c=='b'){
            if((f>0 && board[f-1][3]=='p')||
               (f<7 && board[f+1][3]=='p')){
                key^=RandomEnPassant[f];
            }
        }else{
            if((f>0 && board[f-1][4]=='P')||
               (f<7 && board[f+1][4]=='P')){
                key^=RandomEnPassant[f];
            }
        }
    }
    if(to_move_c=='w'){
        key^=RandomTurn[0];
    }
    return key;
}

int int_from_file(FILE *f, int l, U64 *r) {
    int i,c;
    for(i=0;i<l;i++){
        c=fgetc(f);
        if(c==EOF){
            return 1;
        }
        (*r)=((*r)<<8)+c;
    }
    return 0;
}

int entry_from_file(FILE *f, entry_t *entry) {
    int ret;
    U64 r = 0;
    ret=int_from_file(f,8,&r);
    if(ret) return 1;
    entry->key=r;
    ret=int_from_file(f,2,&r);
    if(ret) return 1;
    entry->move=r;
    ret=int_from_file(f,2,&r);
    if(ret) return 1;
    entry->weight=r;
    ret=int_from_file(f,4,&r);
    if(ret) return 1;
    entry->learn=r;
    return 0;
}

int find_key(FILE *f, U64 key, entry_t *entry) {
    int first, last, middle;
    entry_t last_entry, middle_entry;
    first=-1;
    if(fseek(f,-16,SEEK_END)){
        *entry=entry_none;
        entry->key=key+1;
        return -1;
    }
    last=ftell(f)/16;
    entry_from_file(f,&last_entry);
    while(1){
        if(last-first==1){
            *entry=last_entry;
            return last;
        }
        middle=(first+last)/2;
        fseek(f,16*middle,SEEK_SET);
        entry_from_file(f,&middle_entry);
        if(key<=middle_entry.key){
            last=middle;
            last_entry=middle_entry;
        }else{
            first=middle;
        }
    }
}

void move_to_string(char move_s[6], U16 move){
    int f,fr,ff,t,tr,tf,p;
    f=(move>>6)&077;
    fr=(f>>3)&0x7;
    ff=f&0x7;
    t=move&077;
    tr=(t>>3)&0x7;
    tf=t&0x7;
    p=(move>>12)&0x7;
    move_s[0]=ff+'a';
    move_s[1]=fr+'1';
    move_s[2]=tf+'a';
    move_s[3]=tr+'1';
    if(p){
        move_s[4]=promote_pieces[p];
        move_s[5]='\0';
    }else{
        move_s[4]='\0';
    }
    if(!strcmp(move_s,"e1h1")){
        strcpy(move_s,"e1g1");
    }else  if(!strcmp(move_s,"e1a1")){
        strcpy(move_s,"e1c1");
    }else  if(!strcmp(move_s,"e8h8")){
        strcpy(move_s,"e8g8");
    }else  if(!strcmp(move_s,"e8a8")){
        strcpy(move_s,"e8c8");
    }
}

void book_move(char *move_s)
{
    entry_t entry;
    int offsetb;
    U64 key = 0;
    entry_t entries[MAX_MOVES];
    int count=0;
    int ret, i, j, k;
    int total_weight;
	char EPD[256];
	double y, z;

	Tablero_FEN(EPD);
	/*printf("EPD = %s\n", EPD);*/
	key = hash_book(EPD);

    offsetb=find_key(file,key,&entry);
	if(entry.key!=key){
#if defined (_MSC_VER)
		printf("%016I64x: No position in book\n",key);
#else
		/*printf("%016llx: No position in book\n",key);*/
		/*printf("info No position in book\n");*/
#endif
		fuera_del_libro++;
        return;
    }
    else
    	fuera_del_libro = 0;
    entries[0]=entry;
    count=1;
    fseek(file,16*(offsetb+1),SEEK_SET);
    while(1){
        ret=entry_from_file(file,&entry);
        if(ret){
            break;
        }
        if(entry.key!=key){
            break;
        }
        if(count==MAX_MOVES){
            printf("Too many moves in this position (max=%d)\n",MAX_MOVES);
            return;
        }
        entries[count++]=entry;
    }
    total_weight=0;
    for(i=0;i<count;i++){
        total_weight+=entries[i].weight;
    }
	j = rand() % 10000;
	y = (double) j / 100.00;
	printf("y: %5.2f%%\n", y);
    for(i=0;i<count;i++){
        move_to_string(move_s,entries[i].move);
		z = 100*((double) entries[i].weight/ (double) total_weight);
        printf("move=%s weight=%5.2f%%\n", move_s, z);
    }
	k = 0; z = 0.00;
    for(i=0;i<count;){
		z += 100*((double) entries[i].weight/ (double) total_weight);
		if (y < z) {
			k = i;
			break;
		}
		i++;
    }
	move_to_string(move_s,entries[k].move);
	return;
}

