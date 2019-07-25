CC = clang
#CFLAGS = -march=armv7-a -DUNDER_ARM -Ofast -std=c99 -Wall -Wextra -pedantic -Werror -Wfatal-errors -fno-strict-aliasing -fno-strict-overflow -fno-exceptions -ffunction-sections -fomit-frame-pointer -fdata-sections -flto -w -g0 -s -pthread
#CFLAGS = -m32 -march=armv7-a -DUNDER_ARM -fPIE -mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb -Wall -Wextra -Werror -O2 -std=c99 -fno-strict-aliasing -fno-strict-overflow -ffunction-sections -fdata-sections
CFLAGS = -m64 -march=armv8-a -DUNDER_ARM -DUNDER_64BIT -fPIE -Wall -Wextra -Werror -O2 -std=c99 -fno-strict-aliasing -fno-strict-overflow -ffunction-sections -fdata-sections
#LIBS = -static -ldl -lm -s ./gtb/ARM/32/libgtb.a
LIBS = -static -ldl -lm -s ./gtb/ARM/64/libgtb.a

.SUFFIXES: .c .o
 
OBJS = aleatorio.o atacado.o busqueda.o configura.o consola.o danasah.o \
     egbb.o egtb.o entradas.o evalua.o fen.o hash.o libro.o movimientos.o \
	 multipv.o salidas.o see.o test.o uci.o variables.o xboard.o
	 
all: danasah

.c.o: 
	$(CC) $(CFLAGS) -c $< -o $@

danasah: $(OBJS) 
	$(CC) -o danasah $(OBJS) $(CFLAGS) $(LIBS)

clean:
	del *.o
	del danasah