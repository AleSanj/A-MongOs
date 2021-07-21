/*
 * TAD_PATOTA.c
 *
 *  Created on: 6 may. 2021
 *      Author: utnso
 */

#ifndef TAD_PATOTA
#include "TAD_PATOTA.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/bitarray.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/temporal.h>
#include "TAD_TRIPULANTE.h"


struct tareasPatota
{
	uint8_t id;
	char* tareas;
	uint32_t tareas_length;
};
struct Patota
{
	uint8_t id;
    Tripulante* tripulacion[10] ;
	char* tareas;
	uint32_t tareas_length;
};

void agregarTripulantes(int cantidad, Patota* patota ,t_list* listaTripulantes)
{
	uint8_t i=0;
	while (i < cantidad)
	{
		if(list_is_empty(listaTripulantes)!= true)
		{
			Tripulante* agregar=malloc(sizeof(Tripulante*));
			agregar =(void*)tripulanteCreate(i+1,(uint8_t)list_remove(listaTripulantes,i*2),(uint8_t)list_remove(listaTripulantes,i*2+1));
			patota->tripulacion[i] =  agregar;
		}
		else
		{
			uint8_t pos =0;
			Tripulante* agregar =tripulanteCreate(i+1,pos,pos);
			patota->tripulacion[i]= agregar;
		};
		i++;
	};
}
 Patota* iniciarPatota(int cantTripulantes,int id,t_list* listaTripulantes, char* tareas )
{

	Patota* devolverPatota=malloc(sizeof(Patota*)) ;
	devolverPatota->id=id;
	devolverPatota->tareas=tareas;
	agregarTripulantes(cantTripulantes,devolverPatota,listaTripulantes );
	return devolverPatota;
};


#endif
