/*
 ============================================================================
 Name        : Discord.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/bitarray.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <TAD_TRIPULANTE.h>
#include <TAD_PATOTA.h>
#include <commons/config.h>
#include <pthread.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <semaphore.h>
#include <serializacion.h>
#include <conexion.h>
char* ipMiRam;
char* puertoMiRam;
t_config* config;
sem_t hilosEnEjecucion;
sem_t multiProcesamiento;
pthread_mutex_t mutexIO;
pthread_mutex_t listos;
pthread_mutex_t ejecutando;
pthread_mutex_t bloqueadosIo;
sem_t pararPlanificacion;
sem_t pararIo;
t_list* listaPatotas;
t_queue* ready;
t_list* execute;
t_queue* bloqueados;
char* ipMongoStore;
int* puertoMongoStore;
char* algoritmo;
int a = 0;
int TripulantesTotales = 0;
int multiProcesos;
int quantum;
int retardoCpu;

/*AVISOS PARROQUIALES

 * ESPERO SEAN DE TU AGRADO MIS COMENTARIOS
 * VAMOS QUE YA LO TENEMOS EL MODULO
 * FALTARIAN EL MANEJO DE STRINGS POR ESO HAY VARIABLES NO INICIALIZADAS
 * Y BUENO DESPUES EL ROUND ROBIN VIENDO EL VIDEO TENGO QUE CAMBIAR UNA COSA POR EL IO
 * Y BUENO SOLO FALTARIA VER EL TEMA DE LOS SABOTAJES DESPUES
 * ESPERO SEAN UTILES MIS COMENTARIOS
 */
// esta va a avanzar el tripulante paso a paso Y Enviar a miram
void* moverTripulante(Tripulante* tripu, int socket) {
	int tarea_x;
	int tarea_y;
	int ret = 0;
	id_and_pos actualizar;
	if (tarea_x > tripu->posicionX) {
		tripu->posicionX++;

		actualizar.idTripulante = tripu->id;
		actualizar.posX = tripu->posicionX;
		actualizar.posY = tripu->posicionY;
		serializar_id_and_pos(actualizar, socket);
		free(&actualizar);

		return ret;

	}
	if (tarea_x < tripu->posicionX) {
		tripu->posicionX--;
		actualizar.idTripulante = tripu->id;
		actualizar.posX = tripu->posicionX;
		actualizar.posY = tripu->posicionY;
		serializar_id_and_pos(actualizar, socket);
		free(&actualizar);

		return ret;
	}
	if (tarea_y < tripu->posicionY) {
		tripu->posicionX--;
		actualizar.idTripulante = tripu->id;
		actualizar.posX = tripu->posicionX;
		actualizar.posY = tripu->posicionY;
		serializar_id_and_pos(actualizar, socket);
		free(&actualizar);
		return ret;
	}
	if (tarea_y > tripu->posicionY) {
		tripu->posicionX++;
		actualizar.idTripulante = tripu->id;
		actualizar.posX = tripu->posicionX;
		actualizar.posY = tripu->posicionY;
		serializar_id_and_pos(actualizar, socket);
		free(&actualizar);

		return ret;

	}
	return NULL;
}

//esta funcion es porque no puedo aplicar parcialmente una funcion en c ):
void* identidad(void* t) {
	return t;
}
//esta para saber si es el mismo tripulante
bool esElMismoTripulante(Tripulante* rip, Tripulante* ra) {
	return (rip->id == ra->id);
}

