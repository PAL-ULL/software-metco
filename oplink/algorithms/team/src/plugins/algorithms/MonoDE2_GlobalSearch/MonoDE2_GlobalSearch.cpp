#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include "MonoDE2_GlobalSearch.h"
#include "utils.h"
#include "Definitions.h" 
  
struct extIndType {
  int id;
  Individual* ind;
};

double MonoDE2_GlobalSearch::getEuclideanDistance(Individual *ind1, Individual *ind2) {
  double dist = 0.0;
  for (int i = 0; i < ind1->getNumberOfVar(); i++) {
    dist += ((ind1->getVar(i) - ind2->getVar(i)) * (ind1->getVar(i) - ind2->getVar(i)));
  }
  return sqrt(dist);
}

double MonoDE2_GlobalSearch::getDistanceToClosestNeighbour(Individual *ind) {
  double minDist = DBL_MAX;
  for (int i = 0; i < getPopulationSize(); i++) {
    if (ind != (*population)[i]) {
      double dist = getEuclideanDistance(ind, (*population)[i]);
      if (dist < minDist)
        minDist = dist;
    }
  }
  return minDist;
}

double MonoDE2_GlobalSearch::getMeanDistanceToClosestNeighbour() {
  double sumDist = 0.0;

  for (int i = 0; i < getPopulationSize(); i++) {
    sumDist += getDistanceToClosestNeighbour((*population)[i]);
  }

  double meanDCN = sumDist / (double)getPopulationSize();
  if (meanDCN > maxMeanDCN)
    maxMeanDCN = meanDCN;
  return meanDCN;
}

double MonoDE2_GlobalSearch::getNormalisedMeanDistanceToClosestNeighbour() {
  return getMeanDistanceToClosestNeighbour() / maxMeanDCN;
}

double MonoDE2_GlobalSearch::getMeanPairwiseDistanceAllIndividuals() {
  double sumDist = 0.0;

  for (int i = 0; i < getPopulationSize(); i++) {
    for (int j = i + 1; j < getPopulationSize(); j++) {
      sumDist += getEuclideanDistance((*population)[i], (*population)[j]);
    }
  }

  double meanPairwise = sumDist / (double)(getPopulationSize() * (getPopulationSize() - 1) / 2.0);
  if (meanPairwise > maxMeanPairwise)
    maxMeanPairwise = meanPairwise;
  return meanPairwise;
}

double MonoDE2_GlobalSearch::getNormalisedMeanPairwiseDistanceAllIndividuals() {
  return getMeanPairwiseDistanceAllIndividuals() / maxMeanPairwise;
}

bool sortByObj(const Individual *i1, const Individual *i2){
	if ((i1->getOptDirection(0) == MINIMIZE) && (i1->getObj(0) < i2->getObj(0))){
		return true;
	} else if ((i1->getOptDirection(0) == MAXIMIZE) && (i1->getObj(0) > i2->getObj(0))){
		return false;
	}
}

void readParameter(map<string,string> &parameters, const string &parameter, double &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de JADE no definido" << endl;
		exit(-1);
	}
	value = atof(parameters[parameter].c_str());
	parameters.erase(parameter);
}

void readParameter(map<string,string> &parameters, const string &parameter, int &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de JADE no definido" << endl;
		exit(-1);
	}
	value = atoi(parameters[parameter].c_str());
	parameters.erase(parameter);
}

void readParameter(map<string,string> &parameters, const string &parameter, string &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de JADE no definido" << endl;
		exit(-1);
	}
	value = parameters[parameter];
	parameters.erase(parameter);
}

bool MonoDE2_GlobalSearch::init(const vector<string> &params){
	if ((params.size() % 2) != 0) {
		cerr << "Error en los parametros de JADE. Deben ser multiplos de 2" << endl;
    cerr << "Current number of parameters: " << params.size() << endl;
		return false;
	}
	
	replacements = 0;
	map<string, string> parameters;
	for (int i = 0; i < params.size(); i+=2){
		parameters[params[i]] = params[i+1];
		/*if (params[i+1] != "="){
			cerr << "Error en los parametros de JADE. Deben estar formados por grupos de la forma parametro = valor" << endl;
			return false;
		}*/
	}
	
	readParameter(parameters, "Ps", ps);
	setPopulationSize(ps);
	offspring.insert(offspring.begin(), ps, NULL);
	fValues.insert(fValues.begin(), ps, 0);
	crValues.insert(crValues.begin(), ps, 0);

	string trialStrategy_str;
	readParameter(parameters, "TrialStrategy", trialStrategy_str);
	if (trialStrategy_str == "Current_to_pbest"){
		trialStrategy = TRIAL_CURRENT_TO_PBEST;
		readParameter(parameters, "p", p);
	} else if (trialStrategy_str == "Current_to_pbest_div"){
		trialStrategy = TRIAL_CURRENT_TO_PBEST_DIV;
		readParameter(parameters, "p", p);
	} else if (trialStrategy_str == "Rand_1"){
		trialStrategy = TRIAL_RAND_1;
	} else if (trialStrategy_str == "Rand_1_div"){
		trialStrategy = TRIAL_RAND_1_DIV;
	} else if (trialStrategy_str == "Current_to_rand") {
		trialStrategy = TRIAL_CURRENT_TO_RAND;
	} else if (trialStrategy_str == "Current_to_similarity") {
		trialStrategy = TRIAL_CURRENT_TO_SIM;
    readParameter(parameters, "MinPopPercentage", minPopPercentage);
	} else {
		cout << "Trial vector generation strategy no reconocida: " << trialStrategy_str << endl;
		exit(-1);
	}

	string crossover_str;
	readParameter(parameters, "Crossover", crossover_str);
	if (crossover_str == "Exp"){
		crossover = CROSSOVER_EXP;
	} else if (crossover_str == "Bin"){
		crossover = CROSSOVER_BIN;
	} else if (crossover_str == "Combined"){
		crossover = CROSSOVER_COMBINED;
	} else {
		cout << "Crossover no reconocido: " << crossover_str << endl;
		exit(-1);
	}

	nuF = 0.5;
	nuCR = 0.5;

	string fTypeStr;
	readParameter(parameters, "FType", fTypeStr);
	updateNuFRequired = false;
	if (fTypeStr == "CauchyNuF"){
		fType = F_CAUCHY_NUF;
		string updateFStr;
		readParameter(parameters, "UpdateNuF", updateFStr);
		if (updateFStr == "JADE"){
			updateNuFStrategy = UPDATE_NUF_JADE;
			updateNuFRequired = true;
		} else if (updateFStr == "Incr"){
			updateNuFStrategy = UPDATE_NUF_INCR;
			updateNuFRequired = true;
			readParameter(parameters, "FIncr", fIncr);
			readParameter(parameters, "InitialNuF", nuF);
		} else if (updateFStr == "No"){
			updateNuFStrategy = UPDATE_NUF_NO;
			readParameter(parameters, "NuF", nuF);
			updateNuFRequired = false;
		} else if (updateFStr == "JADEAMean"){
			updateNuFStrategy = UPDATE_NUF_JADEAMEAN;
			updateNuFRequired = true;
		} else {
			cout << "Valor de UpdateF no reconocido" << endl;
			exit(-1);
		}
	} else if (fTypeStr == "Normal"){
		fType = F_NORMAL;
		readParameter(parameters, "FNormalNu", FnormalNu);	
		readParameter(parameters, "FNormalSigma", FnormalSigma);	
	} else if (fTypeStr == "Fixed"){
		fType = F_FIXED;
		readParameter(parameters, "F", FFixed);
	} else if (fTypeStr == "jDE"){
		fType = F_JDE;
		readParameter(parameters, "jDE_Fmin", jDE_Fmin);
		readParameter(parameters, "jDE_Fmax", jDE_Fmax);
		readParameter(parameters, "jDE_TauF", jDE_tauF);
		for (int i = 0; i < ps; i++){
			currentFValues.push_back(generateRandomDoubleMinMax(jDE_Fmin, jDE_Fmax));
		}
	} else if (fTypeStr == "Competitive"){
		fType = F_COMPETITIVE;
		readParameter(parameters, "competitive_Fmin", competitive_fMin);
		readParameter(parameters, "competitive_Fmax", competitive_fMax);
		readParameter(parameters, "competitive_Fcount", competitive_fCount);
		readParameter(parameters, "competitive_FN0", competitive_fN0);
		readParameter(parameters, "competitive_FDelta", competitive_fdelta);
		fValuesCompetitiveIndex.insert(fValuesCompetitiveIndex.begin(), ps, -1000);
		if (competitive_fCount <= 1){
			cout << "competitive_Fcount debe ser mayor que 1" << endl;
			exit(-1);
		}
		if (competitive_fMin >= competitive_fMax){
			cout << "competitive_Fmax debe ser mayor que competitive_Fmin" << endl;
			exit(-1);
		}
		double dif = (competitive_fMax - competitive_fMin) / (competitive_fCount - 1);
		double currentF = competitive_fMin;
		for (int i = 0; i < competitive_fCount; i++){
			competitiveFs.push_back(currentF);
			competitiveFsCount.push_back(competitive_fN0);
			currentF += dif;
		}
	} else {
		cout << "FType no reconocido" << endl;
		exit(-1);
	}

	string crTypeStr;
	readParameter(parameters, "CRType", crTypeStr);
	if (crTypeStr == "NormalNuCR"){
		crType = CR_NORMAL_NUCR;

		string updateNuCRStr;
		readParameter(parameters, "UpdateNuCR", updateNuCRStr);
		updateNuCRRequired = false;

		if (updateNuCRStr == "JADEAMean"){
			updateNuCRRequired = true;
			updateNuCRStrategy = UPDATE_NUCR_JADEAMEAN;
		} else if (updateNuCRStr == "rJADE"){
			updateNuCRRequired = true;
			updateNuCRStrategy = UPDATE_NUCR_RJADE;
		} else if (updateNuCRStr == "SaDE"){
			updateNuCRRequired = true;
			updateNuCRStrategy = UPDATE_NUCR_SADE;
			readParameter(parameters, "LP", LP);
			readParameter(parameters, "CRMemSize", CRMemSize);
		} else if (updateNuCRStr == "No"){
			updateNuCRRequired = false;
			updateNuCRStrategy = UPDATE_NUCR_NO;
			readParameter(parameters, "NuCR", nuCR);
		} else {
			cout << "Valor de UpdateNuCR no reconocido" << endl;
			exit(-1);
		}
	} else if (crTypeStr == "Normal"){
		crType = CR_NORMAL;
		readParameter(parameters, "CRNormalNu", CRnormalNu);	
		readParameter(parameters, "CRNormalSigma", CRnormalSigma);	
	} else if (crTypeStr == "Cauchy"){
		crType = CR_CAUCHY;
		readParameter(parameters, "CRCauchyNu", CRcauchyNu);
		readParameter(parameters, "CRCauchyLocFactor", CRcauchyLocFactor);
	} else if (crTypeStr == "Fixed"){
		crType = CR_FIXED;
		if (crossover != CROSSOVER_COMBINED){
			readParameter(parameters, "CR", CRFixed);
		} else {
			readParameter(parameters, "CRBin", CRBin);
			readParameter(parameters, "CRExp", CRExp);
		}
	} else if (crTypeStr == "jDE"){
		crType = CR_JDE;
		readParameter(parameters, "jDE_CRmin", jDE_CRmin);
		readParameter(parameters, "jDE_CRmax", jDE_CRmax);
		readParameter(parameters, "jDE_TauCR", jDE_tauCR);
		for (int i = 0; i < ps; i++){
			currentCRValues.push_back(generateRandomDoubleMinMax(jDE_CRmin, jDE_CRmax));
		}
	} else if (crTypeStr == "Uni0_015_085_1"){
		crType = CR_UNI_0_015_085_1;
	} else if (crTypeStr == "Uniform"){
		crType = CR_UNIFORM;
		readParameter(parameters, "CRmin", CRMin_uni);
		readParameter(parameters, "CRmax", CRMax_uni);
	} else if (crTypeStr == "Competitive"){
		crType = CR_COMPETITIVE;
		readParameter(parameters, "competitive_CRmin", competitive_crMin);
		readParameter(parameters, "competitive_CRmax", competitive_crMax);
		readParameter(parameters, "competitive_CRcount", competitive_crCount);
		readParameter(parameters, "competitive_CRN0", competitive_crN0);
		readParameter(parameters, "competitive_CRDelta", competitive_crdelta);
		crValuesCompetitiveIndex.insert(crValuesCompetitiveIndex.begin(), ps, -1000);
		if (competitive_crCount <= 1){
			cout << "competitive_crCount debe ser mayor que 1" << endl;
			exit(-1);
		}
		if (competitive_crMin >= competitive_crMax){
			cout << "competitive_crmax debe ser mayor que competitive_crmin" << endl;
			exit(-1);
		}
		double dif = (competitive_crMax - competitive_crMin) / (competitive_crCount - 1);
		double currentCR = competitive_crMin;
		for (int i = 0; i < competitive_crCount; i++){
			competitiveCRs.push_back(currentCR);
			competitiveCRsCount.push_back(competitive_crN0);
			currentCR += dif;
		}
	} else {
		cout << "CRType no reconocido" << endl;
		exit(-1);
	}
	
	if ((updateNuFStrategy == UPDATE_NUF_JADE) || (updateNuFStrategy == UPDATE_NUF_JADEAMEAN) || (updateNuCRStrategy == UPDATE_NUCR_JADEAMEAN) || (updateNuCRStrategy == UPDATE_NUCR_RJADE)){
		readParameter(parameters, "c", c);
	}
	
	readParameter(parameters, "Seed", seed);

	string popInitTypeStr;
	readParameter(parameters, "PopInitType", popInitTypeStr);
	if (popInitTypeStr == "Restart"){
		popInitType = POP_INIT_RESTART;
	} else if (popInitTypeStr == "MersenneTwister") {
		popInitType = POP_INIT_MERSENNE_TWISTER;
	} else if (popInitTypeStr == "TentMap") {
		popInitType = POP_INIT_TENT_MAP;
	} else if (popInitTypeStr == "Sobol") {
		popInitType = POP_INIT_SOBOL;
	} else if (popInitTypeStr == "GoodLatticePoints") {
		popInitType = POP_INIT_LATTICE;
	} else if (popInitTypeStr == "OppositionBasedLearning") {
		popInitType = POP_INIT_OPPOSITION_BASED_LEARNING;
	} else if (popInitTypeStr == "QuasiOppositionBasedLearning") {
		popInitType = POP_INIT_QUASI_OPPOSITION_BASED_LEARNING;
	} else if (popInitTypeStr == "QuasiReflectedOppositionBasedLearning") {
		popInitType = POP_INIT_QUASI_REFLECTED_OPPOSITION_BASED_LEARNING;
	} else {
		cout << "Valor de PopInitType no reconocido: " << popInitTypeStr << endl;
		exit(-1);
	}

	string reinitializationStr;
	readParameter(parameters, "Reinitialization", reinitializationStr);
	if (reinitializationStr == "Random"){
		reinitializationType = REINIT_RANDOM;
	} else if (reinitializationStr == "BounceBack") {
		reinitializationType = REINIT_BOUNCE_BACK;
	} else {
		cout << "Valor de Reinitialization no reconocido: " << reinitializationStr << endl;
		exit(-1);
	}

  string globalSearchTypeStr;
  globalSearchRequired = false;
  readParameter(parameters, "LocalSearchType", globalSearchTypeStr);
  if (globalSearchTypeStr == "No") {
    globalSearchType = GS_NO;
    globalSearchRequired= false;
  } else if (globalSearchTypeStr == "GlobalNeighbourhood") {
    globalSearchType = GS_GLOBAL_NEIGHBOURHOOD;
    globalSearchRequired = true;
  } else if (globalSearchTypeStr == "GlobalNeighbourhoodPathSearch") {
    globalSearchType = GS_GLOBAL_NEIGHBOURHOOD_PATH_SEARCH;
    globalSearchRequired = true;
    readParameter(parameters, "MinPopPercentage", minPopPercentage);
  } else {
    cout << "Valor de LocalSearchType no reconocido: " << globalSearchTypeStr << endl;
    exit(-1);
  }

  maxVarValues = 1000;
  varThreshold = 0.5;
  
  string nelderMeadTypeStr;
  nelderMeadRequired = false;
  readParameter(parameters, "NelderMeadType", nelderMeadTypeStr);
  if (nelderMeadTypeStr == "No") {
    nelderMeadType = NM_NO;
    nelderMeadRequired= false;
  } else if (nelderMeadTypeStr == "NelderMead") {
    nelderMeadType = NM_NELDER_MEAD;
    nelderMeadRequired = true;
  } else if (nelderMeadTypeStr == "NelderMeadAdaptive") {
    nelderMeadType = NM_NELDER_MEAD_ADAPTIVE;
    nelderMeadRequired = true;
  } else {
    cout << "Valor de NelderMeadType no reconocido: " << nelderMeadTypeStr << endl;
    exit(-1);
  }

  string jumpingTypeStr;
  jumpingRequired = false;
  readParameter(parameters, "JumpingType", jumpingTypeStr);
  if (jumpingTypeStr == "No") {
    jumpingType = J_NO;
    jumpingRequired= false;
  } else if (jumpingTypeStr == "QOBL") {
    jumpingType = J_QOBL;
    jumpingRequired = true;
    readParameter(parameters, "JumpingRate", jumpingRate);
  } else if (jumpingTypeStr == "QROBL") {
    jumpingType = J_QROBL;
    jumpingRequired = true;
    readParameter(parameters, "JumpingRate", jumpingRate);
  } else {
    cout << "Valor de JumpingType no reconocido: " << jumpingTypeStr << endl;
    exit(-1);
  }

  if (getSampleInd()->getOptDirection(0) == MINIMIZE) {
    prevGenBestFitness = DBL_MAX;
  }
  else {
    prevGenBestFitness = DBL_MIN;
  }
	
  if (parameters.size()) {
		cout << "Error: algunos parametros no se utilizaron" << endl;
		for (map<string, string>::iterator it = parameters.begin(); it != parameters.end(); it++) {
			cout << it->first << " ";
		}
		cout << endl;
		exit(-1);
	}
  
  maxMeanDCN = DBL_MIN;
  maxMeanPairwise = DBL_MIN;

	return true;
}

