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

static char cadena_original[TAMANIOMAXIMO];


int main ()
{
	printf ("\nInicio del programa.\n");

	

	/* Solicitar cadena a usuario. */
	printf ("\nIntroducir tiempo (milisegundos): ");
	fgets (cadena_original, TAMANIOMAXIMO, stdin);
	printf ("\nLa cadena introducida fue: %s\n",cadena_original);

	int tiempo;
	sscanf (cadena_original, "%d", &tiempo);

	printf ("El tiempo ingresado es: %d\n", tiempo);

	if ( (tiempo > 0) & (tiempo < 60001) )
	{
		char mensaje_rasp[TAMANIOMAXIMO];
		printf ("Comienzo de la interaccion con el modulo.\n");

		int valor_retornado_apertura = open ("/dev/Timer_Rasp", O_RDWR);   /* Apertura del dispositivo. Permisos de lectura y escritura. */
	  if (valor_retornado_apertura < 0)
		{
				perror ("No se pudo abrir el dispositivo.");
				return errno;
		}

		int valor_retornado_escritura = write (valor_retornado_apertura, cadena_original, strlen (cadena_original)); /* Envio el tiempo introducido por el usuario al modulo. */
		if (valor_retornado_escritura < 0)
		{
				perror ("No se pudo escribir la palabra en el dispositivo.");
				return errno;
		}


		int valor_retornado_lectura = read (valor_retornado_apertura, mensaje_rasp, TAMANIOMAXIMO);  /* Lectura de la notificacion del modulo. */
		 if ( valor_retornado_lectura < 0)
		 {
					 perror ("No se pudo leer la palabra encriptada del dispositivo.");
					 return errno;
		 }


		 int valor_retornado_cierre = close (valor_retornado_apertura);   /* Cierre del dispositivo. Permisos de lectura y escritura. */
	  		if (valor_retornado_cierre < 0)
		{
				perror ("No se pudo cerrar el dispositivo.");
				return errno;
		}
	 }

	 else
	 {
		 printf ("Error. Tiempo fuera de los limites.\n");
	 }

	printf ("\nFin del programa.\n");


	return 0;
}
