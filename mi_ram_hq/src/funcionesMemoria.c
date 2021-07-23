/*
 * funcionesMemoria.c
 *
 *  Created on: 21 jul. 2021
 *      Author: utnso
 */
#include "funcionesMemoria.h"

int menorEntreDos(int elem1, int elem2){
    if(elem1 > elem2){
        return elem2;
    }
    else
        return elem1;
}
int encontrarFrameDisponible(){
    for (int j = 0; j < cantidadPaginas; ++j) {
        if (bitarrayMemoria[j] == 0){
            return j;
        }
    }
    return -1;
}

int encontrarFrameEnSwapDisponible(){
    int i=0;
    while (bitarraySwap[i]==1){
        if (i==tamSwap/tamPagina){
            break;
        }
        i++;
    }
    return i;
}
int calcular_direccion_logica_archivo( int idPatota){
	pthread_mutex_lock(&mutexMemoria);
	tipoUniversal ='A';
	t_list *listaFiltrada;
	listaFiltrada = list_filter(listaElementos,filtrarPorTipo);
	elementoEnLista_struct *elementoBuscado = malloc(sizeof(elementoEnLista_struct));
	for(int i =0; i< list_size(listaFiltrada);i++){
		elementoBuscado = list_get(listaFiltrada,i);
		if(elementoBuscado->ID==idPatota){
			break;
		}
	}
	pthread_mutex_unlock(&mutexMemoria);

	return (elementoBuscado->segmentoOPagina+elementoBuscado->offsetEnPagina);


}
int calcular_direccion_logica_patota( int idPatota){
	pthread_mutex_lock(&mutexMemoria);
	tipoUniversal ='P';
	t_list *listaFiltrada;
	listaFiltrada = list_filter(listaElementos,filtrarPorTipo);
	elementoEnLista_struct *elementoBuscado = malloc(sizeof(elementoEnLista_struct));
	for(int i =0; i< list_size(listaFiltrada);i++){
		elementoBuscado = list_get(listaFiltrada,i);
		if(elementoBuscado->ID==idPatota){
			break;
		}
	}
	pthread_mutex_unlock(&mutexMemoria);

	return (elementoBuscado->segmentoOPagina+elementoBuscado->offsetEnPagina);
}

void guardar_en_memoria_general(void* payload,int idElemento,int tamPayload,int pid,char tipo){
    pthread_mutex_lock(&mutexMemoria);
    puts("ENTRE A GUARDAR EN MEMORIA");
	if (strcmp(esquemaMemoria,"PAGINACION")==0){
		puts("ENTRE A GUARDADO DE PAGINACION");
		guardar_en_memoria_paginacion(payload, idElemento, tamPayload, pid, tipo);

	}else if(strcmp(esquemaMemoria,"SEGMENTACION")==0){
		puts("ENTRE A GUARDADO DE SEGMENTACION");
		guardar_en_memoria_segmentacion( payload, idElemento, tamPayload, pid, tipo,tipoDeGuardado);
	}
    pthread_mutex_unlock(&mutexMemoria);


}

void* buscar_en_memoria_general(int idElementoABuscar,int PID, char tipo){
	void* retornar;
    pthread_mutex_lock(&mutexMemoria);
    puts("ENTRE A BUSCAR MEMORIA GENERAL");
	if (strcmp(esquemaMemoria,"PAGINACION")==0){
			puts("ENTRE A BUSCAR DE PAGINACION");
			retornar =  buscar_en_memoria_paginacion( idElementoABuscar, PID,  tipo);
		    pthread_mutex_unlock(&mutexMemoria);


		}else if(strcmp(esquemaMemoria,"SEGMENTACION")==0){
			puts("ENTRE A BUSCAR DE SEGMENTACION");
			retornar =  buscar_de_memoria_segmentacion( idElementoABuscar,  tipo);
		    pthread_mutex_unlock(&mutexMemoria);

		}
	return retornar;



}
void *borrar_de_memoria_general(int idElemento, int idPatota, char tipo){
    pthread_mutex_lock(&mutexMemoria);

	if (strcmp(esquemaMemoria,"PAGINACION")==0){
			borrar_de_memoria_paginacion( idElemento, idPatota,  tipo);

			}else if(strcmp(esquemaMemoria,"SEGMENTACION")==0){
				buscar_de_memoria_segmentacion( idElemento,tipo);
			}
    pthread_mutex_unlock(&mutexMemoria);


}

