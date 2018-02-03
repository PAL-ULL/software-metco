/***********************************************************************************
 * AUTORES
 *    Jesica de Armas Adrian
 * 
 * FECHA
 *    Noviembre 2007
 *
 * DESCRIPCION
 *
 * ********************************************************************************/

#ifndef __CSP2_H__
#define __CSP2_H__

#include "Individual.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


typedef struct {
	double l;
	double w;
	vector<int> p;
} T_Layer;

typedef struct {
	double l;
	int p;
} T_OrderL;

class CSP2 : public Individual{
public:
	// Destructor
	~CSP2 (void){}

	// Initialization method
	bool init (const vector<string> &params);

	// Genetic Operators
	void crossoverPMX(Individual* ind);
	void crossoverOX(Individual* ind);
	void crossoverCX(Individual* ind);
	
	// Evaluation
	void evaluate (void);

	// Clone
	Individual* clone (void) const;

	// Reads a problem from file
   bool readFile(const string filename);

	// Check condition 1 <= no <= np -1
	bool checkCondition();
	
	// Búsqueda de un patron en el segmento de intercambio 
	int findpat(int elem, int *p1, int *p2, int point1, int point2);	
	int findPos(int elem, int *p);	
	bool included(int elem, int *p, int point1, int point2);
	
	// Setters
	void inline setLength(const int i, const int j, const double value) const { len[i][j] = value; }
	void inline setWidth(const int i, const int j, const double value) const { wid[i][j] = value; }
	
	// Getters
   double inline getLength(const int i, const int j) const { return len[i][j]; }
	double inline getWidth(const int i, const int j) const { return wid[i][j]; }

	// Generation of an individual
	void restart(void);
	void randomRestart(void);
	void heuristicRestart(void);

	// Print solution
	void print(void);

	// Fix a given final solution
	void fixSolution(void);
 
 	double inline getMaximum(const int i) const { return numPat-1; }
   double inline getMinimum(const int i) const { return -2; }
	unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }
		 
private:
	void dependentMutation (double pm);
	void dependentCrossover (Individual* ind);		
	// Length and width of patterns
	static double *len[2];
	static double *wid[2];
	static double widSheet; 			// Width of mother sheet
	static int size; 						// Size of gene without orientation
	static int totalSize; 				// Total size of gene
	static int numPat;				 	// Number of patterns
	static vector<T_OrderL> orderPiecesL; 	// Pieces ordered by length
	static vector<int> ov;						// Vertical orientation of pieces
	
	// Constants
	static const int H = -1;
	static const int V = -2;
	static const int NOBJ = 2;
};

#endif
