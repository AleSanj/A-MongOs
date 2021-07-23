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
	int socketCliente, socketServer;
	bool terminar;
	config = config_create("/home/utnso/Escritorio/tp-2021-1c-Cebollitas-subcampeon/mi_ram_hq/src/mi_ram_hq.config");
	tamMemoria = config_get_int_value(config, "TAMANIO_MEMORIA");
	esquemaMemoria = config_get_string_value(config, "ESQUEMA_MEMORIA");
	tamPagina = config_get_int_value(config, "TAMANIO_PAGINA");
	tamSwap = config_get_int_value(config, "TAMANIO_SWAP");
	path_swap = config_get_string_value(config, "PATH_SWAP");
	alg_remplazo = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	crit_seleccion = config_get_string_value(config, "CRITERIO_SELECCION");
	puerto = config_get_string_value(config, "PUERTO");
	pthread_mutex_init(&mutexMemoria, NULL);
	for(int i =0; i<94;i++){
		if (vectorIdTripulantes[i]==0){
			vectorIdTripulantes[i] = 0;
		}
	}

	if (strcmp(crit_seleccion,"FF") == 0){
		tipoDeGuardado = FIRSTFIT;
	}else{
		tipoDeGuardado = BESTFIT;
	}

	memoria = malloc(tamMemoria);
	listaElementos = list_create();

	if (strcmp(esquemaMemoria,"PAGINACION")==0){
		puts("entro al esquema paginacion");
		memoriaSwap = malloc(tamSwap);
		cantidadPaginas = tamMemoria / tamPagina;
		cantidadPaginasSwap = tamSwap / tamPagina;
		bitarrayMemoria = calloc(cantidadPaginas, sizeof(int));
		bitarraySwap = calloc(cantidadPaginasSwap, sizeof(int));
		listaDeTablasDePaginas = list_create();
		tablaDeFrames = queue_create();
		punteroReemplazo = 0;

	}else if (strcmp(esquemaMemoria,"SEGMENTACION")==0){
		listaSegmentos = list_create();

	}
	nivel = crear_mapa();

	socketServer = crear_server(puerto);
	while (1) {
		socketCliente = esperar_cliente(socketServer, 5);
		if (socketCliente == -1)
			continue;
			pthread_t hiloCliente;
			pthread_create(&hiloCliente,NULL,(void*)administrar_cliente,socketCliente);
			pthread_join(hiloCliente,NULL);

	}


	liberar_conexion(socketServer);
	//crear_personajes(nivel, patota);
	return EXIT_SUCCESS;
	}







