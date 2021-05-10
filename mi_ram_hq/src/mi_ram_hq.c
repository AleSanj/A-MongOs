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
#include <nivel-gui/nivel-gui.h>
#include <nivel-gui/tad_nivel.h>
#include <curses.h>
#include <commons/collections/list.h>

#define ASSERT_CREATE(nivel, id, err)

int main(void) {
	//crear_server("6667","127.0.0.1",5);
	//ejemplo();
	NIVEL* nivel = crear_mapa();
	crear_server("6667","127.0.0.1",5);
	//crear_personajes(nivel, patota);
	return EXIT_SUCCESS;
}





int crear_mapa(){
		NIVEL* nivel;

		int cols, rows;
		int err;

		nivel_gui_inicializar();

		nivel_gui_get_area_nivel(&cols, &rows);

		nivel = nivel_crear("Test Chamber 04");
		nivel_gui_dibujar(nivel);
		return nivel;
		//Aca hacemos un while de 0 hasta la cantidad de tripulantes y los creamos/dibujamos


	}

