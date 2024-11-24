#ifndef EJECUTAR_H
#define EJECUTAR_H
#include <sys/types.h>

#include "parser.h"
#include "libmemoria.h"
#include "redirecciones.h"

int **crear_pipes(int numero_ordenes);
pid_t ejecutar_orden(const char *orden, int entrada, int salida, int *pbackgr);
void ejecutar_secuencia_ordenes(char *orden);
void ejecutar_linea_ordenes(const char *orden);

#endif