void guardar_en_memoria_paginacion(void* payload,int idElemento,int tamPayload,int pid,char tipo){
    int indicePaginaCorrespondiente;
    int indiceTablaCorrespondiente;
    elementoEnLista_struct *nuevoElemento= malloc(sizeof(elementoEnLista_struct));
    nuevoElemento->tipo = tipo;
    tablaEnLista_struct *tablaCorrespondiente = malloc(sizeof(tablaEnLista_struct));
    for (int i = 0; i < list_size(listaDeTablasDePaginas); ++i) {
        tablaEnLista_struct *tablaIterante;
        tablaIterante = list_get(listaDeTablasDePaginas,i);
        if(tablaIterante->idPatota == pid){
            tablaCorrespondiente = tablaIterante;
            indiceTablaCorrespondiente = i;
            break;
        }

    }

    paginaEnTabla_struct *paginaParcialmenteLlena = malloc(sizeof (paginaEnTabla_struct));
    paginaParcialmenteLlena->frame = -1;
    for (int i = 0; i < list_size(tablaCorrespondiente->tablaDePaginas); ++i) {
        paginaEnTabla_struct *paginaIterante;
        paginaIterante = list_get(tablaCorrespondiente->tablaDePaginas,i);
        if(paginaIterante->espacioOcupado < tamPagina && paginaIterante->presencia==1){
            paginaParcialmenteLlena = paginaIterante;
            indicePaginaCorrespondiente = i;
            break;
        }

    }

    int payLoadYaGuardado = 0;
    int *direccionFisica;
    if (paginaParcialmenteLlena->frame != -1){
        paginaParaReemplazar_struct *paginaReemplazable3 = malloc(sizeof(paginaParaReemplazar_struct));
        paginaReemplazable3->uso=1;
        paginaReemplazable3->PID=pid;
        direccionFisica = (int)memoria + (int)(paginaParcialmenteLlena->frame * tamPagina + paginaParcialmenteLlena->espacioOcupado);
        int menorEntre2 = menorEntreDos(tamPayload,(tamPagina-paginaParcialmenteLlena->espacioOcupado));
        memcpy(direccionFisica,payload,menorEntre2);
        nuevoElemento->offsetEnPagina = paginaParcialmenteLlena->espacioOcupado;
        nuevoElemento->segmentoOPagina = indicePaginaCorrespondiente;
        nuevoElemento->tamanio = tamPayload;
        nuevoElemento->ID = idElemento;
        payLoadYaGuardado += menorEntreDos(tamPayload,tamPagina-paginaParcialmenteLlena->espacioOcupado);
        payload = (int)payload + menorEntreDos(tamPayload,tamPagina-paginaParcialmenteLlena->espacioOcupado);
        paginaParcialmenteLlena->espacioOcupado += menorEntreDos(tamPayload,tamPagina - paginaParcialmenteLlena->espacioOcupado);
        list_replace(tablaCorrespondiente->tablaDePaginas,indicePaginaCorrespondiente,paginaParcialmenteLlena);
        while (payLoadYaGuardado<tamPayload) {
            int frameDisponible = encontrarFrameDisponible();
            if (frameDisponible == -1){
                guardar_en_swap(payload,idElemento,tamPayload,pid,tipo);
                break;
            }
            paginaReemplazable3->nroFrame = frameDisponible;
            direccionFisica = (int)memoria + (int)(frameDisponible * tamPagina);
            menorEntre2 = menorEntreDos(tamPagina,tamPayload-payLoadYaGuardado);
            memcpy(direccionFisica,payload, menorEntre2);
            paginaEnTabla_struct *nuevaPagina = malloc(sizeof (paginaEnTabla_struct));
            nuevaPagina->frame = frameDisponible;
            nuevaPagina->presencia = 1;
            nuevaPagina->espacioOcupado = menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
            payLoadYaGuardado = (int)payLoadYaGuardado + menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
            payload = (int)payload + menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
            paginaReemplazable3->nroPagina = list_add(tablaCorrespondiente->tablaDePaginas,nuevaPagina);
            queue_push(tablaDeFrames,paginaReemplazable3);
            bitarrayMemoria[frameDisponible] = 1;
            paginaParaReemplazar_struct *paginaDePrueba = malloc(sizeof(paginaParaReemplazar_struct));

        }

    }
    else {
        int frameDisponible = encontrarFrameDisponible();
        paginaParaReemplazar_struct *paginaReemplazable = malloc(sizeof(paginaParaReemplazar_struct));
        paginaReemplazable->uso=1;
        paginaReemplazable->PID=pid;
        if (frameDisponible == -1) {
            guardar_en_swap(payload, idElemento, tamPayload, pid, tipo);
        } else{
            int *direccionFisica = memoria + (frameDisponible * tamPagina);
            memcpy(direccionFisica, payload, menorEntreDos(tamPayload - payLoadYaGuardado, tamPagina));
            paginaEnTabla_struct *nuevaPagina = malloc(sizeof(paginaEnTabla_struct));
            nuevaPagina->frame = frameDisponible;
            nuevaPagina->presencia = 1;
            nuevaPagina->espacioOcupado = menorEntreDos(tamPayload - payLoadYaGuardado, tamPagina);
            paginaReemplazable->nroFrame = frameDisponible;
            payload = (int) payload + menorEntreDos(tamPayload - payLoadYaGuardado, tamPagina);
            payLoadYaGuardado = (int) payLoadYaGuardado + menorEntreDos(tamPayload - payLoadYaGuardado, tamPagina);
            nuevoElemento->segmentoOPagina = list_add(tablaCorrespondiente->tablaDePaginas, nuevaPagina);
            nuevoElemento->offsetEnPagina = 0;
            nuevoElemento->tamanio = tamPayload;
            nuevoElemento->ID = idElemento;
            paginaReemplazable->nroPagina = nuevoElemento->segmentoOPagina;
            bitarrayMemoria[frameDisponible] = 1;
            queue_push(tablaDeFrames, paginaReemplazable);
            paginaParaReemplazar_struct *paginaDePrueba = malloc(sizeof(paginaParaReemplazar_struct));
            while (payLoadYaGuardado < tamPayload) {
                paginaParaReemplazar_struct *paginaReemplazable2 = malloc(sizeof(paginaParaReemplazar_struct));
                paginaReemplazable2->uso=1;
                paginaReemplazable2->PID=pid;
                int frameDisponible = encontrarFrameDisponible();
                if (frameDisponible == -1) {
                    guardar_en_swap(payload, idElemento, tamPayload, pid, tipo);
                }
                int *direccionFisica = memoria + (frameDisponible * tamPagina);
                memcpy(direccionFisica, payload, menorEntreDos(tamPayload - payLoadYaGuardado, tamPagina));
                paginaEnTabla_struct *nuevaPagina = malloc(sizeof(paginaEnTabla_struct));
                nuevaPagina->frame = frameDisponible;
                nuevaPagina->presencia = 1;
                nuevaPagina->presencia = 1;
                nuevaPagina->espacioOcupado = menorEntreDos(tamPayload - payLoadYaGuardado, tamPagina);
                paginaReemplazable2->nroFrame = frameDisponible;
                payload += menorEntreDos(tamPayload - payLoadYaGuardado, tamPagina);
                payLoadYaGuardado += menorEntreDos(tamPayload - payLoadYaGuardado, tamPagina);
                paginaReemplazable2->nroPagina = list_add(tablaCorrespondiente->tablaDePaginas, nuevaPagina);
                queue_push(tablaDeFrames, paginaReemplazable2);
                bitarrayMemoria[frameDisponible] = 1;
            }
        }
    }

    list_add(listaElementos,nuevoElemento);
    /*memcpy(memoria,payload, tamPayload/2);
    printf("Memoria: %d \n", memoria);
    printf("memoria + 500: %d \n", memoria+500);
    payload += tamPayload/2;
    memcpy(memoria+200,payload, tamPayload/2);
    void* payloadRecompuesto = malloc(tamPayload);
    printf("payloadRecompuesto puntero inicial: %d\n", payloadRecompuesto);
    memcpy(payloadRecompuesto,memoria, tamPayload/2);
    memcpy((payloadRecompuesto+tamPayload/2),memoria+200, tamPayload/2);
    tripulante_struct *payloadMegaCasteado = malloc(tamPayload);
    payloadMegaCasteado = payloadRecompuesto;
    printf("puntero payload recompuesto: %d\n",payloadRecompuesto);
    printf("Pos X del payload recompuesto: %d \n",payloadMegaCasteado->posx);*/
}