void enviarMongoStore(Tripulante* enviar, int socketMongo) {
	//semaforo para parar ejecucion
	sem_wait(&pararIo);
	//envia tarea al MONGO STORE
	serializar_tarea(enviar->Tarea, socketMongo);
	//hace el wait pertinente sacado del string el wait
	int esperartarea;
	for (int espera = 0; espera < esperartarea; espera++) {
		sleep(retardoCpu);
	}
	//lo paso a cola ready
	pthread_mutex_lock(&listos);
	queue_push(ready, enviar);
	pthread_mutex_unlock(&listos);
	enviar->estado = "Ready";
	pthread_mutex_unlock(&mutexIO);
	sem_post(&pararIo);
}
void hacerTareaIO(Tripulante* io, int socketMongo) {
	//ACA ME PASE UN POQUITO CON LOS SEMAFOROS REVISARRRR
	pthread_mutex_lock(&bloqueadosIo);
	pthread_mutex_lock(&ejecutando);
	//ACA PUSHEO AL TRIPULANTE A LA COLA DE BLOQUEADOS IO
	queue_push(bloqueados,
			list_remove_by_condition(execute,
					(void*) esElMismoTripulante(io, (void*) identidad)));
	pthread_mutex_unlock(&bloqueadosIo);
	pthread_mutex_unlock(&ejecutando);
	//libero el recurso de multiprocesamiento porque me voy a io
	sem_post(&multiProcesamiento);
	io->Tarea = "Bloqueado IO";
	pthread_mutex_lock(&mutexIO);
	pthread_mutex_lock(&bloqueadosIo);
	//LO ENVIOPARA QUE HAGA SUS COSAS CON MONGOSTORE
	enviarMongoStore((void*) queue_pop(bloqueados), socketMongo);
	pthread_mutex_unlock(&bloqueadosIo);

}
void hacerFifo(Tripulante* tripu, int socket, int socketMongo) {
	//tu tarea es es transformar la taera de sstring en dos int posicion y un int de espera
	// mover al 15|65 20
	int tarea_x;
	int tarea_y;
	int wait;

	while (tripu->posicionX != tarea_x || tripu->posicionY != tarea_y) {
		//este es el semaforo para pausar laejecucion
		sem_wait(&hilosEnEjecucion);
		sleep(retardoCpu);
		moverTripulante(tripu, socket);

		//le tiroun post al semaforo que me permite frenar la ejecucion
		sem_post(&hilosEnEjecucion);

	}

	if (string_contains(tripu->Tarea, "CONSUMIR")
			|| string_contains(tripu->Tarea, "GENERAR")) {
		hacerTareaIO(tripu, socketMongo);
	} else {
		//una vez que llego donde tenia que llegar espera lo que tenia que esperar
		for (int cont = 0; cont < wait; cont++) {
			sem_wait(&hilosEnEjecucion);
			sleep(retardoCpu);
			sem_post(&hilosEnEjecucion);

		}
		//protejo las colas o listas
		pthread_mutex_lock(&listos);
		pthread_mutex_lock(&ejecutando);
		//termino su tarea lo agrego a ready
		queue_push(ready,
				list_remove_by_condition(execute,
						(void*) esElMismoTripulante(tripu, (void*) identidad)));
		pthread_mutex_unlock(&listos);
		pthread_mutex_unlock(&ejecutando);
		tripu->estado = "Ready";
		tripu->Tarea = "";
		//le aviso al semaforo que libere un recurso para que mande otro tripulante
		sem_post(&multiProcesamiento);
	}

}
void hacerRoundRobin(Tripulante* tripulant, int socket) {
	int tarea_x;
	int tarea_y;
	int contadorQuantum = 0;
	int espera;
	int contadorespera = 0;
	while (contadorQuantum < quantum) {
		if (tripulant->posicionX == tarea_x
				&& tripulant->posicionY == tarea_y) {
			break;
		}
		//este es el semaforo para pausar laejecucion
		sem_wait(&hilosEnEjecucion);
		sleep(retardoCpu);
		moverTripulante(tripulant, socket);
		contadorQuantum++;
		sem_post(&hilosEnEjecucion);
		//le tiroun post al semaforo que me permite frenar la ejecucion

	}
	while (contadorespera < espera && contadorQuantum < quantum) {
		sem_wait(&hilosEnEjecucion);
		sleep(retardoCpu);
		contadorespera++;
		contadorQuantum++;
		sem_post(&hilosEnEjecucion);
	}

	//protejo las colas o listas
	pthread_mutex_lock(&listos);
	pthread_mutex_lock(&ejecutando);
	//termino su tarea lo agrego a ready
	queue_push(ready,
			list_remove_by_condition(execute,
					(void*) esElMismoTripulante(tripulant, (void*) identidad)));
	pthread_mutex_unlock(&listos);
	pthread_mutex_unlock(&ejecutando);
	tripulant->estado = "Ready";
	if (tripulant->posicionX == tarea_x && tripulant->posicionY == tarea_y
			&& espera == contadorespera) {
		tripulant->Tarea = "";
	}

	//le aviso al semaforo que libere un recurso para que mande otro tripulante
	sem_post(&multiProcesamiento);
}