void MonoDE2_GlobalSearch::fillPopWithRestart() {
	for (int i = population->size(); i < getPopulationSize(); i++) {
		Individual *ind = getSampleInd()->internalClone();
		ind->restart();
		evaluate(ind);
		population->push_back(ind);
	}
}

void MonoDE2_GlobalSearch::fillPopWithMersenneTwister() {
	//mt19937 generator(seed);
	for (int i = population->size(); i < getPopulationSize(); i++) {
		Individual *ind = getSampleInd()->internalClone();
		for (int j = 0; j < ind->getNumberOfVar(); j++) {
			//double aux = (double) generator() / (double) generator.max();
      double aux = ((double) rand () / (double) RAND_MAX);
			ind->setVar(j, aux * (ind->getMaximum(j) - ind->getMinimum(j)) + ind->getMinimum(j)); 
		}
		evaluate(ind);
		population->push_back(ind);
	}
}

void MonoDE2_GlobalSearch::fillPopWithTentMap() {
	int mu = 1; // Bifurcation factor
	//mt19937 generator(seed); // for z^(i)_0 values
	int firstPopSize = population->size(); // For cases where the population size is greater than 0

	if (firstPopSize < getPopulationSize()) {
		// Generates the first individual
		Individual *ind = getSampleInd()->internalClone();
		for (int i = 0; i < ind->getNumberOfVar(); i++) {
			//double z_ini = (double) generator() / (double) generator.max();
      double z_ini = ((double) rand () / (double) RAND_MAX);
			ind->setVar(i, mu * (1 - 2 * (abs(z_ini - 0.5))));
		}
		population->push_back(ind);

		// Generates the remaining individuals
		for (int i = population->size(); i < getPopulationSize(); i++) {
			Individual *ind = getSampleInd()->internalClone();
			for (int j = 0; j < ind->getNumberOfVar(); j++) {
				ind->setVar(j, mu * (1 - 2 * (abs((*population)[i - 1]->getVar(j) - 0.5))));
			}
			population->push_back(ind);
		}

		// Maps all the generated individuals to the range of the decision variables and evaluates them
		for (int i = firstPopSize; i < getPopulationSize(); i++) {
			Individual *ind = (*population)[i];
			for (int j = 0; j < ind->getNumberOfVar(); j++) {
				ind->setVar(j, ind->getVar(j) * (ind->getMaximum(j) - ind->getMinimum(j)) + ind->getMinimum(j));
			}
			evaluate(ind);
		}
	}
}

void MonoDE2_GlobalSearch::fillPopWithSobol() {
	string fileName = "/home/edusegre/oplink/algorithms/team/src/skeleton/utils/sobol/joe-kuo-old.1111";
	double **sobolPoints = sobol_points(getPopulationSize() - population->size(), getSampleInd()->getNumberOfVar(), fileName.c_str());

	for (int i = population->size(); i < getPopulationSize(); i++) {
		Individual *ind = getSampleInd()->internalClone();
		for (int j = 0; j < ind->getNumberOfVar(); j++) {
			double aux = sobolPoints[i][j];
			ind->setVar(j, aux * (ind->getMaximum(j) - ind->getMinimum(j)) + ind->getMinimum(j));
		}
		evaluate(ind);
		population->push_back(ind);
	}

	delete[] sobolPoints;
}

void MonoDE2_GlobalSearch::fillPopWithGoodLatticePoints() {
	int *z = find_z20(getSampleInd()->getNumberOfVar(), getPopulationSize() - population->size());

	for (int i = population->size(); i < getPopulationSize(); i++) {
		Individual *ind = getSampleInd()->internalClone();
		for (int j = 0; j < ind->getNumberOfVar(); j++) {
			double aux = ((i * z[j]) % getPopulationSize()) / (double) getPopulationSize();
			ind->setVar(j, aux * (ind->getMaximum(j) - ind->getMinimum(j)) + ind->getMinimum(j));
		}
		evaluate(ind);
		population->push_back(ind);
	}
	
	delete[] z;
}

void MonoDE2_GlobalSearch::fillPopWithOppositionBasedLearning() {
	//mt19937 generator(seed);

	// Generates a total number of individuals equal to (getPopulationSize() - population->size()) * 2
	vector<Individual *> genPopulation;
	for (int i = population->size(); i < getPopulationSize(); i++) {
		Individual *ind = getSampleInd()->internalClone();
		Individual *opp_ind = getSampleInd()->internalClone();
		for (int j = 0; j < ind->getNumberOfVar(); j++) {
			// Generates a random population
			//double aux = (double) generator() / (double) generator.max();
      double aux = ((double) rand () / (double) RAND_MAX);
			ind->setVar(j, aux * (ind->getMaximum(j) - ind->getMinimum(j)) + ind->getMinimum(j)); 

			// Generates the oppositional population
			opp_ind->setVar(j, opp_ind->getMinimum(j) + opp_ind->getMaximum(j) - ind->getVar(j));
		}
		evaluate(ind);
		evaluate(opp_ind);
		genPopulation.push_back(ind);
		genPopulation.push_back(opp_ind);
	}

	sort(genPopulation.begin(), genPopulation.end(), sortByObj);

  int j = 0;
	for (int i = population->size(); i < getPopulationSize(); i++) {
		population->push_back(genPopulation[j++]);
	}

  for (int i = j; j < genPopulation.size(); j++)
    delete genPopulation[j];
	genPopulation.clear();
}

void MonoDE2_GlobalSearch::fillPopWithQuasiOppositionBasedLearning() {
	//mt19937 generator(seed);

	// Generates a total number of individuals equal to (getPopulationSize() - population->size()) * 2
	vector<Individual *> genPopulation;
	for (int i = population->size(); i < getPopulationSize(); i++) {
		Individual *ind = getSampleInd()->internalClone();
		Individual *qopp_ind = getSampleInd()->internalClone();
		for (int j = 0; j < ind->getNumberOfVar(); j++) {
			// Generates a random population
			//double aux = (double) generator() / (double) generator.max();
      double aux = ((double) rand () / (double) RAND_MAX);
			ind->setVar(j, aux * (ind->getMaximum(j) - ind->getMinimum(j)) + ind->getMinimum(j)); 

			// Generates the quasi-oppositional population
			double opp = qopp_ind->getMinimum(j) + qopp_ind->getMaximum(j) - ind->getVar(j);
			double m = (qopp_ind->getMinimum(j) + qopp_ind->getMaximum(j)) / 2.0;
			//double aux2 = (double) generator() / (double) generator.max();
      double aux2 = ((double) rand () / (double) RAND_MAX);
			if (ind->getVar(j) < m) {
				qopp_ind->setVar(j, m + (opp - m) * aux2);
			}
			else {
				qopp_ind->setVar(j, opp + (m - opp) * aux2);
			}
		}
		evaluate(ind);
		evaluate(qopp_ind);
		genPopulation.push_back(ind);
		genPopulation.push_back(qopp_ind);
	}

  sort(genPopulation.begin(), genPopulation.end(), sortByObj);
  
  int j = 0;
	for (int i = population->size(); i < getPopulationSize(); i++) {
		population->push_back(genPopulation[j++]);
	}

  for (int i = j; j < genPopulation.size(); j++)
    delete genPopulation[j];
	genPopulation.clear();
}

