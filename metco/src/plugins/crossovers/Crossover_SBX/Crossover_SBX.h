/***********************************************************************************
 * AUTHORS
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * *********************************************************************************/



//Simulated Binary Crossover
//Explicación en "Simulated Binary Crossover for continuous search space"
//Kalyanmoy Deb and Ram Bhusan Agrawal
//
//La implementacion sigue lo explicado en el paper. Ademas, para chequearlo se uso el
//codigo: http://dev.heuristiclab.com/trac/hl/core/browser/trunk/sources/HeuristicLab.Encodings.RealVectorEncoding/3.3/Crossovers/SimulatedBinaryCrossover.cs
//
//Hay otros codigos del propio Deb en los que se hace un SBX diferente a lo que se expone en el articulo
//En los foros de JMetal se dice que desde el principio lo que se decia en el paper y lo que se implemento
//era diferente
//En SBX2 esta la implementacion que hay actualmente en JMetal. Esa esta basada en el codigo de Deb, no en las funciones de
//probabilidad propuestas en el paper
//
//The paper define beta and analyze the probability distribution for beta
//in the binary single point crossover
//SBX have a similar probability distribution for beta, but it is applied
//to real numbers.
//
//The explication of code is not in the paper
//
//When beta < 1 => contracting crossover: p(beta) = 0.5 (n+1) (beta**n)
//When beta > 1 => expanding crossover:   p(beta) = 0.5 (n+1) * (1/(beta**(n+2)))
//
//Integrating can be obtained:
//- contracting: F(beta) = 0.5*(beta**(n+1))
//- expanding:   F(beta) = [0-.5*(beta**(-n-1)) + 0.5]
//
//If u is calculated in function of beta in beta = pow((2*u),1/(v+1.0));
//and in beta = pow((1/(2*(1-u))), 1/(v+1.0)); we can see that they coincide
//
//Note: in the first case: real u = 2*u 
//      in the second csae: real u = 2*u - 0.5
//
//In half cases a contracting crossover is performed and in the other half
//a expanding crossover is performed
//
//Once that beta has been calcalated 2 childs are generated.
//The average value of the alleles in the childrens is the same
//as the average value in the parents and the beta value is the
//one obatained.
//
//The generation of the childs is made with the next equations:
// 0.5[(1 + beta)P1_i + (1-beta)P2_i]
// 0.5[(1 - beta)P1_i + (1+beta)P2_i]
//
// Para aplicarlo a multivariable propone:
// - Aplicarlo en cada variable con probabilidad 0.5
// - Determinar una variable, aplicarlo sobre esa e intercambiar
// los alelos posteriores.
// - No lo propone en este articulo, pero otra gente lo aplica
// sobre todas las variables
//
// Estaa función lo aplica en cada variable con probabilidad 0.5

#ifndef CROSSOVER_SBX_H
#define CROSSOVER_SBX_H

#include "Crossover.h"

class Crossover_SBX : public Crossover {
	void crossover(Individual *ind1, Individual *ind2);
};

#endif
