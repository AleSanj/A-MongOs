/*
 * conexion.c
 *
 *  Created on: 5 may. 2021
 *      Author: utnso
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "conexion.h"

//#define PUERTO "6667"
#define BACKLOG 5			// Define cuantas conexiones vamos a mantener pendientes al mismo tiempo
#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar


void ejemplo(){
    printf("Ejemploo");
}


int crear_server(char* puerto,char* ip){


	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;		// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(NULL, puerto, &hints, &serverInfo);

	/*for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        int activado = 1;
        setsockopt(socket_servidor,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }*/

	int listenningSocket;

	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

	bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);
	printf("Servidor creado, esperando clientes\n");
	return listenningSocket;
}

int esperar_cliente(int socket_server, int backlog){

		listen(socket_server, backlog);

		struct sockaddr_in addr;
		socklen_t addrlen = sizeof(addr);

		int socket_cliente = accept(socket_server, (struct sockaddr *) &addr, &addrlen);


		printf("Cliente conectado. Esperando mensajes:\n");
	/*
		while (status != 0){
			status = recv(socketCliente, (void*) package, PACKAGESIZE, 0);
			if (status != 0) printf("%s", package);

		}
	*/
		return socket_cliente;


}

void terminar_servidor(int socket_server){
	close(socket_server);
}
int crear_conexion(char* ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}
void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}


