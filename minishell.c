#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "internas.h"
#include "entrada_minishell.h"
#include "ejecutar.h"

/*
Este método manea la señal sig_child. La señal sigchlid se utiliza para noticiar al
proceso padre que uno de sus hijos ha terminado. Con el método waitpid manejamos esta señal.
La función waitpid es una llamada al sistema que espera que un proceso hijo termine,
almacenando su estado. 
*/
static void manejar_sigchild(int signo){

   int estado;
   /*
   El -1 indica que espera a cualquier proceso hijo. 
   &estado indica dónde se va a guardar el estado del proceso hijo. 
   WHONHANG hace que la función no se bloquee esperando a que termine procesos hijos. 
   Si se terminan procejos hijos, se va devolviendo el PID de estos. Si no terminan, 
   devuelve  0. 
   */
   waitpid(-1, &estado, WNOHANG);
}
int main(int argc, char *argv[])
{

   char buf[BUFSIZ];
   //La estructura sigaction es un manejador de señales.
   struct sigaction sa;

   memset(&sa, 0, sizeof(sa));//Establece todos los valores de sa a 0.
   /*
   sa_handler es un puntero que apunta a la función que maneje la señal. 
   En nuestro caso, a manejar_sigchild
   */
   sa.sa_handler = manejar_sigchild;
   /*
   sa_flags es un  int que contiene valores que controlan el comportamiento del 
   manejo se señales.
   SA_NOCLDSTOP evita que se envíen señales sigchild cuando el proceso se bloquea
   o continua. Es decir, sólo se envían señales cuando el proceso hijo muere. 
   SA_RESTART hace que las llamas al sistema interrumpidas por sigchild se
   reinicien automáticamente. 
   Aquí se hace una operación or por lo que se guarda el valor correspondiente 
   a tener las dos opciones activas. 
   */
   sa.sa_flags = SA_NOCLDSTOP | SA_RESTART;
   /*
   Establece la estructura de sigaction como un manejador de señales sigchld. 
   En el tercer campo, se podría incluir un puntero para guardar el manejador anterior. 
   En este caso, no nos interesa. 
   */
   sigaction(SIGCHLD, &sa, NULL);

   while (1)
   {
      imprimir_prompt();
      leer_linea_ordenes(buf);
      if (strcmp(buf, "exit") == 0)
      {
         break;
      }
      else
      {
         if (es_ord_interna(buf))
         {
            ejecutar_ord_interna(buf);
         }
         else
         {
            ejecutar_secuencia_ordenes(buf);
         }
      }
   }
   exit(EXIT_SUCCESS);
}