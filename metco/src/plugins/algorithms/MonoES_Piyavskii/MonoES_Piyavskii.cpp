#include <iostream>
#include <stdlib.h>
#include <sys/time.h>

#include "MonoES_Piyavskii.h"
#include "utils.h"
#include "Definitions.h"
#include "MutationOneStepSize.h"
#include "MutationNStepSize.h"
#include "RecombinationGlobal.h"
#include "RecombinationLocal.h"

double getDist(const Individual *i1, const Individual *i2){
	double sum = 0;
	for (int i = 0; i < i1->getNumberOfVar(); i++){
		sum += (i1->getVar(i) - i2->getVar(i)) * (i1->getVar(i) - i2->getVar(i));
	}
	return sqrt(sum);
}

bool sortByObj0(const Individual *i1, const Individual *i2){
	return (i1->getInternalOptDirection(0) == MINIMIZE)?(i1->getObj(0) < i2->getObj(0)):(i1->getObj(0) > i2->getObj(0));
}


void readParameter(map<string,string> &parameters, const string &parameter, double &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MonoES_Piyavskii no definido" << endl;
		exit(-1);
	}
	value = atof(parameters[parameter].c_str());
}

void readParameter(map<string,string> &parameters, const string &parameter, int &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MonoES_Piyavskii no definido" << endl;
		exit(-1);
	}
	value = atoi(parameters[parameter].c_str());
}

void readParameter(map<string,string> &parameters, const string &parameter, string &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MonoES_Piyavskii no definido" << endl;
		exit(-1);
	}
	value = parameters[parameter];
}


MonoES_Piyavskii::MonoES_Piyavskii(){
	mutator = NULL;
}

MonoES_Piyavskii::~MonoES_Piyavskii(){
	delete mutator;
	delete offspring;
	delete recombinator;
	population->resize(mu);
}

void MonoES_Piyavskii::printDiversity(){
	double totalDistance = 0;
	double total = 0;
	for (int i = 0; i < mu; i++){
		double currentDistance = 0;
		for (int j = 0; j < mu; j++){
			double dist = 0;
			for (int k = 0; k < (*population)[i]->getNumberOfVar(); k++){
				dist += ((*population)[i]->getVar(k) - (*population)[j]->getVar(k)) * ((*population)[i]->getVar(k) - (*population)[j]->getVar(k));
			}
			dist = sqrt(dist);
			currentDistance += dist;
		}
		currentDistance /= mu;
		totalDistance += currentDistance;
		total += ((*population)[i])->getAuxData(0);
	}
	totalDistance /= mu;
	total/=mu;
	cout << "Diversity: " << totalDistance << endl;
	cout << "Parametro de estrategia: " << total << endl;
}

