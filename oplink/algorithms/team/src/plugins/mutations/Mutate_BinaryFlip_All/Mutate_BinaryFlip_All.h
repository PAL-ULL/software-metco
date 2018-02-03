/************************************************************************
 * AUTHORS
 *    Gara Miranda Valladares
 *    Eduardo Segredo González
 *    Carlos Segura Gonzalez
 *
 * DATE
 *    July 2007
 *
 * Description
 *    Each gene is flipped with the specified probabilty
 ************************************************************************/

#ifndef MUTATE_BINARYFLIP_ALL_H
#define MUTATE_BINARYFLIP_ALL_H

#include "Mutation.h"

class Mutate_BinaryFlip_All: public Mutation {
	void mutation(Individual *ind, const double pm);
};

#endif
