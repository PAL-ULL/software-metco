/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo
 *   - Carlos Segura
 * 
 * *********************************************************************************/

/* 
Implementación del Parent-Centric BLX con parámetro alpha

Para más detalles véase:

C. Garcia-Martinez, M. Lozano, F. Herrera, D. Molina, A.M. Sanchez.
Global and local real-coded genetic algorithms based on parent-centric
crossover operators
European Journal of Operational Research 185 (2008)

Nota: En esta implementación el parámetro alpha vale 0.5

En vez de devolver un unico offspring como en el caso de la propuesta original,
devuelve dos offsprings. Uno se calcula tomando ind1 como macho e ind2 como hembra
y el otro se calcula tomando ind1 como hembra e ind2 como macho
*/


#ifndef CROSSOVER_PBX_ALPHA_H
#define CROSSOVER_PBX_ALPHA_H

#include "Crossover.h"
#include "utils.h"

class Crossover_PBX_Alpha : public Crossover {
	void crossover(Individual *ind1, Individual *ind2);
};

#endif
