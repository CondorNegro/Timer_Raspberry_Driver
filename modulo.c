/**
 * @file  modulo.c
 * @brief Módulo del Trabajo Práctico 3 de Sistemas de Computación 2018.
 * @author López Gastón - Kleiner Matías
 */


#include <linux/init.h>           			/* Macros para funciones __init __exit. */
#include <linux/module.h>         			/* Header para agregar module al kernel. */
#include <linux/device.h>         			/* Header para Modelo de Driver. */
#include <linux/kernel.h>         			/* Header con variables y funciones del kernel. */
#include <linux/fs.h>             			/* Header for the Linux file system support. */
#include <asm/uaccess.h>          			/* Para uso de funcion copy_to_user. */
#include <linux/uaccess.h>          		/* Por cuestiones de compatibilidad entre diferentes versiones de Linux. */
#define  DEVICE_NAME "Timer_Rasp"   		/* Nombre de device en /dev/DEVICE_NAME */
#define  CLASS_NAME  "siscomp"        	/* Clase de device. Nombre del char driver. */
#define  MODULE_NAME "MODULO_TimerRasp"	/* Nombre del Módulo del Kernel. Usado para printk. */
#define TAMANIOMAXIMO 100           		/* Máximo tamaño de buffers. */

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("Lopez-Kleiner");    	/* Autor. Se puede ver en modinfo */

/* Descripción del módulo. */
MODULE_DESCRIPTION ("Modulo Timer Raspberry. Trabajo Practico 3 Sistemas de Computacion");

static int    number_device;                  					/* Número de device */
static char   cadena_original[TAMANIOMAXIMO] = {0};   	/* Variable con el string pasado por el usuario */
static short  size_of_cadena_original;             	 	/* Tamaño del string del usuario */
static int    number_device_opens = 0;              		/* Contador de módulos encriptadores abiertos. */
static struct class*  class_modulo  = NULL; 			/* Puntero a la estructura class */
static struct device* dispositivo_modulo = NULL; 	/* Puntero a la estructura Device */

static int     dev_open (struct inode *, struct file *);
static int     dev_release (struct inode *, struct file *);
static ssize_t dev_read (struct file *, char *, size_t, loff_t *);
static ssize_t dev_write (struct file *, const char *, size_t, loff_t *);

/* Funciones que implementa el char device del módulo (open, read, write y release) */

static struct file_operations fops = {
   .open = dev_open,
   .release = dev_release,
   .read = dev_read,
   .write = dev_write,
};


/**
 *  @brief	Función de inicialización. Crea el char device. Crea el driver.
  				Asignación dinámica de un número de dispositivo.
					Registración de la clase.
					Registración del dispositivo.
 *  @return 0 si se completo exitosamente.
 */
static int __init inicializacion_modulo (void)
{

	 printk (KERN_INFO "%s : Inicializando el modulo: %s LKM\n", MODULE_NAME, MODULE_NAME);

   /* Trata de asignar dinámicamente un number_device para el dispositivo. */
   number_device = register_chrdev (0, DEVICE_NAME, &fops);
   if (number_device < 0)
	 {
			printk (KERN_ALERT "Fallo en la asignacion %d (number_device) \n", number_device);
      return number_device;
   }
   printk (KERN_INFO "%s: Se ha registrado correctamente con el numero de dispositivo: %d\n", MODULE_NAME, number_device);


   /* Registra la clase. */
   class_modulo = class_create (THIS_MODULE, CLASS_NAME);
   if (IS_ERR (class_modulo)) /* Tratamiento de errores */
	 {
      unregister_chrdev (number_device, DEVICE_NAME);
      printk (KERN_ALERT "Fallo el registro de la clase %s \n", MODULE_NAME);
      return PTR_ERR (class_modulo); /* Macro para devolver el error sobre un puntero. */
   }
   printk(KERN_INFO "%s: La clase se registro correctamente \n", MODULE_NAME);

   /* Registra el dispositivo. */
   dispositivo_modulo = device_create(class_modulo, NULL, MKDEV(number_device, 0), NULL, DEVICE_NAME);
   /* MKDEV para rertornar un tipo de dato dev_t. */
   if (IS_ERR(dispositivo_modulo)) /* Tratamiento de error. */
	 {
      class_destroy (class_modulo);		/* En caso de error, destruye la clase */
      unregister_chrdev (number_device, DEVICE_NAME);
      printk (KERN_ALERT "%s: Fallo en la creacion del dispositivo\n", MODULE_NAME);
      return PTR_ERR (dispositivo_modulo);
   }
   printk (KERN_INFO "%s: El dispositivo encriptador fue creado exitosamente\n", MODULE_NAME); /* El dispositivo es inicializado. */
   return 0;
}

/**
 *  @brief Función de finalización del módulo. Remueve el driver creado.
 						Elimina el dispositivo de caracter. Desregistra la clase dispositivo.
						Elimina la clase dispositivo. Libera el número de dispositivo. (Major).
 */
