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
Funciones que se van a utilizar en el programa
*/

#include <stdio.h>
/* gnu gcc codeblocks 5.1.0 no tiene strtok_r, deberiamos eliminar la siguiente línea */
#define HAVE_STRTOK_R

/* danasah */
int main (int argc, char *argv[]);
void Libera_memoria(void);

/* consola */
void consola(void);

/* xboard */
void xboard(void);
#if defined (UNDER_ELO)
void informa(void);
#endif

/* configura */
void leer_inifile(void);
void imprime_configuracion(int tamano1, int tamano2);
void Analiza_parametros(void);
#if defined (UNDER_ELO)
void Nivel(void);
#endif

/* uci */
void uci(void);

/* libro */
void open_book();
void close_book();
int int_from_file(FILE *f, int l, U64 *r);
int entry_from_file(FILE *f, entry_t *entry);
int find_key(FILE *f, U64 key, entry_t *entry);
U64 hash_book(char *fen);
void move_to_string(char move_s[6], U16 move);
void book_move(char *move_s);

/* bitbases */
void load_bitbases(void);
int LoadEgbbLibrary(char* main_path, int egbb_cache_size);
void unload_bitbases(void);
int probe_bitbases(int *score);

/* tablebases */
void load_tablebases(void);
void reset_tablebases(void);
void unload_tablebases(void);
int probe_tablebases(int *score, int type);

/* test */
U64 perft(int depth);
void Test(void);
void bench(void);

/* hash */
void inicia_hash(void);
void poner_hash(void);
void clear_hash(void);
void store_hash(int depth, char type, movimiento mov, int score);
void probe_hash(int depth, int *hdepth, char *htype, movimiento *hmov, int *hscore, int *hpruning);
void store_eval(int score);
int probe_eval(void);
void alloc_hash(void);
void free_hash(void);

/* ataques */
int	EstaEnJaque(int turno2);
int atacado(int k, int turno2);
int atacado_nr(int k, int turno2);

/* movimientos */
void Generar_Movimiento_PVS(int de, int a, int tipo, movimiento *plista, int *pcuenta);
void Generar_Movimiento_Capturas(int de, int a, int tipo, movimiento *plista, int *pcuenta);
int	Generar(movimiento *plista);
int	Generar_Capturas(movimiento *plista);
int Hacer_Movimiento(movimiento m);
void Deshacer(void);
int Generar_root_legales(void);

/* see */
int malaCaptura(movimiento mcmov);
void calcula_atacantes(int casilla);
int see(int turno2, int casilla, int de, int rapido);

/* busqueda */
void OrdenaMovimiento(int de, int cuenta, movimiento *lista);
void OrdenaRootMayorMenor(void);
void InsertaMejormovimiento_root(void);
//void Ordena_root(void);
void Actualiza_pv(movimiento pvmov);
void MotorPiensa(movimiento *mejormovimiento, movimiento *pondermovimiento);
int	RAIZ(int alpha, int beta, int depth);
int	PVS(int alpha, int beta, int depth, int hacer_nulo);
int Extiende_capturas(int alpha, int beta, int depth);
void Ordena_hmove_pv(int cuenta, movimiento *lista, movimiento hmove);
int repeticion(void);
void checkup(void);
#if defined (UNDER_ELO)
void regular_nps(void);
#endif

/* multipv */
void multiMotorPiensa(movimiento *mejormovimiento, movimiento *pondermovimiento);
int	multiRAIZ(int alpha, int beta, int depth);
void multiprint_pvs(int depth, int score, int longuitud, int m);
void Ordena_PVs(int m1);
void Ordena_hmove_multipv(int cuenta, movimiento *lista, movimiento hmove);
void Ordena_root_lista(int cuenta);

/* evalua */
void Calcula_material(void);
void Busca_peones(void);
int Movilidad_Caballo(int pos);
int Movilidad_Alfil(int pos);
int Movilidad_Torre(int pos);
int Movilidad_Dama(int pos);
int Evaluar(int alpha, int beta);
int rey_blanco(int sq);
int rey_negro(int sq);
void peones_blancos_mediojuego(int i, int *rm, int *rf);
int peones_blancos_final(int i);
void peones_negros_mediojuego(int i, int *rm, int *rf);
int peones_negros_final(int i);
void Evalua_posicion_final(void);
void Evalua_posicion_mediojuego_final(void);
void Evalua_seguridad_rey(void);
int reconoce_tablas(void);
int reclama_tablas_material(void);

/* entradas */
int Comprobar_Tipo_Movimiento(int de, int a, char *s);
void Comprobar_Movimiento(char *s);
int bioskey(void);
void readline();
#ifndef HAVE_STRTOK_R
char *strtok_r(char *s, const char *sep, char **p);
#endif
int get_ms(void);

/* salidas */
void print_pv(int depth, int score);
void Enviar_Movimiento(movimiento mejomovimiento, movimiento pondermovimiento);
void imprime_resultado(int cuenta, movimiento *lista);

/* aleatorio (numeros) */
U32 Rand32(void);
U64 Rand64(void);

/* fen */
void SetBoard(char *string);
void Tablero_FEN(char *string);

#if defined (UNDER_CE)
    #include <stdarg.h>
    int __cdecl PrintF (const char *fmt, ...);
    int GetCh (void);
    typedef struct
    {
        int version;
        void *pIOInst;
        int (__stdcall *p_input_available) (void *pInst);
        char * (__stdcall *p_get_string) (void *pInst, char *s, int nmaxlen);
        void (__stdcall *p_put_string) (void *pInst, const char *s);
        int (__cdecl *p_print) (void *pInst, const char *fmt, ...);
        int (__cdecl *p_vprint) (void *pInst, const char *fmt, va_list args);
        unsigned int (__stdcall *p_available_memory) (void *pInst);
        void * (__stdcall *p_allocate_memory) (void *pInst, int nsize);
        void (__stdcall *p_free_memory) (void *pInst, void *pmem);
        int argc;
        char **argv;
    }
    TEngineIO;
    extern TEngineIO *EngineIO;
    #define malloc(n_) (EngineIO->p_allocate_memory(EngineIO->pIOInst,n_))
    #define free(p_) (EngineIO->p_free_memory(EngineIO->pIOInst,p_))
    #define printf PrintF
    #define puts(s_) printf("%s\n",s_)
    #define getch() GetCh()
    #define gets(s_) (EngineIO->p_get_string (EngineIO->pIOInst, s_, 255))
    #define fflush(f_)
    #define InputAvailable() (EngineIO->p_input_available(EngineIO->pIOInst))
#endif
