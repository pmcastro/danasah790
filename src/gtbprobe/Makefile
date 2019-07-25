
INCLUDE = -Isysport/ \
	  -Icompression/ \
	  -Icompression/liblzf/ \
	  -Icompression/zlib/ \
	  -Icompression/lzma/ \
	  -Icompression/huffman/

SRCFILES := gtb-probe.c gtb-dec.c gtb-att.c sysport/sysport.c \
	   compression/wrap.c compression/huffman/hzip.c \
	   compression/lzma/LzmaEnc.c compression/lzma/LzmaDec.c \
	   compression/lzma/Alloc.c compression/lzma/LzFind.c \
	   compression/lzma/Lzma86Enc.c compression/lzma/Lzma86Dec.c \
	   compression/lzma/Bra86.c compression/zlib/zcompress.c \
	   compression/zlib/uncompr.c compression/zlib/inflate.c \
	   compression/zlib/deflate.c compression/zlib/adler32.c \
	   compression/zlib/crc32.c compression/zlib/infback.c \
	   compression/zlib/inffast.c compression/zlib/inftrees.c \
	   compression/zlib/trees.c compression/zlib/zutil.c \
	   compression/liblzf/lzf_c.c compression/liblzf/lzf_d.c

OBJFILES := $(patsubst %.c,%.o,$(SRCFILES))
PROFFILES := $(SRCFILES:.c=.gcno) $(SRCFILES:.c=.gcda)
LIBNAME := libgtb.a
CFLAGS=-g0 -Wall -Wextra -O3 -msse $(INCLUDE) -DZ_PREFIX

OS = android
ARCH = arm
DEBUG =
STATIC = 

.PHONY: all clean
.DEFAULT_GOAL := all  

ifeq ($(OS),osx)
	CC=gcc
endif

ifeq ($(DEBUG),yes)
	CFLAGS=-g -O0 -Wall -Wextra $(INCLUDE) -DZ_PREFIX
else
	CFLAGS += -DNDEBUG
endif

ifeq ($(STATIC),yes)
	CFLAGS += -static
endif

ifeq ($(OS),linux)
	CC		= gcc
endif

ifeq ($(OS),windows)
	ifeq ($(ARCH),x64)
		CC = x86_64-w64-mingw32-gcc
		AR = x86_64-w64-mingw32-ar
	endif
	ifeq ($(ARCH),x64_SEE4)
		CC = x86_64-w64-mingw32-gcc
		AR = x86_64-w64-mingw32-ar
	endif
	ifeq ($(ARCH),i686)
	    	CC = i586-pc-mingw32-gcc
		AR = i586-pc-mingw32-ar
	endif
	CFLAGS	+= -DMINGW 
endif

ifeq ($(OS),android)
	CC = arm-none-linux-gnueabi-gcc
	AR = arm-none-linux-gnueabi-gcc-ar
	RANLIB = arm-none-linux-gnueabi-gcc-ranlib
	CFLAGS= -g0 -Wall -Wextra -O3 $(INCLUDE) -DNDEBUG -DZ_PREFIX 
endif

ifeq ($(ARCH),x64)
	ARCHFLAGS	= -m64
	CFLAGS		+= -m64
endif

ifeq ($(ARCH),x64_SEE4)
	ARCHFLAGS	= -m64
	CFLAGS		+= -m64
endif

ifeq ($(ARCH),i686)
	ARCHFLAGS	= -m32 -march=i686
	CFLAGS		+= -m32 -march=i686
endif

all: $(LIBNAME)

$(LIBNAME): $(OBJFILES)
	$(AR) rcs $@ $(OBJFILES)
	$(RANLIB) $@

clean:
	$(RM) -f $(OBJFILES) $(LIBNAME)

pgo-clean:
	$(RM) -f $(PROFFILES)

.depend:
	$(CC) -MM $(INCLUDE) $(DEFAULT_CFLAGS) $(SRCFILES) > $@

include .depend