void MonoDE2_GlobalSearch::fillPopWithQuasiReflectedOppositionBasedLearning() {
	//mt19937 generator(seed);

	// Generates a total number of individuals equal to (getPopulationSize() - population->size()) * 2
	vector<Individual *> genPopulation;
	for (int i = population->size(); i < getPopulationSize(); i++) {
		Individual *ind = getSampleInd()->internalClone();
		Individual *qropp_ind = getSampleInd()->internalClone();
		for (int j = 0; j < ind->getNumberOfVar(); j++) {
			// Generates a random population
			//double aux = (double) generator() / (double) generator.max();
      double aux = ((double) rand () / (double) RAND_MAX);
			ind->setVar(j, aux * (ind->getMaximum(j) - ind->getMinimum(j)) + ind->getMinimum(j)); 

			// Generates the quasi-reflected-oppositional population
			double m = (qropp_ind->getMinimum(j) + qropp_ind->getMaximum(j)) / 2.0;
			//double aux2 = (double) generator() / (double) generator.max();
      double aux2 = ((double) rand () / (double) RAND_MAX);
			if (ind->getVar(j) < m) {
				qropp_ind->setVar(j, ind->getVar(j) + (m - ind->getVar(j)) * aux2);
			}
			else {
				qropp_ind->setVar(j, m + (ind->getVar(j) - m) * aux2);
			}
		}
		evaluate(ind);
		evaluate(qropp_ind);
		genPopulation.push_back(ind);
		genPopulation.push_back(qropp_ind);
	}

  sort(genPopulation.begin(), genPopulation.end(), sortByObj);
  
  int j = 0;
	for (int i = population->size(); i < getPopulationSize(); i++) {
		population->push_back(genPopulation[j++]);
	}

  for (int i = j; j < genPopulation.size(); j++)
    delete genPopulation[j];
	genPopulation.clear();
}

void MonoDE2_GlobalSearch::fillPopWithNewIndsAndEvaluate() {
	if (popInitType == POP_INIT_RESTART) {
		fillPopWithRestart();
	}
	else if (popInitType == POP_INIT_MERSENNE_TWISTER) {
		fillPopWithMersenneTwister();
	}
	else if (popInitType == POP_INIT_TENT_MAP) {
		fillPopWithTentMap();
	}
	else if (popInitType == POP_INIT_SOBOL) {
		fillPopWithSobol();
	}
	else if (popInitType == POP_INIT_LATTICE) {
		fillPopWithGoodLatticePoints();
	}
	else if (popInitType == POP_INIT_OPPOSITION_BASED_LEARNING) {
		fillPopWithOppositionBasedLearning();
	}
	else if (popInitType == POP_INIT_QUASI_OPPOSITION_BASED_LEARNING) {
		fillPopWithQuasiOppositionBasedLearning();
	}
	else if (popInitType == POP_INIT_QUASI_REFLECTED_OPPOSITION_BASED_LEARNING) {
		fillPopWithQuasiReflectedOppositionBasedLearning();
	}
}

void MonoDE2_GlobalSearch::globalSearch() {
  if (globalSearchType == GS_GLOBAL_NEIGHBOURHOOD) {
    globalSearchGlobalNeighbourhood();
  }
  else if (globalSearchType == GS_GLOBAL_NEIGHBOURHOOD_PATH_SEARCH) {
    globalSearchGlobalNeighbourhood12();
  }
}

void MonoDE2_GlobalSearch::NelderMeadSelect() {
  if (nelderMeadType == NM_NELDER_MEAD) {
    NelderMeadNoShrink();
  }
  else if (nelderMeadType == NM_NELDER_MEAD_ADAPTIVE) {
    NelderMeadAdaptive();
  }
}

// Implementation of the original proposal
void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood() {
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

  // Randomly selects three mutually exclusive integers belonging to
  // the set {0, 1, ..., NP - 1}
	int k, r1, r2;

  k = getRandomInteger0_N(ps - 1);

  do {
    r1 = getRandomInteger0_N(ps - 1);
  } while (r1 == k);

  do {
    r2 = getRandomInteger0_N(ps - 1);
  } while ((r2 == r1) || (r2 == k));

  // Determines the best individual in the population
  Individual *best = (*population)[0];
  for (int i = 1; i < getPopulationSize(); i++) {
    if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
        (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
      best = (*population)[i];
    }
  }

  // Applies global neighbourhood search strategy
  Individual *v = getSampleInd()->internalClone();
  for (int i = 0; i < v->getNumberOfVar(); i++) {
    v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

    // Checks lower and upper limits of variables
    if (reinitializationType == REINIT_RANDOM) {
      if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
        double r = ((double) rand () / (double) RAND_MAX);
        v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
      }
    } else if (reinitializationType == REINIT_BOUNCE_BACK){
      if (v->getVar(i) < v->getMinimum(i)) {
        v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMinimum(i) - (*population)[r1]->getVar(i)));
      } else if (v->getVar(i) > v->getMaximum(i)){
        v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMaximum(i) - (*population)[r1]->getVar(i)));
      }
    }
    v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
    v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
  }

  // Evaluates the new individual
  evaluate(v);

  // Selects the best individual between v and k
  if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
      ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
    delete (*population)[k];
    (*population)[k] = v;
  } else {
    delete v;
	}
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood2() {
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;
      
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

//  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(ps - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      int r1, r2;

     	do {
    		r1 = getRandomInteger0_N(ps - 1);
    	} while (r1 == k);

    	do {
    		r2 = getRandomInteger0_N(ps - 1);
    	} while ((r2 == r1) || (r2 == k));
  
      // Determines the best individual in the population
      Individual *best = (*population)[0];
      for (int i = 1; i < getPopulationSize(); i++) {
        if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
            (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
          best = (*population)[i];
        }
      }

      // Applies global neighbourhood search strategy
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

        // Checks lower and upper limits of variables
        if (reinitializationType == REINIT_RANDOM) {
          if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
            double r = ((double) rand () / (double) RAND_MAX);
            v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
          }
        } else if (reinitializationType == REINIT_BOUNCE_BACK){
          if (v->getVar(i) < v->getMinimum(i)) {
            v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMinimum(i) - (*population)[r1]->getVar(i)));
          } else if (v->getVar(i) > v->getMaximum(i)){
            v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMaximum(i) - (*population)[r1]->getVar(i)));
          }
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
          ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        delete (*population)[k];
        (*population)[k] = v;
        improvement = true;
        numberImp++;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
 // } while ((numberImp > 0) && (numberExp < getPopulationSize()));
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood3() {
  sort(population->begin(), population->end(), sortByObj);
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1, r2;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));

      // Applies global neighbourhood search strategy
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * (*population)[0]->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood3_2() {
  sort(population->begin(), population->end(), sortByObj);
  Individual *best = (*population)[0];
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1, r2;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));

      // Applies global neighbourhood search strategy
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;

        // Checks if the new individual v is the best one
        if ((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < best->getObj(0)))
          best = v;
        else if ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > best->getObj(0)))
          best = v;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood4() {
  sort(population->begin(), population->end(), sortByObj);
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  // Calculates the centroid of the current population
  Individual *c = getSampleInd()->internalClone();
  for (int i = 0; i < c->getNumberOfVar(); i++) {
    double sum = 0;
    for (int j = 0; j < getPopulationSize(); j++) {
      sum += (*population)[j]->getVar(i);
    }
    c->setVar(i, sum / getPopulationSize());
  }

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1, r2;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));

      // Applies global neighbourhood search strategy considering the centroid of the population
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * c->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood5() {
  sort(population->begin(), population->end(), sortByObj);
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  // Calculates the centroid of the current population
  Individual *c = getSampleInd()->internalClone();
  for (int i = 0; i < c->getNumberOfVar(); i++) {
    double sum = 0;
    for (int j = 0; j < getPopulationSize(); j++) {
      sum += (*population)[j]->getVar(i);
    }
    c->setVar(i, sum / getPopulationSize());
  }

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	/*do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));*/

      // Applies global neighbourhood search strategy considering the centroid of the population
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * (*population)[0]->getVar(i) + a3 * (c->getVar(i) - (*population)[r1]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood5_2() {
  sort(population->begin(), population->end(), sortByObj);
  Individual *best = (*population)[0];
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  // Calculates the centroid of the current population
  Individual *c = getSampleInd()->internalClone();
  for (int i = 0; i < c->getNumberOfVar(); i++) {
    double sum = 0;
    for (int j = 0; j < getPopulationSize(); j++) {
      sum += (*population)[j]->getVar(i);
    }
    c->setVar(i, sum / getPopulationSize());
  }

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	/*do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));*/

      // Applies global neighbourhood search strategy considering the centroid of the population
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * (c->getVar(i) - (*population)[r1]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;

        // Checks if the new individual v is the best one
        if ((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < best->getObj(0)))
          best = v;
        else if ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > best->getObj(0)))
          best = v;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood6() {
  sort(population->begin(), population->end(), sortByObj);
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  // Calculates the centroid of the current population
  Individual *c = getSampleInd()->internalClone();
  for (int i = 0; i < c->getNumberOfVar(); i++) {
    double sum = 0;
    for (int j = 0; j < getPopulationSize(); j++) {
      sum += (*population)[j]->getVar(i);
    }
    c->setVar(i, sum / getPopulationSize());
  }

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	/*do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));*/

      // Applies global neighbourhood search strategy considering the centroid of the population
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * c->getVar(i) + a3 * ((*population)[0]->getVar(i) - (*population)[r1]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood6_2() {
  sort(population->begin(), population->end(), sortByObj);
  Individual *best = (*population)[0];
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  // Calculates the centroid of the current population
  Individual *c = getSampleInd()->internalClone();
  for (int i = 0; i < c->getNumberOfVar(); i++) {
    double sum = 0;
    for (int j = 0; j < getPopulationSize(); j++) {
      sum += (*population)[j]->getVar(i);
    }
    c->setVar(i, sum / getPopulationSize());
  }

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while ((explored[k]) && (numberExp < getPopulationSize()));

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	/*do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));*/

      // Applies global neighbourhood search strategy considering the centroid of the population
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * c->getVar(i) + a3 * (best->getVar(i) - (*population)[r1]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;

        // Checks if the new individual v is the best one
        if ((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < best->getObj(0)))
          best = v;
        else if ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > best->getObj(0)))
          best = v;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood6_3() {
  sort(population->begin(), population->end(), sortByObj);
  Individual *best = (*population)[0];
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  // Calculates the centroid of the current population
  Individual *c = getSampleInd()->internalClone();
  for (int i = 0; i < c->getNumberOfVar(); i++) {
    double sum = 0;
    for (int j = 0; j < getPopulationSize(); j++) {
      sum += (*population)[j]->getVar(i);
    }
    c->setVar(i, sum / getPopulationSize());
  }

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;
      
    // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
    // The condition a1 + a2 + a3 == 1 must be satisfied
    double a1, a2, a3;

    do {
      a1 = ((double) rand () / (double) RAND_MAX);
      a2 = ((double) rand () / (double) RAND_MAX);
      a3 = 1.0 - a1 - a2;
    } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

    do {
    
      // Random individual different from k
      int r1;
 
      do {
  	  	r1 = getRandomInteger0_N(getPopulationSize() - 1);
  	  } while (r1 == k);

    	/*do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));*/

      // Applies global neighbourhood search strategy considering the centroid of the population
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * c->getVar(i) + a3 * (best->getVar(i) - (*population)[r1]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;

        // Checks if the new individual v is the best one
        if ((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < best->getObj(0)))
          best = v;
        else if ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > best->getObj(0)))
          best = v;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);

  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood7() {
  sort(population->begin(), population->end(), sortByObj);
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  // Calculates the centroid of the current population
  Individual *c = getSampleInd()->internalClone();
  for (int i = 0; i < c->getNumberOfVar(); i++) {
    double sum = 0;
    for (int j = 0; j < getPopulationSize(); j++) {
      sum += (*population)[j]->getVar(i);
    }
    c->setVar(i, sum / getPopulationSize());
  }

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

     	/*do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);*/

    	/*do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));*/

      // Applies global neighbourhood search strategy considering the centroid of the population
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * (*population)[0]->getVar(i) + a3 * ((*population)[0]->getVar(i) - c->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood7_2() {
  sort(population->begin(), population->end(), sortByObj);
  Individual *best = (*population)[0];
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  // Calculates the centroid of the current population
  Individual *c = getSampleInd()->internalClone();
  for (int i = 0; i < c->getNumberOfVar(); i++) {
    double sum = 0;
    for (int j = 0; j < getPopulationSize(); j++) {
      sum += (*population)[j]->getVar(i);
    }
    c->setVar(i, sum / getPopulationSize());
  }

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	/*do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));*/

      // Applies global neighbourhood search strategy considering the centroid of the population
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * (best->getVar(i) - c->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;

        // Checks if the new individual v is the best one
        if ((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < best->getObj(0)))
          best = v;
        else if ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > best->getObj(0)))
          best = v;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood8() {
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

  bool improvement = false;
  int k;

  k = getRandomInteger0_N(getPopulationSize() - 1);

  // Calculate the number of elegible individuals for r1 and r2 depending on the
  // current moment of the search (given by the number of evaluations performed)
  int min_pop = max((int)(round(getPopulationSize() * 0.1)), 4);
  int max_pop = getPopulationSize();
  double m = (double)(min_pop - max_pop) / (double)getEvaluations();
  int y = (int)(round(m * getPerformedEvaluations() + max_pop));

  do {
    // Determines the best individual in the population
    Individual *best = (*population)[0];
    for (int i = 1; i < getPopulationSize(); i++) {
      if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
          (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
        best = (*population)[i];
      }
    }
    
    // Sort the population in ascending order in terms of the distance with respect to
    // the fittest individual
    sort(population->begin(), population->end(), bind(&MonoDE2_GlobalSearch::sortByDistanceToBest, this, std::placeholders::_1, std::placeholders::_2, best));

    /*for (int i = 0; i < getPopulationSize(); i++) {
      cout << (*population)[i]->getEuclideanDistanceDecisionSpace(best) << endl;
    }*/

    int r1, r2;

   	do {
  		r1 = getRandomInteger0_N(y - 1);
  	} while (r1 == k);

  	do {
  		r2 = getRandomInteger0_N(y - 1);
  	} while ((r2 == r1) || (r2 == k));

    // Applies global neighbourhood search strategy
    Individual *v = getSampleInd()->internalClone();
    for (int i = 0; i < v->getNumberOfVar(); i++) {
      v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

      // Checks lower and upper limits of variables
      if (reinitializationType == REINIT_RANDOM) {
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
      } else if (reinitializationType == REINIT_BOUNCE_BACK){
        if (v->getVar(i) < v->getMinimum(i)) {
          v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMinimum(i) - (*population)[r1]->getVar(i)));
        } else if (v->getVar(i) > v->getMaximum(i)){
          v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMaximum(i) - (*population)[r1]->getVar(i)));
        }
      }
      v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
      v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
    }

    // Evaluates the new individual
    evaluate(v);

    // Selects the best individual between v and k
    if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
        ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
      delete (*population)[k];
      (*population)[k] = v;
      improvement = true;
    } else {
      delete v;
      improvement = false;
	  }
  } while (improvement);
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood9() {
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

  bool improvement = false;
  int k;

  k = getRandomInteger0_N(getPopulationSize() - 1);

  // Calculate the number of elegible individuals for r1 and r2 depending on the
  // current moment of the search (given by the number of evaluations performed)
  int min_pop = max((int)(round(getPopulationSize() * 0.1)), 4);
  int max_pop = getPopulationSize();
  double m = (double)(max_pop - min_pop) / (double)getEvaluations();
  int y = (int)(round(m * getPerformedEvaluations() + min_pop));

  do {
    // Determines the best individual in the population
    Individual *best = (*population)[0];
    for (int i = 1; i < getPopulationSize(); i++) {
      if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
          (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
        best = (*population)[i];
      }
    }
    
    // Sort the population in ascending order in terms of the distance with respect to
    // the fittest individual
    sort(population->begin(), population->end(), bind(&MonoDE2_GlobalSearch::sortByDistanceToBest, this, std::placeholders::_1, std::placeholders::_2, best));

    int r1, r2;

   	do {
  		r1 = getRandomInteger0_N(y - 1);
  	} while (r1 == k);

  	do {
  		r2 = getRandomInteger0_N(y - 1);
  	} while ((r2 == r1) || (r2 == k));

    // Applies global neighbourhood search strategy
    Individual *v = getSampleInd()->internalClone();
    for (int i = 0; i < v->getNumberOfVar(); i++) {
      v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

      // Checks lower and upper limits of variables
      if (reinitializationType == REINIT_RANDOM) {
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
      } else if (reinitializationType == REINIT_BOUNCE_BACK){
        if (v->getVar(i) < v->getMinimum(i)) {
          v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMinimum(i) - (*population)[r1]->getVar(i)));
        } else if (v->getVar(i) > v->getMaximum(i)){
          v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMaximum(i) - (*population)[r1]->getVar(i)));
        }
      }
      v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
      v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
    }

    // Evaluates the new individual
    evaluate(v);

    // Selects the best individual between v and k
    if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
        ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
      delete (*population)[k];
      (*population)[k] = v;
      improvement = true;
    } else {
      delete v;
      improvement = false;
	  }
  } while (improvement);
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood10() {
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

  bool improvement = false;
  int k;

  k = getRandomInteger0_N(getPopulationSize() - 1);

  do {
    // Calculates the mean value of each decision variable of the current population
    vector<double> means((*population)[0]->getNumberOfVar(), 0);

    for (int i = 0; i < (*population)[0]->getNumberOfVar(); i++) {
      double sum = 0;
      for (int j = 0; j < getPopulationSize(); j++) {
        sum += (*population)[j]->getVar(i);
      }
      means[i] = sum / getPopulationSize();
    }
   
    // Calculates the diversity of the current population 
		double var = 0;
    for (int i = 0; i < getPopulationSize(); i++) {
      double sum = 0;
      for (int j = 0; j < (*population)[0]->getNumberOfVar(); j++) {
        double aux = (*population)[i]->getVar(j) - means[j];
        sum += (aux * aux);
      }
      var += sqrt(sum);
    }
    var /= getPopulationSize();
    
    //cout << "var (no norm): " << var << endl;

    // Inserts the new variance value into the variance value vector
    if (varValues.size() < maxVarValues) {
      varValues.push_back(var);
    }
    else {
      varValues.erase(varValues.begin());
      varValues.push_back(var);
    }

    // Normalises the value of the variance taking into account the
    // different values of the variance value vector
    double maxVar = DBL_MIN;
    for (int i = 0; i < varValues.size(); i++) {
      if (varValues[i] > maxVar) {
        maxVar = varValues[i];
      }
    }
    var /= maxVar;

    //cout << "var (norm): " << var << endl;
    //cout << "varThreshold: " << varThreshold << endl;

    // Determines the best individual in the population
    Individual *best = (*population)[0];
    for (int i = 1; i < getPopulationSize(); i++) {
      if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
          (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
        best = (*population)[i];
      }
    }
    
    // Sort the population in ascending order in terms of the distance with respect to
    // the fittest individual
    sort(population->begin(), population->end(), bind(&MonoDE2_GlobalSearch::sortByDistanceToBest, this, std::placeholders::_1, std::placeholders::_2, best));
    
    // Calculate the range of elegible individuals for r1 and r2 depending on the
    // current moment of the search (given by the variance of the population)
    int minRange, maxRange;
    int minPop = max((int)(round(getPopulationSize() * 0.1)), 4);

    // We should promote exploitation
    if (var >= varThreshold) {
      minRange = 0;
      maxRange = max((int)(round((1 - var) * getPopulationSize()) - 1), minPop - 1);
    }
    // we should promote diversification
    else {
      minRange = min((int)(round((1 - var) * getPopulationSize()) - 1), getPopulationSize() - minPop - 1);
      maxRange = getPopulationSize() - 1;
    }

    //cout << "minRange: " << minRange << endl;
    //cout << "maxRange: " << maxRange << endl;

    int r1, r2;

   	do {
  		r1 = generateRandom(minRange, maxRange);
  	} while (r1 == k);

  	do {
  		r2 = generateRandom(minRange, maxRange);
  	} while ((r2 == r1) || (r2 == k));

    //cout << "r1: " << r1 << endl;
    //cout << "r2: " << r2 << endl;

    // Applies global neighbourhood search strategy
    Individual *v = getSampleInd()->internalClone();
    for (int i = 0; i < v->getNumberOfVar(); i++) {
      v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

      // Checks lower and upper limits of variables
      if (reinitializationType == REINIT_RANDOM) {
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
      } else if (reinitializationType == REINIT_BOUNCE_BACK){
        if (v->getVar(i) < v->getMinimum(i)) {
          v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMinimum(i) - (*population)[r1]->getVar(i)));
        } else if (v->getVar(i) > v->getMaximum(i)){
          v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMaximum(i) - (*population)[r1]->getVar(i)));
        }
      }
      v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
      v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
    }

    // Evaluates the new individual
    evaluate(v);

    // Selects the best individual between v and k
    if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
        ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
      delete (*population)[k];
      (*population)[k] = v;
      improvement = true;
    } else {
      delete v;
      improvement = false;
	  }
  } while (improvement);
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood11() {
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

  int k;

  k = getRandomInteger0_N(getPopulationSize() - 1);

  // Calculate the number of elegible individuals for r1 and r2 depending on the
  // current moment of the search (given by the number of evaluations performed)
  int min_pop = max((int)(round(getPopulationSize() * 0.1)), 4);
  int max_pop = getPopulationSize();
  double m = (double)(min_pop - max_pop) / (double)getEvaluations();
  int y = (int)(round(m * getPerformedEvaluations() + max_pop));

  // Determines the best individual in the population
  Individual *best = (*population)[0];
  for (int i = 1; i < getPopulationSize(); i++) {
    if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
        (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
      best = (*population)[i];
    }
  }
    
  // Sort the population in ascending order in terms of the distance with respect to
  // the fittest individual
  sort(population->begin(), population->end(), bind(&MonoDE2_GlobalSearch::sortByDistanceToBest, this, std::placeholders::_1, std::placeholders::_2, best));

  int r1, r2;

 	do {
		r1 = getRandomInteger0_N(y - 1);
 	} while (r1 == k);

 	do {
 		r2 = getRandomInteger0_N(y - 1);
 	} while ((r2 == r1) || (r2 == k));

  // Applies global neighbourhood search strategy
  Individual *v = getSampleInd()->internalClone();
  for (int i = 0; i < v->getNumberOfVar(); i++) {
    v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

    // Checks lower and upper limits of variables
    if (reinitializationType == REINIT_RANDOM) {
      if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
        double r = ((double) rand () / (double) RAND_MAX);
        v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
      }
    } else if (reinitializationType == REINIT_BOUNCE_BACK){
      if (v->getVar(i) < v->getMinimum(i)) {
        v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMinimum(i) - (*population)[r1]->getVar(i)));
      } else if (v->getVar(i) > v->getMaximum(i)){
        v->setVar(i, (*population)[r1]->getVar(i) +  ((double) rand () / (double) RAND_MAX) * (v->getMaximum(i) - (*population)[r1]->getVar(i)));
      }
    }
    v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
    v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
  }

  // Evaluates the new individual
  evaluate(v);

  // Selects the best individual between v and k
  if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
      ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
    delete (*population)[k];
    (*population)[k] = v;
  } else {
    delete v;
  }
}

