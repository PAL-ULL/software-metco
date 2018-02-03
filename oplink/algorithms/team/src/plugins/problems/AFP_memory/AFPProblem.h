#ifndef AFPPROBLEM_H__
#define AFPPROBLEM_H__

#include <iostream>
#include <fstream>
#include <set>
#include <vector>
#include <string>
#include <map>
#include <math.h>
#include "AFP_memory.h"

using namespace std;

struct TRX {
	unsigned short trxID;
	unsigned short sectorID;
	unsigned short siteID;
	string type;
	vector<unsigned short> * validFrequencies;
	set<unsigned short> *validFrequenciesSet;
};

class AFP_memory;

#define COCOST 0
#define ADJCOST 1
class PairSectorCost{
	public:
		//constructor
		PairSectorCost(const unsigned short victim, const unsigned short interferer, const double cost, const unsigned short costType){
			this->victim = victim;
			this->interferer = interferer;
			this->cost = cost;
			this->costType = costType;
		}
		//getters
		inline unsigned short getVictim() const { return victim; }
		inline unsigned short getInterferer() const { return interferer; }
		inline double getCost() const { return cost; }
		inline unsigned short getCostType() const { return costType; }
		//comparation operator
		inline bool operator< (const PairSectorCost &pair) const{
			return (cost < pair.cost);
		}
	private:
		unsigned short victim;
		unsigned short interferer;
		double cost;
		unsigned short costType;
};


class AFPProblem {
	public:
		AFPProblem(const string &baseFile);
		int getIndexesInTable() { return indexesInTable; }
		//getters
		inline string getBaseFile() const { return baseFile; }
		inline vector<PairSectorCost> *getSortCosts() { return &sortCosts; }
		inline unsigned short getTrxSize() const { return trxs.size(); }
		inline unsigned short getSectorsSize() const { return sectors2trxs.size(); }
		inline vector< vector<unsigned short> > *getRelatedSectors() { return &relatedSectors; }
		inline vector<unsigned short> *getIndependentSectors() { return &independentSectors; }
		inline vector<unsigned short> *getDependentSectors() { return &dependentSectors; }
		inline vector< vector<unsigned short> > *getSectors2trxs() { return &sectors2trxs; }
		inline vector< TRX > *getTrxs() { return &trxs; }
		inline vector< vector <double> > *getCoCostBothSect() { return coCostBothSect; }
		inline vector< vector <double> > *getAdjCostBothSect() { return adjCostBothSect; }
		inline vector< map <unsigned short, int> > *getVictimOrInterfererTrxCost() { return &victimOrInterfererTrxCost; }
		inline vector< vector <unsigned short> > *getDirectInfluencedTrxs() { return &directInfluencedTrxs; }
		inline vector< vector <unsigned short > > *getSectorsInterfereOrVictim() { return &sectorsInterfereOrVictim; }
		void completeEvaluation(AFP_memory *ind);
		inline unsigned short getMaxFrequency() { return maxFrequency; }
		inline unsigned short getMinFrequency() { return minFrequency; }
		inline unsigned short getHighestSector() { return highestSector; }
	
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
		void computeSameSectorCost(unsigned short victimTrx, unsigned short interfererTrx, double &coCost, double &adjCost);
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
		unsigned short maxFrequency;
		unsigned short minFrequency;

		//trx, sites, sectors
		vector<TRX>                               trxs                ; //transceivers 
		map<unsigned short, map<unsigned short, pair<double,double> > > interferenceMatrix  ; //IM
		vector< vector <unsigned short> >                     sectorsInterfereOrVictim   ; 
		vector< vector<unsigned short> >                     sectors2trxs;
		map<int, int>                             sectorConstraintSep ; //sector constraint separation
		map<unsigned short, vector<unsigned short> >                    site2sectors        ; //map sites to sectors
		map<unsigned short, vector<unsigned short> >                    neighbors           ; //1-hop neighbors
		map<unsigned short, vector<unsigned short> >                    secondNeighbors     ; //2-hop neighbors
		map<int, int>                             siteConstraintSep   ; //site constraint separation

		//pre-calculated tables of costs
		vector< vector <double> > *coCostBothSect;
		vector< vector <double> > *adjCostBothSect;

		//groups of related trxs
		vector< vector<unsigned short> > relatedSectors;
		vector<unsigned short> independentSectors;
		vector<unsigned short> dependentSectors;
		//indexes of costs (information for individuals)
		static int indexesInTable;
		static vector< map <unsigned short, int> > victimOrInterfererTrxCost;
		static vector< vector <unsigned short> > directInfluencedTrxs;
		vector<PairSectorCost> sortCosts;
		unsigned short highestSector;
};



#endif
