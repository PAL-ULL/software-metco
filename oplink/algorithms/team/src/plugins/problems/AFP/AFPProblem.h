#ifndef AFPPROBLEM_H__
#define AFPPROBLEM_H__

#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <string>
#include <map>
#include <math.h>
#include "AFP.h"

using namespace std;

struct TRX {
  int trxID;
  int sectorID;
  int siteID;
  string type;
  vector<int> * validFrequencies;
	set<int> *validFrequenciesSet;
};

class AFP;

#define COCOST 0
#define ADJCOST 1
class PairSectorCost{
	public:
		//constructor
		PairSectorCost(const int victim, const int interferer, const double cost, const int costType){
			this->victim = victim;
			this->interferer = interferer;
			this->cost = cost;
			this->costType = costType;
		}
		//getters
		inline int getVictim() const { return victim; }
		inline int getInterferer() const { return interferer; }
		inline double getCost() const { return cost; }
		inline int getCostType() const { return costType; }
		//comparation operator
		inline bool operator< (const PairSectorCost &pair) const{
			return (cost < pair.cost);
		}
	private:
		int victim;
		int interferer;
		double cost;
		int costType;
};


class AFPProblem {
	public:
		AFPProblem(const string &baseFile);
		int getIndexesInTable() { return indexesInTable; }
		//getters
		inline string getBaseFile() const { return baseFile; }
		inline vector<PairSectorCost> *getSortCosts() { return &sortCosts; }
		inline int getTrxSize() const { return trxs.size(); }
		inline int getSectorsSize() const { return sectors2trxs.size(); }
		inline vector< vector<int> > *getRelatedSectors() { return &relatedSectors; }
		inline vector<int> *getIndependentSectors() { return &independentSectors; }
		inline vector<int> *getDependentSectors() { return &dependentSectors; }
		inline vector< vector<int> > *getSectors2trxs() { return &sectors2trxs; }
		inline vector< TRX > *getTrxs() { return &trxs; }
		inline vector< vector <double> > *getCoCost() { return coCost; }
		inline vector< vector <double> > *getCoCostBoth() { return coCostBoth; }
		inline vector< vector <double> > *getAdjCost() { return adjCost; }
		inline vector< vector <double> > *getAdjCostBoth() { return adjCostBoth; }
		inline vector< map <int, int > > *getInterfererTrxCost() { return &interfererTrxCost; }
		inline vector< map <int, int > > *getVictimTrxCost() { return &victimTrxCost; }
		inline vector< map <int, int > > *getVictimOrInterfererTrxCost() { return &victimOrInterfererTrxCost; }
		inline vector< set <int > > *getSectorsInterfereOrVictim() { return &sectorsInterfereOrVictim; }
		void completeEvaluation(AFP *ind);
		inline int getMaxFrequency() { return maxFrequency; }
		inline int getMinFrequency() { return minFrequency; }
		inline int getHighestSector() { return highestSector; }
	
	private:
		string baseFile;
		//load functions
		void loadTrxs(const string &filename);
		void loadIM(const string &filename);
		void loadSectors(const string &filename);
		void loadSites(const string &filename);
		void loadOptions(const string &filename);
		void loadNeighbors(const string &filename);
		void loadSecondNeighbors(const string &filename);

		//cost functions
		void generateCostTrxMatrix();
		void computeSameSectorCost(int victimTrx, int interfererTrx, double &coCost, double &adjCost);
		void computeDifferentSectorsCost(double mean, double std, double &coCost, double &adjCost);
		float erfcc(float x);
		void fillCostIndexes();
		void analyzeNetwork();

		//scenario parameters
		//costs
		double sameSectorCoChBcch;
		double sameSectorCoChTch;
 		double sameSectorAdjChBcch;
		double sameSectorAdjChTch;

		double sameSiteCoBcchBcch;
		double sameSiteCoBcchTch;
		double sameSiteCoTchTch;
		double sameSiteAdjBcchBcch;
		double sameSiteAdjBcchTch;
		double sameSiteAdjTchTch;

		double nbrCoBcchBcch;
		double nbrCoBcchTch;
		double nbrCoTchTch;

		double nbrAdjBcchBcch;
		double nbrAdjBcchTch;
		double nbrAdjTchTch;
  
		double nbr2CoBcchBcch;
		double nbr2CoBcchTch;
		double nbr2CoTchTch;
  
		//constraints
		double sameSectorChSep;
		double sameSiteChSep;
  
		//signaling
		double adjChannelRejection;
		double signallingThreshold;
	
		//frequencies
		int maxFrequency;
		int minFrequency;

		//trx, sites, sectors
		vector<TRX>                               trxs                ; //transceivers 
		map<int, map<int, pair<double,double> > > interferenceMatrix  ; //IM
		vector< set <int> >                     sectorsInterfereOrVictim   ; 
		vector< vector<int> >                     sectors2trxs;
		map<int, int>                             sectorConstraintSep ; //sector constraint separation
		map<int, vector<int> >                    site2sectors        ; //map sites to sectors
		map<int, vector<int> >                    neighbors           ; //1-hop neighbors
		map<int, vector<int> >                    secondNeighbors     ; //2-hop neighbors
		map<int, int>                             siteConstraintSep   ; //site constraint separation

		//pre-calculated tables of costs
		vector< vector <double> > *coCost;
		vector< vector <double> > *adjCost;
		vector< vector <double> > *coCostBoth;
		vector< vector <double> > *adjCostBoth;

		//groups of related trxs
		vector< vector<int> > relatedSectors;
		vector<int> independentSectors;
		vector<int> dependentSectors;
		//indexes of costs (information for individuals)
		static int indexesInTable;
		static vector< map <int, int> > interfererTrxCost;
		static vector< map <int, int> > victimTrxCost;
		static vector< map <int, int> > victimOrInterfererTrxCost;
		vector<PairSectorCost> sortCosts;
		int highestSector;
};



#endif