void MonoDE2_GlobalSearch::globalSearchGlobalNeighbourhood12() {
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  /*do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);*/
  
  a1 = ((double) rand () / (double) RAND_MAX);
  a2 = 1.0 - a1;

  //a1 = ((double) getPerformedEvaluations() / (double) getEvaluations());
  //a2 = 1.0 - a1;

  //cout << "a1: " << a1 << ", a2: " << a2 << ", a3: " << a3 << endl;

  int k = getRandomInteger0_N(getPopulationSize() - 1);

  // Calculate the number of elegible individuals for r1 depending on the
  // current moment of the search (given by the number of evaluations performed)
  int min_pop = min(max((int)(round(getPopulationSize() * minPopPercentage)), 5) - 1, getPopulationSize() - 1);
  int max_pop = getPopulationSize() - 1;
  double m = (double)(max_pop - min_pop) / (double)getEvaluations();
  int upper = (int)(round(m * getPerformedEvaluations() + min_pop));
  int lower = upper - min_pop;

  //cout << "lower: " << lower << ", upper: " << upper << endl;

  // Determines the best individual in the population
  Individual *best = (*population)[0];
  for (int i = 1; i < getPopulationSize(); i++) {
    if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
        (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
      best = (*population)[i];
    }
  }

  // Sorts the population in descending order in terms of the Euclidean distance in the decision space
  // with respect to the fittest individual
  sort(population->begin(), population->end(), bind(&MonoDE2_GlobalSearch::sortByDistanceToBestReversed, this, std::placeholders::_1, std::placeholders::_2, best));

  //for (int i = 0; i < getPopulationSize(); i++)
  // cout << "i: " << i << " Fitness: " << (*population)[i]->getObj(0) << " Distance to best: " << getEuclideanDistance(best, (*population)[i]) << endl;

  int r1, r2;

 	do {
    //r1 = getRandomInteger0_N(upper - 1);
    r1 = generateRandom(lower, upper);
 	} while (r1 == k);

  /*do {
  	r2 = generateRandom(lower, upper);
  } while ((r2 == r1) || (r2 == k));*/
  
  //cout << "k: " << k << ", r1: " << r1 << ", r2: " << r2 << endl;

  // Applies global neighbourhood search strategy
  Individual *v = getSampleInd()->internalClone();
  for (int i = 0; i < v->getNumberOfVar(); i++) {
    //v->setVar(i, (*population)[k]->getVar(i) + a1 * ((*population)[r1]->getVar(i) - (*population)[k]->getVar(i)));
    v->setVar(i, (*population)[k]->getVar(i) + a1 * (best->getVar(i) - (*population)[k]->getVar(i)) + a2 * ((*population)[r1]->getVar(i) - (*population)[k]->getVar(i)));
    //v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

    // Checks lower and upper limits of variables
    if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
      double r = ((double) rand () / (double) RAND_MAX);
      v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
    }
    v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
    v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
  }

  // Evaluates the new individual
  evaluate(v);

  //cout << "Fitness del nuevo individuo generado: " << v->getObj(0) << " Distancia al mejor: " << getEuclideanDistance(best, v) << endl;

  // 1. Looks for the best individual's closest neighbour in order to replace it by the novel individual
  /*int index_closest = getPopulationSize() - 1;

  while ((getEuclideanDistance(best, (*population)[index_closest]) == 0) && (index_closest > 0))
    index_closest--;

  //cout << "Individuo mas cercano al best: " << index_closest << endl;

  delete (*population)[index_closest];
  (*population)[index_closest] = v;*/
  
  // 2. Looks for the best individual's farthest neighbour in order to replace it by the novel individual
  int index_far = 0;
  delete (*population)[index_far];
  (*population)[index_far] = v;
  
  // 3. Selects the best individual between v and k
  /*if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
      ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
    delete (*population)[k];
    (*population)[k] = v;
  } else {
    delete v;
	}*/

  // 4. Looks for the best individual's most far away neighbour in order to replace it by the novel individual only
  // in the case the latter is better than the former 
  /*int index_far = 0;
  if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[index_far]->getObj(0))) ||
      ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[index_far]->getObj(0)))) {
    delete (*population)[index_far];
    (*population)[index_far] = v;
  } else {
    delete v;
  }*/
}

