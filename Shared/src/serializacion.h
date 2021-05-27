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

void serializar_tripulante(Tripulante* unTripulante, int socket);
Tripulante* deserializar_tripulante(t_buffer* buffer);
void serializar_patota( Patota* unaPatota, int socket);
Patota* deserializarPatota(t_buffer* buffer);

#endif /* SRC_SERIALIZACION_H_ */
