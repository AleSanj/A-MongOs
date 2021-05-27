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
sem_t sem;
char* ipMiRam;
char* puertoMiRam;
t_queue* colaNew;
t_queue* colaReady;
t_queue* colaExecute;
t_queue* colaBloqueadosIO;
 int i=0;
 int socketMirram;
 struct
 {
	uint32_t x;
	 uint32_t y;
 }typedef Tarea;

 int hacerConsola(t_queue* new, t_queue* ready, t_queue* execute, t_queue* bloqueados)
 {
	 return 4;
 }

int hacerTareaFifo(Tripulante* trip, Tarea tarea)
 {
	 while(trip->id!=0)
	 {
		     sem_wait(&sem);
		     trip->estado="Execute";
			 while(trip->posicionX<tarea.x)
			 {
				 trip->posicionX++;
				 serializar_tripulante(trip,crear_conexion(ipMiRam,puertoMiRam));
			 }
			 while(trip->posicionY<tarea.y)
			 {
				 serializar_tripulante(trip,crear_conexion(ipMiRam,puertoMiRam));
				 trip->posicionY++;
			 }
			 sem_post(&sem);

	 }
	 return 0;
 }

t_config* config;
char* IPMiRam;
char* PuertoMIRam;
int multitarea;

int main(){
	config= config_create("/home/utnso/workspace/Discordiador/TP.config");
	IPMiRam=config_get_string_value(config,"IP_MI_RAM_HQ");
	PuertoMIRam=config_get_string_value(config,"PUERTO_MI_RAM_HQ");
    multitarea=config_get_int_value(config,"GRADO_MULTITAREA");

    pthread_t consola;
    pthread_create(&consola,NULL,(void *)hacerConsola(colaNew,colaReady,colaExecute,colaBloqueadosIO),NULL);

	sem_init(&sem,0,multitarea);
	int i=0;
	printf("hola mundo\n");
	colaReady =queue_create(colaReady);
	while (i<8)
	{
		Tripulante* agregar= tripulanteCreate(i,i,i);
		queue_push(colaReady,agregar);

	}
    Tarea f;
	f.x=16;
	f.y=23;
	while(queue_size(colaReady)!=0)
	{
		Tripulante* trip=queue_pop(colaReady);
		pthread_create(&trip->hilo, NULL, (void *)hacerTareaFifo(trip,f), NULL);
	}

	return 0;

	/* ADIO' wachin ! */
}
