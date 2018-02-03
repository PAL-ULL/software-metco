/***********************************************************************************
 * AUTORES
 *    Jesica de Armas Adrian
 * 
 * FECHA
 *    Junio 2010
 *
 * DESCRIPCION
 *
 * ********************************************************************************/

#ifndef __TDCSP_H__
#define __TDCSP_H__

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
        int p;
} T_Order;


typedef struct T_Node{
        int id;
        T_Node* father;
        T_Node* left;
        T_Node* right;
} T_Node;


typedef struct {
        double l;
        double w;
        double a;
        double x;
        double y;
        T_Node* ptr;
} T_Hole;

typedef T_Node* Tree;


class TDCSP : public Individual{
public:
	// Destructor
	~TDCSP (void){}

	// Initialization method
	bool init (const vector<string> &params);


	// Clone
	Individual* clone (void) const;

	// Reads a problem from file
  	 bool readFile(const string filename);

	// Check condition 1 <= no <= np -1
	bool checkCondition();
	
	// Búsqueda de un patron en el segmento de intercambio 
	int findpat(int elem, int *p1, int *p2, int point1, int point2);	
	
	// Generation of an individual
	void restart(void);
	void randomRestart(void);

	bool improveOne(Individual*, int);
	
	int uniformRandom(int maxValue) const;
 	double inline getMaximum(const int i) const { return totalNumPat-1; }
	double inline getMinimum(const int i) const { return -2; }
	unsigned int getOptDirection(const int i) const;


	void mutation_moves (double pm);
	void mutation_moves_random (double pm);
	void mutation_add (double pm, int pos);
	void mutation_add_random (double pm, int pos);
	void mutation_remove (double pm, int pos);
	void mutation_remove_random (double pm, int pos);
	void mutation_replace (double pm, int pos);
                 
        
	void crossover_onepoint_h (Individual* ind);
	void crossover_twopoint_h (Individual* ind);
	void crossover_onepoint_p (Individual* ind);
	void crossover_twopoint_p (Individual* ind);
	void crossover_onepoint_orient(Individual* ind);


	 // Evaluation
	void calculateProfitCuts();
	void evaluate (void);


	// Imprimir individuo
	void printInd(Individual *ind);
	// Limpiar la parte del gen que no se usa
	void cleanInd(Individual *ind);


	// Heuristics
	void copiar(Tree &ct, Tree t, Tree pre);
	void podar(Tree &t);
	void postorden(Tree t);
	void getNotation(void);
	void insertTopHole(int id, double w);
	void delHole(int id);
	void insertCopyOrderHole(int id, double param);
	void delOrderHoleId(int id);
	void insertOrderHole(int id, double param);
	void getTreeHole2(Tree &node, int it, int p, int subtree);
	void getTreeHole0(Tree &node, int it, int p, int subtree);
	int NFDH(int n, int tOrder, int tOrderP);
	int FFDH(int n, int tOrder, int tOrderP);
	int BFDH(int n, int tOrder, int tOrderP);
	void fixSolution(void);        
        
		 
private:
	typedef int (TDCSP::*hptr)(int, int, int);

	void dependentMutation (double pm);
	void dependentCrossover (Individual* ind);
		
	// Length and width of patterns
	static double *len;
	static double *wid;
	static double *prof;
	static double widSheet; 			// Width of mother sheet
	static double lenSheet; 			// Length of mother sheet
	static int totalSize; 			// Total size of gene
	static int numPat;				 // Number of patterns
	static int totalNumPat;

	// Constants
	static const int H = -1;
	static const int V = -2;
	static const int NOBJ = 2;

	static const int NORDER = 4;            // Número de ordenes/seleccion huecos
	static const int NORDERP = 4;           // Número de ordenes/seleccion piezas
	static const int NHS = 3;              // Número de heurísticas implementadas
	static const int NMOVES = 3;            // Movimientos posibles para la mutación
	static const int ADD = 0;
	static const int REMOVE = 1;
	static const int REPLACE = 2;
	static int sizeGen;                        // Size of gene
	static hptr heuristic[NHS];
	static int generatedInds;               // Numero de individuos generados con el restart

	vector<T_Order> order;  // piezas ordenadas
	vector<T_Hole> holes;   // huecos
	vector<int> orderHoles; // huecos ordenados
	vector<int> topHoles;   // huecos superiores

	Tree tree;
	int* notation;                  // notacion postfija
	int* tempNotation;              // notacion postfija con huecos
	int itTemp;
	int itFinal;
	int typeOrder;          // tipo de orden de seleccion de piezas previo

	int numHS;	// Número actual de heurísticas en el gen
	int size;
	int put;		// piezas colocadas
};

#endif