bool MonoES_Piyavskii::init(const vector<string> &params){
	if ((params.size() % 3) != 0){
		cerr << "Error en los parametros de MonoES_Piyavskii. Deben ser multiplos de 3" << endl;
		return false;
	}
	for (int i = 0; i < params.size(); i+=3){
		parameters[params[i]] = params[i+2];
		if (params[i+1] != "="){
			cerr << "Error en los parametros de MonoES_Piyavskii. Deben estar formados por grupos de la forma parametro = valor" << endl;
			return false;
		}
	}
	initialize();
	return true;
}
//Carga de parámetros, apertura del fichero de salida, reserva de memoria
//
//Parámetros que se cargan siempre: Mu, Lambda, Epsilon, Evaluations, PrintPeriod, SurvivorSelection, VariableRecombinationParents,
//                                  StrategyRecombinationParents, VariableRecombinationType, StrategyRecombinationType, ParameterInitializationType, 
//																	NumberOfStepSizes, Tau
//
//Parámetros que se cargan dependiendo de otros parámetros:
// - InitSigma: se carga si ParameterInitializationType es Fixed
// - MaxSigma: se carga si ParameterInitializationType es RandomWithMaximum
// - TauPrim: se carga de NumberOfStepSizes es igual al número de variables
void MonoES_Piyavskii::initialize(){
	//Load Parameters
	readParameter(parameters, "Mu", mu);
	readParameter(parameters, "Lambda", lambda);
	readParameter(parameters, "Epsilon", epsilon);
	string survivorSelectionStr;
	readParameter(parameters, "SurvivorSelection", survivorSelectionStr);
	convertSurvivorSelectionToCode(survivorSelectionStr);
	string parentsStr;
	readParameter(parameters, "RecombinationParents", parentsStr);
	convertParentsToCode(parentsStr);
	string recombinationTypeStr;
	readParameter(parameters, "VariableRecombinationType", recombinationTypeStr);
	convertTypeToCode(recombinationTypeStr, variablesRecombinationTypeCode);
	readParameter(parameters, "StrategyRecombinationType", recombinationTypeStr);
	convertTypeToCode(recombinationTypeStr, strategyRecombinationTypeCode);

	recombinator = getRecombinator(recombinationParentsCode);

	string parameterInitializationTypeStr;
	readParameter(parameters, "ParameterInitializationType", parameterInitializationTypeStr);
	convertParameterInitializationTypeToCode(parameterInitializationTypeStr);
	readParameter(parameters, "NumberOfStepSizes", numberOfStepSizes);
	readParameter(parameters, "Tau", tau);

	if (parameterInitializationTypeCode == FIXED){
		readParameter(parameters, "InitSigma", initSigma);
	} else if (parameterInitializationTypeCode == RANDOM_WITH_MAXIMUM){
		readParameter(parameters, "MaxSigma", maxSigma);
	}


	vector<double> mutParams;
	if (numberOfStepSizes == 1){
		mutator = new MutationOneStepSize();
		mutParams.push_back(tau);
		mutParams.push_back(epsilon);
		if (parameterInitializationTypeCode == FIXED){
			mutParams.push_back(initSigma);
		} else if (parameterInitializationTypeCode == RANDOM_WITH_MAXIMUM){
			mutParams.push_back(maxSigma);
		} else {
			mutParams.push_back(getSampleInd()->getMaximum(0) / 1.96);
		}
		mutator->initialize(mutParams);
	} else if (numberOfStepSizes == getSampleInd()->getNumberOfVar()){
		mutator = new MutationNStepSize();
		mutParams.push_back(tau);
		mutParams.push_back(epsilon);
		readParameter(parameters, "TauPrim", tauPrim);
		mutParams.push_back(tauPrim);
		for (int i = 0; i < numberOfStepSizes; i++){
			if (parameterInitializationTypeCode == FIXED){
				mutParams.push_back(initSigma);
			} else if (parameterInitializationTypeCode == RANDOM_WITH_MAXIMUM){
				mutParams.push_back(maxSigma);
			} else {
				mutParams.push_back(getSampleInd()->getMaximum(i) / 1.96);
			}
		}
		mutator->initialize(mutParams);
	} else {
		cerr << "Error: NumberOfStepSizes debe ser igual a 1 o al número de variables del problema" << endl;
		exit(-1);
	}

	getSampleInd()->setAuxDataSize(numberOfStepSizes);

	optDirection = getSampleInd()->getOptDirection(0);
	k_slope = 1000;
	
	//Check sanity of parameters
	checkSanity();

	//Reserve memory
	population->insert(population->begin(), mu + lambda, (Individual *) NULL);//Lambda because of temporary in the case of plus selection
	offspring = new vector<Individual*>;
	offspring->insert(offspring->begin(), lambda, (Individual *) NULL);
}

void MonoES_Piyavskii::fillPopWithNewIndsAndEvaluate(){
	//Create Initial population
	for (int i = 0; i < mu; i++){
		(*population)[i] = getSampleInd()->internalClone();
		//Initialize variables randomly
		(*population)[i]->restart();
		//Initialize adaptive parameters
		initializeAdaptiveParameters(i);
		evaluate((*population)[i]);
		evaluated.push_back((*population)[i]->internalClone());
	}
	bestObjective = (*population)[0]->getObj(0);
	for (int i = 1; i < mu; i++){
		if ((optDirection == MINIMIZE) && ((*population)[i]->getObj(0) < bestObjective)){
			bestObjective = (*population)[i]->getObj(0);
		} else if ((optDirection == MAXIMIZE) && ((*population)[i]->getObj(0) > bestObjective)){
			bestObjective = (*population)[i]->getObj(0);
		}
	}
}


	
Recombination* MonoES_Piyavskii::getRecombinator(int parents){
	if (parents == GLOBAL_PARENTS){
		return (new RecombinationGlobal());
	} else if ((parents == LOCAL_PARENTS)){
		return (new RecombinationLocal());
	} else {
		cerr << "Error interno: codigos de recombinación no reconocidos" << endl;
		exit(-1);
	}
}