//CLASIFICA LA TAREA DEL TRIPULANTE
void hacerTarea(Tripulante* trip, int socketMiRam, int socketMongo) {

	if (algoritmo == "FIFO") {
		hacerFifo(trip, socketMiRam, socketMongo);
	} else {
		hacerRoundRobin(trip, socketMiRam);
	}

}
// LARGA VIDA TRIPULANTE ESPEREMOS CADA TRIPULANTE VIVA UNA VIDA FELIZ Y PLENA
void* vivirTripulante(Tripulante* tripulante) {
	int socketMiRam = crear_conexion(ipMiRam, puertoMiRam);
	int socketMongo = crear_conexion(ipMongoStore, puertoMongoStore);
	while (tripulante->id != 0) {

		//BUENO ACA ESTA LA MAGIC
		//SI PLANIFICAR LO AGREGO A LA LISTA DE EJECUTAR VA A HACER UNA TAREA
		while (list_find(execute,
				(void*) esElMismoTripulante(tripulante, (void*) identidad)) != 0) {
			//LE CAMBIO EL ESTADO A EJECUTANDO MUITO IMORTANTE
			tripulante->estado = "Execute";
			//ESTE SEMAFORO CHOTO PARA FRENAR LA PLANIFICACION
			sem_wait(&hilosEnEjecucion);

			if (tripulante->Tarea == "") {
				//ACA LE PIDE LA TAREA SI TAN SOLO HUIBIERAMOS SERIALIZADO ESOS MENSAJES
				tareaTripulante tarea;
				tarea.idTripulante = tripulante->id;
				tarea.idPatota = tripulante->idPatota;
				serializar_tarea_tripulante(tarea, socketMiRam);
				free(&tarea);

				t_paquete* paquete = malloc(sizeof(t_paquete));
				paquete->buffer = malloc(sizeof(t_buffer));

				// Primero recibimos el codigo de operacion
				recv(socketMiRam, &(paquete->codigo_operacion), sizeof(uint8_t),
						0);

				// Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
				recv(socketMiRam, &(paquete->buffer->size), sizeof(uint32_t),
						0);
				paquete->buffer->stream = malloc(paquete->buffer->size);
				if (paquete->codigo_operacion == 5) {
					//ACA LE AGREGO LA TAREA AL TRIPULANTE CARITA FACHERA FACHERITA (:

					char* recibido = deserializar_tarea_tripulante(
							paquete->buffer);
					tripulante->Tarea = recibido;
					free(recibido);
				}

				// Liberamos memoria
				free(paquete->buffer->stream);
				free(paquete->buffer);
				free(paquete);
			}
			sem_post(&hilosEnEjecucion);
			//ES HORA DE QUE EL VAGO DEL TRIPULANTE SE PONGA A LABURAR
			hacerTarea(tripulante, socketMiRam, socketMongo);
		}

	}
	//ESTE RETURN NULL ES PARA CASTEARLA EN EL CREATE UNA PEQUEÑA BOLUDEZ
	return NULL;

}