void administrar_cliente(int socketCliente){
	int respuesta;
		t_paquete* paquete_recibido = recibir_paquete(socketCliente, &respuesta); // @suppress("Type cannot be resolved")

		if (paquete_recibido->codigo_operacion == -1 || respuesta == ERROR) {
			liberar_conexion(socketCliente);
			eliminar_paquete(paquete_recibido);
		}

		//printf("PAQUETE DE TIPO %d RECIBIDO\n",paquete_recibido->codigo_operacion);

	switch(paquete_recibido->codigo_operacion) {
		case INICIAR_PATOTA:;
			//printf("Inicio una patota /n");
			t_iniciar_patota* estructura_iniciar_patota = deserializar_iniciar_patota(paquete_recibido);
			//imprimir_paquete_iniciar_patota(estructura_iniciar_patota);
			pcb* nuevaPatota = malloc(sizeof(pcb));
			nuevaPatota->id = estructura_iniciar_patota->idPatota;
			if (strcmp(esquemaMemoria,"PAGINACION")==0){
				tablaEnLista_struct *nuevaTablaPatota = malloc(sizeof(tablaEnLista_struct));
				nuevaTablaPatota->idPatota = nuevaPatota->id;
				nuevaTablaPatota->tablaDePaginas = list_create();
				list_add(listaDeTablasDePaginas, nuevaTablaPatota);
			}
			guardar_en_memoria_general(estructura_iniciar_patota->Tareas,estructura_iniciar_patota->idPatota,estructura_iniciar_patota->tamanio_tareas,estructura_iniciar_patota->idPatota,'A');

			nuevaPatota->tareas =  calcular_direccion_logica_archivo(estructura_iniciar_patota->idPatota);

			guardar_en_memoria_general(nuevaPatota,estructura_iniciar_patota->idPatota,estructura_iniciar_patota->tamanio_tareas,estructura_iniciar_patota->idPatota,'A');
			break;
		case TRIPULANTE:;
			//printf("CASE TRIPULANTE /n");
				t_tripulante* estructura_tripulante = deserializar_tripulante(paquete_recibido);
				tcb *nuevoTripulante = malloc(sizeof(tcb));
				nuevoTripulante->id = estructura_tripulante->id_tripulante;
				nuevoTripulante->estado = 'R';
				nuevoTripulante->posX = estructura_tripulante->posicion_x;
				nuevoTripulante->posY = estructura_tripulante->posicion_y;
				nuevoTripulante->proxTarea=0;
				nuevoTripulante->dirLogicaPcb=calcular_direccion_logica_patota(estructura_iniciar_patota->idPatota);
				guardar_en_memoria_general(nuevoTripulante,estructura_tripulante->id_tripulante,21,estructura_tripulante->id_patota,'T');
				for(int i =0; i<94;i++){
					if (vectorIdTripulantes[i]==0){
						//printf("Dentro del if i= %d \n",i);
						vectorIdTripulantes[i] = nuevoTripulante->id;
						dibujarTripulante(nuevoTripulante,(i+33));
						break;
					}
				}


		//		liberar_tripulante(estructura_tripulante);
				liberar_conexion(socketCliente);
				break;
		case ELIMINAR_TRIPULANTE:;
			//printf("eliminar TRIPULANTE /n");
				t_tripulante* tripulante_a_eliminar = deserializar_eliminar_tripulante(paquete_recibido);
				borrar_de_memoria_general(tripulante_a_eliminar->id_tripulante, tripulante_a_eliminar->id_patota, 'T');
				for(int i =0; i<94;i++){
					if (vectorIdTripulantes[i]==tripulante_a_eliminar->id_tripulante){
						vectorIdTripulantes[i] = 0;
						borrarTripulante((i+33));
						break;
					}
				}

					break;

		case PEDIR_TAREA:;
			//printf("PEDIR TAREA /n");
				t_tripulante* tripulante_solicitud = deserializar_tripulante(paquete_recibido);
				imprimir_paquete_tripulante(tripulante_solicitud);
				char*tareas = string_new();
				tareas = (char*)buscar_en_memoria_general(tripulante_solicitud->id_patota,tripulante_solicitud->id_patota,'A');
				char **arrayTareas = string_split(tareas,"|");
				tcb *tripulanteATraer = malloc(sizeof(tcb));
				int totalDeTareas=0;
				tripulanteATraer = buscar_en_memoria_general(tripulante_solicitud->id_patota,tripulante_solicitud->id_patota,'T');
				int tamanio_tarea = strlen(arrayTareas[tripulanteATraer->proxTarea])+1;
				int i=0;
				while(arrayTareas[i]!=NULL){
					totalDeTareas++;
					i++;
				}
				if(tripulanteATraer->proxTarea==totalDeTareas-1){
					puts("ENTRO AL IF");
					send(socketCliente,6,sizeof(uint32_t),0);
					send(socketCliente, "fault",tamanio_tarea,0);
				}else{
					puts("ENTRO AL ELSE");
					send(socketCliente, &tamanio_tarea,sizeof(uint32_t),0);
					send(socketCliente, arrayTareas[tripulanteATraer->proxTarea],tamanio_tarea,0);
				}

				liberar_conexion(socketCliente);

				break;


		case ACTUALIZAR_POS:;
			//printf("ACTUALIZAR POS/n");
				t_tripulante* tripulante_a_mover = deserializar_tripulante(paquete_recibido);
				imprimir_paquete_tripulante(tripulante_a_mover);
				tcb *tripulanteAMover = malloc(sizeof(tcb));
				id_and_pos *tripulanteEnMapa = malloc(sizeof(id_and_pos));

				tripulanteAMover = buscar_en_memoria_general(tripulante_a_mover->id_tripulante,tripulante_a_mover->id_patota, 'T');
				borrar_de_memoria_general(tripulante_a_mover->id_tripulante,tripulante_a_mover->id_patota, 'T');
				tripulanteAMover->posX = tripulante_a_mover->posicion_x;
				tripulanteAMover->posY = tripulante_a_mover->posicion_y;
				tripulanteEnMapa->idTripulante = tripulanteAMover->id;
				tripulanteEnMapa->posX = tripulanteAMover->posX;
				tripulanteEnMapa->posY = tripulanteAMover->posY;

				for(int i =0; i<94;i++){
					if (vectorIdTripulantes[i]==tripulanteEnMapa->idTripulante){
						actualizarPosicion(tripulanteAMover,i+33);
						break;
					}
				}
				guardar_en_memoria_general(tripulanteAMover,tripulante_a_mover->id_tripulante,21,tripulante_a_mover->id_patota,'T');

				break;

		case ACTUALIZAR_ESTADO:;
			////printf("ACTUALIZAR_ESTADO /n");
				t_cambio_estado* tripulante_a_actualizar = deserializar_cambio_estado(paquete_recibido);
				imprimir_paquete_cambio_estado(tripulante_a_actualizar);
				tcb *tripulanteAActualizar = malloc(sizeof(tcb));
				tripulanteAActualizar = buscar_en_memoria_general(tripulante_a_actualizar->id_tripulante,tripulante_a_actualizar->id_patota, 'T');
				borrar_de_memoria_general(tripulante_a_actualizar->id_tripulante,tripulante_a_actualizar->id_patota, 'T');
				tripulanteAActualizar->estado = tripulante_a_actualizar->estado;
				guardar_en_memoria_general(tripulanteAActualizar,tripulante_a_actualizar->id_tripulante,21,tripulante_a_actualizar->id_patota,'T');
				break;

		default:;
				////printf("No se especifico el caso para recibir el paquete tipo: %d\n",paquete_recibido->codigo_operacion);
				break;


	}
}

char intAChar(int numero){
	return numero + '0';
}

void actualizarPosicion(id_and_pos* nuevaPos,char id){

	item_mover(nivel, id, nuevaPos-> posX,nuevaPos->posY);
}


void dibujarTripulante(tcb* tripulante, char id){
	int err;
	//printf("el id en dibu tripu es: %c",id);
	//char* id[3] = '0';
	//char id = intAChar(tripulante->id);
	err = personaje_crear(nivel, id, tripulante->posX, tripulante->posY);
	ASSERT_CREATE(nivel, 'M', err);

	if(err) {
		//printf("Error: %s\n", nivel_gui_string_error(err));
	}
	nivel_gui_dibujar(nivel);

	//free (id);
}
void borrarTripulante( char id){
	item_borrar(nivel, id);
}

NIVEL *crear_mapa(){
		NIVEL *nivel;
		int cols, rows;
		int err;

		nivel_gui_inicializar();

		nivel_gui_get_area_nivel(&cols, &rows);

		nivel = nivel_crear("Test Chamber 04");
		nivel_gui_dibujar(nivel);
		//printf("Ya Dibuje \n");
		return nivel;
}

