CFLAGS = -CC = gcc
CFLAGS = -g -Wall

# Creación de la minishell, enlazando todos los archivos .o, junto a la biblioteca libshell.a
minishell: libmemoria.o entrada_minishell.o redirecciones.o ejecutar.o minishell.o libshell.a
	$(CC) $(CFLAGS) libmemoria.o entrada_minishell.o redirecciones.o ejecutar.o minishell.o -L./ -lshell -o minishell

# Creación del archivo para enlazar libmemoria.o
libmemoria.o: libmemoria.c libmemoria.h
	$(CC) $(CFLAGS) -c libmemoria.c

# Creación del archivo para enlazar entrada_minishell.o
entrada_minishell.o: entrada_minishell.c entrada_minishell.h
	$(CC) $(CFLAGS) -c entrada_minishell.c

# Creación del archivo para enlazar redirecciones.o
redirecciones.o: redirecciones.c redirecciones.h
	$(CC) $(CFLAGS) -c redirecciones.c

# Creación del archivo para enlazar ejecutar.o
ejecutar.o: ejecutar.c ejecutar.h minishell.h parser.h libmemoria.h redirecciones.h
	$(CC) $(CFLAGS) -c ejecutar.c

# Creación del archivo para enlazar minishell.o
minishell.o: minishell.c minishell.h internas.h entrada_minishell.h ejecutar.h
	$(CC) $(CFLAGS) -c minishell.c

# Creación de la biblioteca libshell.a, que contiene internas.o y parser.o
# Código comentado ya que ya se ha creado la librería estática
#libshell.a: internas.o parser.o
#	ar -rs libshell.a internas.o parser.o

.PHONY: clean
# Función encargada de borrar los archivos intermedios
clean:
	 rm -f libmemoria.o entrada_minishell.o ejecutar.o minishell.o redirecciones.o 
