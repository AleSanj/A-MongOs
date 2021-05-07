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

struct Tripulante
{
	int id;
	char* estado;
	char* Tarea;
	int posicionX;
	int posicionY;

};
 struct Tripulante* tripulanteCreate(int id, int posicionX,int posicionY)
 {
	struct Tripulante* devolverTripulante;
	devolverTripulante->id=id;
	devolverTripulante->posicionX= posicionX;
	devolverTripulante->posicionY=posicionY;
	devolverTripulante->estado="";
	return devolverTripulante ;
 }

 void mostrarTripulante(int idPatota, struct Tripulante* tripulante)
 {

 	printf ("Patota: %i Tripulante: %i Estado: %s ",idPatota,tripulante->id,tripulante->estado);
 }

#endif

