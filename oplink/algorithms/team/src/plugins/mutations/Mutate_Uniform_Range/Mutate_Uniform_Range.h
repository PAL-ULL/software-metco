/*************************************************************************
 * AUTHORS
 *    Gara Miranda Valladares
 *    Carlos Segura Gonzalez
 *************************************************************************/

#ifndef MUTATE_UNIFORM_RANGE_H
#define MUTATE_UNIFORM_RANGE_H

#include "Mutation.h"

class Mutate_Uniform_Range : public Mutation {
	void mutation(Individual *ind, const double pm);
};

#endif
