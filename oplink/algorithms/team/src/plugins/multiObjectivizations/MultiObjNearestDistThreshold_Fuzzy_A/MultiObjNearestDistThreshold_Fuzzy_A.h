/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González 
 *   - Carlos Segura González
 * 
 * *********************************************************************************/


#ifndef MULTIOBJNEARESTDISTTHR_FUZZY_A_H 
#define MULTIOBJNEARESTDISTTHR_FUZZY_A_H 

#include "MultiObjectivization.h"
#include "fl/Headers.h"

class MultiObjNearestDistThreshold_Fuzzy_A : public MultiObjectivization {

  public:
	  void convertToMultiObjective(vector<Individual *> *population, vector<Individual *> *offSpring, vector<Individual *> *reference, int ordObj, int numberGeneration);
	  int getOptDirection() { return MAXIMIZE; }
    bool init(const vector<string> &params);

  private:

		// Private Members
		// Fuzzy Engine
		fl::Engine* multiObjFuzzyEngine;
		// Fuzzy Input Variables
		fl::InputVariable* fit;
		fl::InputVariable* thrIn;
		fl::InputVariable* var;
		// Fuzzy Output Variables
		fl::OutputVariable* thrOut;

		long double shiftValue;
		long double delta;
		int numGenToFuzzify;
		int k;

		bool initStage;
    long double pThreshold;
		long double bestPThreshold;
		long double prevBestFitness;

		vector<long double> initStageValues;
		vector< vector<long double> > improvements;
		vector< vector<long double> > memberships;
		vector<long double> weightedImprovements;
		vector<long double> thresholdHistory;	
		vector<long double> improvementHistory;
		vector<long double> momentHistory;

		// Private Methods
		// Looks for the best individual between the parents and the offsprings
		Individual* lookForBestIndividual(vector<Individual *> *population, vector<Individual *> *offSpring);

		// Methods to obtain the values to fuzzify the variables THR-IN and VAR
		long double getFitValue();
		long double getVarValue();

		// Updates the objective value of the population and the offspring considering the current threshold value
		bool updateObjectiveValue(vector<Individual *> *population, vector<Individual *> *offSpring, Individual *best, int ordObj);

};
#endif