void guardar_en_swap(void* payload,int idElemento,int tamPayload,int pid,char tipo){
    int indicePaginaCorrespondiente;
    int indiceTablaCorrespondiente;

    elementoEnLista_struct *nuevoElemento= malloc(sizeof(elementoEnLista_struct));
    nuevoElemento->tipo = tipo;
    tablaEnLista_struct *tablaCorrespondiente = malloc(sizeof(tablaEnLista_struct));

    for (int i = 0; i < list_size(listaDeTablasDePaginas); ++i) {
        tablaEnLista_struct *tablaIterante;
        tablaIterante = list_get(listaDeTablasDePaginas,i);
        if(tablaIterante->idPatota == pid){
            tablaCorrespondiente = tablaIterante;
            indiceTablaCorrespondiente = i;
            break;
        }

    }

    paginaEnTabla_struct *paginaParcialmenteLlena = malloc(sizeof (paginaEnTabla_struct));
    paginaParcialmenteLlena->frame = -1;
    for (int i = 0; i < list_size(tablaCorrespondiente->tablaDePaginas); ++i) {
        paginaEnTabla_struct *paginaIterante;
        paginaIterante = list_get(tablaCorrespondiente->tablaDePaginas,i);
        if(paginaIterante->espacioOcupado < tamPagina && paginaIterante->presencia==0){
            paginaParcialmenteLlena = paginaIterante;
            indicePaginaCorrespondiente = i;
            break;
        }

    }

    int payLoadYaGuardado = 0;
    int *direccionFisica;

    if (paginaParcialmenteLlena->frame != -1){

        direccionFisica = (int)memoriaSwap + (int)(paginaParcialmenteLlena->frame * tamPagina + paginaParcialmenteLlena->espacioOcupado);
        int menorEntre2 = menorEntreDos(tamPayload,(tamPagina-paginaParcialmenteLlena->espacioOcupado));
        memcpy(direccionFisica,payload,menorEntre2);
        nuevoElemento->offsetEnPagina = paginaParcialmenteLlena->espacioOcupado;
        nuevoElemento->segmentoOPagina = indicePaginaCorrespondiente;
        nuevoElemento->tamanio = tamPayload;
        nuevoElemento->ID = idElemento;
        payLoadYaGuardado += menorEntreDos(tamPayload,tamPagina-paginaParcialmenteLlena->espacioOcupado);
        payload = (int)payload + menorEntreDos(tamPayload,tamPagina-paginaParcialmenteLlena->espacioOcupado);
        paginaParcialmenteLlena->espacioOcupado += menorEntreDos(tamPayload,tamPagina - paginaParcialmenteLlena->espacioOcupado);
        list_replace(tablaCorrespondiente->tablaDePaginas,indicePaginaCorrespondiente,paginaParcialmenteLlena);
        while (payLoadYaGuardado<tamPayload) {
            int frameDisponible = encontrarFrameEnSwapDisponible();
            direccionFisica = (int)memoriaSwap + (int)(frameDisponible * tamPagina);
            menorEntre2 = menorEntreDos(tamPagina,tamPayload-payLoadYaGuardado);
            memcpy(direccionFisica,payload, menorEntre2);
            paginaEnTabla_struct *nuevaPagina = malloc(sizeof (paginaEnTabla_struct));
            nuevaPagina->frame = frameDisponible;
            nuevaPagina->presencia = 0;
            nuevaPagina->espacioOcupado = menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
            payLoadYaGuardado = (int)payLoadYaGuardado + menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
            payload = (int)payload + menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
            list_add(tablaCorrespondiente->tablaDePaginas,nuevaPagina);
            bitarrayMemoria[frameDisponible] = 1;
        }

    }
    else{
        int frameDisponible = encontrarFrameEnSwapDisponible();
        int *direccionFisica =(int) memoriaSwap + (frameDisponible * tamPagina);
        memcpy(direccionFisica,payload, menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina));
        paginaEnTabla_struct *nuevaPagina = malloc(sizeof (paginaEnTabla_struct));
        nuevaPagina->frame = frameDisponible;
        nuevaPagina->presencia = 0;
        nuevaPagina->espacioOcupado = menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
        payload =(int)payload + menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
        payLoadYaGuardado =(int)payLoadYaGuardado + menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
        nuevoElemento->segmentoOPagina = list_add(tablaCorrespondiente->tablaDePaginas,nuevaPagina);
        nuevoElemento->offsetEnPagina=0;
        nuevoElemento->tamanio = tamPayload;
        nuevoElemento->ID = idElemento;
        bitarraySwap[frameDisponible] = 1;

        while (payLoadYaGuardado<tamPayload) {

            int frameDisponible = encontrarFrameEnSwapDisponible();
            int *direccionFisica = (int) memoriaSwap + (frameDisponible * tamPagina);
            memcpy(direccionFisica,payload, menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina));
            paginaEnTabla_struct *nuevaPagina = malloc(sizeof (paginaEnTabla_struct));
            nuevaPagina->frame = frameDisponible;
            nuevaPagina->presencia=0;
            nuevaPagina->espacioOcupado = menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
            payload +=menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
            payLoadYaGuardado += menorEntreDos(tamPayload-payLoadYaGuardado,tamPagina);
            list_add(tablaCorrespondiente->tablaDePaginas,nuevaPagina);
            bitarraySwap[frameDisponible] = 1;
        }

    }

    list_add(listaElementos,nuevoElemento);
}




