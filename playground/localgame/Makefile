# Nosso compilador (nao vejo porque utilizar o clang)
CC=gcc

# Compiler flags
CFLAGS=`pkg-config --cflags SDL2_image` -g -Wall -std=c99 -O2
LDLIBS=`pkg-config --libs SDL2_image` -lm
#CFLAGS=`sdl2-config --libs --cflags` -ggdb3 -O0 --std=c99 -Wall -lSDL2_image -lm

# Observacoes:
# 1.
# "-ggdb3" eh similar ao "-g", mas ele produz informacao extra para DEBUGGING
# O problema parece ser: "-ggdb3" depende do sistema operacional. No Linux ele funciona, mas em outros talvez nao.
# 2.
# "sdl2-config --libs --cflags" eh extremamente parecido com o comando "pkg-config"
# Comandos para obter flags para o SDL2: pkg-config --libs SDL2_<Tab> *e* pkg-config --cflags SDL2_<Tab>
# Comuns: -I/usr/include/SDL2 -D_REENTRANT, -L/usr/lib -lSDL2_image -pthread -lSDL2
# 3.
# "-lm" eh para a biblioteca de Matematica <math.h>

# Caso queiramos colocar headers em comum, da para colocar aqui
HDRS=

# Source files
S=main.c

# Gerar os object files
OBJS=$(S:.c=.o)

# Nome do arquivo executavel (o jogo em si)
EXEC=game

# default recipe (NAO ENTENDI)
all: $(EXEC)

# recipe for building the final executable
#$(EXEC): $(OBJS) $(HDRS) Makefile
#	$(CC) -o $@ $(OBJS) $(CFLAGS)
$(EXEC): $(OBJS) $(HDRS) Makefile
	$(CC) -o $@ $(OBJS) $(LDLIBS) $(CFLAGS)

# recipe for building object files
#$(OBJS): $(@:.o=.c) $(HDRS) Makefile
#	$(CC) -o $@ $(@:.o=.c) -c $(CFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)
