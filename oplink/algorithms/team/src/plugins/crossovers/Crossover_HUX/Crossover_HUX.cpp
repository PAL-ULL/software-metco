/***********************************************************************************
 * AUTHORS
 *   - Francisco Jose Morales
 * 
 * *********************************************************************************/


#include "Crossover_HUX.h"

#include <math.h>


void Crossover_HUX::crossover (Individual *ind1, Individual *ind2) {
   int distanciaInd = hammingDistance(ind1, ind2);
   vector<int> vectorIndices;
   
   //Se genera un vector de valores aleatorios para seleccionar de forma aleatoria la primera
   //mitad de los elementos
   generateRandomValues (0, distanciaInd-1, distanciaInd, vectorIndices);
 
   for (unsigned int i = 0; i < ind1->getNumberOfVar(); i++) {  
      
      //Se cruza si los elementos son diferentes
      if (ind1->getVar(i) != ind2->getVar(i)) {

	 //Se comprueba si el elemento a cruzar esta en la primera mitad de la lista aleatoria
	 for (unsigned int j = 0; j < floor(vectorIndices.size()/2); j++) {
	    
	    //Si esta en la lista se realiza el cruce
	    if (i == vectorIndices[j]) {	    
	       double tmp = ind2->getVar(i);
	       ind2->setVar(i, ind1->getVar(i));
	       ind1->setVar(i, tmp);
	    }
	 }
      }
   }
   
}