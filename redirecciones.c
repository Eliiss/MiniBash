#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

#include "redirecciones.h"


/* funcion que abre el archivo situado en la posicion indice_entrada+1 */
/* de la orden args y elimina de ella la redireccion completa          */
                                  
void redirec_entrada(char **args, int indice_entrada, int *entrada)
{
	*entrada = open(args[indice_entrada +1], O_RDONLY);

	if(*entrada == -1)
	{
		perror("Error al abrir el archivo de redirecion de entrada\n");
	}
    else
    {
        args[indice_entrada] = NULL;
    	args[indice_entrada +1] = NULL;
    }
}

/* funcion que abre el archivo situado en la posicion indice_salida+1 */
/* de la orden args y elimina de ella la redireccion completa         */

void redirec_salida(char **args, int indice_salida, int *salida)
{
	*salida = open(args[indice_salida +1], O_RDWR | O_CREAT | O_TRUNC, 0666);
		
	if(*salida == -1)
	{
		perror("Error al abrir el archivo de redirecion de salida\n");
	}
	else
    {
		args[indice_salida] = NULL;
		args[indice_salida +1] = NULL;
    }
}