void* minimo_segmentos_en_tabla(void *elem1,void*elem2){
    segmentoEnTabla_struct *elem1bis = elem1;
    segmentoEnTabla_struct *elem2bis = elem2;
    if(elem1bis->inicio < elem2bis->inicio){
        return elem1bis;
    } else{
        return elem2bis;
    }

}
void* minimo_hueco_libre(void *elem1,void*elem2){
    espacio_struct *elem1bis = elem1;
    espacio_struct *elem2bis = elem2;
    if(elem1bis->tamanio < elem2bis->tamanio){
        return elem1bis;
    } else{
        return elem2bis;
    }

}
bool ordenar_por_posicion(void *tam1, void *tam2){
    segmentoEnTabla_struct *tam1bis = tam1;
    segmentoEnTabla_struct *tam2bis = tam2;
    return (tam1bis->inicio < tam2bis->inicio);

}

bool ordenar_por_nro_frame(void *pag1, void *pag2){
    paginaParaReemplazar_struct *pag1bis = pag1;
    paginaParaReemplazar_struct  *pag2bis = pag2;
    return (pag1bis->nroFrame < pag2bis->nroFrame);

}

bool filtrarPorTipo(void* elemento){
    elementoEnLista_struct *comparador = elemento;
    return comparador->tipo == tipoUniversal;
}

