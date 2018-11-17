/***********************************************************************************
 * AUTHOR
 *   - Alejandro Marrero
 *
 * DATE
 *   November 2018
 *
 * DESCRIPTION
 *
 * Class which represents the Tchebycheff decomposition approach
 *
 * **********************************************************************************/

#ifndef __TCHEBYCHEFF_H__
#define __TCHEBYCHEFF_H__

#include "Decomposition.h"

class Tchebycheff : public Decomposition {
public:
  virtual double evaluate(Individual* ind, Individual* reference, std::vector<double>& weights);
};


#endif
