/***********************************************************************************
 * AUTHORS
 *   Gara Miranda Valladares
 *   Carlos Segura González
 * 
 * DATE
 *   MARCH 2009
 *
 * DESCRIPTION
 *   This class inherits from Individual to represent an individual which solves the
 *   AFP (Assignment Frequency Problem). 
 *   The problem consists is a mono-objective problem whose objective is to search 
 *   the trxs frequency assigment which minimizes the produced interference.
 *   Several mutation algorithms have been implemented (depending on the parameters
 *   the corresonding mutation operators is selected), and after the mutation
 *   a local search proccess is performed.
 **********************************************************************************/

#ifndef __AFP_memory_H__
#define __AFP_memory_H__

#include <queue>
#include <set>
#include <vector>
#include <map>

#include "MiSetInt.h"
#include "AFPProblem.h"
#include "Individual.h"

using namespace std;

class AFPProblem;
class PairSectorCost;
class TRX; 

class AFP_memory : public Individual {
	public:
		AFP_memory(){};
		~AFP_memory(){};

		// Clonation operator
		Individual* clone() const;

		// Individual initialization 
		bool init (const vector<string> &params);

		// mutation operator
		void dependentMutation(double pm);
		
		// Operador de salida por pantalla
		void print(ostream &os) const;

		// evaluate
		void evaluate();

		// Objective Direction
		unsigned int getOptDirection (const int i) const { return MINIMIZE; }

		// Variable ranges
		double getMaximum (const int i) const { return (*(*trxs)[i].validFrequencies)[(*trxs)[i].validFrequencies->size() - 1]; }
		double getMinimum (const int i) const { return (*(*trxs)[i].validFrequencies)[0]; }

		// Random Individual Initialization
		void restart();
	
		// add cost values in table (used by AFPProblem during the evaluation): cache
		// of interferences
		void addVictimInterfererCostValue(int index, double cost);

		void deserialized();
		void cloned();

		void inline setCacheDataSize(const int n)                     { cacheData.clear(); cacheData.resize(n, 0); }
		void inline setCacheData    (const int i, const double value) { cacheData[i] = value;  }
		double inline getCacheData   (const int i) const { return cacheData[i];     }
	private:
		// localSearch and its auxiliar methods
		void localSearch();
		void improveSectors(MiSetInt *newSectorsToTry);
		bool improveSector(unsigned short selectedSector);
		bool assignTrxToFrequency(set<unsigned short> &notAssignedTrxs, unsigned short freq, multiset<unsigned short> &possibleFrequencies, vector<unsigned short> &assignedTrxs);
		void getAssignmentCost(unsigned short trx, vector<double> &cost) const; 
		void removeTrx(unsigned short trx); 
		void assignTRX(unsigned short trx, unsigned short freq);

		// Mutation Operators and its auxiliar methods
		void mutationNeighbours(double pm);
		void mutationUniform(double pm);
		void mutationRename(double pm);
		void removeAssignTRX(unsigned short trx, unsigned short freq);
		void removeAssignTRX2(unsigned short trx, unsigned short freq, set<unsigned short> &interfPrevious);

		// Random intialization
		void randomAssignment();
		
		// Print initialization error
		void initialization_error();

		// Variable which must be cloned
		int maxMutated; // Used in mutation neighbours
		int mutationType; // Kind of mutation performed
		
		//Cache
		vector<double> cacheData;

		// static components to access the problem characteristics
		static AFPProblem *problem;            
		static vector<PairSectorCost> *sortCosts;
		static vector<unsigned short> *dependentSectors;
		static vector<unsigned short> *independentSectors;
		static vector< vector<unsigned short> > *sectors2trxs;
		static vector<TRX> *trxs;
		static vector< vector <double> > *adjCostBothSect;
		static vector< vector <double> > *coCostBothSect;
		static vector< vector <unsigned short > > *sectorsInterfereTo;
		static vector< vector <unsigned short> > *sectorsVictimOf;
		static vector< vector <unsigned short > > *sectorsInterfereOrVictim;
		static vector<map <unsigned short, int> > *victimOrInterfererTrxCost;
		static vector< vector <unsigned short> > *directInfluencedTrxs;

		// Static variable to prevent multiple initialization 
		static unsigned short firstInitialization;
		static bool origCodeLoaded;
};

//Mutation types
#define MUTATION_UNIFORM 0
#define MUTATION_RENAME 1
#define MUTATION_NEIGHBOURS 2

//Macros

#define MAYOR(X, Y)  ((X) < (Y) ? (Y) : (X))
#define MENOR(X, Y)  ((X) < (Y) ? (X) : (Y))

#endif
