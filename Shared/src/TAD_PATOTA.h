/*
 * TAD_PATOTA.h
 *
 *  Created on: 6 may. 2021
 *      Author: utnso
 */

#ifndef SRC_TAD_PATOTA_H_
#define SRC_TAD_PATOTA_H_

#ifndef SRC_TAD_PATOTAS_H_
#define SRC_TAD_PATOTAS_H_
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
	t_list* tripulacion ;
	char* tarea;
}Patota;


void agregarTripulantes(int cantidad,Patota* patota ,t_list* listaTripulantes);

struct Patota* iniciarPatota(int cantTripulantes,int id,t_list* listaTripulantes, char* tareas );


#endif /* SRC_TAD_PATOTAS_H_ */

#endif /* SRC_TAD_PATOTA_H_ */
