/*************************************************************************
 * AUTHORS
 *    Gara Miranda Valladares
 *    Carlos Segura Gonzalez
 *
 * DATE
 *    July 2007
 *
 * DESCRIPTION
 *    Each gene is mutated following the rules of a polynomial mutation
 *    Code obtained from DEME
 *    http://neo.lcc.uma.es/Software/deme/html/RealGene_8cpp-source.html
 ************************************************************************/

#ifndef MUTATE_POLYNOMIAL_ALL_H
#define MUTATE_POLYNOMIAL_ALL_H

#include "Mutation.h"

class Mutate_Polynomial_All : public Mutation {
	void mutation(Individual *ind, const double pm);
};

#endif
