/*
 * TAD_TRIPULANTE.c
 *
 *  Created on: 6 may. 2021
 *      Author: utnso
 */

#ifndef TAD_TRIPULANTE


#include "TAD_TRIPULANTE.h"

#include <stdio.h>
#include <stdlib.h>
#include <commons/bitarray.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/temporal.h>
#include <stdint.h>
#include <pthread.h>
struct t_tripulante
{
	//El estado_length al igual q tarea los pusimos con mati
	//porque hay que guardar el sizeof del char estado y tarea en
	//algun lugar para despues mandarselo al buffer (MIRAR DOC DE SERIALIZACION)
	uint8_t id;
	uint8_t idPatota;
	uint32_t estado_length;
	char* estado;
	uint32_t Tarea_length;
	char* Tarea;
	uint8_t posicionX;
	uint8_t posicionY;
	pthread_t hilo;

};

t_tripulante* tripulanteCreate(uint8_t id, uint8_t posicionX,uint8_t posicionY)
 {
	t_tripulante* devolverTripulante;
	devolverTripulante->id=id;
	devolverTripulante->posicionX= posicionX;
	devolverTripulante->posicionY=posicionY;
	devolverTripulante->estado="New";
	return devolverTripulante ;
 }

 void mostrarTripulante(uint8_t idPatota, Tripulante* tripulante)
 {

 	printf ("Patota: %i Tripulante: %i Estado: %s ",idPatota,tripulante->id,tripulante->estado);
 }

#endif