bool MonoDE2_GlobalSearch::sortByDistanceToBest(const Individual *i1, const Individual *i2, const Individual *best) {
	return (i1->getEuclideanDistanceDecisionSpace(best) < i2->getEuclideanDistanceDecisionSpace(best));
}

bool MonoDE2_GlobalSearch::sortByDistanceToBestReversed(const Individual *i1, const Individual *i2, const Individual *best) {
	return (i1->getEuclideanDistanceDecisionSpace(best) > i2->getEuclideanDistanceDecisionSpace(best));
}

bool MonoDE2_GlobalSearch::sortByDistanceToBestExtInd(const extIndType *i1, const extIndType *i2, const Individual *best) {
	return (i1->ind->getEuclideanDistanceDecisionSpace(best) < i2->ind->getEuclideanDistanceDecisionSpace(best));
}

bool MonoDE2_GlobalSearch::sortByDistanceToBestExtIndReversed(const extIndType *i1, const extIndType *i2, const Individual *best) {
	return (i1->ind->getEuclideanDistanceDecisionSpace(best) > i2->ind->getEuclideanDistanceDecisionSpace(best));
}

void MonoDE2_GlobalSearch::jumping() {
  if (jumpingType == J_QOBL) {
    jumping_QOBL();
  }
  else if (jumpingType == J_QROBL) {
    jumping_QROBL();
  }
}

/* This method considers the maximum and minimum values of the current population variables
   instead of the maximum and minimum values fixed by the problem */
bool MonoDE2_GlobalSearch::jumping_QOBL() {
  bool improvement = false;
  sort(population->begin(), population->end(), sortByObj);

  // Looks for the minimum and maximum values of the variables in the current population
  vector<double> currentMinVar;
  vector<double> currentMaxVar;
  for (int i = 0; i < (*population)[0]->getNumberOfVar(); i++) {
    currentMinVar.push_back((*population)[0]->getMaximum(i));
    currentMaxVar.push_back((*population)[0]->getMinimum(i));
    for (int j = 0; j < getPopulationSize(); j++) {
      if ((*population)[j]->getVar(i) < currentMinVar[i]) {
        currentMinVar[i] = (*population)[j]->getVar(i);
      }
      if ((*population)[j]->getVar(i) > currentMaxVar[i]) {
        currentMaxVar[i] = (*population)[j]->getVar(i);
      }
    }
  }

  // Generates the quasi-oppositional population
	for (int i = 0; i < getPopulationSize(); i++) {
		Individual *qopp_ind = getSampleInd()->internalClone();

		for (int j = 0; j < qopp_ind->getNumberOfVar(); j++) {
			// Generates the quasi-oppositional individual
			double opp = currentMinVar[j] + currentMaxVar[j] - (*population)[i]->getVar(j);
			double m = (currentMinVar[j] + currentMaxVar[j]) / 2.0;
			double r = ((double) rand () / (double) RAND_MAX);

			if ((*population)[i]->getVar(j) < m) {
				qopp_ind->setVar(j, m + (opp - m) * r);
			}
			else {
				qopp_ind->setVar(j, opp + (m - opp) * r);
			}

      // Checks lower and upper limits of variables
      if (reinitializationType == REINIT_RANDOM) {
        if ((qopp_ind->getVar(j) < currentMinVar[j]) || (qopp_ind->getVar(j) > currentMaxVar[j])) {
          double r = ((double) rand () / (double) RAND_MAX);
          qopp_ind->setVar(j, r * (currentMaxVar[j] - currentMinVar[j]) + currentMinVar[j]);
        }
      } else if (reinitializationType == REINIT_BOUNCE_BACK) {
  	    int r1 = getRandomInteger0_N(getPopulationSize() - 1);
        if (qopp_ind->getVar(j) < currentMinVar[j]) {
          qopp_ind->setVar(j, (*population)[r1]->getVar(j) +  ((double) rand () / (double) RAND_MAX) * (currentMinVar[j] - (*population)[r1]->getVar(j)));
        } else if (qopp_ind->getVar(j) > currentMaxVar[j]) {
          qopp_ind->setVar(j, (*population)[r1]->getVar(j) +  ((double) rand () / (double) RAND_MAX) * (currentMaxVar[j] - (*population)[r1]->getVar(j)));
        }
      }
      qopp_ind->setVar(j, max(qopp_ind->getVar(j), currentMinVar[j]));
      qopp_ind->setVar(j, min(qopp_ind->getVar(j), currentMaxVar[j]));
		}

 		evaluate(qopp_ind);
		population->push_back(qopp_ind);

    if (((qopp_ind->getOptDirection(0) == MINIMIZE) && (qopp_ind->getObj(0) < (*population)[0]->getObj(0))) || ((qopp_ind->getOptDirection(0) == MAXIMIZE) && (qopp_ind->getObj(0) > (*population)[0]->getObj(0))))
      improvement = true;
	}
  
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < getPopulationSize(); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}

  return improvement;
}

/* This method considers the maximum and minimum values of the current population variables
   instead of the maximum and minimum values fixed by the problem */
bool MonoDE2_GlobalSearch::jumping_QROBL() {
  bool improvement = false;
  sort(population->begin(), population->end(), sortByObj);

  // Looks for the minimum and maximum values of the variables in the current population
  vector<double> currentMinVar;
  vector<double> currentMaxVar;
  for (int i = 0; i < (*population)[0]->getNumberOfVar(); i++) {
    currentMinVar.push_back((*population)[0]->getMaximum(i));
    currentMaxVar.push_back((*population)[0]->getMinimum(i));
    for (int j = 0; j < getPopulationSize(); j++) {
      if ((*population)[j]->getVar(i) < currentMinVar[i]) {
        currentMinVar[i] = (*population)[j]->getVar(i);
      }
      if ((*population)[j]->getVar(i) > currentMaxVar[i]) {
        currentMaxVar[i] = (*population)[j]->getVar(i);
      }
    }
  }

  // Generates the quasi-reflected oppositional population
	for (int i = 0; i < getPopulationSize(); i++) {
		Individual *qropp_ind = getSampleInd()->internalClone();

		for (int j = 0; j < qropp_ind->getNumberOfVar(); j++) {
			// Generates the quasi-reflected oppositional individual
			double m = (currentMinVar[j] + currentMaxVar[j]) / 2.0;
			double r = ((double) rand () / (double) RAND_MAX);

			if ((*population)[i]->getVar(j) < m) {
				qropp_ind->setVar(j, (*population)[i]->getVar(j) + (m - (*population)[i]->getVar(j)) * r);
			}
			else {
				qropp_ind->setVar(j, m + ((*population)[i]->getVar(j) - m) * r);
			}

      // Checks lower and upper limits of variables
      if (reinitializationType == REINIT_RANDOM) {
        if ((qropp_ind->getVar(j) < currentMinVar[j]) || (qropp_ind->getVar(j) > currentMaxVar[j])) {
          double r = ((double) rand () / (double) RAND_MAX);
          qropp_ind->setVar(j, r * (currentMaxVar[j] - currentMinVar[j]) + currentMinVar[j]);
        }
      } else if (reinitializationType == REINIT_BOUNCE_BACK) {
  	    int r1 = getRandomInteger0_N(getPopulationSize() - 1);
        if (qropp_ind->getVar(j) < currentMinVar[j]) {
          qropp_ind->setVar(j, (*population)[r1]->getVar(j) +  ((double) rand () / (double) RAND_MAX) * (currentMinVar[j] - (*population)[r1]->getVar(j)));
        } else if (qropp_ind->getVar(j) > currentMaxVar[j]){
          qropp_ind->setVar(j, (*population)[r1]->getVar(j) +  ((double) rand () / (double) RAND_MAX) * (currentMaxVar[j] - (*population)[r1]->getVar(j)));
        }
      }
      qropp_ind->setVar(j, max(qropp_ind->getVar(j), currentMinVar[j]));
      qropp_ind->setVar(j, min(qropp_ind->getVar(j), currentMaxVar[j]));
		}
	
 		evaluate(qropp_ind);
		population->push_back(qropp_ind);

    if (((qropp_ind->getOptDirection(0) == MINIMIZE) && (qropp_ind->getObj(0) < (*population)[0]->getObj(0))) || ((qropp_ind->getOptDirection(0) == MAXIMIZE) && (qropp_ind->getObj(0) > (*population)[0]->getObj(0))))
      improvement = true;
	}
  
  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < getPopulationSize(); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}

  return improvement;
}

void MonoDE2_GlobalSearch::generateF(int candidate){
	if (fType == F_CAUCHY_NUF){
		do {
			fValues[candidate] = getRandomCauchy(nuF, 0.1);
		} while(fValues[candidate] < 0);
		fValues[candidate] = min(fValues[candidate], 1.0);
	} else if (fType == F_NORMAL){
		fValues[candidate] = generateRandomNormal(FnormalNu, FnormalSigma);
	} else if (fType == F_FIXED){
		fValues[candidate] = FFixed;
	} else if (fType == F_JDE){
		if (generateRandomDouble0_Max(1) < jDE_tauF){
			fValues[candidate] = generateRandomDoubleMinMax(jDE_Fmin, jDE_Fmax);
		} else {
			fValues[candidate] = currentFValues[candidate];
		}
	} else if (fType == F_COMPETITIVE){
		int sumSuccess = 0;
		for (int i = 0; i < competitiveFs.size(); i++){
			sumSuccess += competitiveFsCount[i];//Esto se podrÃ­a llevar actualizado en vez de calcularlo, pero son pocos asi que da igual
		}
		//Check if any of them is lower than delta
		for (int i = 0; i < competitiveFs.size(); i++){
			if (((double)	competitiveFsCount[i]) / sumSuccess < competitive_fdelta){
				sumSuccess = 0;
				for (int j = 0; j < competitiveFs.size(); j++){
					competitiveFsCount[j] = competitive_fN0;
					sumSuccess += competitive_fN0;
				}
				break;
			}
		}

		double r = generateRandomDouble0_Max(sumSuccess);
		int currentSum = 0;
		bool selected = false;
		for (int i = 0; i < competitiveFs.size(); i++){
			currentSum += competitiveFsCount[i];
			if (r <= currentSum){
				selected = true;
				fValues[candidate] = competitiveFs[i];
				fValuesCompetitiveIndex[candidate] = i;
				break;
			}
		}
		if (!selected){
			cout << "Error interno: no se selecciona valor de f en la competicion" << endl;
		}
	} else {
		cerr << "Error fType no reconocido" << endl;
		exit(-1);
	}
}

void MonoDE2_GlobalSearch::generateCR(int candidate){
	if (crossover == CROSSOVER_COMBINED){
		if ((getGeneration() % 2) == 1){//Exp
			crValues[candidate] = CRExp;
		} else {
			crValues[candidate] = CRBin;
		}
	} else if (crType == CR_NORMAL_NUCR){
		crValues[candidate] = generateRandomNormal(nuCR, 0.1);
		if (updateNuCRStrategy == UPDATE_NUCR_SADE){
			while( (crValues[candidate] < 0) || (crValues[candidate] > 1)){
				crValues[candidate] = generateRandomNormal(nuCR, 0.1);
			}
		}
	} else if (crType == CR_NORMAL){
		crValues[candidate] = generateRandomNormal(CRnormalNu, CRnormalSigma);
	} else if (crType == CR_CAUCHY){
		crValues[candidate] = getRandomCauchy(CRcauchyNu, CRcauchyLocFactor);
	} else if (crType == CR_FIXED){
		crValues[candidate] = CRFixed;
	} else if (crType == CR_JDE){
		if (generateRandomDouble0_Max(1) < jDE_tauCR){
			crValues[candidate] = generateRandomDoubleMinMax(jDE_CRmin, jDE_CRmax);
		} else {
			crValues[candidate] = currentCRValues[candidate];
		}
	} else if (crType == CR_UNI_0_015_085_1) {
		if (generateRandomDoubleMinMax(0,1) > 0.5){
			crValues[candidate] = generateRandomDoubleMinMax(0, 0.15);
		} else {
			crValues[candidate] = generateRandomDoubleMinMax(0.85, 1);
		}
	} else if (crType == CR_UNIFORM){
		crValues[candidate] = generateRandomDoubleMinMax(CRMin_uni, CRMax_uni);
	} else if (crType == CR_COMPETITIVE){
		int sumSuccess = 0;
		for (int i = 0; i < competitiveCRs.size(); i++){
			sumSuccess += competitiveCRsCount[i];//Esto se podrÃ­a llevar actualizado en vez de calcularlo, pero son pocos asi que da igual
		}

		//Check if any of them is lower than delta
		for (int i = 0; i < competitiveCRs.size(); i++){
			if (((double)	competitiveCRsCount[i]) / sumSuccess < competitive_crdelta){
				sumSuccess = 0;
				for (int j = 0; j < competitiveCRs.size(); j++){
					competitiveCRsCount[j] = competitive_crN0;
					sumSuccess += competitive_crN0;
				}
				break;
			}
		}

		double r = generateRandomDouble0_Max(sumSuccess);
		int currentSum = 0;
		bool selected = false;
		for (int i = 0; i < competitiveCRs.size(); i++){
			currentSum += competitiveCRsCount[i];
			if (r <= currentSum){
				selected = true;
				crValues[candidate] = competitiveCRs[i];
				crValuesCompetitiveIndex[candidate] = i;
				break;
			}
		}
		if (!selected){
			cout << "Error interno: no se selecciona valor de cr en la competicion" << endl;
		}

	} else {
		cerr << "Error crType no reconocido" << endl;
		exit(-1);
	}
	crValues[candidate] = min(crValues[candidate], 1.0);
	crValues[candidate] = max(crValues[candidate], 0.0);
	//usedCRValues.push_back(crValues[candidate]);
}

