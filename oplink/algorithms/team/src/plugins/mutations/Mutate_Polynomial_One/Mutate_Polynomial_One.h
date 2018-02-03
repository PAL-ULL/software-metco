#ifndef MUTATE_POLYNOMIAL_ONE_H
#define MUTATE_POLYNOMIAL_ONE_H

#include "Mutation.h"

// IMPLEMENTADO POR ANTONIO J. NEBRO (15/02/2008). Copiado de la version Java
// incluida en jMetal (http://neo.lcc.uma.es/metal)
class Mutate_Polynomial_One : public Mutation {
	void mutation(Individual *ind, const double pm);
};

#endif
