/*
 * serializacion.h
 *
 *  Created on: 6 may. 2021
 *      Author: utnso
 */

#ifndef SRC_SERIALIZACION_H_
#define SRC_SERIALIZACION_H_
#include "TAD_TRIPULANTE.h"
#include "TAD_PATOTA.h"


typedef struct {
    uint32_t size; // Tamaño del payload
    void* stream; // Payload
} t_buffer;

typedef struct {
    uint8_t codigo_operacion;
    t_buffer* buffer;
} t_paquete;

typedef struct
{
	uint8_t idTripulante;
	uint8_t posX;
	uint8_t posY;
}id_and_pos;

typedef struct
{
	uint32_t idTripulante;
	uint32_t estado_length;
	char estado;
}cambio_estado;

typedef struct
{
	uint8_t idPatota;
	uint8_t cantTripulantes;
	FILE* Tareas;
}inicio_patota;



void serializar_tripulante(Tripulante* unTripulante, int socket);
Tripulante* deserializar_tripulante(t_buffer* buffer);
void serializar_patota( Patota* unaPatota, int socket);
Patota* deserializarPatota(t_buffer* buffer);
void serializar_tarea_patota( tareasPatota tareaPatota, int socket);
tareasPatota* deserializar_tareas_patota(t_buffer* buffer);
void serializar_tarea_tripulante( tareaTripulante tareaTrip, int socket);
tareaTripulante* deserializar_tarea_tripulante(t_buffer* buffer);
void serializar_tarea(char* tarea, int socket);
char* deserializar_tarea(t_buffer* buffer);
void serializar_id_and_pos(id_and_pos pos, int socket);



#endif /* SRC_SERIALIZACION_H_ */
