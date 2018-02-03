/*************************************************************************
 * AUTHORS
 *    Gara Miranda Valladares
 *    Carlos Segura Gonzalez
 *
 * Nota: no veo la relacion entre el codigo y lo que aparece en el libro
 * "Multi-Objective Optimization using evolutionary algorithms", pero esta
 * igual que en Paradiseo, JMetal y PISA así que supongo que el código está
 * bien y que lo que no estoy entendiendo es la explicación del libro 
 * (Carlos)
 *************************************************************************/ 

#include "Mutate_Polynomial_All.h"

#include <math.h>

void Mutate_Polynomial_All::mutation(Individual *ind, const double pm){
	const double n = 20;			//(distribution index)
	double delta, rnd, deltaq, mu, temp;

	for (unsigned int i = 0; i < ind->getNumberOfVar(); i++){
		double vmut = ((double) rand()) / (double) (RAND_MAX);
		if (vmut < pm) {
			if ((ind->getVar(i) - ind->getMinimum(i)) < (ind->getMaximum(i) - ind->getVar(i)))
				delta = (ind->getVar(i) - ind->getMinimum(i)) / (ind->getMaximum(i) - ind->getMinimum(i));
			else
				delta = (ind->getMaximum(i) - ind->getVar(i)) / (ind->getMaximum(i) - ind->getMinimum(i));
			rnd = ((double)rand())/(RAND_MAX);
			mu = 1.0/(n + 1);
			if (rnd <= 0.5){
				double xy = 1.0-delta;
				temp = 2*rnd+(1-2*rnd)*(pow(xy,(n+1)));
				deltaq = pow(temp, mu) - 1.0;
			} else{
				double xy = 1.0-delta;
				temp = 2.0*(1.0-rnd)+2.0*(rnd-0.5)*(pow(xy,(n + 1)));
				deltaq = 1.0 - (pow(temp,mu));
			}
			ind->setVar(i, ind->getVar(i) + deltaq *(ind->getMaximum(i) - ind->getMinimum(i)));
			if (ind->getVar(i) > ind->getMaximum(i))
				ind->setVar(i, ind->getMaximum(i));
			if (ind->getVar(i) < ind->getMinimum(i))
				ind->setVar(i, ind->getMinimum(i));
		}
	}
}

