/*
 * conexion.h
 *
 *  Created on: 6 may. 2021
 *      Author: utnso
 */

#ifndef SRC_CONEXION_H_
#define SRC_CONEXION_H_

int crear_server(char*,char*,int);
int esperar_cliente(int);
void ejemplo();
int crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);


#endif /* SRC_CONEXION_H_ */