//Conversión del string al código del selector
void MonoES_Piyavskii::convertSurvivorSelectionToCode(const string &survivorSelectionStr){
	if (survivorSelectionStr == "Comma"){
		survivorSelectionCode = COMMA_SELECTION;
	} else if (survivorSelectionStr == "Plus"){
		survivorSelectionCode = PLUS_SELECTION;
	} else {
		cerr << "Error. Esquema de selección de supervivientes no reconocido: " << survivorSelectionStr << endl;
		exit(-1);
	}
}

//Conversión del string al código del tipo de selección de padres para la recombinación
void MonoES_Piyavskii::convertParentsToCode(const string &parentsStr){
	if (parentsStr == "Global"){
		recombinationParentsCode = GLOBAL_PARENTS;
	} else if (parentsStr == "Local"){
		recombinationParentsCode = LOCAL_PARENTS;
	} else {
		cerr << "Error. Tipo de padres no reconocido: " << parentsStr << endl;
		exit(-1);
	}
}

void MonoES_Piyavskii::convertTypeToCode(const string &recombinationTypeStr, int &code){
	if (recombinationTypeStr == "Discrete"){
		code = DISCRETE_RECOMBINATION;
	} else if (recombinationTypeStr == "Intermediary"){
		code = INTERMEDIARY_RECOMBINATION;
	} else {
		cerr << "Error. Tipo de recombinación no reconocida: " << recombinationTypeStr << endl;
		exit(-1);
	}
}

//Conversión del string al código de inicialización de los parámetros de la estrategia
void MonoES_Piyavskii::convertParameterInitializationTypeToCode(const string &parameterInitializationTypeStr){
	if (parameterInitializationTypeStr == "Fixed"){
		parameterInitializationTypeCode = FIXED;
	} else if (parameterInitializationTypeStr == "RandomWithMaximum"){
		parameterInitializationTypeCode = RANDOM_WITH_MAXIMUM;
	} else if (parameterInitializationTypeStr == "Random196"){
		parameterInitializationTypeCode = RANDOM_196;
	}
}

//Chequear si los parámetros son correctos: números positivos, rangos con valor mínimo que el valor máximo, etc.
void MonoES_Piyavskii::checkSanity() const{
	if (mu <= 0){
		cerr << "Error: Mu debe ser mayor que 0" << endl;
		exit(-1);
	}
	if (lambda <= 0){
		cerr << "El parámetro Lambda debe ser mayor que 0" << endl;
		exit(-1);
	}
	if ((survivorSelectionCode == COMMA_SELECTION) && (lambda < mu)){
		cerr << "lambda debe ser mayor o igual que mu al usar selección coma" << endl;
		exit(-1);
	}
	if (numberOfStepSizes == 1){
		//Chequeamos que se cumplen los requisitos para el uso de Random_196 con 1 step size
		if (parameterInitializationTypeCode == RANDOM_196){
			double maxValue = getSampleInd()->getMaximum(0);
			double minValue = getSampleInd()->getMinimum(0);
			if (maxValue != -minValue){
				cerr << "Error: la inicializacion Random_196 asume que maxValue y minValue son iguales en valor absoluto para todas las variables" << endl;
				exit(-1);
			}
			if (maxValue / 1.96 < epsilon){
				cerr << "Error: el valor máximo de la variable dividida entre 1.96 es menor que epsilon" << endl;
				exit(-1);
			}
			for (int i = 1; i < getSampleInd()->getNumberOfVar(); i++){
				if ((getSampleInd()->getMaximum(i) != maxValue) || (getSampleInd()->getMinimum(i) != minValue)){
					cerr << "Error: la inicializacion Random_196 asume que maxValue y minValue son iguales en valor absoluto para todas las variables" << endl;
					exit(-1);
				}
			}
		}
	} else if (numberOfStepSizes == getSampleInd()->getNumberOfVar()){
		//Chequeamos que se cumplen los requisitos para el uso de Random_196 con n step size
		for (int i = 0; i < getSampleInd()->getNumberOfVar(); i++){
			if (getSampleInd()->getMaximum(i) != -getSampleInd()->getMinimum(i)){
				cerr << "Error: la inicializacion Random_196 asume que maxValue y minValue son iguales en valor absoluto en cada variable" << endl;
				exit(-1);
			}
			if (getSampleInd()->getMaximum(i) / 1.96 < epsilon){
				cerr << "Error: el valor máximo de la variable dividida entre 1.96 es menor que epsilon" << endl;
				exit(-1);
			}
		}
	} else {
		cerr << "Error: el parametro numberOfStepSizes debe ser 1 o igual al número de variables del problema" << endl;
		exit(-1);
	}
	if (parameterInitializationTypeCode == FIXED){
		if (initSigma < epsilon){
			cerr << "Error: InitialSigma no puede ser menor que epsilon" << endl;
			exit(-1);
		}
	} else if (parameterInitializationTypeCode == RANDOM_WITH_MAXIMUM){
		if (maxSigma < epsilon){
			cerr << "Error: InitialSigma no puede ser menor que epsilon" << endl;
			exit(-1);
		}
	}
}

