/*
 ============================================================================
 Name        : mi_ram_hq.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <conexion.h>
#include <serializacion.c>
#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include <curses.h>
#include <sys/socket.h>
#include "TAD_TRIPULANTE.h"
#include "TAD_PATOTA.h"
#include <commons/collections/list.h>
#include <pthread.h>

#define ASSERT_CREATE(nivel, id, err)

int main(void) {
	//crear_server("6667","127.0.0.1",5);
	//ejemplo();
	int socketCliente, socketServer;
	bool terminar;
	NIVEL* nivel = crear_mapa();
	socketServer = crear_server("6667","127.0.0.1",5);
	while(1){
		socketCliente = esperar_cliente(socketServer);



		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));

		// Primero recibimos el codigo de operacion
		recv(socketCliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

		// Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
		recv(socketCliente, &(paquete->buffer->size), sizeof(uint32_t), 0);
		paquete->buffer->stream = malloc(paquete->buffer->size);
		recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);

		// Ahora en función del código recibido procedemos a deserializar el resto
		Tripulante* tripulante;
		switch(paquete->codigo_operacion) {
		    case TRIPULANTE:
		    	tripulante = deserializar_patota(paquete->buffer);
		    	pthread_t hiloTripulante;
		    	pthread_create(hiloTripulante,NULL,(void*)dibujarTripulante, (tripulante,nivel));

		    	pthread_detach(hiloTripulante);
		    	serializar_string("ok", socketCliente);
		    	free(tripulante);
		        break;
		        // Evaluamos los demás casos según corresponda
		}








		terminar_servidor(socketServer);


	}

	//crear_personajes(nivel, patota);
	return EXIT_SUCCESS;
}



void dibujarTripulante(Tripulante* tripulante, NIVEL* nivel){

}

int crear_mapa(){
		NIVEL* nivel;

		int cols, rows;
		int err;

		nivel_gui_inicializar();

		nivel_gui_get_area_nivel(&cols, &rows);

		nivel = nivel_crear("Test Chamber 04");
		nivel_gui_dibujar(nivel);
		return nivel;
		//Aca hacemos un while de 0 hasta la cantidad de tripulantes y los creamos/dibujamos


	}

