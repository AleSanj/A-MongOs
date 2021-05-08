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


int crear_server(char* puerto,char* ip,int backlog){


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

	listen(listenningSocket, backlog);

	freeaddrinfo(serverInfo);
	return listenningSocket;
}

int esperar_cliente(int socket_server){
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

void terminar_servidor(int socket){
	close(socket_cliente());
}

