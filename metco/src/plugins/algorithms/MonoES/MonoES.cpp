#include <iostream>
#include <stdlib.h>
#include <sys/time.h>

#include "MonoES.h"
#include "utils.h"
#include "Definitions.h"
#include "MutationOneStepSize.h"
#include "MutationNStepSize.h"
#include "RecombinationGlobal.h"
#include "RecombinationLocal.h"

bool sortByObj0(const Individual *i1, const Individual *i2){
	return (i1->getInternalOptDirection(0) == MINIMIZE)?(i1->getObj(0) < i2->getObj(0)):(i1->getObj(0) > i2->getObj(0));
}

void readParameter(map<string,string> &parameters, const string &parameter, double &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MonoES no definido" << endl;
		exit(-1);
	}
	value = atof(parameters[parameter].c_str());
}

void readParameter(map<string,string> &parameters, const string &parameter, int &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MonoES no definido" << endl;
		exit(-1);
	}
	value = atoi(parameters[parameter].c_str());
}

void readParameter(map<string,string> &parameters, const string &parameter, string &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MonoES no definido" << endl;
		exit(-1);
	}
	value = parameters[parameter];
}


MonoES::MonoES(){
	mutator = NULL;
}

MonoES::~MonoES(){
	delete mutator;
	delete offspring;
	delete recombinator;
	population->resize(mu);
}

void MonoES::printDiversity(){
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
		total += ((*population)[i])->getAuxData(0);
		currentDistance /= mu;
		totalDistance += currentDistance;
	}
	totalDistance /= mu;
	total /= mu;
	//cout << "Diversity: " << totalDistance << endl;
	cout << "Parametro de estrategia: " << total << endl;
}


