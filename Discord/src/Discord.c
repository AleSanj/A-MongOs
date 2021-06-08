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
int puertoMiRam;
t_config* config;
sem_t hilosEnEjecucion;
sem_t multiProcesamiento;
pthread_mutex_t mutexIO;
pthread_mutex_t listos;
pthread_mutex_t ejecutando;
pthread_mutex_t bloqueadosIo;
pthread_mutex_t finalizados;
sem_t pararPlanificacion;
sem_t pararIo;
t_list* listaPatotas;
t_queue* ready;
t_list* execute;
t_list* finalizado;
t_queue* bloqueados;
char* ipMongoStore;
int puertoMongoStore;
char* algoritmo;
int a = 0;
int TripulantesTotales = 0;
int multiProcesos;
int quantum;
int retardoCpu;
bool correr_programa=true;

/*AVISOS PARROQUIALES

 * ESPERO SEAN DE TU AGRADO MIS COMENTARIOS
 * VAMOS QUE YA LO TENEMOS EL MODULO
 * FALTARIAN EL MANEJO DE STRINGS POR ESO HAY VARIABLES NO INICIALIZADAS
 * Y BUENO DESPUES EL ROUND ROBIN VIENDO EL VIDEO TENGO QUE CAMBIAR UNA COSA POR EL IO
 * Y BUENO SOLO FALTARIA VER EL TEMA DE LOS SABOTAJES DESPUES
 * ESPERO SEAN UTILES MIS COMENTARIOS
 */
//esta funcion es porque no puedo aplicar parcialmente una funcion en c ):
void* identidad(void* t)
{
	return t;
}
//esta para saber si es el mismo tripulante
bool esElMismoTripulante(Tripulante* rip, Tripulante* ra)
{
	return (rip->id == ra->id);
}
bool es_tarea_IO(char* tarea)
{
	bool ret= string_contains(tarea, "GENERAR");
	ret=ret||string_contains(tarea,"CONSUMIR");
	return ret;
}
int string_to_int(char* palabra)
{
	int ret;
	if(strlen(palabra)==2)
	{
		 ret= (palabra[0]-'0')*10+palabra[1]-'0';
	}
	else
	{
		ret=palabra[0]-'0';
	}
	return ret;
}
void obtener_parametros_patota(char* consol, int* ctidad,char* ptareas, t_list* poci)
{
	char** get_parametros= string_split(consol,(char*)' ');
	ctidad=string_to_int(get_parametros[1]);
	ptareas=get_parametros[2];
	char** get_posicion=string_split(get_parametros[3], (char*) '|');
	for(int auxi=0; get_posicion[auxi]!=NULL; auxi++)
	{
		list_add(poci,(void*)string_to_int(get_posicion[auxi]));
	}
	free(get_parametros);
	free(get_posicion);
}
void obtener_parametros_tarea(Tripulante* t, int* posX, int* posY)
{
	if(es_tarea_IO(t->Tarea))
	{
		char** list = string_split(t->Tarea,(char*)' ');
		if(list[0]==t->Tarea)
		{
			char** parametros=string_split(t->Tarea,(char*)';');
			posX=string_to_int(parametros[1]);
			posY=string_to_int(parametros[2]);
			t->espera=string_to_int(parametros[3]);
		}
		else
		{
			char** parametros=string_split(list[1],(char*)';');
			posX=string_to_int(parametros[2]);
			posY=string_to_int(parametros[3]);
			t->espera=string_to_int(parametros[4]);

		}

	}
	else
	{
		char** parametros=string_split(t->Tarea,(char*)';');
		posX=string_to_int(parametros[1]);
		posY=string_to_int(parametros[2]);
		t->espera=string_to_int(parametros[3]);
	}
}

void elimiarTripulante(Tripulante* tripulante,int socketMiRam,int socketMongo)
{
	serializar_eliminar_tripulante(tripulante->id,socketMiRam);
	serializar_eliminar_tripulante(tripulante->id,socketMongo);
	free(tripulante->estado);
	pthread_detach(tripulante->hilo);
	free(tripulante);
}

void ejecutando_a_bloqueado(Tripulante* trp, int socketMR, int socketMS )
{
	pthread_mutex_lock(&bloqueadosIo);
		pthread_mutex_lock(&ejecutando);
		//ACA PUSHEO AL TRIPULANTE A LA COLA DE BLOQUEADOS IO
		queue_push(bloqueados,list_remove_by_condition(execute,(void*) esElMismoTripulante(trp, (void*) identidad)));
		pthread_mutex_unlock(&bloqueadosIo);
		pthread_mutex_unlock(&ejecutando);
		trp->Tarea = "Bloqueado IO";
		cambio_estado act;
		act.idTripulante=trp->id;
		act.estado=trp->estado;
		act.estado_length=strlen(act.estado)+1;
		serializar_cambio_estado(act,socketMR);
		free (act.estado);

}

