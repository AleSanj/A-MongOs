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

NIVEL* nivel;

int main(void) {
	int socketCliente, socketServer;
	bool terminar;
	Tripulante* tripulante = {8 ,5 ,10 ,'Nuevo' ,32 ,'Tarea' ,10 ,15};
	nivel = crear_mapa();

	socketServer = crear_server("6667","127.0.0.1",5);
	while(1){
		socketCliente = esperar_cliente(socketServer);
		pthread_t hiloCliente;
		pthread_create(hiloCliente,NULL,(void*)administrar_cliente,socketCliente);
		pthread_join(hiloCliente,NULL);


		terminar_servidor(socketServer);


	}

	//crear_personajes(nivel, patota);
	return EXIT_SUCCESS;
}


void dibujarTripulante(Tripulante* tripulante){
	int err;
	//char* id[3] = '0';
	char id = intAChar(tripulante->id);
	err = personaje_crear(nivel, id, 10, 10);
	ASSERT_CREATE(nivel, id, err);

	if(err) {
		printf("Error: %s\n", nivel_gui_string_error(err));
	}

	//free (id);
}

int crear_mapa(){
		NIVEL* nivel;

		int cols, rows;
		int err;

		nivel_gui_inicializar();

		nivel_gui_get_area_nivel(&cols, &rows);

		nivel = nivel_crear("Test Chamber 04");
		nivel_gui_dibujar(nivel);
		printf("Ya Dibuje \n");
		return nivel;
}



int procesar_archivo(FILE* archivoTareas){
	archivoTareas = fopen("Tareas.txt","r");

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

void administrar_cliente(int socketCliente){

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	// Primero recibimos el codigo de operacion
	recv(socketCliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

	// Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
	recv(socketCliente, &(paquete->buffer->size), sizeof(uint32_t), 0);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);

	// Ahora en función del código recibido procedemos a deserializar el resto


	switch(paquete->codigo_operacion) {
		case INICIOPATOTA:;
			Patota* nuevaPatota;
			nuevaPatota = deserializar_tareas_patota(paquete->buffer);
	    	PCB* pcb;
	    	pcb ->	pid = nuevaPatota -> idPatota;
	    	uint32_t* tareas = procesar_archivo(nuevaPatota -> Tareas);
	    	pcb -> tareas = tareas;
	    	//Guardar PCB en memoria
	    	uint32_t* punteroPCB;
	    	for(int i = 0;i < nuevaPatota -> cantTripulantes;i++){
	    		recv(socketCliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0);
	    		recv(socketCliente, &(paquete->buffer->size), sizeof(uint32_t), 0);
	    		paquete->buffer->stream = malloc(paquete->buffer->size);
	    		recv(socketCliente, paquete->buffer->stream, paquete->buffer->size, 0);
	    		Tripulante* tripulante;
	    		tripulante = deserializar_tripulante(paquete->buffer);
	    		iniciar_tripulante(tripulante, pcb);
	    	}
	    	break;
		case ACTUALIZAR_POS:;

			id_and_pos* nuevaPos;
			nuevaPos = deserializar_id_and_pos(paquete->buffer);
			actualizarPosicion(nuevaPos, nivel);
			break;
		case PEDIRTAREA:
			break;
		case ELIMINAR_TRIPULANTE:
			break;
		case ACTUALIZAR_ESTADO:
			break;

	}
}
char intAChar(int numero){
	return numero + '0';
}

void actualizarPosicion(id_and_pos* nuevaPos, NIVEL* nivel){
	char idAMover = intAChar(nuevaPos->idTripulante);
	item_mover(nivel, idAMover, nuevaPos-> posX,nuevaPos->posY);
}



void iniciar_tripulante(Tripulante* tripulante, uint32_t* pcb){
	TCB* tcb = crearTCB(tripulante,pcb);
	dibujarTripulante(tripulante);
}

TCB* crearTCB(Tripulante* tripulante, uint32_t* pcb){
	TCB* tcb;
	tcb -> tid = tripulante -> id;
	tcb -> posx = tripulante -> posicionX;
	tcb -> posy = tripulante -> posicionY;
	tcb -> estado = tripulante -> estado;
	tcb ->puntero_pcb = pcb;

	return tcb;
}
