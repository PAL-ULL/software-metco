/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo Gonzalez
 * 
 * FECHA
 *    Julio de 2008
 *
 * DESCRIPCION
 *    Individuo para la resolucion del problema de Benchmark WFG3
 *    
 *    El problema WFG3 esta definido en:
 *     "A Scalable Multi-objective Test Problem Toolkit"
 *      Simon Huband, Luigi Barone, Lyndon While and Phil Hingston
 * 
 *    Se han definido los metodos requeridos por la clase Individual: 
 *     - mutation
 *     - crossover 
 *     - evaluate
 *     - init 
 *     - clone
 *
 *    Para mas informacion acerca de la especificacion de un problema, referirse a
 *    al fichero Individual.h
 *
 * ********************************************************************************/


#ifndef __INDIVIDUALWFG3_H__
#define __INDIVIDUALWFG3_H__


#include <iostream>
#include <math.h>

#include "Individual.h"


class WFG3 : public Individual {
 
	public:
  
	// Funcion de inicializacion
	bool init (const vector<string> &params);
	
	
	// Evaluacion y seleccion del individuo
	void evaluate (void);
	
	// Clonacion
	Individual* clone (void) const;
	
	// Rangos
	double inline getMaximum(const int i) const { return 1; }
	double inline getMinimum(const int i) const { return 0; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }
 
  private:
	// Funciones auxiliares al problema
	vector<double> subvector(const vector<double> &v, const int head, const int tail);
	double correct_to_01(const double &a, const double &epsilon = 1.0e-10);
	vector<double> calculate_x (const vector<double> &t_p, const vector<short> &A);
  vector<double> calculate_f (const double &D, const vector<double> &x, const vector<double> &h,
								            	const vector<double> &S);

	// Transiciones genericas
	double s_linear (const double &y, const double &A);
	double r_nonsep(const vector<double> &y, const int A);
	double r_sum (const vector<double> &y, const vector<double> &w);
	
  // Superficies genericas
	double linear(const vector<double> &x, const int m);

	// Transiciones del problema WFG3
	vector<double> WFG3_t1(const vector<double> &y, const int k);
	vector<double> WFG3_t2(const vector<double> &y, const int k);
	vector<double> WFG3_t3(const vector<double> &y, const int k, const int M);
	
	// Superficie del problema WFG3
	vector<double> WFG3_shape(const vector<double> &t_p);

	// Funciones propias de los problemas WFG
	vector<short> WFG_create_A (const int M, const bool degenerate);
	vector<double> WFG_calculate_f (const vector< double >& x, const vector< double >& h);
	
	// Constantes del problema WFG3
	static int k;             // Parametros relacionados con la posicion
	static const int l = 20;  // Parametros relacionados con la distancia

};

#endif
