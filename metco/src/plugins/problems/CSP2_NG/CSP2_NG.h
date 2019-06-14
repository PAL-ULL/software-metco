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

#ifndef __CSP2_NG_H__
#define __CSP2_NG_H__

#include "Individual.h"
#include <math.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>

typedef struct {
   int elem1;
   int elem2;
   int op;
} TBuild;

typedef struct {
   double x1;
   double y1;
   double x2;
   double y2;
} TCoordinate;


class CSP2_NG : public Individual{
public:
   // Destructor
   ~CSP2_NG (void){}

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
   
	// Metodos auxiliares
	void print(void);
	void fixSolution(void);
	
   // Setters
   void inline setLength(const int i, const int j, const double value) const { len[i][j] = value; }
   void inline setWidth(const int i, const int j, const double value) const { wid[i][j] = value; }
   
   // Getters
   double inline getLength(const int i, const int j) const { return len[i][j]; }
   double inline getWidth(const int i, const int j) const { return wid[i][j]; }

   // Random generation of an individual
   void restart(void);
 
   double inline getMaximum(const int i) const { return numPat-1; }
   double inline getMinimum(const int i) const { return -2; }
   unsigned int inline getOptDirection(const int i) const { return MINIMIZE; }
       
private:
   void dependentCrossover (Individual* ind);      
   void dependentMutation (double pm);
   // Length and width of patterns
   static double *len[2];
   static double *wid[2];
   static double widSheet;    // Width of mother sheet
   static int size;    // Size of gene without orientation
   static int totalSize;    // Total size of gene
   static int numPat;   // Number of patterns
   
   // Constants
   static const int H = -1;
   static const int V = -2;
   static const int NOBJ = 2;
	static const int NEDGES = 4;
	static const int TOP = 0;
	static const int RIG = 1;
	static const int BOT = 2;
	static const int LEF = 3;
};

#endif
