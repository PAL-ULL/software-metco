/***********************************************************************************
 * AUTHORS
 *   - Alejandro Cervantes
 *   - Sandra Garcia
 *   - Cristobal Luque
 * 
 * DATE
 *   December 2007
 *
 * DESCRIPTION
 * 
 * - MOPSO code from http://delta.cs.cinvestav.mx/~ccoello/EMOO
 * 
 * - MOPSO with restart: not implemented
 * 
 * - Received individuals (from migration): each received solution is inserted
 *   as a population individual, replacing another individual of the population.
 *   Its velocity is initialized randomly in the range (-1.0, 1.0). The number of 
 *   migrated individuals be smaller than the population size. The archive is
 *   regenerated.
 *
 ************************************************************************************/


#ifndef __MOPSO_H__
#define __MOPSO_H__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>

#include "Individual.h"
#include "MOFront.h"
#include "EA.h"


class MOPSO : public EA {

	public:
		// Constructor
		MOPSO ();
	
		// Destructor
		~MOPSO (void);

		// Define una generaci?n de b?squeda del algoritmo
		void runGeneration();

		// Inicializa los par?metros iniciales del algoritmo
		bool init(const vector<string> &params);

		// Rellena un frente con las soluciones actuales
		void getSolution(MOFront *p) ;

		// Muestra la informaci?n relativa al algoritmo
		void printInfo( ostream &os) const;
		
		// Gestiona la recepcion de un individuo migrado
    void received(vector<Individual*> &mig);
	  
	private:
		unsigned int aSize;  // Archive size
		float pc;            // Crossover probability
	

  double pM;         // Mutation probability
  int fun;           // Function Identifier
  unsigned int M;    // Particles
  unsigned int Gmax; // Max. cycles
  unsigned int * opt;  // Optimization
  unsigned int D;    // Dimension of function
  unsigned int NF;   // Number of functions
  unsigned int MEM;  // Memory size (archive)
  unsigned int ndiv; // Number of divisions
  char *cad;         // Return parameter

  unsigned int h, i, j, t, rest, Gbestpos, gbestpos, GT;
  unsigned int lastPart;
  unsigned int maxCube;
  double Gbest, gbest, x, passed;
//  double *pop;
//  double *fitness;
  double *pbests, *vel, *fbests;
  double *noDomP, *noDomF;
  double *amp, *start;
  unsigned int  *linf, *lsup, *hyperspace, *partPos;
  unsigned int *hyperPoolL, poolC;
  double *hyperPoolF, hypsum;
  clock_t  now, later;
  //MPTR noDom;
  //DSPTR aux;
  //DPTR auxV;
  //cambio por gtp 25 junio 2002
//  unsigned int *selec;
  bool state;
  bool primeraGeneracion;

  int pStep;
  
	// Versions of COELLO_MOPSO.psolib functions adapted to this representation
	void evaluation(); // Llama a EA::evaluate() para cada individuo
	
	void initialize_arrays ();
	void free_arrays();
	void copy_fbests (); // Copy the objs in the individual to the fbest array
	void copy_pbests (); // Copy the pos in the individual to the pbest array
	void searchAndInsert(double *noDomP, double *noDomF, unsigned int * opt, unsigned int *lastPart,unsigned int fun);
	void delPartDom(double * noDomP, double *noDomF, unsigned int *lastPart, unsigned int D, unsigned int NF, unsigned int * opt, unsigned int fun);
	unsigned int compareVector2(double *noDomF, unsigned int h, unsigned int i, unsigned int * opt, unsigned int fun ,double *noDomP);	
	unsigned int verifyDom(double *noDomP, double *noDomF, unsigned int *hyperspace, unsigned int *partPos, unsigned int * opt, unsigned int *lastPart, unsigned int i,unsigned int fun);	
	unsigned int locateHypercube(double *start, double *amp, unsigned int ndiv, unsigned int part);
	int searchAndInsert2(double *noDomP, double *noDomF, unsigned int *linf, unsigned int *lsup, double *amp, double *start, unsigned int *hyperspace, unsigned int *partPos, unsigned int ndiv, unsigned int * opt, unsigned int maxCube, unsigned int *lastPart, unsigned int fun);
	unsigned int bestMoo(unsigned int part, unsigned int * opt,unsigned int fun);
	int ifbestInterchange(double *fbests, double *pbests,unsigned int * opt,unsigned int fun);
	void makeHyper(double *noDomF, unsigned int *linf, unsigned int *lsup, double *amp, double *start, unsigned int *hyperspace, unsigned int *partPos, unsigned int ndiv, unsigned int MEM, unsigned int NF, unsigned int lastPart, unsigned int maxCube);
	void hyperFit(unsigned int *hyperspace, unsigned int *hyperPoolL, double *hyperPoolF, double *hypsum, unsigned int maxCube, unsigned int *poolC);
	unsigned int lead(unsigned int *hyperspace, unsigned int *hyperPoolL, unsigned int *partPos, double *hyperPoolF, double hypsum, unsigned int lastPart, unsigned int poolC);
	unsigned int hyperWheel(unsigned int *hyperPoolL, double *hyperPoolF, double hypsum, unsigned int poolC);
	double velocity(double W, double Vi, double C1, double C2, double Pb, double Pg, double Xi);
	unsigned int compVec(double *noDomF, unsigned int NF, unsigned int h, unsigned int i, unsigned int * opt,unsigned int fun,double *noDomP,unsigned int D);
	unsigned int locHyper(double *vect, double *start, double *amp, unsigned int ndiv, unsigned int NF, unsigned int part);
	unsigned int search_max(unsigned int *f, unsigned int M);
	unsigned int search_max(double *f, unsigned int M);
	unsigned int search_min(double *f, unsigned int M);
	void muta(double pM, int M, unsigned int D,int Gen,int totGen);
	void ranges(double *linf,double * lsup);
	void printMOPSOArchive (int generation,bool printVars);
    void printMOPSOPopulation(int generation,int dim);


};

#endif