void bloqueado_a_ready(Tripulante* bloq, int sockerMIram, int socketMongoS)
{
	pthread_mutex_lock(&listos);
	queue_push(ready,bloq);
	pthread_mutex_unlock(&listos);
	bloq->estado = "Ready";
	cambio_estado act;
	act.idTripulante=bloq->id;
	act.estado=bloq->estado;
	act.estado_length=strlen(act.estado)+1;
	serializar_cambio_estado(act,sockerMIram);
	free (act.estado);

}
// esta va a avanzar el tripulante paso a paso Y Enviar a miram
void* moverTripulante(Tripulante* tripu, int socket, int tarea_x,int tarea_y)
{
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



void enviarMongoStore(Tripulante* enviar,int socketMiR, int socketMongo) {

	//envia tarea al MONGO STORE
	serializar_tarea(enviar->Tarea, socketMongo);
	while(enviar->espera!=0)
	{
		//semaforo para parar ejecucion
		sem_wait(&pararIo);
		sleep(retardoCpu);
		enviar->espera--;
		sem_post(&pararIo);
	}
	//lo paso a cola ready
	bloqueado_a_ready(enviar,socketMiR,socketMongo);
	pthread_mutex_unlock(&mutexIO);

}
void hacerTareaIO(Tripulante* io, int socketMongo, int sMR) {
	//ACA ME PASE UN POQUITO CON LOS SEMAFOROS REVISARRRR
	ejecutando_a_bloqueado(io,sMR,socketMongo);
	//libero el recurso de multiprocesamiento porque me voy a io
	sem_post(&multiProcesamiento);
	io->Tarea = "Bloqueado IO";
	pthread_mutex_lock(&mutexIO);
	pthread_mutex_lock(&bloqueadosIo);
	//LO ENVIOPARA QUE HAGA SUS COSAS CON MONGOSTORE
	enviarMongoStore((void*) queue_pop(bloqueados),sMR, socketMongo);
	pthread_mutex_unlock(&bloqueadosIo);

}
void hacerFifo(Tripulante* tripu, int socket, int socketMongo) {
	//tu tarea es es transformar la taera de sstring en dos int posicion y un int de espera
	// mover al 15|65 20
	int tarea_x;
	int tarea_y;
	obtener_parametros_tarea(tripu,&tarea_x,&tarea_y);
	while (tripu->posicionX != tarea_x || tripu->posicionY != tarea_y) {
		//este es el semaforo para pausar laejecucion
		sem_wait(&hilosEnEjecucion);
		sleep(retardoCpu);
		moverTripulante(tripu, socket, tarea_x, tarea_y);

		//le tiroun post al semaforo que me permite frenar la ejecucion
		sem_post(&hilosEnEjecucion);

	}

	if (es_tarea_IO(tripu->Tarea))
	{
		hacerTareaIO(tripu, socketMongo, socket);
	}
	else {
		//una vez que llego donde tenia que llegar espera lo que tenia que esperar
		while(tripu->espera !=0)
		{
			sem_wait(&hilosEnEjecucion);
			sleep(retardoCpu);
			sem_post(&hilosEnEjecucion);
			tripu->espera --;
		}

		tripu->Tarea = "";
	}

}
void hacerRoundRobin(Tripulante* tripulant, int socket, int socketMong) {
	int tarea_x;
	int tarea_y;
	int contadorQuantum = 0;
	obtener_parametros_tarea(tripulant, &tarea_x,&tarea_y);

	while (contadorQuantum < quantum)
	{
		if (tripulant->posicionX == tarea_x && tripulant->posicionY == tarea_y)
		{
			break;
		}
		//este es el semaforo para pausar laejecucion
		sem_wait(&hilosEnEjecucion);
		sleep(retardoCpu);
		moverTripulante(tripulant, socket, tarea_x, tarea_y);
		contadorQuantum++;
		sem_post(&hilosEnEjecucion);
		//le tiroun post al semaforo que me permite frenar la ejecucion

	}
	if(contadorQuantum<quantum && es_tarea_IO(tripulant->Tarea))
	{
		hacerTareaIO(tripulant, socketMong, socket);
	}
	while (contadorQuantum < quantum) {
		sem_wait(&hilosEnEjecucion);
		sleep(retardoCpu);
		tripulant->espera--;
		contadorQuantum++;
		sem_post(&hilosEnEjecucion);
	}


	if (tripulant->posicionX == tarea_x && tripulant->posicionY == tarea_y && tripulant->espera==0)
	{
		tripulant->Tarea = "";
	}else
	{
		//protejo las colas o listas
		pthread_mutex_lock(&listos);
		pthread_mutex_lock(&ejecutando);
			//termino su quantum lo agrego a ready
		queue_push(ready,list_remove_by_condition(execute,(void*) esElMismoTripulante(tripulant, (void*) identidad)));
		pthread_mutex_unlock(&listos);
		pthread_mutex_unlock(&ejecutando);
		tripulant->estado = "Ready";
	//le aviso al semaforo que libere un recurso para que mande otro tripulante
	sem_post(&multiProcesamiento);
	}
}

//CLASIFICA LA TAREA DEL TRIPULANTE
void hacerTarea(Tripulante* trip, int socketMiRam, int socketMongo)
{
	if (algoritmo == "FIFO")
	{
		hacerFifo(trip, socketMiRam, socketMongo);
	} else {
		hacerRoundRobin(trip, socketMiRam, socketMongo);
	}

}
// LARGA VIDA TRIPULANTE ESPEREMOS CADA TRIPULANTE VIVA UNA VIDA FELIZ Y PLENA
void* vivirTripulante(Tripulante* tripulante)
{
	int socketMiRam = crear_conexion(ipMiRam, puertoMiRam);
	int socketMongo = crear_conexion(ipMongoStore, puertoMongoStore);
	while (tripulante->id != 0)
	{
		//BUENO ACA ESTA LA MAGIC
		//SI PLANIFICAR LO AGREGO A LA LISTA DE EJECUTAR VA A HACER UNA TAREA
		while (list_find(execute,(void*) esElMismoTripulante(tripulante, (void*) identidad)) != 0)
		{
			if (tripulante->estado!="Ready")
			{
				//LE CAMBIO EL ESTADO A EJECUTANDO MUITO IMORTANTE
					tripulante->estado = "Execute";
					cambio_estado estado;
					estado.idTripulante=tripulante->id;
					estado.estado=tripulante->estado;
					estado.estado_length=strlen(estado.estado)+1;
					serializar_cambio_estado(estado, socketMiRam);
			}
			sem_wait(&hilosEnEjecucion);

		if (tripulante->Tarea == "") {
				//ACA LE PIDE LA TAREA
				tareaTripulante tarea;
				tarea.idTripulante = tripulante->id;
				tarea.idPatota = tripulante->idPatota;
				serializar_tarea_tripulante(tarea, socketMiRam);
				free(&tarea);
				//AHORA RECIBIMOS TAREA
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

					char* recibido = deserializar_tarea(paquete->buffer);

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
			if(tripulante->Tarea!=NULL)
			{
			hacerTarea(tripulante, socketMiRam, socketMongo);
			}
			else
			{
				pthread_mutex_lock(&ejecutando);
				pthread_mutex_lock(&finalizados);
				list_add(finalizado,list_remove_by_condition(execute,(void*) esElMismoTripulante(tripulante, (void*) identidad)));
				pthread_mutex_unlock(&ejecutando);
				pthread_mutex_unlock(&finalizados);
				tripulante->id=0;
				sem_post(&multiProcesamiento);

			}
		}

	}

	elimiarTripulante(tripulante, socketMiRam, socketMongo);

	//ESTE RETURN NULL ES PARA CASTEARLA EN EL CREATE UNA PEQUEÑA BOLUDEZ
	return NULL;

}
void* atender_sabotaje(char* instruccion_sabotaje)
{
	//separo los parametros del sabotaje

	//busco el tripulante mas cerca

	//lo pongo a ejecutar el sabotaje

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
		if (string_contains(linea, "INICIAR_PATOTA")) {
			//PD ESTE MANEJO DE STRINS TE TOCA A VOS PILAR
			char* tarea;
			int cantidad;
			t_list* list_posicion;
			obtener_parametros_patota(linea,&cantidad,tarea,list_posicion);

			//AHORA SI PA INICIAMOS LA PATOTA
			Patota* pato = iniciarPatota(cantidad, cantidad, list_posicion,tarea);
			free(tarea);
			list_destroy(list_posicion);
			iniciar_patota* enviar;
			enviar->idPatota = pato->id;
			enviar->Tareas = fopen(tarea,"w+r");
			enviar->cantTripulantes=cantidad;
			serializar_iniciar_patota(enviar, socketMR);
			free(enviar->Tareas);
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
		if (linea == "INICIAR_PLANIFICACION")
		{
			a = 0;
			//DEFINO UN SEM CONTADOR QUE NOS VA A SERVIR PARA PAUSAR LA PLANIFICACION DONDE QUERAMOS DESPUES
			while (a < multiProcesos) {
				sem_post(&hilosEnEjecucion);
				a++;
			}
			//ESTE SEM NOS VA A PERMITIR FRENAR LOS PROCESOS IO CUANDO NECESITEMOS
			sem_post(&pararIo);
			//SEM CONTADOR QUE NOS PERMITE PONER EN EJECECUCION SEGUN CUANTO MULTIPROCESAMIENTO TENGAMOS
			while (linea != "PAUSAR_PLANIFICACION")
			{
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
		if (linea == "LISTAR_TRIPULANTES")
		{
			//ESTO LO VOY A CAMBIAR PORQUE ME SUENA QUE TENRER TODAS LAS PATOTAS ME GENERA MEMORY LEAK
			//ME DA PAJA CODEARLO PERO ESTA EN MI MENTE
			t_list* get_all_tripulantes;
			int in=0;
			pthread_mutex_lock(&finalizados);
			while(in<list_size(finalizado))
			{
				list_add(get_all_tripulantes,list_get(finalizado,in));
				in++;
			}
			pthread_mutex_unlock(&finalizados);
			in=0;
			pthread_mutex_lock(&ejecutando);
			while(in<list_size(execute))
			{
				list_add(get_all_tripulantes,list_get(execute,in));
				in++;
			}
			pthread_mutex_unlock(&ejecutando);

			in=0;
			pthread_mutex_lock(&listos);
			while(in<queue_size(ready))
			{
				Tripulante* trip_agregar=queue_pop(ready);
				list_add(get_all_tripulantes,trip_agregar);
				queue_push(ready,trip_agregar);
				in++;
			}
			pthread_mutex_unlock(&listos);

			in=0;
			pthread_mutex_lock(&bloqueadosIo);
			while(in<queue_size(bloqueados))
			{
				Tripulante* trip_agregar=queue_pop(bloqueados);
				list_add(get_all_tripulantes,trip_agregar);
				queue_push(bloqueados,trip_agregar);
				in++;
			}
			pthread_mutex_unlock(&bloqueadosIo);

			in=0;
			while(in<list_size(get_all_tripulantes))
			{
			Tripulante* trip_mostrar=(Tripulante*)list_remove(get_all_tripulantes,in);
			mostrarTripulante(trip_mostrar);
			in++;
			}
			list_destroy(get_all_tripulantes);
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
		if (string_contains(linea, "EXPULSAR_TRIPULANTE"))
		{
			//BUENO ACA UN PEQUEÑO INTENTO DE TU TAREA DE MANEJO DE STRINGS PILI
			// FIJATE QUE SOLO SIRVE SI ES DE UN DIGIITO VAS A TENER QUE DIVIDIR EL ESTRING EN EL ESPACIO
			// Y FIJARTE SI EL SUBSTRING TIENE 1 O 2 CARACTERES
			char** obtener_id_trip=string_split(linea, (char*)' ');
			int id_trip_expulsar=string_to_int(obtener_id_trip[1]);
			//envio a MIRAM QUE BORREEL TRIPULANTE DEL MAPA
			serializar_eliminar_tripulante(id_trip_expulsar,socketMR);

		}

	}

}

int main() {
	config = config_create("/home/utnso/workspace/Discordiador/TP.config");
	ipMiRam = config_get_string_value(config, "IP_MI_RAM_HQ");
	puertoMiRam = config_get_int_value(config, "PUERTO_MI_RAM_HQ");
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

	int socket_mongo_store_sabotaje=crear_conexion(ipMongoStore,puertoMongoStore);
	char* sabotaje;
	while(correr_programa)
	{
		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));

		// Primero recibimos el codigo de operacion
		recv(socket_mongo_store_sabotaje, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

		// Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
		recv(socket_mongo_store_sabotaje, &(paquete->buffer->size), sizeof(uint32_t), 0);
		paquete->buffer->stream = malloc(paquete->buffer->size);
		recv(socket_mongo_store_sabotaje, paquete->buffer->stream, paquete->buffer->size, 0);

		// Ahora en función del código recibido procedemos a deserializar el resto

		if(paquete->codigo_operacion==10)
		{
		  sabotaje=deserializar_sabotaje(paquete->buffer);
		  int parar_todo_sabotaje = 0;
		  while (parar_todo_sabotaje< multiProcesos)
		  {
		  sem_wait(&hilosEnEjecucion);
		  parar_todo_sabotaje++;
		  }
		sem_wait(&pararIo);

		}

		// Liberamos memoria
		free(paquete->buffer->stream);
		free(paquete->buffer);
		free(paquete);
		pthread_t hilo_sabotaje;
		pthread_create(&hilo_sabotaje,NULL,atender_sabotaje(sabotaje),NULL);
	}



	printf("hola mundo\n");

	return 0;

	/* ADIO' wachin ! */
}
