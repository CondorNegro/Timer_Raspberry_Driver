#Makefile TP3. Sistemas de Computación. López Gastón. Kleiner Matías.


obj-m+= archivos_compilacion/modulo.o
KVERSION = $(shell uname -r)

userHandler: userHandler.c
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) modules
	gcc -Wall -Werror -pedantic -o userHandlerEx userHandler.c 
    # -Wall nos mostrará todos los avisos que produzca el compilador, no solamente  los errores. Los avisos nos indican dónde y/o porqué podría surgir algún error en nuestro programa.
	# -pedantic nos aporta más información sobre los errores y los avisos mostrados por GCC.
	# -Werror tratara a cualquier warning como si un error se tratase. 
 
	
clean:
	make -C /lib/modules/$(KVERSION)/build/ M=$(PWD) clean 
	rm -f $(obj) userHandlerEx