void traerPaginaAMemoria(paginaEnTabla_struct* paginaATraer, t_list* tablaDePaginas,int indiceDeLaPaginaATraer,int PID){
    paginaParaReemplazar_struct *paginaAReemplazar = malloc(sizeof(paginaParaReemplazar_struct));
    if (strcmp(algoritmoReemplazo,"LRU")==0){
        paginaAReemplazar = queue_pop(tablaDeFrames);
    } else{
        while (1) {
            list_sort(tablaDeFrames->elements,ordenar_por_nro_frame);
            paginaAReemplazar = list_get(tablaDeFrames->elements, punteroReemplazo);
            if (paginaAReemplazar->uso==1){
                paginaAReemplazar->uso = 0;
                list_replace(tablaDeFrames->elements,punteroReemplazo,paginaAReemplazar);
                if (punteroReemplazo+1 == queue_size(tablaDeFrames)){
                    punteroReemplazo = 0;
                } else{
                    punteroReemplazo++;
                }
            } else{
                list_remove(tablaDeFrames->elements,punteroReemplazo);
                break;
            }
        }

    }

    int* direccionFisicaPaginaEnSwap = (int)memoriaSwap + (paginaATraer->frame * tamPagina);
    int* direccionFisicaPaginaEnMemoria = (int) memoria + (paginaAReemplazar->nroFrame * tamPagina);
    void* direccionAuxiliar = malloc(tamPagina);

    memcpy(direccionAuxiliar,direccionFisicaPaginaEnMemoria,tamPagina);
    memcpy(direccionFisicaPaginaEnMemoria,direccionFisicaPaginaEnSwap,tamPagina);
    memcpy(direccionFisicaPaginaEnSwap,direccionAuxiliar,tamPagina);
    paginaEnTabla_struct *paginaAActualizar = malloc(sizeof(paginaEnTabla_struct));
    //paginaAActualizar = list_get(tablaDePaginas,indiceDeLaPaginaATraer);
    memcpy(paginaAActualizar,list_get(tablaDePaginas,indiceDeLaPaginaATraer),tamPagina);
    //printf("Pagina a reemplazar: %d \n",paginaAReemplazar->nroPagina);
    //printf("Pagina a traer: %d \n",paginaATraer->frame);
    paginaAActualizar->presencia = 1;
    paginaAActualizar->frame = paginaAReemplazar->nroFrame;
    list_replace(tablaDePaginas,indiceDeLaPaginaATraer,paginaAActualizar);
    paginaParaReemplazar_struct *paginaAReponer = malloc(sizeof(paginaParaReemplazar_struct));
    paginaAReponer->nroFrame=paginaAReemplazar->nroFrame;
    paginaAReponer->nroPagina = indiceDeLaPaginaATraer;
    paginaAReponer->uso = 1;
    paginaAReponer->PID = PID;
    queue_push(tablaDeFrames,paginaAReponer);
    tablaEnLista_struct *tablaBuscada = malloc(sizeof(tablaEnLista_struct));
    t_list *tablaDePaginasBuscada;
    for (int i = 0; i < list_size(listaDeTablasDePaginas); ++i) {
        tablaBuscada = list_get(listaDeTablasDePaginas,i);
        if (tablaBuscada->idPatota == paginaAReemplazar->PID){
            tablaDePaginasBuscada = tablaBuscada->tablaDePaginas;
            break;
        }
    }
    paginaEnTabla_struct *paginaAActualizar2 = malloc(sizeof(paginaEnTabla_struct));
    paginaAActualizar2 = list_get(tablaDePaginasBuscada,paginaAReemplazar->nroPagina);
    paginaAActualizar2->presencia = 0;
    paginaAActualizar2->frame = paginaATraer->frame;
    list_replace(tablaDePaginasBuscada,paginaAReemplazar->nroPagina,paginaAActualizar2);

    //free(direccionAuxiliar);
}