//Inicialización de los parámetros de estrategia (sigmas)
void MonoES_Piyavskii::initializeAdaptiveParameters(const int i){
	switch (parameterInitializationTypeCode){
		case FIXED:
			for (int j = 0; j < numberOfStepSizes; j++)
				(*population)[i]->setAuxData(j, initSigma);
			break;
		case RANDOM_WITH_MAXIMUM:
			for (int j = 0; j < numberOfStepSizes; j++)
				(*population)[i]->setAuxData(j, generateRandomDoubleMinMax(epsilon, maxSigma));
			break;
		case RANDOM_196:
			for (int j = 0; j < numberOfStepSizes; j++)
				(*population)[i]->setAuxData(j, (generateRandomDoubleMinMax(epsilon, (*population)[i]->getMaximum(j) / 1.96)));
			break;
	}
}

//Execute a generation: variate (includes the parent selection, because depending on the type of combination the parents are
//selected in different ways), evaluate offspring, execute survivor selection
void MonoES_Piyavskii::runGeneration(){
	variation();
	//evaluateOffspring();
	survivorSelection();
	//printDiversity();
	k_slope /= 1.1;
}

void MonoES_Piyavskii::eval(Individual *ind){
	/*double maxEstimated = -DBL_MAX;
	for (int i = 0; i < evaluated.size(); i++){
		double distance = 0;
		for (int j = 0; j < ind->getNumberOfVar(); j++){
			distance += ((ind->getVar(j) - evaluated[i]->getVar(j)) * (ind->getVar(j) - evaluated[i]->getVar(j)));
		}
		distance = sqrt(distance);
		maxEstimated = max(maxEstimated, (double)(evaluated[i]->getObj(0)) - distance * k_slope);
	}*/
	evaluate(ind);
	evaluated.push_back(ind->internalClone());
	if ((optDirection == MINIMIZE) && (ind->getObj(0) < bestObjective)){
		bestObjective = ind->getObj(0);
	} else if ((optDirection == MAXIMIZE) && (ind->getObj(0) > bestObjective)){
		bestObjective = ind->getObj(0);
	}
}
//Evaluate the new lambda individuals
void MonoES_Piyavskii::evaluateOffspring(){
	for (int i = 0; i < lambda; i++){
		/*double maxBound;
		for (int j = 0; j < evaluated.size(); j++){
			double distance = getDist(evaluated[j], (*offspring)[i]);
			//Suponemos minimizar!!!
			double bound = evaluated[j]->getObj(0) - distance * k;
			if (j == 0){
				maxBound = bound;
			} else {
				maxBound = max(maxBound, bound);
			}
		}*/
		eval((*offspring)[i]);
	}
}

//Select indiviuals that survive and copy them to population
// Erase the rest of individuals
void MonoES_Piyavskii::survivorSelection(){
	switch(survivorSelectionCode){
		case COMMA_SELECTION:
			survivorSelectionComma();
			break;
		case PLUS_SELECTION:
			survivorSelectionPlus();
			break;
	}
}