int hacerConsola() {
	//SIEMPRE HAY QUE SER CORTEZ Y SALUDAR
	puts("Bienvenido a A-MongOS de Cebollita Subcampeon \n");
	char* linea;
	int socketMR = crear_conexion(ipMiRam, puertoMiRam);
	while (1) {
//leo los comandos
		linea = readline(">");
		if (string_contains(linea, "INICIAR_PATOTA") == true) {
			//PD ESTE MANEJO DE STRINS TE TOCA A VOS PILAR
			char* tarea = string_substring_from(linea, 16);
			int cantidad;

			// DEFINIR CANTIDAD
			int i = 0;
			while (isDigit(tarea[i])==true){
				i++;
			}

			char* cant="";
			for(int k=0; k<i ; k++){
				cant = cant && tarea[k];
			}

			cantidad=(int)cant;

	}
			}
			//AHORA SI PA INICIAMOS LA PATOTA
			Patota* pato = iniciarPatota(cantidad, 0, NULL, tarea);
			tareasPatota enviar;
			enviar.id = pato->id;
			enviar.tareas = pato->tareas;
			enviar.tareas_length = strlen(enviar.tareas) + 1;
			serializar_tarea_patota(enviar, socketMR);
			free(enviar.tareas);
			free(&enviar);

			//LE PASO A MI RAM CADA TRIPULANTE PARA QUE MUESTRE EN PANTALLA
			for (int i = 0; pato->tripulacion[i] != NULL; i++) {
				//SERIALIZO Y EN VIO
				serializar_tripulante(pato->tripulacion[i], socketMR);
				//UNA VEZ QUE SE CARGA EN MEMORIA PASA A READY
				pato->tripulacion[i]->estado = "READY";
				//LO AGREGO A LA COLA
				queue_push(ready, pato->tripulacion[i]);
				//CORRO EL HILO DEL TRIPULANTE
				pthread_create(&(pato->tripulacion[i]->hilo), NULL,
						vivirTripulante(pato->tripulacion[i]), NULL);

			}
			//LIBERO MEMORIA BORRANDO EL ARRAY FEO DE TRIPULANTES
			free(pato->tripulacion);
			free(pato->tareas);
			free(pato);

		}
		if (linea == "INICIAR_PLANIFICACION") {
			a = 0;
			//DEFINO UN SEM CONTADOR QUE NOS VA A SERVIR PARA PAUSAR LA PLANIFICACION DONDE QUERAMOS DESPUES
			while (a < multiProcesos) {
				sem_post(&hilosEnEjecucion);
				a++;
			}
			//ESTE SEM NOS VA A PERMITIR FRENAR LOS PROCESOS IO CUANDO NECESITEMOS
			sem_post(&pararIo);
			//SEM CONTADOR QUE NOS PERMITE PONER EN EJECECUCION SEGUN CUANTO MULTIPROCESAMIENTO TENGAMOS
			while (linea != "PAUSAR_PLANIFICACION") {
				sem_wait(&multiProcesamiento);
				//ESTE MUTEX ES PARA PROTEGER LA COLA DE READY
				pthread_mutex_lock(&listos);
				//este para proteger la lista de ejecutados
				pthread_mutex_lock(&ejecutando);
				//AGREGO A LISTA DE EJECUCION
				list_add(execute, queue_pop(ready));
				pthread_mutex_unlock(&listos);
				pthread_mutex_unlock(&ejecutando);

			}

		}
		if (linea == "LISTAR_TRIPULANTES") {
			//ESTO LO VOY A CAMBIAR PORQUE ME SUENA QUE TENRER TODAS LAS PATOTAS ME GENERA MEMORY LEAK
			//ME DA PAJA CODEARLO PERO ESTA EN MI MENTE
			int i = 0;
			while (i < list_size(listaPatotas)) {
				Patota* patota = (Patota*) list_get(listaPatotas, i);
				int x = 0;
				while (patota->tripulacion[x] != NULL) {
					mostrarTripulante(patota->id, patota->tripulacion[x]);
					x++;
				}
				i++;
			}

		}
		if (linea == "PAUSAR_PLANIFICACION") {
			///ver como lo hacemos
			// YA LO HICE LOL BASUCAMENTE LES TIRAS UN WAIT HASTA QUE LLEGUEN A 0 PARA QUE NO PUEDAN EJECUTAR
			a = 0;
			while (a < multiProcesos) {
				sem_wait(&hilosEnEjecucion);
				a++;
			}
			sem_wait(&pararIo);
		}
		if (string_contains(linea, "EXPULSAR_TRIPULANTE")) {
			//BUENO ACA UN PEQUEÑO INTENTO DE TU TAREA DE MANEJO DE STRINGS PILI
			// FIJATE QUE SOLO SIRVE SI ES DE UN DIGIITO VAS A TENER QUE DIVIDIR EL ESTRING EN EL ESPACIO
			// Y FIJARTE SI EL SUBSTRING TIENE 1 O 2 CARACTERES
			int idTripulante;

			char* tripulanteId = string_substring_from(linea, 20);

			int i = 0;
			while (isDigit(tripulanteId[i])==true){
				i++;
			}

			char* id="";
			for(int k=0; k<i ; k++){
				id = id && tripulanteId[k];
			}

			idTripulante=(int)id;


			//envio a MIRAM QUE BORREEL TRIPULANTE DEL MAPA

		}

	}

}

int main() {
	config = config_create("/home/utnso/workspace/Discordiador/TP.config");
	ipMiRam = config_get_string_value(config, "IP_MI_RAM_HQ");
	puertoMiRam = config_get_string_value(config, "PUERTO_MI_RAM_HQ");
	multiProcesos = config_get_int_value(config, "GRADO_MULTITAREA");
	retardoCpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
	algoritmo = config_get_string_value(config, "ALGORITMO");
	puertoMongoStore = config_get_int_value(config, "PUERTO_I_MONGO_STORE");
	ipMongoStore = config_get_string_value(config, "IP_I_MONGO_STORE");
	quantum = config_get_int_value(config, "QUANTUM");

	//INICIALIAZAMOS LOS SEMAFOROS
	sem_init(&multiProcesamiento, 0, multiProcesos);
	sem_init(&hilosEnEjecucion, 0, multiProcesos);
	sem_init(&pararIo, 0, 1);
	pthread_mutex_init(&listos, NULL);
	pthread_mutex_init(&ejecutando, NULL);
	pthread_mutex_init(&bloqueadosIo, NULL);
	printf("hola mundo\n");
	pthread_t consola;
	pthread_create(&consola, NULL, (void *) hacerConsola(), NULL);

	printf("hola mundo\n");

	return 0;

	/* ADIO' wachin ! */
}
