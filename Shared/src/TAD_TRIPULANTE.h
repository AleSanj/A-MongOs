/*
 * TAD_TRIPULANTE.h
 *
 *  Created on: 6 may. 2021
 *      Author: utnso
 */

#ifndef SRC_TAD_TRIPULANTE_H_
#define SRC_TAD_TRIPULANTE_H_
#include <commons/collections/list.h>
#include <stdbool.h>
#include <commons/collections/node.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/bitarray.h>
#include <commons/log.h>
#include <commons/string.h>

typedef struct
{
	int id;
	char* estado;
	char* Tarea;
	int posicionX;
	int posicionY;

}Tripulante;

Tripulante* tripulanteCreate(int id, int posicionX,int posicionY);

void mostrarTripulante(int idPatota, Tripulante* tripulante);




#endif /* SRC_TAD_TRIPULANTE_H_ */
