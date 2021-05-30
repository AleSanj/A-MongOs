/*
 ============================================================================
 Name        : mi_ram_hq.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "mi_ram_hq.h"


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
		Patota* nuevaPatota;
		switch(paquete->codigo_operacion) {
		    /*case TRIPULANTE:
		    	tripulante = deserializar_patota(paquete->buffer);
		    	pthread_t hiloTripulante;
		    	pthread_create(hiloTripulante,NULL,(void*)dibujarTripulante, (tripulante,nivel));

		    	pthread_detach(hiloTripulante);
		    	serializar_string("ok", socketCliente);
		    	free(tripulante);
		        break;
		      */  // Evaluamos los demás casos según corresponda
		    case INICIOPATOTA:
		    	nuevaPatota = deserializar_tareas_patota(paquete->buffer);
		    	PCB* pcb;
		    	pcb ->	pid = nuevaPatota -> idPatota;
		    	uint32_t* tareas = procesar_archivo(nuevaPatota -> Tareas);
		    	pcb -> tareas = tareas;
		    	//Guardar PCB en memoria
		    	uint32_t punteroPCB;
		    	for(int i = 0;i < nuevaPatota -> cantTripulantes;i++){
		    		recv(socketCliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0);
		    		recv(socketCliente, &(paquete->buffer->size), sizeof(uint32_t), 0);
		    		paquete->buffer->stream = malloc(paquete->buffer->size);
		    		recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);
		    		tripulante = deserializar_tripulante(paquete->buffer);
		    		pthread_t hiloTripulante;
		    		pthread_create(hiloTripulante, NULL, (void*)administrar_tripulante, (tripulante,nivel,punteroPCB));

		    		pthread_join(hiloTripulante);
		    	}
		    	break;

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

int procesar_archivo(FILE* archivoTareas){
	archivoTareas = fopen("Putito.txt","r");

	if(archivoTareas == NULL){
		printf("Error! No se pudo abrir el archivo\n");
		exit(-1);
	}

	char tarea[30];
	while (fgets(tarea, 30, archivoTareas) != NULL){
		//Guardar en memoria
	}
	fclose(archivoTareas);
	//return posicionDeMemoriaDePrimerTarea
}

void administrar_tripulante(Tripulante tripulante, NIVEL* nivel,uint32_t* pcb){
	TCB* tcb = crearTCB(tripulante,pcb);
	int socketClienteTripulante, socketServerTripulante;
	char* puerto[4];
	itoa(PUERTOPREDETERMINADO + tripulante -> id,puerto,10)
	socketServerTripulante = crear_server(puerto,"127.0.0.1",5);
	dibujarTripulante(tripulante,nivel);
	while(1){
		socketClienteTripulante = esperar_cliente(socketServerTripulante);
		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));

		// Primero recibimos el codigo de operacion
		recv(socketClienteTripulante, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

		// Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
		recv(socketClienteTripulante, &(paquete->buffer->size), sizeof(uint32_t), 0);
		paquete->buffer->stream = malloc(paquete->buffer->size);
		recv(socketClienteTripulante, paquete->buffer->stream, paquete->buffer->size, 0);

		// Ahora en función del código recibido procedemos a deserializar el resto
		switch (paquete -> codigo_operacion){
			case PEDIRTAREA:
				break;
			case ACTUALIZAR_POS:
				break;
			case ELIMINAR_TRIPULANTE:
				break;
			case ACTUALIZAR_ESTADO:
				break;
		}
	}

}

TCB crearTCB(Tripulante tripulante, uint32_t* pcb){
	TCB* tcb;
	tcb -> tid = tripulante -> id;
	tcb -> posx = tripulante -> posicionX;
	tcb -> posy = tripulante -> posicionY;
	tcb -> estado = tripulante -> estado;
	tcb ->puntero_pcb = pcb;

	return tcb;
}