void MonoDE2_GlobalSearch::mutate(Individual *trial, int r1, int r2, int r3, int bestp, int candidate, int var) {
	if (trialStrategy == TRIAL_CURRENT_TO_PBEST) { 
		trial->setVar(var, (*population)[candidate]->getVar(var) + fValues[candidate] * ((*population)[bestp]->getVar(var) - (*population)[candidate]->getVar(var)) + fValues[candidate] * ((*population)[r1]->getVar(var) - (*population)[r2]->getVar(var)));
  } else if (trialStrategy == TRIAL_CURRENT_TO_PBEST_DIV) {
		trial->setVar(var, (*population)[candidate]->getVar(var) + fValues[candidate] * ((*population)[bestp]->getVar(var) - (*population)[candidate]->getVar(var)) + fValues[candidate] * ((*population)[r2]->getVar(var) - (*population)[r3]->getVar(var)));
	} else if ((trialStrategy == TRIAL_RAND_1) || (trialStrategy == TRIAL_RAND_1_DIV)) {
		trial->setVar(var, (*population)[r1]->getVar(var) + fValues[candidate] * ((*population)[r2]->getVar(var) - (*population)[r3]->getVar(var)));
	} else if (trialStrategy == TRIAL_CURRENT_TO_RAND) {
		trial->setVar(var, (*population)[candidate]->getVar(var) + fValues[candidate] * ((*population)[r1]->getVar(var) - (*population)[candidate]->getVar(var)) + fValues[candidate] * ((*population)[r2]->getVar(var) - (*population)[r3]->getVar(var)));
	} else if (trialStrategy == TRIAL_CURRENT_TO_SIM) {
		trial->setVar(var, (*population)[candidate]->getVar(var) + fValues[candidate] * ((*population)[r3]->getVar(var) - (*population)[candidate]->getVar(var)) + fValues[candidate] * ((*population)[r1]->getVar(var) - (*population)[r2]->getVar(var)));
	} else {
		cout << "Estrategia de generacion de trials no reconocida" << endl;
		exit(-1);
	}
	//check boundary
	if (reinitializationType == REINIT_RANDOM){
		if ((trial->getVar(var) < trial->getMinimum(var)) || (trial->getVar(var) > trial->getMaximum(var))){
			double r = ((double) rand () / (double) RAND_MAX);
			trial->setVar(var, r * (trial->getMaximum(var) - trial->getMinimum(var)) + trial->getMinimum(var));
		}
	} else if (reinitializationType == REINIT_BOUNCE_BACK){
		if (trial->getVar(var) < trial->getMinimum(var)){
			trial->setVar(var, (*population)[r1]->getVar(var) +  ((double) rand () / (double) RAND_MAX) * (trial->getMinimum(var) - (*population)[r1]->getVar(var)));
		} else if (trial->getVar(var) > trial->getMaximum(var)){
			trial->setVar(var, (*population)[r1]->getVar(var) +  ((double) rand () / (double) RAND_MAX) * (trial->getMaximum(var) - (*population)[r1]->getVar(var)));
		}
	}
	//Esto no deberia hacer falta con las estrategias que tenemos de reinicializacion, pero por si se implementa alguna otra
	trial->setVar(var, max(trial->getVar(var), trial->getMinimum(var)));
	trial->setVar(var, min(trial->getVar(var), trial->getMaximum(var)));
}

void MonoDE2_GlobalSearch::variation(int candidate){
  generateF(candidate);
  generateCR(candidate);

  //Prepare values for any of the trial vector generation strategies
  int r1, r2, r3;
  do{
    r1 = getRandomInteger0_N(ps - 1);
  } while(r1==candidate);

  do {
    r2 = getRandomInteger0_N(ps - 1);
  } while((r2 == r1) || (r2 == candidate));

  do {
    r3 = getRandomInteger0_N(ps - 1);
  } while((r3 == r2) || (r3 == r1) || (r3 == candidate));
  
  int accepted = p * ps;
  if ((accepted < 1) && ((trialStrategy == TRIAL_CURRENT_TO_PBEST) || (trialStrategy == TRIAL_CURRENT_TO_PBEST_DIV))) {
    cout << "Error, valor de p demasiado pequeÃ±o" << endl; 
    exit(-1);
  }
  int bestp = getRandomInteger0_N(accepted - 1);
 
  // r2 and r3 are selected in a different way 
  if ((trialStrategy == TRIAL_RAND_1_DIV) || (trialStrategy == TRIAL_CURRENT_TO_PBEST_DIV)) {
    // Calculate the number of elegible individuals for r1 and r2 depending on the
    // current moment of the search (given by the number of evaluations performed)
    int min_pop = max((int)(round(getPopulationSize() * 0.1)), 4);
    int max_pop = getPopulationSize();
    double m = (double)(min_pop - max_pop) / (double)getEvaluations();
    int y = (int)(round(m * getPerformedEvaluations() + max_pop));

    /*for (int i = 0; i < getPopulationSize(); i++) {
      cout << "(*auxPop)[" << i << "]->id: " << (*auxPop)[i]->id << endl;
      cout << "(*auxPop)[" << i << "]->ind->getObj(0): " << (*auxPop)[i]->ind->getObj(0) << endl;
      cout << "(*auxPop)[" << i << "]->ind->getDistanceToBest: " << (*auxPop)[i]->ind->getEuclideanDistanceDecisionSpace((*population)[0]) << endl;
    }*/

   	do {
  		r2 = (*auxPop)[getRandomInteger0_N(y - 1)]->id;
    } while ((r2 == r1) || (r2 == candidate));

  	do {
  		r3 = (*auxPop)[getRandomInteger0_N(y - 1)]->id;
    } while ((r3 == r2) || (r3 == r1) || (r3 == candidate));

    /*cout << "(*population)[" << r2 << "]->getObj: " << (*population)[r2]->getObj(0) << endl;
    cout << "(*population)[" << r2 << "]->getDistanceToBest: " << (*population)[r2]->getEuclideanDistanceDecisionSpace((*population)[0]) << endl;
    cout << "(*population)[" << r3 << "]->getObj: " << (*population)[r3]->getObj(0) << endl;
    cout << "(*population)[" << r3 << "]->getDistanceToBest: " << (*population)[r3]->getEuclideanDistanceDecisionSpace((*population)[0]) << endl;*/
  }
        
  if (trialStrategy == TRIAL_CURRENT_TO_SIM) {
    int min_pop = min(max((int)(round(getPopulationSize() * minPopPercentage)), 5) - 1, getPopulationSize() - 1);
    int max_pop = getPopulationSize() - 1;
    double m = (double)(max_pop - min_pop) / (double)getEvaluations();
    int upper = (int)(round(m * getPerformedEvaluations() + min_pop));
    int lower = upper - min_pop;

    //cout << "lower: " << lower << ", upper: " << upper << endl;

  	do {
  		r3 = (*auxPop)[generateRandom(lower, upper)]->id;
    } while ((r3 == r2) || (r3 == r1) || (r3 == candidate));

    //cout << "candidate: " << candidate << ", r1: " << r1 << ", r2: " << r2 << ", r3: " << r3 << endl;
  }

  //Create trial
  Individual *trial = (*population)[candidate]->internalClone();
  offspring[candidate] = trial;

  if ((crossover == CROSSOVER_BIN) || ((crossover == CROSSOVER_COMBINED) && ((getGeneration() % 2) == 0))){ 
    int jrand = getRandomInteger0_N(trial->getNumberOfVar() - 1);
    for(int i = 0; i < trial->getNumberOfVar(); i++){
      if ((i == jrand) || (generateRandomDouble0_Max(1) < crValues[candidate])){
        mutate(trial, r1, r2, r3, bestp, candidate, i);
      }
    }
  } else if ((crossover == CROSSOVER_EXP) || ((crossover == CROSSOVER_COMBINED) && ((getGeneration() % 2) == 1))){
    int initialVar = getRandomInteger0_N(trial->getNumberOfVar() - 1);
    int currentVar = initialVar;
    do {
      mutate(trial, r1, r2, r3, bestp, candidate, currentVar);
      currentVar = (currentVar + 1) % (trial->getNumberOfVar());
    } while ((currentVar != initialVar) && ((generateRandomDouble0_Max(1) < crValues[candidate])));
  } else {
    cout << "Error, operador de cruce no fijado" << endl;
    exit(-1);
  }
  evaluate(trial);
}

void MonoDE2_GlobalSearch::updateCRmSADE(){
	vector<double> crs;
	if (successfullCRsQueue.size()){
		for (list<double>::iterator it = successfullCRsQueue.begin(); it != successfullCRsQueue.end(); it++){
			crs.push_back(*it);
		}
		sort(crs.begin(), crs.end());
		nuCR = crs[crs.size() / 2];
	}
}

//Execute a generation: variate (includes the parent selection, because depending on the type of combination the parents are
//selected in different ways), evaluate offspring, execute survivor selection
void MonoDE2_GlobalSearch::runGeneration(){
	sort(population->begin(), population->end(), sortByObj);
   
  // Auxiliary vector with the population sorted in ascending order by the Euclidean distance in the decision
  // space with respect to the best individual 
  if ((trialStrategy == TRIAL_RAND_1_DIV) || (trialStrategy == TRIAL_CURRENT_TO_PBEST_DIV)) {
    auxPop = new vector<extIndType*>;

    for (int i = 0; i < getPopulationSize(); i++) {
      extIndType *auxInd = new extIndType;
      auxInd->id = i;
      auxInd->ind = (*population)[i];
      auxPop->push_back(auxInd);
    }

    sort(auxPop->begin(), auxPop->end(), bind(&MonoDE2_GlobalSearch::sortByDistanceToBestExtInd, this, std::placeholders::_1, std::placeholders::_2, (*population)[0]));
  }
  
  // Auxiliary vector with the population sorted in descending order by the Euclidean distance in the decision
  // space with respect to the best individual 
  if (trialStrategy == TRIAL_CURRENT_TO_SIM) {
    auxPop = new vector<extIndType*>;

    for (int i = 0; i < getPopulationSize(); i++) {
      extIndType *auxInd = new extIndType;
      auxInd->id = i;
      auxInd->ind = (*population)[i];
      auxPop->push_back(auxInd);
    }

    sort(auxPop->begin(), auxPop->end(), bind(&MonoDE2_GlobalSearch::sortByDistanceToBestExtIndReversed, this, std::placeholders::_1, std::placeholders::_2, (*population)[0]));
    
    /*for (int i = 0; i < auxPop->size(); i++) {
      cout << "(*auxPop)[" << i << "]->id: " << (*auxPop)[i]->id << endl;
      cout << "(*auxPop)[" << i << "]->ind->getObj(0): " << (*auxPop)[i]->ind->getObj(0) << endl;
      cout << "(*auxPop)[" << i << "]->ind->getDistanceToBest: " << (*auxPop)[i]->ind->getEuclideanDistanceDecisionSpace((*population)[0]) << endl;
    }*/
  }

  // Trial vector generation strategy
	for(int candidate = 0; candidate < ps; candidate++) {
		variation(candidate);
	}
 
  // Frees the memory of the auxiliary vector with the population 
  if ((trialStrategy == TRIAL_RAND_1_DIV) || (trialStrategy == TRIAL_CURRENT_TO_PBEST_DIV)) {
    for (int i = 0; i < getPopulationSize(); i++) {
      delete (*auxPop)[i];
    }
    delete auxPop;
  }
  
  if (trialStrategy == TRIAL_CURRENT_TO_SIM) {
    for (int i = 0; i < getPopulationSize(); i++) {
      delete (*auxPop)[i];
    }
    delete auxPop;
  }

  // Replacement operator
	replacement();

  // Global search
  if (globalSearchRequired) {
    globalSearch();
  }

  // Calculates the current error with respect the global optimum
  /*double error = abs((*population)[0]->getObj(0) - (*population)[0]->getGlobalOptimum());

  if ((nelderMeadRequired) && (error >= ERR_THR) && ((getPerformedEvaluations() / (double)getEvaluations()) >= EVAL_THR)) {
    NelderMeadSelect();
  }*/

  /*if (nelderMeadRequired) {
    NelderMeadSelect();
  }*/

  /*if (jumpingRequired) {
    double jr = ((double) rand () / (double) RAND_MAX);
    if ((jr < jumpingRate) && ((*population)[0]->getObj(0) >= prevGenBestFitness)) {
      jumping();
    }
  }*/

	if (updateNuFRequired){
		updateNuF();
	}

	if (updateNuCRRequired){
		updateNuCR();
	}

  //prevGenBestFitness = (*population)[0]->getObj(0);
}

