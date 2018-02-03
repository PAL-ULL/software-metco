/*************************************************************************
 * AUTHORS
 *    Gara Miranda Valladares
 *    Carlos Segura Gonzalez
 *************************************************************************/

#ifndef MUTATE_UNIFORM_H
#define MUTATE_UNIFORM_H

#include "Mutation.h"

class Mutate_Uniform_All : public Mutation {
	void mutation(Individual *ind, const double pm);
};

#endif
