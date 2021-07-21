/*
 * funcionesMemoria.h
 *
 *  Created on: 21 jul. 2021
 *      Author: utnso
 */

#ifndef FUNCIONESMEMORIA_H_
#define FUNCIONESMEMORIA_H_
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/bitarray.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <TAD_PATOTA.h>
#include <TAD_TRIPULANTE.h>
#include "mi_ram_hq.h"

typedef struct
{
    int *inicio;
    int tamanio;

} segmentoEnTabla_struct;
typedef struct
{
    int ID;
    int segmentoOPagina;
    int offsetEnPagina;
    char tipo;
    int tamanio;
} elementoEnLista_struct;
typedef struct
{
    void *ptrHuecoLibre;
    int tamanio;
} espacio_struct;


typedef struct{
    int frame;
    int espacioOcupado;
    unsigned int presencia;
}paginaEnTabla_struct;
typedef struct {
    unsigned int idPatota;
    t_list *tablaDePaginas;
}tablaEnLista_struct;
typedef struct{
    int nroFrame;
    int PID;
    int nroPagina;
    unsigned int uso;
}paginaParaReemplazar_struct;
t_list *listaDeTablasDePaginas;
t_list *listaSegmentos;
t_list *listaElementos;
t_queue *tablaDeFrames;
void compactacion();
void *minimo_segmentos_en_tabla(void *, void *);
void *minimo_hueco_libre(void *, void *);
void guardar_en_memoria_segmentacion(void*,int,int,uint32_t,char,int);
void borrar_de_memoria_segmentacion(int, char);
bool ordenar_por_posicion(void *, void *);
bool encontrarTablaDePaginas(void*);
void *buscar_de_memoria_segmentacion(int, char);
char tipoUniversal;
void guardar_en_memoria_paginacion(void*,int,int,uint32_t,char);
void* buscar_en_memoria_paginacion(void*,int, char);
void *borrar_de_memoria_paginacion(int, int, char);
void guardar_en_swap(void*,int,int,uint32_t,char);
segmentoEnTabla_struct *punteroMinimo;
int paginaUniversal;
int punteroReemplazo;

#endif /* FUNCIONESMEMORIA_H_ */