void MonoDE2_GlobalSearch::updateNuF(){
	if (updateNuFStrategy == UPDATE_NUF_INCR){
		nuF += fIncr;
	} else if (updateNuFStrategy == UPDATE_NUF_JADE){
		if (successfullFs.size() > 0){//Update nuf
			//Calculate Lehmer mean
			double num = 0;
			double denom = 0;
			for (int i = 0; i < successfullFs.size(); i++){
				num += (successfullFs[i] * successfullFs[i]);
				denom += successfullFs[i];
			}
			double lMean = num / denom;
			nuF = (1 - c) * nuF + c * lMean;
			successfullFs.clear();
		}
	} else if (updateNuFStrategy == UPDATE_NUF_JADEAMEAN){
		if (successfullFs.size() > 0){
			double num = 0;
			for (int i = 0; i < successfullFs.size(); i++){
				num += (successfullFs[i]);
			}
			double aMean = num / successfullFs.size();
			nuF = (1 - c) * nuF + c * aMean;
			successfullFs.clear();
		}
	} else {
		cerr << "Error estrategia de actualizacion de NuF no reconocida" << endl;
		exit(-1);
	}
}

void MonoDE2_GlobalSearch::updateNuCR(){
	if (updateNuCRStrategy == UPDATE_NUCR_JADEAMEAN){
		if (successfullCRs.size() > 0){//Update nuCR
			double num = 0;
			double denom = 0;
			for (int i = 0; i < successfullCRs.size(); i++){
				num += (successfullCRs[i]);
			}
			double aMean = num / successfullCRs.size();
			nuCR = (1 - c) * nuCR + c * aMean;
			successfullCRs.clear();
		}
	} else if (updateNuCRStrategy == UPDATE_NUCR_RJADE){
		if (successfullCRs.size() > 0){//Update nuCR
			double sumImprovements = 0;
			for (int i = 0; i < successfullCRs.size(); i++){
				sumImprovements += lastImprovements[i];
			}
			double weightedMean = 0;
			for (int i = 0; i < successfullCRs.size(); i++){
				weightedMean += (successfullCRs[i]) * lastImprovements[i] / sumImprovements;
			}
			nuCR = (1 - c) * nuCR + c * weightedMean;
			successfullCRs.clear();
			lastImprovements.clear();
		}
	} else if (updateNuCRStrategy == UPDATE_NUCR_SADE){
		if (getGeneration() > LP){
			updateCRmSADE();
		}
	} else {
		cerr << "Error estrategia de actualizacion de NuCR no reconocida" << endl;
		exit(-1);
	}
}

void MonoDE2_GlobalSearch::replacement(){
	for (int i = 0; i < ps; i++){
		if ( (((offspring)[i]->getOptDirection(0) == MINIMIZE) && ((offspring)[i]->getObj(0) < (*population)[i]->getObj(0))) ||
	  	   (((offspring)[i]->getOptDirection(0) == MAXIMIZE) && ((offspring)[i]->getObj(0) > (*population)[i]->getObj(0)))){
			if ((updateNuFStrategy == UPDATE_NUF_JADE) || (updateNuFStrategy == UPDATE_NUF_JADEAMEAN)){
				successfullFs.push_back(fValues[i]);
			}
			if (updateNuCRStrategy == UPDATE_NUCR_JADEAMEAN){
				successfullCRs.push_back(crValues[i]);
			} else if (updateNuCRStrategy == UPDATE_NUCR_RJADE){
				successfullCRs.push_back(crValues[i]);
				lastImprovements.push_back(fabs((offspring)[i]->getObj(0)- (*population)[i]->getObj(0)));
			} else if (updateNuCRStrategy == UPDATE_NUCR_SADE){
				successfullCRsQueue.push_back(crValues[i]);
				if (successfullCRsQueue.size() > CRMemSize){
					successfullCRsQueue.pop_front();
				}
			}
			if (fType == F_JDE){
				currentFValues[i] = fValues[i];
			}
			if (crType == CR_JDE){
				currentCRValues[i] = crValues[i];
			}
			if (fType == F_COMPETITIVE){
				competitiveFsCount[fValuesCompetitiveIndex[i]]++;
			}
			if (crType == CR_COMPETITIVE){
				competitiveCRsCount[crValuesCompetitiveIndex[i]]++;
			}
			delete (*population)[i];
			(*population)[i] = offspring[i];
			replacements++;
		} else {
			delete offspring[i];
		}
	}
}

void MonoDE2_GlobalSearch::getSolution(MOFront *p){
	for (int i = 0; i < ps; i++){
		p->insert((*population)[i]);
	}
}

void MonoDE2_GlobalSearch::printInfo(ostream &os) const {
	os << "MonoObjective Differential Evolution" << endl;
	os << "Population size: " << ps << endl;
	switch(reinitializationType){
		case REINIT_RANDOM:
			os << "Reinialization: Random" << endl;
			break;
		case REINIT_BOUNCE_BACK:
			os << "Reinitialization: Bounce Back" << endl;
			break;
	}

	switch(crType){
		case CR_NORMAL_NUCR:
			os << "CR manager: Normal NuCR" << endl;
			switch(updateNuCRStrategy){
				case UPDATE_NUCR_NO:
					os << "nuCR update mechanism: none" << endl;
					break;
				case UPDATE_NUCR_JADEAMEAN:
					os << "nuCR update mechanism: JADE (Amean)" << endl;
					os << "c: " << c << endl;
					break;
				case UPDATE_NUCR_RJADE:
					os << "nuCR update mechanism: RJADE" << endl;
					os << "c: " << c << endl;
					break;
				case UPDATE_NUCR_SADE:
					os << "nuCR update mechanism: SADE" << endl;
					os << "LP: " << LP << endl;
					os << "CRMemSize: " << CRMemSize << endl;
					break;
				default:
					os << "Error nuCR update mechanism unrecognized!!!!!!" << endl;
					exit(-1);
			}
			os << "NuCR (final): " << nuCR << endl;
			break;
		case CR_NORMAL:
			os << "CR manager: Normal" << endl;
			os << "CRnormalNu: " << CRnormalNu << endl;
			os << "CRNormalSigma: " << CRnormalSigma << endl;
			break;
		case CR_CAUCHY:
			os << "CR manager: Cauchy" << endl;
			os << "CRCauchyNu: " << CRcauchyNu << endl;
			os << "CRCauchyLocFactor: " << CRcauchyLocFactor << endl;
			break;
		case CR_FIXED:
			os << "CR manager: Fixed" << endl;
			os << "CR: " << CRFixed << endl;
			break;
		case CR_JDE:
			os << "CR manager: JDE" << endl;
			os << "CRMin: " << jDE_CRmin << endl;
			os << "CRMax: " << jDE_CRmax << endl;
			os << "TauCR: " << jDE_tauCR << endl;
			break;
		case CR_UNI_0_015_085_1:
			os << "CR manager: Uni 0-0.15,0.85-1" << endl;
			break;
		case CR_UNIFORM:
			os << "CR manager: Uniform" << endl;
			os << "CRMin: " << CRMin_uni << endl;
			os << "CRMax: " << CRMax_uni << endl;
			break;
		case CR_COMPETITIVE:
			os << "CR manager: Competitive" << endl;
			os << "Pool: " << competitiveCRs[0];
			for (int i = 1; i < competitiveCRs.size(); i++){
				os << ", " << competitiveCRs[i];
			}
			os << endl;
			os << "CRN0: " << competitive_crN0 << endl;
			os << "CRDelta: " << competitive_crdelta << endl;
			break;
		default:
			os << "Error CR manager unrecognized!!!!!" << endl;
			exit(-1);
	}

	switch(fType){
		case F_CAUCHY_NUF:
			os << "F manager: Cauchy" << endl;
			switch(updateNuFStrategy){
				case UPDATE_NUF_INCR:
					os << "nuF update mechanism: incr" << endl;
					os << "FIncr: " << fIncr << endl;
					break;
				case UPDATE_NUF_NO:
					os << "nuF update mechanism: none" << endl;
					break;
				case UPDATE_NUF_JADE:
					os << "nuF update mechanism: JADE" << endl;
					os << "c: " << c << endl;
					break;
				case UPDATE_NUF_JADEAMEAN:
					os << "nuF update mechanism: JADEAMean" << endl;
					os << "c: " << c << endl;
					break;
				default:
					os << "Error nuF update mechanism unrecognized!!!!!!!!!!" << endl;
					exit(-1);
			}
			os << "NuF (final): " << nuF << endl;
			break;
		case F_NORMAL:
			os << "F manager: Normal(" << FnormalNu << ", " << FnormalSigma << ")" << endl;	
			break;
		case F_FIXED:
			os << "F manager: Fixed (" << FFixed << ")" << endl;
			break;
		case F_JDE:
			os << "F manager: jDE" << endl;
			os << "FMin: " << jDE_Fmin << endl;
			os << "FMax: " << jDE_Fmax << endl;
			os << "TauF: " << jDE_tauF << endl;
			break;
		case F_COMPETITIVE:
			os << "F manager: Competitive" << endl;
			os << "Pool: " << competitiveFs[0];
			for (int i = 1; i < competitiveFs.size(); i++){
				os << ", " << competitiveFs[i];
			}
			os << endl;
			os << "FN0: " << competitive_fN0 << endl;
			os << "FDelta: " << competitive_fdelta << endl;
			break;
		default:
			os << "Error F manager unrecognized!!!!!!!" << endl;
			exit(-1);
	}


	switch(trialStrategy){
		case TRIAL_CURRENT_TO_PBEST:
			os << "Trial vector generation strategy: Current to pbest" << endl;
			os << "p: " << p << endl;
			break;
		case TRIAL_CURRENT_TO_PBEST_DIV:
			os << "Trial vector generation strategy: Current to pbest (diversity)" << endl;
			os << "p: " << p << endl;
			break;
		case TRIAL_RAND_1:
			os << "Trial vector generation strategy: rand/1" << endl;
			break;
		case TRIAL_RAND_1_DIV:
			os << "Trial vector generation strategy: rand/1 (diversity)" << endl;
			break;
		case TRIAL_CURRENT_TO_RAND:
			os << "Trial vector generation strategy: Current to rand" << endl;
			break;
		case TRIAL_CURRENT_TO_SIM:
			os << "Trial vector generation strategy: Current to similarity" << endl;
			os << "MinPopPercentage: " << minPopPercentage << endl;
			break;
		default:
			os << "Error trial vector generation strategy unrecognized!!!!!!!!" << endl;
			exit(-1);
	}

	switch(crossover){
		case CROSSOVER_BIN:
			os << "Crossover: Bin" << endl;
			break;
		case CROSSOVER_EXP:
			os << "Crossover: Exp" << endl;
			break;
		default:
			os << "Error crossover unrecognized!!!!!!" << endl;
			exit(-1);
	}

	/*os << "Used CRs: ";
	for (int i = 0; i < usedCRValues.size(); i++){
		os << usedCRValues[i] << " ";
	}
	os << endl;
*/

	switch(reinitializationType){
		case REINIT_RANDOM:
			os << "Reinitialization scheme: Reinit Random" << endl;
			break;
		case REINIT_BOUNCE_BACK:
			os << "Reinitialization scheme: Reinit Bounce Back" << endl;
			break;
	}
	os << "Replacements: " << replacements << endl;
}

