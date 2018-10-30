
#ifndef __MOEA_D_H__
#define __MOEA_D_H__

#include <string>
#include <iostream>
#include <array>
#include <math.h>
#include <set>

#include "MOFront.h"
#include "Individual.h"
#include "EA.h"


class MOEA_D : public EA {
		  
public:
	// Constructor
	MOEA_D() {};
	// Define una generación de búsqueda del algoritmo	
	void runGeneration();
	// Inicializa los parámetros iniciales del algoritmo
	bool init(const vector<string> &params);
	// Rellena un frente con las soluciones actuales
	void getSolution(MOFront *p) ;
	// Muestra la información relativa al algoritmo	
	void printInfo(ostream &os) const;
	string getName() const { return "MOEA_D"; }
};


#endif
