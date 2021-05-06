/*
 ============================================================================
 Name        : mi_ram_hq.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <conexion.h>

int main(void) {
	crear_server("6667","127.0.0.1",5);
	//ejemplo();
	return EXIT_SUCCESS;
}