static void __exit finalizacion_modulo(void){
   device_destroy (class_modulo, MKDEV(number_device, 0)); /* Elimina el dispositivo. */
   class_unregister (class_modulo);                        /* Desregistra la clase dispositivo. */
   class_destroy (class_modulo);                           /* Elimina la clase dispositivo. */
   unregister_chrdev (number_device, DEVICE_NAME);         /* Libera el numberDevice. (Major). */
   printk (KERN_INFO "%s: Finalizacion del linux kernel module\n", MODULE_NAME);
}



/**
 *  @brief Función que se ejecuta cuando se llama al char device.
 					Incrementa el contador de módulos de este TP abiertos.
 *  @param inodep  (struct inode*) Puntero a un inodo.
 *  @param filep  (struct file *) Puntero a una file structure.
 *  @return 0 si se pudo abrir correctamente el char device. -16 en caso de ya tener un char device abierto.
 */
static int dev_open(struct inode *inodep, struct file *filep){
   if (number_device_opens)
	 {
      return -EBUSY; /* -16 */
   }
   number_device_opens++;
   printk (KERN_INFO "%s: Se abrio el dispositivo timer (modulo): %d veces \n", MODULE_NAME, number_device_opens);
   try_module_get (THIS_MODULE); /* Abrir de forma segura. (Basado principalmente en contadores de uso del módulo). */
   return 0;
}



/**
 *  @brief Función que muestra al usuario la notificación enviado cuando el timer finaliza.
 *  @param filep  (struct file *) Puntero a una file structure.
 *  @param buffer (char *) Puntero al buffer donde se escribirá la información leída.
 *  @param len    (size_t) Tamaño del buffer.
 *  @param offset (loff_t *) Desplazamiento dentro del file.
 *  @return ssize_t 	El tamaño en bytes de la cadena de caracteres enviados en caso exitoso. -14 en caso de error.
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){


   int contador_errores = 0;

	 /* Se usa la función copy_to_user por los permisos que se requieren para acceder a espacio de memoria del proceso de usuario. */
   contador_errores = copy_to_user (buffer, cadena_original, size_of_cadena_original);


   if (contador_errores == 0) /* Tratamiento de errores */
	 {
      printk (KERN_INFO "%s: Se enviaron %d caracteres al usuario\n", MODULE_NAME, size_of_cadena_original);
      return size_of_cadena_original;
   }

   else
	 {
      printk (KERN_INFO "%s: Error en %d caracteres enviados al usuario\n", MODULE_NAME, contador_errores);
      return -EFAULT; /* -14 */
   }

}

/**
 *  @brief Función que permite el ingreso del tiempo a setear al timer de la Raspberry PI por parte del usuario.
 *  @param filep  (struct file *) Puntero a una file structure.
 *  @param buffer (const char *) Puntero al buffer que contiene el tiempo a setear al timer.
 *  @param len    (size_t) Tamaño del buffer.
 *  @param offset (loff_t *) Desplazamiento dentro del file.
 *  @return ssize_t 	Parametro len en caso de escritura exitosa. -14 en caso de error en la escritura.
 */
static ssize_t dev_write (struct file *filep, const char *buffer, size_t len, loff_t *offset)
{

   char auxBuffer[TAMANIOMAXIMO] = {0};

	 /* Se usa la función copy_from_user por los permisos que se requieren para acceder a espacio de memoria del proceso de usuario. */
   int errorManagement=copy_from_user(auxBuffer, buffer, len);

	 /* errorManagement, en caso de error, posee como valor la cantidad de bytes no copiados. */
   if(errorManagement != 0)
	 {
       printk(KERN_INFO "%s: ERROR. %d caracteres no encriptados.\n", MODULE_NAME, errorManagement);
       return -EFAULT; //-14
   }

   sprintf(cadena_original, "%s", auxBuffer);   /* Para formatear como string */
   size_of_cadena_original = strlen(cadena_original);

   printk(KERN_INFO "%s: Se recibieron %zu caracteres del usuario.\n", MODULE_NAME, len);
   return len;
}


/**
 *  @brief Función que se llama al cerrar el dispositivo desde el programa de usuario.
 				Decrementa el contador de módulos de este TP abiertos.
 *  @param inodep (struct inode *) Puntero a un Inodo.
 *  @param filep  (struct file *) Puntero a una file structure.
 *  @return int 0 en caso de cierre exitoso.
 */

static int dev_release (struct inode *inodep, struct file *filep)
{
   number_device_opens--;
   printk (KERN_INFO "%s: Dispositivo cerrado correctamente.\n", MODULE_NAME);
   module_put (THIS_MODULE); /* Cerrar de forma segura. (Basado principalmente en contadores).
	 														Evita cerrar un dispositivo mientras está en uso, por ejemplo,
															transmitiendo a un proceso de usuario un conjunto de caracteres. */
   return 0;
}

module_init (inicializacion_modulo);
module_exit (finalizacion_modulo);
