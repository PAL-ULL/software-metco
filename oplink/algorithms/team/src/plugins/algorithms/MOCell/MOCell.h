/***********************************************************************************
 * AUTHORS
 *   - Antonio J. Nebro 
 * 
 * DATE
 *   December 2007
 *
 * DESCRIPTION
 *   This class inherits from EA, in order to implement the evolutionary algorithm
 *   MoCell, a cellular GA. Specifically, it is an asynchronous version called
 *   aMOCell4:
 *   
 *   A.J. Nebro, J.J. Durillo, F. Luna, B. Dorronsoro, E. Alba 
 *   "Design Issues in a Multiobjective Cellular Genetic Algorithm". 
 *   Evolutionary Multi-Criterion Optimization. 4th International Conference, 
 *   EMO 2007. Sendai/Matsushima, Japan, March 2007
 *    
 * **********************************************************************************/


#ifndef __MOCELL_H__
#define __MOCELL_H__

#include <string>
#include <iostream>
#include <vector>
#include <math.h>

#include "MOFront.h"
#include "Individual.h"
#include "EA.h"


class Neighborhood ;

class MOCell : public EA {
		  
public:
	// Constructor
	MOCell();

	// Destructor
	virtual ~MOCell(void) ;

	// Define una generaci� de bsqueda del algoritmo	
	void runGeneration();

	// Inicializa los par�etros iniciales del algoritmo
	bool init(const vector<string> &params);

	// Rellena un frente con las soluciones actuales
	void getSolution(MOFront *p) ;

	// Muestra la informaci� relativa al algoritmo	
	void printInfo(ostream &os) const;
	
	static int objOrder;	// Variable indice para el objetivo actual

	bool addToArchive(Individual *, vector<Individual *> *) ;
private:
	unsigned int archiveSize_ ; // Archive size
	vector<Individual *>* archive_ ; // Archive pool
	float pm; // Mutation probability
	float pc; // Crossover probability
	
	bool genPareto;	// Indicates if necesary to generate a local front (sequential executions)

	static const int POPSIZE_DEFAULT = 100;	// Population size by default

	// Evaluation of the individuals in the population
	void evaluation (void);

	Neighborhood *         neighborhood_ ;
	vector<Individual *>** neighboors_ ;
};


/**
 * Neighborhood. Class representing the neighborhood of an individual (cell)
 * in the MOCell algorithm
 */
class Neighborhood {
public:
  /**
   * Maximum rate considered
   */
  static const int MAXRADIO = 2;
  
  /**
   * Stores the neighborhood.
   * structure_ [i] represents a neighborhood for a solution.
   * structure_ [i][j] represents a neighborhood with a ratio.
   * structure_ [i][j][k] represents a neighbor of a solution.
   */
  int *** structure_;
  
  /**
   * Stores the size of the solutionSet.
   */
  int neighborhoodSize_;
  
  /**
   * Stores the size for each row
   */
  int rowSize_;
 
  /**
   * Enum type for defining the North, South, East, West, North-West, South-West,
   * North-East, South-East neighbor.
   */
  enum Row {N_, S_, E_, W_, NW_, SW_, NE_, SE_};

  // Constructor
  Neighborhood(int neighborhoodSize) ;
  
  // Destructor
  ~Neighborhood() ;

  vector<Individual *>* getEightNeighbors(vector<Individual *>* pop, int individual) ;

} ; // Neighborhood



#endif
