/** 
* @file userHandler.c
* @brief Archivo que permite el ingreso por parte del usuario del tiempo a setear en el timer de la Raspberry PI 3 Model B.
* @author Kleiner Matías, López Gastón.
*/


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


#define TAMANIOMAXIMO 100
extern int errno ;

static char cadena_tiempo[TAMANIOMAXIMO];     /* Buffer de la palabra encriptada por el módulo del kernel. */
static int test = 1; /* Variable entera para testing. */



int main () 
{
	printf ("\nInicio del programa.\n");
	


	/* Solicitar cadena a usuario. */
	char cadena_original[TAMANIOMAXIMO];
	printf ("\nIntroducir tiempo: ");
	fgets (cadena_original, TAMANIOMAXIMO, stdin);
	printf ("\nLa cadena introducida fue: %s\n",cadena_original);
	

	printf ("Comienzo de la interaccion con el modulo.\n");
   
   	int valorRetornado_Apertura = open("/dev/SOPEncriptador", O_RDWR);  /* Apertura el dispositivo. Permisos de lectura y escritura. */
  	if (valorRetornado_Apertura < 0){ 
      	perror("No se pudo abrir el dispositivo.");
      	return errno;
   	}
   
   	int valorRetornado_Escritura = write(valorRetornado_Apertura, cadenaOriginal, strlen(cadenaOriginal)); // Envio la palabra introducida por el usuario al encriptador.
   	if (valorRetornado_Escritura < 0){
      	perror("No se pudo escribir la palabra en el dispositivo.");
      	return errno;
  	}

   
	int valorRetornado_Lectura = read(valorRetornado_Apertura, cadenaEncriptada, TAMANIOMAXIMO);  // Leo la palabra cifrada por el dispositivo.
	 if ( valorRetornado_Lectura < 0){
	       perror("No se pudo leer la palabra encriptada del dispositivo.");
	       return errno;
	 }

    
    

	printf ("\nFin del programa.\n");

	
	return 0;
}