bool MonoES::init(const vector<string> &params){
	if ((params.size() % 3) != 0){
		cerr << "Error en los parametros de MonoES. Deben ser multiplos de 3" << endl;
		return false;
	}
	for (int i = 0; i < params.size(); i+=3){
		parameters[params[i]] = params[i+2];
		if (params[i+1] != "="){
			cerr << "Error en los parametros de MonoES. Deben estar formados por grupos de la forma parametro = valor" << endl;
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
void MonoES::initialize(){
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
	
	//Check sanity of parameters
	checkSanity();

	//Reserve memory
	population->insert(population->begin(), mu + lambda, (Individual *) NULL);//Lambda because of temporary in the case of plus selection
	offspring = new vector<Individual*>;
	offspring->insert(offspring->begin(), lambda, (Individual *) NULL);
}

void MonoES::fillPopWithNewIndsAndEvaluate(){
	//Create Initial population
	for (int i = 0; i < mu; i++){
		(*population)[i] = getSampleInd()->internalClone();
		//Initialize variables randomly
		(*population)[i]->restart();
		//Initialize adaptive parameters
		initializeAdaptiveParameters(i);
		evaluate((*population)[i]);
	}
}


	
Recombination* MonoES::getRecombinator(int parents){
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
void MonoES::convertSurvivorSelectionToCode(const string &survivorSelectionStr){
	if (survivorSelectionStr == "Comma"){
		survivorSelectionCode = COMMA_SELECTION;
	} else if (survivorSelectionStr == "Plus"){
		survivorSelectionCode = PLUS_SELECTION;
	} else if (survivorSelectionStr == "DE"){
		survivorSelectionCode = DE_SELECTION;
	} else {
		cerr << "Error. Esquema de selección de supervivientes no reconocido: " << survivorSelectionStr << endl;
		exit(-1);
	}
}

//Conversión del string al código del tipo de selección de padres para la recombinación
void MonoES::convertParentsToCode(const string &parentsStr){
	if (parentsStr == "Global"){
		recombinationParentsCode = GLOBAL_PARENTS;
	} else if (parentsStr == "Local"){
		recombinationParentsCode = LOCAL_PARENTS;
	} else {
		cerr << "Error. Tipo de padres no reconocido: " << parentsStr << endl;
		exit(-1);
	}
}

void MonoES::convertTypeToCode(const string &recombinationTypeStr, int &code){
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
void MonoES::convertParameterInitializationTypeToCode(const string &parameterInitializationTypeStr){
	if (parameterInitializationTypeStr == "Fixed"){
		parameterInitializationTypeCode = FIXED;
	} else if (parameterInitializationTypeStr == "RandomWithMaximum"){
		parameterInitializationTypeCode = RANDOM_WITH_MAXIMUM;
	} else if (parameterInitializationTypeStr == "Random196"){
		parameterInitializationTypeCode = RANDOM_196;
	}
}

//Chequear si los parámetros son correctos: números positivos, rangos con valor mínimo que el valor máximo, etc.
void MonoES::checkSanity() const{
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
void MonoES::initializeAdaptiveParameters(const int i){
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
void MonoES::runGeneration(){
	long double maxi = (*population)[0]->getObj(0), mini = (*population)[0]->getObj(0);
	for (int i = 0; i < mu; i++){
		maxi = max(maxi, (*population)[i]->getObj(0));
		mini = min(mini, (*population)[i]->getObj(0));
	}
	cout << "Valores: " << maxi << ", " << mini << endl;
	variation();
	evaluateOffspring();
	survivorSelection();
	printDiversity();
}

//Evaluate the new lambda individuals
void MonoES::evaluateOffspring(){
	for (int i = 0; i < lambda; i++){
		evaluate((*offspring)[i]);
		//cout << (*offspring)[i]->getVar(0) << " " << (*offspring)[i]->getVar(1) << endl;
	}
}

//Select indiviuals that survive and copy them to population
// Erase the rest of individuals
void MonoES::survivorSelection(){
	switch(survivorSelectionCode){
		case COMMA_SELECTION:
			survivorSelectionComma();
			break;
		case PLUS_SELECTION:
			survivorSelectionPlus();
			break;
		case DE_SELECTION:
			DESelection();
			break;
	}
}

void MonoES::DESelection(){//Assumes lambda == mu
	int optDirection = (*offspring)[0]->getOptDirection(0);
	for (int i = 0; i < mu; i++){
		if ( ((optDirection == MAXIMIZE) &&  (((*offspring)[i]->getObj(0)) > ((*population)[i]->getObj(0)))) || 
		     ((optDirection == MINIMIZE) &&  (((*offspring)[i]->getObj(0)) < ((*population)[i]->getObj(0)))) ){
			delete (*population)[i];
			(*population)[i] = (*offspring)[i];
		} else {
			delete (*offspring)[i];
		}
	}
}
//Comma selection: the selection is done considering only the offspring
void MonoES::survivorSelectionComma(){
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
void MonoES::survivorSelectionPlus(){
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
void MonoES::variation(){
	recombinateParents();
	mutateOffspring();
}

//
void MonoES::recombinateParents(){
	for (int i = 0; i < lambda; i++){
		(*offspring)[i] = (*population)[0]->internalClone();//Their values are ignored
		//TODO:Dado que siempre se recombina, se podrian evitar los delete y los clones, dejando en offspring
		//los individuos que se iban a borrar
		recombinator->recombine(population, mu, variablesRecombinationTypeCode, strategyRecombinationTypeCode, (*offspring)[i]);
	}
}

//Mutate each new generated individual
void MonoES::mutateOffspring(){
	for (int i = 0; i < lambda; i++){
		mutator->mutate((*offspring)[i]);
	}
}

void MonoES::getSolution(MOFront *p){
	//En el secuencial basta con devolver el 0,
	//en el paralelo, en algunas migraciones se podria desordenar, con lo que hay
	//que pasar por todos
	for (int i = 0; i < mu; i++){
		p->insert((*population)[i]);
	}
}

void MonoES::printInfo(ostream &os) const {
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