void* buscar_en_memoria_paginacion(int idElementoABuscar,int PID, char tipo){
    tipoUniversal = tipo;
    printf("TIPO: %c \n",tipo);
    t_list* listaFiltrada = list_filter(listaElementos,filtrarPorTipo);
    elementoEnLista_struct *elementoEvaluado = malloc(sizeof(elementoEnLista_struct));
    int paginaInicial= -1,offset=-1,tamanioPayload=-1;
    for (int i = 0; i < list_size(listaFiltrada); ++i) {
        elementoEvaluado = list_get(listaFiltrada,i);
        if (elementoEvaluado->ID == idElementoABuscar){
            paginaInicial = elementoEvaluado->segmentoOPagina;
            offset = elementoEvaluado->offsetEnPagina;
            tamanioPayload = elementoEvaluado->tamanio;
            break;
        }
    }
    if (paginaInicial == -1){
        printf("El elemento buscado no existe \n");
        return 0;
    }
    tablaEnLista_struct *tablaBuscada = malloc(sizeof(tablaEnLista_struct));
    t_list *tablaDePaginas;
    for (int i = 0; i < list_size(listaDeTablasDePaginas); ++i) {
        tablaBuscada = list_get(listaDeTablasDePaginas,i);
        if (tablaBuscada->idPatota == PID){
            tablaDePaginas = tablaBuscada->tablaDePaginas;
            printf("El id de la patota: %d \n", tablaBuscada->idPatota);
            break;
        }
    }

    void* payloadADevolver = malloc(tamanioPayload);
    int tamanioPorGuardar = tamanioPayload;
    printf("Tamanio del payload a devolver: %d \n",tamanioPayload);
    paginaEnTabla_struct *paginaDeLectura = malloc(sizeof(paginaEnTabla_struct));
    int movimientoDepagina=1;
    paginaDeLectura = list_get(tablaDePaginas,paginaInicial);
    if (paginaDeLectura->presencia == 0){
        traerPaginaAMemoria(paginaDeLectura,tablaDePaginas,paginaInicial,PID);
        paginaDeLectura = list_get(tablaDePaginas,paginaInicial);
    }else{
        paginaParaReemplazar_struct *paginaAux = malloc(sizeof(paginaParaReemplazar_struct));
        paginaAux = list_remove(tablaDeFrames->elements,paginaInicial);
        queue_push(tablaDeFrames,paginaAux);
    }
    int* direccionFisicaDeLaPagina;
    direccionFisicaDeLaPagina = memoria + (paginaDeLectura->frame * tamPagina + offset);
    memcpy(payloadADevolver,direccionFisicaDeLaPagina, menorEntreDos(tamanioPayload,tamPagina-offset));
    payloadADevolver += menorEntreDos(tamanioPayload,tamPagina-offset);
    tamanioPorGuardar -= menorEntreDos(tamanioPayload,tamPagina-offset);
    printf("Tamanio por guardar antes del while: %d \n",tamanioPorGuardar);
    while(tamanioPorGuardar>0){
        paginaDeLectura = list_get(tablaDePaginas,paginaInicial+movimientoDepagina);
        if (paginaDeLectura->presencia == 0){
            traerPaginaAMemoria(paginaDeLectura,tablaDePaginas, paginaInicial+movimientoDepagina,PID);
            paginaDeLectura = list_get(tablaDePaginas,paginaInicial+movimientoDepagina);
        }else{
            paginaParaReemplazar_struct *paginaAux = malloc(sizeof(paginaParaReemplazar_struct));
            paginaAux = list_remove(tablaDeFrames->elements,paginaInicial);
            queue_push(tablaDeFrames,paginaAux);
        }
        direccionFisicaDeLaPagina = (int)memoria + (paginaDeLectura->frame * tamPagina);
        memcpy(payloadADevolver,direccionFisicaDeLaPagina, menorEntreDos(tamanioPorGuardar,tamPagina));
        payloadADevolver += menorEntreDos(tamanioPorGuardar,tamPagina);
        tamanioPorGuardar -= menorEntreDos(tamanioPorGuardar,tamPagina);
        movimientoDepagina ++;
    }

    payloadADevolver -= tamanioPayload;
    printf("el payload que devuelve es: %s", payloadADevolver);
    return payloadADevolver;

}
void actualizarListaElementos(int paginaEliminada,int PID){
    for (int i = 0; i < list_size(listaElementos); ++i) {
        elementoEnLista_struct *elementoIterante = list_get(listaElementos,i);
        if(elementoIterante->segmentoOPagina > paginaEliminada){
            elementoIterante->segmentoOPagina -= 1;
            list_replace(listaElementos,i,elementoIterante);
        }
    }
}
void *borrar_de_memoria_paginacion(int idElemento, int idPatota, char tipo){
	tipoUniversal = tipo;
    t_list* listaFiltrada = list_filter(listaElementos,filtrarPorTipo);
    elementoEnLista_struct *elementoEvaluado = malloc(sizeof(elementoEnLista_struct));
    int paginaInicial,offset,tamanioPayload,posicionElementoEvaluado;
    for (int i = 0; i < list_size(listaFiltrada); ++i) {
        elementoEvaluado = list_get(listaFiltrada,i);
        if (elementoEvaluado->ID == idElemento){
            paginaInicial = elementoEvaluado->segmentoOPagina;
            offset = elementoEvaluado->offsetEnPagina;
            tamanioPayload = elementoEvaluado->tamanio;
            posicionElementoEvaluado = i;
            break;
        }
    }
    tablaEnLista_struct *tablaBuscada = malloc(sizeof(tablaEnLista_struct));
    t_list *tablaDePaginas;
    for (int i = 0; i < list_size(listaDeTablasDePaginas); ++i) {
        tablaBuscada = list_get(listaDeTablasDePaginas,i);
        if (tablaBuscada->idPatota == idPatota){
            tablaDePaginas = tablaBuscada->tablaDePaginas;
            break;
        }
    }

    if ((offset == 0 && tamanioPayload >= tamPagina) || (offset == 0 && paginaInicial == list_size(tablaDePaginas)-1)){
        bitarrayMemoria[paginaInicial] = 0;
        list_remove_and_destroy_element(tablaDePaginas,paginaInicial,free);
        list_remove_and_destroy_element(listaElementos,posicionElementoEvaluado,free);
        actualizarListaElementos(paginaInicial,idPatota);
    }else if(offset!=0){
        paginaEnTabla_struct *paginaAActualizar = list_get(tablaDePaginas,paginaInicial);
        paginaAActualizar->espacioOcupado = offset;
        list_replace(tablaDePaginas,paginaInicial,paginaAActualizar);
        list_remove_and_destroy_element(listaElementos,posicionElementoEvaluado,free);
    }
}