void MonoDE2_GlobalSearch::NelderMead() {

  int dimension = (*population)[0]->getNumberOfVar() + 1;
  vector<Individual *> nelderPop;
  
  if (population->size() >= dimension) {
    for (int i = 0; i < dimension; i++) {
      Individual* ind = (*population)[i]->internalClone();
      nelderPop.push_back(ind);
    }
  }
  else {
    for (int i = 0; i < population->size(); i++) {
      Individual* ind = (*population)[i]->internalClone();
      nelderPop.push_back(ind);
    }
    for (int i = 0; i < (dimension - population->size()); i++) {
  	  int k = getRandomInteger0_N(ps - 1);
      Individual *ind = (*population)[k]->internalClone();
      nelderPop.push_back(ind);
    }
  }

  for (int iter = 0; iter < NMSC; iter++)
  {
  	sort(nelderPop.begin(), nelderPop.end(), sortByObj);
	  Individual *xc = getSampleInd()->internalClone();
	  bool shrink = false;
	  for (int i = 0; i < xc->getNumberOfVar(); i++) {
	    double sum = 0;
	    for (int j = 0; j < dimension - 1; j++) {
	      sum += nelderPop[j]->getVar(i);
	    }
	    xc->setVar(i, sum / (dimension - 1)); //Calculamos el centroide de toda la población excepto del peor
	  }
	  
	  Individual *xr = getSampleInd()->internalClone(); // punto reflejado
	  for (int i = 0; i < xr->getNumberOfVar(); i++) {
	  	double valorCalculado=xc->getVar(i) + NM1 * (xc->getVar(i) - nelderPop[dimension - 1]->getVar(i));
	  	xr->setVar(i, valorCalculado);	
	  }
	  evaluate(xr);	
	  
	  if(xr->getObj(0) < nelderPop[0]->getObj(0)) // Si el reflejado mejora la mejor solución
	  {
	  	Individual *xe = getSampleInd()->internalClone(); // punto extendido
	  	for (int i = 0; i < xe->getNumberOfVar(); i++) {
	  		double valorCalculado = xc->getVar(i) + NM2 * (xr->getVar(i) - xc->getVar(i));
	  		xe->setVar(i, valorCalculado);	
	  	}
	  	evaluate(xe);

	    if(xe->getObj(0) < nelderPop[0]->getObj(0))
	    {
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xe; // agregamos el nuevo
		  }
		else
		{
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xr; // agregamos el nuevo	
		}
	  }
	  
	  else if((xr->getObj(0) >= nelderPop[0]->getObj(0)) && (xr->getObj(0) < nelderPop[dimension-1]->getObj(0)))
	  {
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xr; // agregamos el nuevo	
	  }
	  
	  else if(xr->getObj(0) >= nelderPop[dimension - 1]->getObj(0))
	  {
	   	 if(xr->getObj(0) < nelderPop[dimension - 1]->getObj(0))
	   	 {
		   	 	Individual *xcf = getSampleInd()->internalClone(); // punto de expansión
		     	for (int i = 0; i < xcf->getNumberOfVar(); i++) {
		    		double valorCalculado = xc->getVar(i) + NM3 * (xr->getVar(i) - xc->getVar(i));
		    		xcf->setVar(i, valorCalculado);	
		    	}
		  		evaluate(xcf);
			    if(xcf->getObj(0) < nelderPop[0]->getObj(0))
				  {
             delete nelderPop[dimension - 1];
		    		 nelderPop[dimension - 1] = xcf; // agregamos el nuevo
				  }
			    else
			    {
				 	  shrink = true;
			    }
		  }
		  else
		  {
			  	Individual *xcd = getSampleInd()->internalClone(); // punto de contracción
		   	   	for (int i = 0; i < xcd->getNumberOfVar(); i++) {
		  			double valorCalculado=xc->getVar(i) - NM3*(xc->getVar(i)-nelderPop[dimension - 1]->getVar(i));
		  			xcd->setVar(i, valorCalculado);	
		  		}
		  		evaluate(xcd);
			    if(xcd->getObj(0) < nelderPop[0]->getObj(0))
				{
             delete nelderPop[dimension - 1];
		    		 nelderPop[dimension - 1] = xcd; // agregamos el nuevo	
				}
				else
				{
					shrink = true;
				}
	      }
	  }
	  if(shrink==true)
	  {
			    for (int j = 1; j < dimension; j++) {
			      for (int i = 0; i < nelderPop[j]->getNumberOfVar(); i++) {	
			      double valorCalculado = nelderPop[0]->getVar(i) + NM4 * (nelderPop[j]->getVar(i)-nelderPop[0]->getVar(i));
			      nelderPop[j]->setVar(i, valorCalculado);	
			    }
			    evaluate(nelderPop[j]);
			} 	  
    }
  }

  // Selects the best individuals from the current population and the Nelder population
  for (int i = 0; i < nelderPop.size(); i++) {
    population->push_back(nelderPop[i]);
  }

  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::NelderMeadAdaptive() {
  int dimension = (*population)[0]->getNumberOfVar() + 1;

  vector<Individual *> nelderPop;
  double nm1 = 1.0;
  double nm2 = 1.0 + (2.0 / (dimension-1.0));
  double nm3 = 0.75 - (1.0 / (2.0 * (dimension - 1.0)));
  double nm4 = 1.0 + (1.0 / (dimension - 1.0));

  if (population->size() >= dimension) {
    for (int i = 0; i < dimension; i++) {
      Individual* ind = (*population)[i]->internalClone();
      nelderPop.push_back(ind);
    }
  }
  else {
    for (int i = 0; i < population->size(); i++) {
      Individual* ind = (*population)[i]->internalClone();
      nelderPop.push_back(ind);
    }
    for (int i = 0; i < (dimension - population->size()); i++) {
  	  int k = getRandomInteger0_N(ps - 1);
      Individual *ind = (*population)[k]->internalClone();
      nelderPop.push_back(ind);
    }
  }

  for (int iter = 0; iter < NMSC; iter++)
  {
  	sort(nelderPop.begin(), nelderPop.end(), sortByObj);
	  Individual *xc = getSampleInd()->internalClone();
	  bool shrink = false;
	  for (int i = 0; i < xc->getNumberOfVar(); i++) {
	    double sum = 0;
	    for (int j = 0; j < dimension - 1; j++) {
	      sum += nelderPop[j]->getVar(i);
	    }
	    xc->setVar(i, sum / (dimension - 1)); //Calculamos el centroide de toda la poblaci\F3n excepto del peor
	  }
	  
	  Individual *xr = getSampleInd()->internalClone(); // punto reflejado
	  for (int i = 0; i < xr->getNumberOfVar(); i++) {
	  	double valorCalculado=xc->getVar(i) + nm1 * (xc->getVar(i) - nelderPop[dimension - 1]->getVar(i));
	  	xr->setVar(i, valorCalculado);	
	  }
	  evaluate(xr);	
	  
	  if(xr->getObj(0) < nelderPop[0]->getObj(0)) // Si el reflejado mejora la mejor soluci\F3n
	  {
	  	Individual *xe = getSampleInd()->internalClone(); // punto extendido
	  	for (int i = 0; i < xe->getNumberOfVar(); i++) {
	  		double valorCalculado = xc->getVar(i) + nm2 * (xr->getVar(i) - xc->getVar(i));
	  		xe->setVar(i, valorCalculado);	
	  	}
	  	evaluate(xe);

	    if(xe->getObj(0) < nelderPop[0]->getObj(0))
	    {
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xe; // agregamos el nuevo
		  }
		else
		{
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xr; // agregamos el nuevo	
		}
	  }
	  
	  else if((xr->getObj(0) >= nelderPop[0]->getObj(0)) && (xr->getObj(0) < nelderPop[dimension-1]->getObj(0)))
	  {
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xr; // agregamos el nuevo	
	  }
	  
	  else if(xr->getObj(0) >= nelderPop[dimension - 1]->getObj(0))
	  {
	   	 if(xr->getObj(0) < nelderPop[dimension - 1]->getObj(0))
	   	 {
		   	 	Individual *xcf = getSampleInd()->internalClone(); // punto de expansi\F3n
		     	for (int i = 0; i < xcf->getNumberOfVar(); i++) {
		    		double valorCalculado = xc->getVar(i) + nm3 * (xr->getVar(i) - xc->getVar(i));
		    		xcf->setVar(i, valorCalculado);	
		    	}
		  		evaluate(xcf);
			    if(xcf->getObj(0) < nelderPop[0]->getObj(0))
				  {
             delete nelderPop[dimension - 1];
		    		 nelderPop[dimension - 1] = xcf; // agregamos el nuevo
				  }
			    else
			    {
				 	  shrink = true;
			    }
		  }
		  else
		  {
			  	Individual *xcd = getSampleInd()->internalClone(); // punto de contracci\F3n
		   	   	for (int i = 0; i < xcd->getNumberOfVar(); i++) {
		  			double valorCalculado=xc->getVar(i) - nm3*(xr->getVar(i)-xc->getVar(i));
		  			xcd->setVar(i, valorCalculado);	
		  		}
		  		evaluate(xcd);
			    if(xcd->getObj(0) < nelderPop[0]->getObj(0))
				{
             delete nelderPop[dimension - 1];
		    		 nelderPop[dimension - 1] = xcd; // agregamos el nuevo	
				}
				else
				{
					shrink = true;
				}
	      }
	  }
	  if(shrink==true)
	  {
			    for (int j = 1; j < dimension; j++) {
			      for (int i = 0; i < nelderPop[j]->getNumberOfVar(); i++) {	
			      double valorCalculado = nelderPop[0]->getVar(i) + nm4 * (nelderPop[j]->getVar(i)-nelderPop[0]->getVar(i));
			      nelderPop[j]->setVar(i, valorCalculado);	
			    }
			    evaluate(nelderPop[j]);
			} 	  
    }
  }

  // Selects the best individuals from the current population and the Nelder population
  for (int i = 0; i < nelderPop.size(); i++) {
    population->push_back(nelderPop[i]);
  }

  sort(population->begin(), population->end(), sortByObj);

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoDE2_GlobalSearch::NelderMeadNoShrink() {

  int dimension = (*population)[0]->getNumberOfVar() + 1;
  vector<Individual *> nelderPop;
  
  if (population->size() >= dimension) {
    for (int i = 0; i < dimension; i++) {
      Individual* ind = (*population)[i]->internalClone();
      nelderPop.push_back(ind);
    }
  }
  else {
    for (int i = 0; i < population->size(); i++) {
      Individual* ind = (*population)[i]->internalClone();
      nelderPop.push_back(ind);
    }
    for (int i = 0; i < (dimension - population->size()); i++) {
  	  int k = getRandomInteger0_N(ps - 1);
      Individual *ind = (*population)[k]->internalClone();
      nelderPop.push_back(ind);
    }
  }
  bool shrink = false;
  while(!shrink)
  {
  	sort(nelderPop.begin(), nelderPop.end(), sortByObj);
	  Individual *xc = getSampleInd()->internalClone();
	  
	  for (int i = 0; i < xc->getNumberOfVar(); i++) {
	    double sum = 0;
	    for (int j = 0; j < dimension - 1; j++) {
	      sum += nelderPop[j]->getVar(i);
	    }
	    xc->setVar(i, sum / (dimension - 1)); //Calculamos el centroide de toda la poblaci\F3n excepto del peor
	  }
	  
	  Individual *xr = getSampleInd()->internalClone(); // punto reflejado
	  for (int i = 0; i < xr->getNumberOfVar(); i++) {
	  	double valorCalculado=xc->getVar(i) + NM1 * (xc->getVar(i) - nelderPop[dimension - 1]->getVar(i));
	  	xr->setVar(i, valorCalculado);	
	  }
	  evaluate(xr);	
	  
	  if(xr->getObj(0) < nelderPop[0]->getObj(0)) // Si el reflejado mejora la mejor soluci\F3n
	  {
	  	Individual *xe = getSampleInd()->internalClone(); // punto extendido
	  	for (int i = 0; i < xe->getNumberOfVar(); i++) {
	  		double valorCalculado = xc->getVar(i) + NM2 * (xr->getVar(i) - xc->getVar(i));
	  		xe->setVar(i, valorCalculado);	
	  	}
	  	evaluate(xe);

	    if(xe->getObj(0) < nelderPop[0]->getObj(0))
	    {
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xe; // agregamos el nuevo
		  }
		else
		{
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xr; // agregamos el nuevo	
		}
	  }
	  
	  else if((xr->getObj(0) >= nelderPop[0]->getObj(0)) && (xr->getObj(0) < nelderPop[dimension-1]->getObj(0)))
	  {
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xr; // agregamos el nuevo	
	  }
	  
	  else if(xr->getObj(0) >= nelderPop[dimension - 1]->getObj(0))
	  {
	   	 if(xr->getObj(0) < nelderPop[dimension - 1]->getObj(0))
	   	 {
		   	 	Individual *xcf = getSampleInd()->internalClone(); // punto de expansi\F3n
		     	for (int i = 0; i < xcf->getNumberOfVar(); i++) {
		    		double valorCalculado = xc->getVar(i) + NM3 * (xr->getVar(i) - xc->getVar(i));
		    		xcf->setVar(i, valorCalculado);	
		    	}
		  		evaluate(xcf);
			    if(xcf->getObj(0) < nelderPop[0]->getObj(0))
				  {
             delete nelderPop[dimension - 1];
		    		 nelderPop[dimension - 1] = xcf; // agregamos el nuevo
				  }
			    else
			    {
				 	  shrink = true;
			    }
		  }
		  else
		  {
			  	Individual *xcd = getSampleInd()->internalClone(); // punto de contracci\F3n
		   	   	for (int i = 0; i < xcd->getNumberOfVar(); i++) {
		  			double valorCalculado=xc->getVar(i) - NM3*(xc->getVar(i)-nelderPop[dimension - 1]->getVar(i));
		  			xcd->setVar(i, valorCalculado);	
		  		}
		  		evaluate(xcd);
			    if(xcd->getObj(0) < nelderPop[0]->getObj(0))
				{
             delete nelderPop[dimension - 1];
		    		 nelderPop[dimension - 1] = xcd; // agregamos el nuevo	
				}
				else
				{
					shrink = true;
				}
	    }
	  }
  }
  // Replaces the current population with the Nelder population
  if (population->size() >= dimension) {
    for (int i = 0; i < dimension; i++) {
      delete (*population)[i];
      (*population)[i] = nelderPop[i];
    }
  }
  else {
    for (int i = 0; i < population->size(); i++) {
      delete (*population)[i];
      (*population)[i] = nelderPop[i];
    }
  }
}
