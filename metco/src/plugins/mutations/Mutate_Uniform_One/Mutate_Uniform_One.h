/*************************************************************************
 * AUTHORS
 *    Gara Miranda Valladares
 *    Carlos Segura Gonzalez
 *************************************************************************/


#ifndef MUTATE_UNIFORM_ONE_H
#define MUTATE_UNIFORM_ONE_H

#include "Mutation.h"

class Mutate_Uniform_One : public Mutation {
	void mutation(Individual *ind, const double pm);
};

#endif
