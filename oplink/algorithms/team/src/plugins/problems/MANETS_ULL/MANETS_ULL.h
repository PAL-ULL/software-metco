/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 *
 * DESCRIPCION
 *    Individuo para la resolucion del problema del broadcast en redes MANET.
 *    El algoritmo de broadcast utilizado es DFCN. DFCN es configurable a 
 *    través de 5 parámetros: proD, minGain, safeDensity, lowerBoundRAD y 
 *    upperBoundRAD. Usando el simulador externo MADHOC se realiza una simulación 
 *    de la operación del broadcast obtieniéndose los valores de los objetivos que 
 *    se quieren optimizar: Bandwidth, Coverage y Duration. Los objetivos son a 
 *    minimizar, maximizar y minimizar respectivamente.
 *    El individuo acepta un parámetro que es la ruta de un script que permite la 
 *    ejecución del simulador MADHOC. De esta forma, cambiando el script pasado
 *    el individuo sirve para realizar optimizaciones de distintos escenarios.
 *    
 *    Se han definido los metodos requeridos por Individual: init, mutation,
 *    crossover, evaluate y clone
 * ********************************************************************************/

#ifndef __MANETS_ULL_H__
#define __MANETS_ULL_H__

#include "Individual.h"
#include "stringConversion.h"
#include <stdlib.h>
#include <string>

class MANETS_ULL: public Individual {
public:
	// Initialization method
	bool init (const vector<string> &params);


	// Evaluation 
	void evaluate (void);

	// Ejecución del simulador
	void execSimulator(int proD, double minGain, int safeDensity, double lowerBoundRad, double upperBoundRad);

	// Clone 
	Individual* clone (void) const;

	// Operador de impresion 
	void print(ostream &os) const;

	// Ranges
	double inline getMaximum(const int i) const { return MAXIMUM[i]; }
	double inline getMinimum(const int i) const { return MINIMUM[i]; }
	unsigned int inline getOptDirection(const int i) const { return OBJFUNC[i]; }

private:
	// Constantes
	static const int NPARAM = 5; // [proD, minGain, safeDensity, lowerBoundRAD, upperBoundRAD]
	static const int NOBJ   = 3; // [Bandwidth, Coverage, Duration]
	static const double MINIMUM[];
	static const double MAXIMUM[];
	static const int OBJFUNC[];
	static string executionScript;
};

#endif
