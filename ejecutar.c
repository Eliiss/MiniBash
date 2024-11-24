#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>

#include "parser.h"
#include "ejecutar.h"
#include "libmemoria.h"
#include "redirecciones.h"
#include "minishell.h"

// creación de tuberias para la comunicación entre comandos
int **crear_pipes(int nordenes)
{
   //array de pipes 	 
   int **pipes = NULL;
   int i;

   pipes = (int **)malloc(sizeof(int *) * (nordenes - 1));
   for (i = 0; i < (nordenes - 1); i++)
   {
      // asociar una tubería para cada par de comandos
      pipes[i] = (int *)malloc(sizeof(int) * 2);
      pipe(pipes[i]);
   }
   return pipes;
}

//ejecución de comandos individuales
pid_t ejecutar_orden(const char *orden, int entrada, int salida, int *pbackgr)
{
    char **args;
    pid_t pid;
    int indice_ent = -1, indice_sal = -1; //índices para manejar las redirecciones 

    //pasar el comando y manejo de las redirecciones
    if ((args = parser_orden(orden, &indice_ent, &indice_sal, pbackgr)) == NULL)
    {
        return (-1); //error
    }
    else
    {
        //manejo de la entrada y la salida de las redirecciones
        if (indice_ent != -1)
        {
            redirec_entrada(args, indice_ent, &entrada);
        }

        if (indice_sal != -1)
        {
            redirec_salida(args, indice_sal, &salida);
        }

        //Creación de un nuevo proceso hijo
        pid = fork();

	//Manejo de errores en la creación del proceso hijo    
        if (pid == -1)
        {
            perror ("error");
            exit (-1);
        }
        else if (pid != 0) //proceso padre
        {
	    //cierre de descriptores 	
            if (entrada != 0)
            {
                close(entrada);
            }

            if (salida != 1)
            {
                close(salida);
            }

            free_argumentos(args); //liberar memoria 
            return pid; //retorno del pid del proceso hijo al padre 
        }
        else if (pid == 0) //proceso hijo
        {
	    //Redirección a entrada estándar (teclado)
            if (entrada != 0)
            {
                dup2(entrada, STDIN_FILENO);
                close(entrada);
            }
		
            //Redirección a salida estándar (pantalla)
            if (salida != 1)
            {
                dup2(salida, STDOUT_FILENO);
                close(salida);
            }

            execvp(args[0], args); //ejecución del nuevoo programa 
            free_argumentos(args); //liberar memoria 
            exit(-1);
        }
        else
        {
            if (entrada != 0)
            {
                close (entrada);
            }

            if (salida !=1)
            {
                close (salida);
            }

            return pid; //retorno de pid del proceso hijo 
        }
    }
}

//ejecución de ordenes separadas por ';', si no lo contiene
//se llama una sola vez a ejecutar_linea_ordenes 
void ejecutar_secuencia_ordenes(char *orden)
{
   // Duplicado de la cadena pata evitar modificaciones en la original	
   char *orden_copia = strdup(orden);
   char *delim;
   delim = ";";
	
   //División de la cadena en tokens, usando delim
   char *token = strtok(orden_copia, delim);
	
   //iteración a través de la cadena de tokens 
   while (token!= NULL)
   {
     ejecutar_linea_ordenes(token); //se llama a la función para cada token 
     token = strtok(NULL, delim); //obtención del siguiente token
   }

   free(orden_copia); //eliminar duplicado de la memoria

}

void ejecutar_linea_ordenes(const char *orden)
{
   char **ordenes;
   int nordenes = 0;
   ordenes = parser_pipes(orden, &nordenes);
	
   // Reserva memoria para almacenar lis pids de los procesos hijos 	
   pid_t *pids = (pid_t *)malloc(nordenes * sizeof(pid_t));
   
 // Manejo del error de asignación de memoria 
 if(pids == NULL)
   {
      perror("Error en el malloc para pids");
      exit(EXIT_FAILURE);
   }
	
   int **pipes;
   int backgr, entrada, salida;

   
   pipes = crear_pipes(nordenes);

   // para cada orden , invocar adecuadamente a ejecutar_orden
   for(int i = 0; i <= (nordenes - 1); i++)
   {
      if(i==0)
      {
	 //Primera orden: la entrada es el teclado y la salida la tubería hacia la siguiente orden       
         entrada = STDIN_FILENO;
         if(nordenes>1)
         {
            salida = pipes[0][1];
         }
         pids[0] = ejecutar_orden(ordenes[0], entrada, salida, &backgr);
         
      }
      else if((i==(nordenes - 1)) && (nordenes > 1))
      {
	 //Última orden : la entrada es la tubería desde la orden anterior y la salida es la pantalla    
         entrada = pipes[nordenes-2][0];

         pids[i] = ejecutar_orden(ordenes[i], entrada, salida, &backgr);
      }
      else
      {
	 //Órdenes intermedias: la entrada es la tubería desde la orden anterior y la salida es la tubería hacia la siguiente orden     
         entrada = pipes[i-1][0];
         salida = pipes[i][1];

         pids[i] = ejecutar_orden(ordenes[i], entrada, salida, &backgr);
      }
   }
	
   //Tratamiento de background para ordenes simples y compuestas	
   if ((backgr == 0) && (pids[nordenes-1] > 0))
   {
      // Si no hay ejecución en segundo plano, padre espera a que último hijo termine   
      waitpid(pids[nordenes-1], NULL, 0);
   }

   // liberar estructuras de datos dinámicas utilizadas
   free_ordenes_pipes(ordenes, pipes, nordenes);
   free(pids);

}