//Comma selection: the selection is done considering only the offspring
void MonoES_Piyavskii::survivorSelectionComma(){
	//Sort offspring
	sort(offspring->begin(), offspring->end(), sortByObj0);
	//Delete parents and not selected individuals
	for (int i = 0; i < mu; i++){
		delete (*population)[i];
	}
	for (int i = mu; i < lambda; i++){
		delete (*offspring)[i];
	}
	//Copy to next population
	for (int i = 0; i < mu; i++){
		(*population)[i] = (*offspring)[i];
	}
}

//Plus selection: the selection is done considering the previous population
//and the offspring
void MonoES_Piyavskii::survivorSelectionPlus(){
	//Join both populations
	for (int i = 0; i < lambda; i++){
		(*population)[mu + i] = (*offspring)[i];
	}
	//Sort
	sort(population->begin(), population->end(), sortByObj0); 
	//Remove not selected elements
	for (int i = mu; i < mu + lambda; i++){
		delete (*population)[i];
	}
}

//Recombine and mutate
void MonoES_Piyavskii::variation(){
	recombinateParents();
	mutateOffspring();
}

//
void MonoES_Piyavskii::recombinateParents(){
	for (int i = 0; i < lambda; i++){
		(*offspring)[i] = (*population)[0]->internalClone();//Their values are ignored
		//TODO:Dado que siempre se recombina, se podrian evitar los delete y los clones, dejando en offspring
		//los individuos que se iban a borrar
		recombinator->recombine(population, mu, variablesRecombinationTypeCode, strategyRecombinationTypeCode, (*offspring)[i]);
	}
}

bool sortByFirst(const pair<double, double> &a, const pair<double, double> &b){
	return a.first < b.first;
}

#define EPSILON 0.0000000001
//Mutate each new generated individual
void MonoES_Piyavskii::mutateOffspring(){
	for (int i = 0; i < lambda; i++){
		int trials = 0;
		double success = false;
		while(!success){
			trials++;
			Individual *original = (*offspring)[i]->internalClone();
			mutator->mutate((*offspring)[i]);
			//Calculate v
			double v[original->getNumberOfVar()];
			double dist = 0;
			double D = 0;
			for (int j = 0; j < original->getNumberOfVar(); j++){
				v[j] = (*offspring)[i]->getVar(j) - original->getVar(j);
				dist += v[j] * v[j];
				D += v[j] * v[j];
			}
			dist = sqrt(dist);
			double originalt = 1;
			//Normalizamos v
			//for (int i = 0; i < original->getNumberOfVar(); i++){
			//	v[i] /= dist;
			//}
			//D = 1;
			//Nos interesa el segumento S = original + t * v
			//Calculamos lim, el valor de t que provoca que nos salgamos
			//de la zona factible
			double lim = DBL_MAX;
			for (int j = 0; j < original->getNumberOfVar(); j++){
				if (v[j] > 0){
					double thislimit = (original->getMaximum(j) - original->getVar(j)) / v[j];
					lim = min(lim, thislimit);
				} else if (v[j] < 0){
					double thislimit = (original->getMinimum(j) - original->getVar(j)) / v[j];
					lim = min(lim, thislimit);
				}
			}
			vector< pair <double, double> > validValues;
			validValues.push_back(pair<double, double>(0, lim));
			//Calculamos distancia minima
			double ARecta = (v[1] / v[0]);
			double BRecta = (original->getVar(0) - original->getVar(1) * v[1] / v[0]);
			for (int j = 0; j < evaluated.size(); j++){
				double distanciaARecta = fabs(ARecta * evaluated[j]->getVar(0) - evaluated[j]->getVar(1) + BRecta) / sqrt(ARecta*ARecta + 1);
				if (validValues.size() == 0){
					break;
				}
				double A = (evaluated[j]->getObj(0) - bestObjective) * (evaluated[j]->getObj(0) - bestObjective);
				//Calculate E, F
				double E = 0;
				double F = 0;
				for (int k = 0; k < original->getNumberOfVar(); k++){
					E +=  2 * (original->getVar(k) - evaluated[j]->getVar(k)) * v[k];
					F += (original->getVar(k) - evaluated[j]->getVar(k)) * (original->getVar(k) - evaluated[j]->getVar(k));
				}
				double finalA = k_slope * k_slope * D;
				double finalB = k_slope * k_slope * E;
				double finalC = k_slope * k_slope * F - A;
				double raiz = finalB * finalB - 4 * finalA * finalC;
				if (raiz > 0){//remove range
					double s1 = (-finalB - sqrt(raiz)) / (2 * finalA);
					double s2 = (-finalB + sqrt(raiz)) / (2 * finalA);
					for (int k = 0; k < validValues.size(); k++){
						if ((s1 <= validValues[k].first) && (s2 >= validValues[k].first)){
							validValues[k].first = s2;
						} else if ((s1 <= validValues[k].second) && (s2 >= validValues[k].second)){
							validValues[k].second = s1;
						} else if ((s1 <= validValues[k].first) && (s2 >= validValues[k].second)){
							validValues[k].first = validValues[k].second;
						} else if ((s1 > validValues[k].first) && (s2 < validValues[k].second)){
							validValues.push_back(pair<double, double>(s2, validValues[k].second));
							validValues[k].second = s1;
						}
						if (validValues[k].first + EPSILON >= validValues[k].second){
							validValues[k] = validValues[validValues.size()-1];
							validValues.resize((int)((validValues.size()))-1);
							k--;
						}
					}
				}
			}
			if (validValues.size() >= 1){//Sucess
				success = true;
				//ordenamos los rangos
				sort(validValues.begin(), validValues.end(), sortByFirst);
				//Obtain new size
				double newSize = 0;
				bool originalIsValid = false;
				for (int j = 0; j < validValues.size(); j++){
					newSize += (validValues[j].second - validValues[j].first);
					if ((originalt + EPSILON >= validValues[j].first) && (originalt <= validValues[j].second + EPSILON)){
						originalIsValid = true;
						break;
					}
				}
				//Calculate new t
				double t = -1;
				if (!originalIsValid){
					double newDist = originalt * newSize / lim;
					for (int j = 0; j < validValues.size(); j++){
						double s = validValues[j].second - validValues[j].first;
						if (s + EPSILON >= newDist){
							t = validValues[j].first + newDist;	
						} else {
							newDist -= s;
						}
					}
					if (t == -1){
						cerr << "Error interno. t es -1" << endl;
						exit(-1);
					}
					success = false;//!!!!!!!
				} else {
					t = originalt;
				}
				double realDistance = t * dist;
				for (int j = 0; j < original->getNumberOfVar(); j++){
					(*offspring)[i]->setVar(j, original->getVar(j) + v[j] * t);
					(*offspring)[i]->setVar(j, max((*offspring)[i]->getVar(j), (*offspring)[i]->getMinimum(j)));
					(*offspring)[i]->setVar(j, min((*offspring)[i]->getVar(j), (*offspring)[i]->getMaximum(j)));
				}
				eval((*offspring)[i]);
				delete original;
			} else {
				delete (*offspring)[i];
				(*offspring)[i] = original;
				if (trials == 10){
					trials = 0;
					k_slope *= 3;
				}
			}
		}
	}
}

