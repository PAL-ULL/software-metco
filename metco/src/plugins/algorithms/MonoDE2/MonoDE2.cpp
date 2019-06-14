#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include "MonoDE2.h"
#include "utils.h"
#include "Definitions.h" 

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

bool MonoDE2::init(const vector<string> &params){
	if ((params.size() % 3) != 0){
		cerr << "Error en los parametros de JADE. Deben ser multiplos de 3" << endl;
		return false;
	}
	
	replacements = 0;
	map<string, string> parameters;
	for (int i = 0; i < params.size(); i+=3){
		parameters[params[i]] = params[i+2];
		if (params[i+1] != "="){
			cerr << "Error en los parametros de JADE. Deben estar formados por grupos de la forma parametro = valor" << endl;
			return false;
		}
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
	} else if (trialStrategy_str == "Rand_1"){
		trialStrategy = TRIAL_RAND_1;
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
	if (parameters.size()){
		cout << "Error: algunos parametros no se utilizaron" << endl;
		for (map<string, string>::iterator it = parameters.begin(); it != parameters.end(); it++){
			cout << it->first << " ";
		}
		cout << endl;
		exit(-1);
	}
	return true;
}

void MonoDE2::fillPopWithRestart() {
	for (int i = population->size(); i < getPopulationSize(); i++) {
		Individual *ind = getSampleInd()->internalClone();
		ind->restart();
		evaluate(ind);
		population->push_back(ind);
	}
}

void MonoDE2::fillPopWithMersenneTwister() {
	mt19937 generator(seed);
	for (int i = population->size(); i < getPopulationSize(); i++) {
		Individual *ind = getSampleInd()->internalClone();
		for (int j = 0; j < ind->getNumberOfVar(); j++) {
			double aux = (double) generator() / (double) generator.max();
			ind->setVar(j, aux * (ind->getMaximum(j) - ind->getMinimum(j)) + ind->getMinimum(j)); 
		}
		evaluate(ind);
		population->push_back(ind);
	}
}

void MonoDE2::fillPopWithTentMap() {
	int mu = 1; // Bifurcation factor
	mt19937 generator(seed); // for z^(i)_0 values
	int firstPopSize = population->size(); // For cases where the population size is greater than 0

	if (firstPopSize < getPopulationSize()) {
		// Generates the first individual
		Individual *ind = getSampleInd()->internalClone();
		for (int i = 0; i < ind->getNumberOfVar(); i++) {
			double z_ini = (double) generator() / (double) generator.max();
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

void MonoDE2::fillPopWithSobol() {
	string fileName = "~/oplink/algorithms/team/src/skeleton/utils/sobol/joe-kuo-old.1111";
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

void MonoDE2::fillPopWithGoodLatticePoints() {
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

void MonoDE2::fillPopWithOppositionBasedLearning() {
	mt19937 generator(seed);

	// Generates a total number of individuals equal to (getPopulationSize() - population->size()) * 2
	vector<Individual *> genPopulation;
	for (int i = 0; i < (getPopulationSize() - population->size()); i++) {
		Individual *ind = getSampleInd()->internalClone();
		Individual *opp_ind = getSampleInd()->internalClone();
		for (int j = 0; j < ind->getNumberOfVar(); j++) {
			// Generates a random population
			double aux = (double) generator() / (double) generator.max();
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

	for (int i = 0; i < genPopulation.size() / 2; i++) {
		population->push_back(genPopulation[i]);
		delete genPopulation[genPopulation.size() - 1 - i];
	}

	genPopulation.clear();
}

void MonoDE2::fillPopWithQuasiOppositionBasedLearning() {
	mt19937 generator(seed);

	// Generates a total number of individuals equal to (getPopulationSize() - population->size()) * 2
	vector<Individual *> genPopulation;
	for (int i = 0; i < (getPopulationSize() - population->size()); i++) {
		Individual *ind = getSampleInd()->internalClone();
		Individual *qopp_ind = getSampleInd()->internalClone();
		for (int j = 0; j < ind->getNumberOfVar(); j++) {
			// Generates a random population
			double aux = (double) generator() / (double) generator.max();
			ind->setVar(j, aux * (ind->getMaximum(j) - ind->getMinimum(j)) + ind->getMinimum(j)); 

			// Generates the quasi-oppositional population
			double opp = qopp_ind->getMinimum(j) + qopp_ind->getMaximum(j) - ind->getVar(j);
			double m = (qopp_ind->getMinimum(j) + qopp_ind->getMaximum(j)) / 2.0;
			double rand = (double) generator() / (double) generator.max();
			if (ind->getVar(j) < m) {
				qopp_ind->setVar(j, m + (opp - m) * rand);
			}
			else {
				qopp_ind->setVar(j, opp + (m - opp) * rand);
			}
		}
		evaluate(ind);
		evaluate(qopp_ind);
		genPopulation.push_back(ind);
		genPopulation.push_back(qopp_ind);
	}

  sort(genPopulation.begin(), genPopulation.end(), sortByObj);

	for (int i = 0; i < genPopulation.size() / 2; i++) {
		population->push_back(genPopulation[i]);
		delete genPopulation[genPopulation.size() - 1 - i];
	}

	genPopulation.clear();
}

void MonoDE2::fillPopWithNewIndsAndEvaluate() {
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
}

void MonoDE2::generateF(int candidate){
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
			sumSuccess += competitiveFsCount[i];//Esto se podría llevar actualizado en vez de calcularlo, pero son pocos asi que da igual
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

void MonoDE2::generateCR(int candidate){
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
			sumSuccess += competitiveCRsCount[i];//Esto se podría llevar actualizado en vez de calcularlo, pero son pocos asi que da igual
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

void MonoDE2::mutate(Individual *trial, int r1, int r2, int r3, int bestp, int candidate, int var){
	if (trialStrategy == TRIAL_CURRENT_TO_PBEST){
		trial->setVar(var, (*population)[candidate]->getVar(var) + fValues[candidate] * ((*population)[bestp]->getVar(var) - (*population)[candidate]->getVar(var)) + fValues[candidate] * ((*population)[r1]->getVar(var) - (*population)[r2]->getVar(var)));
	} else if (trialStrategy == TRIAL_RAND_1){
		trial->setVar(var, (*population)[r1]->getVar(var) + fValues[candidate] * ((*population)[r2]->getVar(var) - (*population)[r3]->getVar(var)));
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

void MonoDE2::variation(int candidate){
	generateF(candidate);
	generateCR(candidate);

	//Prepare values for any of the trial vector generation strategies
	int r1, r2, r3;
	do{
		r1 = getRandomInteger0_N(ps - 1);
	} while(r1==candidate);

	do {
		r2 = getRandomInteger0_N(ps - 1);
	} while((r2== r1) || (r2==candidate));

	do {
		r3 = getRandomInteger0_N(ps - 1);
	} while((r3== r2) || (r3 == r1) || (r3==candidate));

	int accepted = p * ps;
	if ((accepted < 1) && (trialStrategy == TRIAL_CURRENT_TO_PBEST)){
		cout << "Error, valor de p demasiado pequeño" << endl;
		exit(-1);
	}
	int bestp = getRandomInteger0_N(accepted-1);

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

void MonoDE2::updateCRmSADE(){
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
void MonoDE2::runGeneration(){
	sort(population->begin(), population->end(), sortByObj);
	for(int candidate = 0; candidate < ps; candidate++){
		variation(candidate);
	}
	replacement();
	if (updateNuFRequired){
		updateNuF();
	}
	if (updateNuCRRequired){
		updateNuCR();
	}
}

void MonoDE2::updateNuF(){
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

void MonoDE2::updateNuCR(){
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

void MonoDE2::replacement(){
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


void MonoDE2::getSolution(MOFront *p){
	for (int i = 0; i < ps; i++){
		p->insert((*population)[i]);
	}
}

void MonoDE2::printInfo(ostream &os) const {
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
		case TRIAL_RAND_1:
			os << "Trial vector generation strategy: rand/1" << endl;
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
