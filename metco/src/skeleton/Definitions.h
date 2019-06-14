/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 *
 * DESCRIPCION
 *    Definiciones generales usadas por el esqueleto 
 ***********************************************************************************/


#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

// Direcciones de optimizacion
#define MINIMIZE 0
#define MAXIMIZE 1

// Tipos de mensajes de envio/recepcion
#define FINISH    400
#define PARETO    401
#define THE_BEST  402
#define MIGRATION 403
#define ALG_TAG   404
#define INIT_IND  405
#define ISLAND_INFO 406
#define NUM_EVALUATIONS 407
#define USER_COMMUNICATION 408
#define LOCAL_STOP 409
#define LOCAL_SCORE 410

// Orden de finalizacion
#define ALG_FINISH 9999

#endif
