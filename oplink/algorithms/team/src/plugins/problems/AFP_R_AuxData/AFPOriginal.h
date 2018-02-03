#ifndef AFP_ORIGINAL_H
#define AFP_ORIGINAL_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <cfloat>
#include <cmath>
#include <stdlib.h>
#include <time.h>

using namespace std;

struct TRXOriginal {
  int trxID;
  int sectorID;
  int siteID;
  string type;
  vector<int> * validFrequencies;
};

struct IndividualAFP { 
  vector<int> * plan;
  vector<double> * planCC;
  double cost;
};

struct Parameters {
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
	
} projectParameters; 

//methods
void loadTrxs(const char * filename);
void loadIM(const char * filename);
void loadSectors(const char * filename);
void loadSites(const char * filename);
void loadOptions(const char * filename);
void loadNeighbors(const char * filename);
void loadSecondNeighbors(const char * filename);
double computeChannelInterference(vector<int> * plan, vector<double> * planningCC, int victimTrx, int interfererTrx);
double c2iCost(vector<int> * planning, vector<double> * planningCC);
void randomPlanning(IndividualAFP * solution);
void filePlanning(IndividualAFP *solution, char *file);
double interferenceBasedFitness(IndividualAFP * solution);

//Variables
extern vector<TRXOriginal>                       _trxs                ; //transceivers 
extern map<int, map<int, pair<double,double> > > _interferenceMatrix  ; //IM
extern map<int, vector<int> >                    _interfereTo         ; //sectors to which each sector interfere
extern map<int, vector<int> >                    _sectors2trxs        ; //map sectors to trxs
extern map<int, int>                             _sectorConstraintSep ; //sector constraint separation
extern map<int, vector<int> >                    _site2sectors        ; //map sites to sectors
extern map<int, int>                             _siteConstraintSep   ; //site constraint separation
extern map<int, vector<int> >                    _neighbors           ; //1-hop neighbors
extern map<int, vector<int> >                    _secondNeighbors     ; //2-hop neighbors

#endif
