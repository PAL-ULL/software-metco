/***********************************************************************************
 * AUTHORS
 *   - ANTONIO J. NEBRO
 *
 * NOTES
 *   JMetal Implementation
 * *********************************************************************************/

#include "Crossover_SBX2.h"

#include <math.h>

void Crossover_SBX2::crossover (Individual *ind1, Individual *ind2) {
	const double EPSILON = 1e-10 ;
	const double distributionIndex = 20 ;
	double y1, y2, c1, c2 ;
	double alpha, beta, betaq ;
	
	for (int i = 0; i < ind1->getNumberOfVar(); i++) {
		if ((double)rand()/RAND_MAX <= 0.5) {
			if (ind1->getVar(i) - ind2->getVar(i) > EPSILON) {
				if (ind1->getVar(i) < ind2->getVar(i)) {
					y1 = ind1->getVar(i);
					y2 = ind2->getVar(i);
				} // if
				else {
					y1 = ind2->getVar(i);
					y2 = ind1->getVar(i);
				} // else
				double random ;
				random = (double)rand()/RAND_MAX ;
				beta = 1.0 + (2.0*(y1-ind1->getMinimum(i))/(y2-y1)) ;
				alpha = 2.0 - pow(beta, -(distributionIndex+1.0)) ; 
				if (random <= (1.0/alpha)) 
					betaq = pow(random*alpha, 1.0/(distributionIndex+1.0)) ;
				else
					betaq = pow(1.0/(2.0 - random*alpha), 1.0/(distributionIndex+1.0)) ;
				
				c1 = 0.5*((y1+y2)-betaq*(y2-y1)) ;
				beta = 1.0 + (2.0*(ind1->getMaximum(i)-y2)/(y2-y1)) ;
				alpha = 2.0 - pow(beta, -(distributionIndex+1.0)) ;
				
				if (random <= (1.0/alpha)) 
					betaq = pow((random*alpha),(1.0/(distributionIndex+1.0))) ;
				else
					betaq = pow((1.0/(2.0 - random*alpha)),(1.0/(distributionIndex+1.0))) ;
					
				c2 = 0.5*((y1+y2)+betaq*(y2-y1)) ;
				
				if (c1 < ind1->getMinimum(i)) c1 = ind1->getMinimum(i) ;
				if (c2 < ind1->getMinimum(i)) c2 = ind1->getMinimum(i) ;
				if (c1 > ind1->getMaximum(i)) c1 = ind1->getMaximum(i) ;
				if (c2 > ind1->getMaximum(i)) c2 = ind1->getMaximum(i) ;
				
				if ((double)rand()/RAND_MAX <= 0.5) {
					ind1->setVar(i, c2) ;
					ind2->setVar(i, c1) ;
				} // if
				else {
					ind1->setVar(i, c1) ;
					ind2->setVar(i, c2) ;
				} // else
			} // if
		} // if
		else {
			double tmp ;
			tmp = ind1->getVar(i) ;
			ind1->setVar(i, ind2->getVar(i)) ;
			ind2->setVar(i, tmp) ;
		} // else
	} // if
}
