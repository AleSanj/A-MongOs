/*
 * serializacion.c
 *
 *  Created on: 6 may. 2021
 *      Author: utnso
 */


#include "serializacion.h"

#include "TAD_TRIPULANTE.h"
#define TRIPULANTE 1

typedef struct {
    uint32_t size; // Tamaño del payload
    void* stream; // Payload
} t_buffer;


typedef struct {
    uint8_t codigo_operacion;
    t_buffer* buffer;
} t_paquete;


void serializar_tripulante(Tripulante* unTripulante, int socket){
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(uint8_t) * 3 // Id, posx, posy
	             + strlen(unTripulante->Tarea) + 1 // Tarea
	             + strlen(unTripulante->estado) + 1; // La longitud del string nombre. Le sumamos 1 para enviar tambien el caracter centinela '\0'. Esto se podría obviar, pero entonces deberíamos agregar el centinela en el receptor.

	void* stream = malloc(buffer->size);
	int offset = 0; // Desplazamiento

	memcpy(stream + offset, &unTripulante->id, sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(stream + offset, &unTripulante->posicionX, sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(stream + offset, &unTripulante->posicionY, sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(stream + offset, &unTripulante->estado_length, sizeof(uint32_t));
	offset += sizeof(uint8_t);
	memcpy(stream + offset, unTripulante->estado, strlen(unTripulante->estado));
	offset += sizeof(uint32_t);
	// Para la tarea primero mandamos el tamaño y luego el texto en sí:
	memcpy(stream + offset, &unTripulante->Tarea_length, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, unTripulante->Tarea, strlen(unTripulante->Tarea) + 1);
	// No tiene sentido seguir calculando el desplazamiento, ya ocupamos el buffer completo

	buffer->stream = stream;

	// Si usamos memoria dinámica para el nombre, y no la precisamos más, ya podemos liberarla:

	//ESTO NO ESTA CHECKEADO SI ALGUIEN ENTIENDE q lo explique xd
	free(unTripulante->Tarea);
	free(unTripulante->estado);
	//LO Q SIGUE SI ESTA CHECKEADO


	t_paquete* paquete = malloc(sizeof(t_paquete));


	//CODIGO DE OPERACION 1 = UN TRIUPLANTE
	paquete->codigo_operacion = TRIPULANTE; // Podemos usar una constante por operación
	paquete->buffer = buffer; // Nuestro buffer de antes.

	// Armamos el stream a enviar
	void* a_enviar = malloc(buffer->size + sizeof(uint8_t) + sizeof(uint32_t));
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(uint8_t));

	send(socket, a_enviar, buffer->size + sizeof(uint8_t) + sizeof(uint32_t), 0);

	// No nos olvidamos de liberar la memoria que ya no usaremos
	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);


}

//ESTA FUNCION DESPUES TENEMOS QUE LLAMARLA EN NUESTRO MAIN (VER DOCUMENTACION DE SERIALIZACION)
Tripulante* deserializar_tripulante(t_buffer* buffer) {
	Tripulante* unTripulante = malloc(sizeof(Tripulante));

    void* stream = buffer->stream;
    // Deserializamos los campos que tenemos en el buffer
    memcpy(&(unTripulante->id), stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    memcpy(&(unTripulante->posicionX), stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    memcpy(&(unTripulante->posicionY), stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    memcpy(&(unTripulante->estado_length), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    unTripulante->estado = malloc(unTripulante->estado_length);
    memcpy(&(unTripulante->estado), stream, unTripulante->estado_length);
    stream += sizeof(uint32_t);
    // Por último, para obtener el nombre, primero recibimos el tamaño y luego el texto en sí:
    memcpy(&(unTripulante->Tarea_length), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    unTripulante->Tarea = malloc(unTripulante->Tarea_length);
    memcpy(unTripulante->Tarea, stream, unTripulante->Tarea_length);

    return unTripulante;
}

// FUNCION PARA EL MAIN PARA RECIBIR UN PAQUETE adaptarla cuando se necesite!!!
/*
 * t_paquete* paquete = malloc(sizeof(t_paquete));
paquete->buffer = malloc(sizeof(t_buffer));

// Primero recibimos el codigo de operacion
recv(unSocket, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

// Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
recv(unSocket, &(paquete->buffer->size), sizeof(uint32_t), 0);
paquete->buffer->stream = malloc(paquete->buffer->size);
recv(unSocket, paquete->buffer->stream, paquete->buffer->size, 0);

// Ahora en función del código recibido procedemos a deserializar el resto
switch(paquete->codigo_operacion) {
    case PERSONA:
        t_persona* persona = deserializar_persona(paquete->buffer);
        ...
        // Hacemos lo que necesitemos con esta info
        // Y eventualmente liberamos memoria
        free(persona);
        ...
        break;
    ... // Evaluamos los demás casos según corresponda
}

// Liberamos memoria
free(paquete->buffer->stream);
free(paquete->buffer);
free(paquete);
*/