void MonoES_Piyavskii::getSolution(MOFront *p){
	//En el secuencial basta con devolver el 0,
	//en el paralelo, en algunas migraciones se podria desordenar, con lo que hay
	//que pasar por todos
	for (int i = 0; i < mu; i++){
		p->insert((*population)[i]);
	}
}

void MonoES_Piyavskii::printInfo(ostream &os) const {
	os << "MonoObjective Evolution Strategy"  << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Mu = " << mu << endl;
	os << "Lambda = " << lambda << endl;
	os << "Epsilon = " << epsilon << endl;
	os << "Tau = " << tau << endl;
	if (numberOfStepSizes == getSampleInd()->getNumberOfVar()){
		os << "Tau' = " << tauPrim << endl;
	}
	os << "NumberOfStepSizes = " << numberOfStepSizes << endl;
	if (parameterInitializationTypeCode == FIXED){
		os << "initSigma = " << initSigma << endl;
	} else if (parameterInitializationTypeCode == RANDOM_WITH_MAXIMUM){
		os << "maxSigma = " << maxSigma << endl;
	}
	os << "Survivor selection = " << parameters.at("SurvivorSelection") << endl;
	os << "RecombinationParents = " << parameters.at("RecombinationParents") << endl;
	os << "VariableRecombinationType = " << parameters.at("VariableRecombinationType") << endl;
	os << "StrategyRecombinationType = " << parameters.at("StrategyRecombinationType") << endl;
	os << "ParameterInitializationType = " << parameters.at("ParameterInitializationType") << endl;
}

