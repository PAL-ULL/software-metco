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

#ifndef __AFP_SELFADAPTIVE_H__
#define __AFP_SELFADAPTIVE_H__

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

class AFP_SelfAdaptive : public Individual {
	public:
		AFP_SelfAdaptive(){};
		~AFP_SelfAdaptive(){};

		// Clonation operator
		Individual* clone() const;

		// Individual initialization 
		bool init (const vector<string> &params);

		// mutation operator
		void dependentMutation(double pm);

    // crossover operator
    void dependentCrossover(Individual *ind);
		
		// Operador de salida por pantalla
		void print(ostream &os) const;

		// evaluate
		void evaluate();

		// Objective Direction
		unsigned int getOptDirection (const int i) const { return MINIMIZE; }

		// Variable ranges
		double getMaximum (const int i) const;
		double getMinimum (const int i) const;

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
		bool improveSector(int selectedSector);
		bool assignTrxToFrequency(set<int> &notAssignedTrxs, int freq, multiset<int> &possibleFrequencies, vector<int> &assignedTrxs);
		void getAssignmentCost(int trx, vector<double> &cost) const; 
		void removeTrx(int trx); 
		void assignTRX(int trx, int freq);

		// Mutation Operators and its auxiliar methods
		void mutationNeighbours(double pm);
		void mutationUniform(double pm);
		void mutationRename(double pm);
		void calculateInfluencedTrxs();
		void removeAssignTRX(int trx, int freq);
		void removeAssignTRX2(int trx, int freq, set<int> &interfPrevious);

		// Random intialization
		void randomAssignment();
		
		// Print initialization error
		void initialization_error();

		// Variable which must be cloned
		int maxMutated; // Used in mutation neighbours
		int mutationType; // Kind of mutation performed
		int crossoverType; // Kind of crossover performed
		
		//Cache
		vector<double> cacheData;

		// static components to access the problem characteristics
		static AFPProblem *problem;            
		static vector<PairSectorCost> *sortCosts;
		static vector<int> *dependentSectors;
		static vector<int> *independentSectors;
		static vector< vector<int> > *sectors2trxs;
		static vector<TRX> *trxs;
		static vector< vector <double> > *adjCostBoth;
		static vector< vector <double> > *coCostBoth;
		static vector<map <int, int> > *victimOrInterfererTrxCost;
		static vector< vector <int > > *sectorsInterfereTo;
		static vector< vector <int> > *sectorsVictimOf;
		static vector< set <int > > *sectorsInterfereOrVictim;
		static vector< set <int> > directInfluencedTrxs;

		// Static variable to prevent multiple initialization 
		static int firstInitialization;
		static bool origCodeLoaded;
};

//Mutation types
#define MUTATION_UNIFORM 0
#define MUTATION_RENAME 1
#define MUTATION_NEIGHBOURS 2

//Crossover types
#define CROSSOVER_UNIFORM 0
#define CROSSOVER_INTERFERED 1
#endif