void guardar_en_memoria_segmentacion(void* payload,int idElemento,int tamPayload,int pid,char tipo, int tipoDeGuardado)
{
    int huecoLibre;
    if (list_is_empty(listaSegmentos) == 1)
    {
        //Guarda directamente en memoria y lo agrega a la lista de segmentos
        //*(tripulante_struct *)memoria = *(tripulante_struct *)payload;
        memcpy(memoria,payload,tamPayload);
        printf("TamPayload: %d\n",tamPayload);
        segmentoEnTabla_struct *nuevoSegmento = malloc(sizeof(segmentoEnTabla_struct));
        nuevoSegmento->inicio = memoria;
        nuevoSegmento->tamanio = tamPayload;
        elementoEnLista_struct *elementoNuevo = malloc(sizeof(elementoEnLista_struct));
        int segmentoGuardado = list_add_sorted(listaSegmentos,nuevoSegmento,ordenar_por_posicion);
        elementoNuevo->segmentoOPagina = segmentoGuardado;
        elementoNuevo->offsetEnPagina=0;
        elementoNuevo->tipo = tipo;
        elementoNuevo->tamanio = tamPayload;
        elementoNuevo->ID = idElemento;
        list_add(listaElementos,elementoNuevo);
    }
    else
    {
        int tamanioListaSegmentos = list_size(listaSegmentos);
        switch (tipoDeGuardado) {
            case FIRSTFIT: {
                if (((segmentoEnTabla_struct *)(list_get(listaSegmentos, 0)))->inicio != memoria){
                    huecoLibre = ((segmentoEnTabla_struct *)(list_get(listaSegmentos, 0)))->inicio - (int*)memoria;
                    if (huecoLibre >= tamPayload){
                        segmentoEnTabla_struct *nuevoSegmento = malloc(sizeof(segmentoEnTabla_struct));
                        nuevoSegmento->inicio = memoria;
                        nuevoSegmento->tamanio = tamPayload;
                        elementoEnLista_struct *elementoNuevo = malloc(sizeof(elementoEnLista_struct));
                        elementoNuevo->segmentoOPagina = list_add_sorted(listaSegmentos,nuevoSegmento,ordenar_por_posicion);
                        elementoNuevo->offsetEnPagina=0;
                        elementoNuevo->tipo = tipo;
                        elementoNuevo->tamanio = tamPayload;
                        elementoNuevo->ID = idElemento;
                        list_add(listaElementos,elementoNuevo);
                    }

                }
                for (int i = 0; i < tamanioListaSegmentos; i++) {
                    segmentoEnTabla_struct *segmentoIterante;
                    segmentoIterante = list_get(listaSegmentos, i);
                    if (i + 1 == (list_size(listaSegmentos))) {

                        huecoLibre = (int) (memoria + 1024) -
                                     (int) ((segmentoIterante->inicio) + (segmentoIterante->tamanio));
                    } else{

                        segmentoEnTabla_struct *segmentoSiguiente = list_get(listaSegmentos, i + 1);
                        huecoLibre =(segmentoSiguiente->inicio) - ((segmentoIterante->inicio) + segmentoIterante->tamanio);
                    }

                    if (tamPayload <= huecoLibre) {

                        int *posicionInicioHuecoLibre = (int)(segmentoIterante->inicio) + (segmentoIterante->tamanio);
                        //*(tripulante_struct *) posicionInicioHuecoLibre = tcb;
                        memcpy(posicionInicioHuecoLibre,payload,tamPayload);
                        printf("TamPayload: %d\n",tamPayload);
                        segmentoEnTabla_struct *nuevoSegmento = malloc(sizeof(segmentoEnTabla_struct));
                        nuevoSegmento->inicio = posicionInicioHuecoLibre;
                        nuevoSegmento->tamanio = tamPayload;
                        elementoEnLista_struct *elementoNuevo = malloc(sizeof(elementoEnLista_struct));
                        //int segmentoGuardado = list_add(listaSegmentos, nuevoSegmento);
                        elementoNuevo->segmentoOPagina = list_add_sorted(listaSegmentos,nuevoSegmento,ordenar_por_posicion);
                        //elementoNuevo->segmentoOPagina = segmentoGuardado;
                        elementoNuevo->offsetEnPagina = 0;
                        elementoNuevo->tipo = tipo;
                        elementoNuevo->tamanio = tamPayload;
                        elementoNuevo->ID = idElemento;
                        list_add(listaElementos,elementoNuevo);
                    }
                }
                if (tamanioListaSegmentos == list_size(listaSegmentos)){
                   	compactacion();

                }
                break;
            }

            case BESTFIT:{

                t_list *listaDeEspaciosLibres;
                listaDeEspaciosLibres = list_create();
                if ((int)((segmentoEnTabla_struct *)(list_get(listaSegmentos, 0)))->inicio != (int)memoria){
                    huecoLibre = (int)((segmentoEnTabla_struct *)(list_get(listaSegmentos, 0)))->inicio - (int)memoria;
                    if (huecoLibre >= tamPayload){
                        espacio_struct *nuevoHuecoLibre = malloc(sizeof (espacio_struct)) ;
                        nuevoHuecoLibre->tamanio = huecoLibre;
                        nuevoHuecoLibre->ptrHuecoLibre = memoria;
                        list_add(listaDeEspaciosLibres,nuevoHuecoLibre);

                    }
                }
                for (int i = 0; i < tamanioListaSegmentos; i++) {
                    segmentoEnTabla_struct *segmentoIterante;
                    segmentoIterante = list_get(listaSegmentos, i);

                    if (i + 1 == (list_size(listaSegmentos))) {

                        huecoLibre = (int) (memoria + 1024) -
                                     (int) ((segmentoIterante->inicio) + (segmentoIterante->tamanio));
                    } else {

                        segmentoEnTabla_struct *segmentoSiguiente = list_get(listaSegmentos, i + 1);
                        huecoLibre =
                                (segmentoSiguiente->inicio) - ((segmentoIterante->inicio) + segmentoIterante->tamanio);
                    }

                    if (tamPayload <= huecoLibre) {
                        int *posicionInicioHuecoLibre = (int)(segmentoIterante->inicio) + (segmentoIterante->tamanio);
                        espacio_struct *nuevoHuecoLibre = malloc(sizeof (espacio_struct)) ;
                        nuevoHuecoLibre->tamanio = huecoLibre;
                        nuevoHuecoLibre->ptrHuecoLibre = posicionInicioHuecoLibre;
                        list_add(listaDeEspaciosLibres,nuevoHuecoLibre);
                    }


                }
                if (list_is_empty(listaDeEspaciosLibres) == 1){
                    //aca va la compactacion
                	compactacion();

                }
                else{
                    espacio_struct *punteroHuecoMinimo;
                    punteroHuecoMinimo = list_get_minimum(listaDeEspaciosLibres,minimo_hueco_libre);
                    //*(tripulante_struct *) punteroHuecoMinimo->ptrHuecoLibre = tcb;
                    memcpy(punteroHuecoMinimo->ptrHuecoLibre,payload,tamPayload);
                    segmentoEnTabla_struct *nuevoSegmento = malloc(sizeof(segmentoEnTabla_struct));
                    nuevoSegmento->inicio = punteroHuecoMinimo->ptrHuecoLibre;
                    nuevoSegmento->tamanio = tamPayload;
                    elementoEnLista_struct *elementoNuevo = malloc(sizeof(elementoEnLista_struct));
                    int segmentoGuardado = list_add_sorted(listaSegmentos,nuevoSegmento,ordenar_por_posicion);
                    elementoNuevo->segmentoOPagina = segmentoGuardado;
                    elementoNuevo->offsetEnPagina=0;
                    elementoNuevo->tipo = tipo;
                    elementoNuevo->tamanio = tamPayload;
                    elementoNuevo->ID = idElemento;
                    list_add(listaElementos,elementoNuevo);
                    printf("Guardado \n");
                }
                list_destroy(listaDeEspaciosLibres);
                break;
            }
        }
    }
}


void borrar_de_memoria_segmentacion(int idElementoABorrar, char tipoDeElemento){
    tipoUniversal = tipoDeElemento;
    t_list* listaFiltrada = list_filter(listaElementos,filtrarPorTipo);
    for (int i=0;i< list_size(listaFiltrada);i++){
        elementoEnLista_struct *elementoEvaluado = list_get(listaFiltrada,i);
        segmentoEnTabla_struct *segmentoEvaluado = list_get(listaSegmentos,elementoEvaluado->segmentoOPagina);
        if (tipoDeElemento == 'T'){
            if (elementoEvaluado->ID== idElementoABorrar){
            	list_remove_and_destroy_element(listaSegmentos,elementoEvaluado->segmentoOPagina,free);
                printf("Elemento %d borrado de la lista de segmentos \n",elementoEvaluado->segmentoOPagina);
                break;
            }
        }else if(tipoDeElemento == 'P'){

            if (elementoEvaluado->ID == idElementoABorrar){
                list_remove_and_destroy_element(listaSegmentos,elementoEvaluado->segmentoOPagina,free);
                printf("Elemento %d borrado de la lista de segmentos \n",elementoEvaluado->segmentoOPagina);
            }
        }else if(tipoDeElemento =='A'){
        	if(elementoEvaluado->ID==idElementoABorrar){
        		list_remove_and_destroy_element(listaSegmentos,elementoEvaluado->segmentoOPagina,free);
        		printf("Elemento %d borrado de la lista de segmentos \n",elementoEvaluado->segmentoOPagina);

        	}
        }
    }

}

void *buscar_de_memoria_segmentacion(int idElementoABuscar, char tipoDeElemento){
	tipoUniversal = tipoDeElemento;
	printf("tipo de elemento: %c \n", tipoDeElemento);
    t_list* listaFiltrada = list_filter(listaElementos,filtrarPorTipo);
    for (int s=0;s< list_size(listaFiltrada);s++){
        elementoEnLista_struct *elementoEvaluado = malloc(sizeof(elementoEnLista_struct));
        elementoEvaluado= list_get(listaFiltrada,s);
        segmentoEnTabla_struct *segmentoEvaluado = malloc(sizeof(segmentoEnTabla_struct));
        segmentoEvaluado = list_get(listaSegmentos,elementoEvaluado->segmentoOPagina);

        if (tipoDeElemento == 'T'){
            Tripulante* elementoABuscar = malloc(sizeof(Tripulante));
            //elementoABuscar = (tripulante_struct*)segmentoEvaluado->inicio;
            memcpy(elementoABuscar,segmentoEvaluado->inicio, sizeof(Tripulante));
            if (elementoABuscar->id == idElementoABuscar){
                return elementoABuscar;
            }else{
                //free(elementoEvaluado);
                //free(segmentoEvaluado);
            }
        }else if(tipoDeElemento == 'P'){
            pcb *elementoABuscar = malloc(sizeof(pcb));
            if (elementoEvaluado->ID== idElementoABuscar){
            	memcpy(elementoABuscar,segmentoEvaluado->inicio,elementoEvaluado->tamanio);
            	return elementoABuscar;
            }
        }else if(tipoDeElemento == 'A'){

            char *elementoABuscar = malloc(elementoEvaluado->tamanio);
            printf("ELEMENTO EVALUADO.TAMANIO:%d \n", elementoEvaluado->tamanio);
            if (elementoEvaluado->ID== idElementoABuscar){
            	memcpy(elementoABuscar,segmentoEvaluado->inicio,elementoEvaluado->tamanio);
            	return elementoABuscar;
            }
        }
    }
    return 0;
}

void compactacion(){
    for (int i =0;i<list_size(listaSegmentos);i++){
        if(i==0){
            segmentoEnTabla_struct *primerSegmento = list_get(listaSegmentos,0);
            if(primerSegmento->inicio != memoria){
                memcpy(memoria,primerSegmento->inicio,primerSegmento->tamanio);
                primerSegmento->inicio = memoria;
                list_replace(listaSegmentos,0,primerSegmento);
            }
        }else{
            segmentoEnTabla_struct *segmentoActual = list_get(listaSegmentos,i);
            segmentoEnTabla_struct *segmentoAnterior = list_get(listaSegmentos,i-1);
            if ((int)segmentoActual->inicio != ((int)segmentoAnterior->inicio + segmentoAnterior->tamanio)){
                memcpy((int)segmentoAnterior->inicio+segmentoAnterior->tamanio,segmentoActual->inicio,segmentoActual->tamanio);
                segmentoActual->inicio = (int)segmentoAnterior->inicio+segmentoAnterior->tamanio;
                list_replace(listaSegmentos,i,segmentoActual);
            }
        }
    }
}
