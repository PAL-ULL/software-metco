#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

#include "MonoDE_Piyavskii.h"
#include "utils.h"
#include "Definitions.h"

bool sortByFirst(const pair<double, double> &a, const pair<double, double> &b){
	return a.first < b.first;
}

void readParameter(map<string,string> &parameters, const string &parameter, double &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MonoDE_Piyavskii no definido" << endl;
		exit(-1);
	}
	value = atof(parameters[parameter].c_str());
}

void readParameter(map<string,string> &parameters, const string &parameter, int &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MonoDE_Piyavskii no definido" << endl;
		exit(-1);
	}
	value = atoi(parameters[parameter].c_str());
}

void readParameter(map<string,string> &parameters, const string &parameter, string &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MonoDE_Piyavskii no definido" << endl;
		exit(-1);
	}
	value = parameters[parameter];
}

void MonoDE_Piyavskii::printDiversity(){
	double totalDistance = 0;
	for (int i = 0; i < ps; i++){
		double currentDistance = 0;
		for (int j = 0; j < ps; j++){
			double dist = 0;
			for (int k = 0; k < (*population)[i]->getNumberOfVar(); k++){
				dist += ((*population)[i]->getVar(k) - (*population)[j]->getVar(k)) * ((*population)[i]->getVar(k) - (*population)[j]->getVar(k));
			}
			dist = sqrt(dist);
			currentDistance += dist;
		}
		currentDistance /= ps;
		totalDistance += currentDistance;
	}
	totalDistance /= ps;
	cout << "Diversity: " << totalDistance << endl;
}

bool MonoDE_Piyavskii::init(const vector<string> &params){
	if ((params.size() % 3) != 0){
		cerr << "Error en los parametros de MonoDE_Piyavskii. Deben ser multiplos de 3" << endl;
		return false;
	}
	for (int i = 0; i < params.size(); i+=3){
		parameters[params[i]] = params[i+2];
		if (params[i+1] != "="){
			cerr << "Error en los parametros de MonoDE_Piyavskii. Deben estar formados por grupos de la forma parametro = valor" << endl;
			return false;
		}
	}
	readParameter(parameters, "Ps", ps);
	setPopulationSize(ps);
	readParameter(parameters, "F", F);
	readParameter(parameters, "CR", CR);
	optDirection = getSampleInd()->getOptDirection(0);
	k_slope = 1000000;
	return true;
}

void MonoDE_Piyavskii::fillPopWithNewIndsAndEvaluate(){
	//Create Initial population
	population->resize(ps);
	for (int i = 0; i < ps; i++){
		(*population)[i] = getSampleInd()->internalClone();
		(*population)[i]->restart();
		evaluate((*population)[i]);
		evaluated.push_back((*population)[i]->internalClone());
		updateKValuesWithLast();
	}
	bestObjective = (*population)[0]->getObj(0);
	for (int i = 1; i < ps; i++){
		if ((optDirection == MINIMIZE) && ((*population)[i]->getObj(0) < bestObjective)){
			bestObjective = (*population)[i]->getObj(0);
		} else if ((optDirection == MAXIMIZE) && ((*population)[i]->getObj(0) > bestObjective)){
			bestObjective = (*population)[i]->getObj(0);
		}
	}
}

#define EPSILON 0.0000000001

void MonoDE_Piyavskii::updateKValuesWithLast(){
	int last = evaluated.size() - 1;
	double minDist = DBL_MAX;
	int nearest = 0;
	for (int i = 0; i < (int)evaluated.size() - 1; i++){
		double dist = 0;
		for (int j = 0; j < evaluated[0]->getNumberOfVar(); j++){
			dist += ((evaluated[i]->getVar(j) - evaluated[last]->getVar(j)) * (evaluated[i]->getVar(j) - evaluated[last]->getVar(j)));
		}
		dist = sqrt(dist);
		if ((dist < minDist) && (dist > EPSILON)){
			nearest = i;
			minDist = dist;
		}
		if ((dist < minDistances[i]) && (dist > EPSILON)){
			double k_estimated = (fabs(evaluated[i]->getObj(0) - evaluated[last]->getObj(0)) / dist);
			minDistances[i] = dist;
			estimatedKs[i] = k_estimated;
			nearests[i] = nearest;
		}
	}
	double k_estimated = (fabs(evaluated[last]->getObj(0) - evaluated[nearest]->getObj(0)) / minDist);
	estimatedKs.push_back(k_estimated);
	minDistances.push_back(minDist);
	nearests.push_back(nearest);
}



void MonoDE_Piyavskii::Rand1Exp(int candidate){
	//Select R values

	double success = false;
	int trials = 0;
	while(!success){
		trials++;
	
		int r1, r2, r3;
		do{
			r1 = getRandomInteger0_N(ps - 1);
		} while(r1==candidate);

		do {
			r2 = getRandomInteger0_N(ps - 1);
		} while(r2== r1 || r2==candidate);

		do{
			r3 = getRandomInteger0_N(ps - 1);
		}while(r3==r2 || r3==r1 || r3==candidate);

		//Create trial
		Individual *trial = (*population)[candidate]->internalClone();
		int n = getRandomInteger0_N(trial->getNumberOfVar() - 1);
		for(int i=0; (generateRandomDouble0_Max(1) < CR) && (i < trial->getNumberOfVar()); i++){
			trial->setVar(n, (*population)[r1]->getVar(n) + F * ((*population)[r2]->getVar(n) - (*population)[r3]->getVar(n)));
			//check boundary
			trial->setVar(n, max(trial->getVar(n), trial->getMinimum(n)));
			trial->setVar(n, min(trial->getVar(n), trial->getMaximum(n)));
			n = (n+1)%trial->getNumberOfVar();
		}


		trial->evaluate();

		Individual *original = (*population)[candidate];
		//Calculate v
		double v[original->getNumberOfVar()];
		double dist = 0;
		double D = 0;
		for (int j = 0; j < original->getNumberOfVar(); j++){
			v[j] = trial->getVar(j) - original->getVar(j);
			dist += v[j] * v[j];
			D += v[j] * v[j];
		}
		dist = sqrt(dist);
		if (dist < EPSILON){//!!!!!!
			trials--;
			continue;
		}

		if (generateRandomDouble0_Max(1) < 0.5){//!!!!
			eval(trial);
			//compare the trial vector with the current vector
			if ( ((trial->getOptDirection(0) == MINIMIZE) && (trial->getObj(0) < (*population)[candidate]->getObj(0))) ||
			     ((trial->getOptDirection(0) == MAXIMIZE) && (trial->getObj(0) > (*population)[candidate]->getObj(0))) ){
				delete (*population)[candidate];
				(*population)[candidate] = trial;
			} else {
				delete trial;
			}
			return;
		}


		//cout << "Distancia al original: " << dist << endl;
		double originalt = 1;
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
		for (int j = 0; j < evaluated.size(); j++){
			//Calculate E, F
			double E = 0;
			double F = 0;
			for (int k = 0; k < original->getNumberOfVar(); k++){
				E +=  2 * (original->getVar(k) - evaluated[j]->getVar(k)) * v[k];
				F += (original->getVar(k) - evaluated[j]->getVar(k)) * (original->getVar(k) - evaluated[j]->getVar(k));
			}

			k_slope = estimatedKs[j];
			if (validValues.size() == 0){
				break;
			}
			double A = (evaluated[j]->getObj(0) - bestObjective) * (evaluated[j]->getObj(0) - bestObjective);
			double distanceThreshold = 70.700;
			double distanceToConsider = min(distanceThreshold * distanceThreshold, A / k_slope / k_slope);
			double finalA = D;
			double finalB = E;
			double finalC = F - distanceToConsider;
			double raiz = finalB * finalB - 4 * finalA * finalC;
			if (raiz > 0){//remove range
				double s1 = (-finalB - sqrt(raiz)) / (2 * finalA);
				double s2 = (-finalB + sqrt(raiz)) / (2 * finalA);
				for (int k = 0; k < validValues.size(); k++){
					if ((s1 <= validValues[k].first) && (s2 >= validValues[k].first)){
						//cout << "Recorta con solucion: " << evaluated[j]->getObj(0) << endl;
						//cout << "s1, s2: " << s1 << ", " << s2 << endl;
						//cout << "Best: " << bestObjective << endl;
						validValues[k].first = s2;
					} else if ((s1 <= validValues[k].second) && (s2 >= validValues[k].second)){
						//cout << "Recorta con solucion: " << evaluated[j]->getObj(0) << endl;
						//cout << "s1, s2: " << s1 << ", " << s2 << endl;
						//cout << "Best: " << bestObjective << endl;
						validValues[k].second = s1;
					} else if ((s1 <= validValues[k].first) && (s2 >= validValues[k].second)){
						//cout << "Recorta con solucion: " << evaluated[j]->getObj(0) << endl;
						//cout << "s1, s2: " << s1 << ", " << s2 << endl;
						//cout << "Best: " << bestObjective << endl;
						validValues[k].first = validValues[k].second;
					} else if ((s1 > validValues[k].first) && (s2 < validValues[k].second)){
						//cout << "Recorta con solucion: " << evaluated[j]->getObj(0) << endl;
						//cout << "s1, s2: " << s1 << ", " << s2 << endl;
						//cout << "Best: " << bestObjective << endl;
						validValues.push_back(pair<double, double>(s2, validValues[k].second));
						validValues[k].second = s1;
					}
					if (validValues[k].first + EPSILON >= validValues[k].second){
						//cout << "Recorta con solucion: " << evaluated[j]->getObj(0) << endl;
						//cout << "s1, s2: " << s1 << ", " << s2 << endl;
						//cout << "Best: " << bestObjective << endl;
						validValues[k] = validValues[validValues.size()-1];
						validValues.resize((int)((validValues.size()))-1);
						k--;
					}
				}
			}
		}

		//!!!!!!!!
		/*double newSize = 0;
		bool originalIsValid = false;
		for (int j = 0; j < validValues.size(); j++){
			newSize += (validValues[j].second - validValues[j].first);
			if ((originalt + EPSILON >= validValues[j].first) && (originalt <= validValues[j].second + EPSILON)){
				originalIsValid = true;
				break;
			}
		}*/

		if (validValues.size() >= 1){//Sucess
		//if (originalIsValid){//!!!!!!!!!!!!
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

				if (trial->getObj(0) < bestObjective){
					double minDist = DBL_MAX;
					double objectiveValue = 0;
					for (int j = 0; j < evaluated.size(); j++){
						double dist = 0;
						for (int k = 0; k < evaluated[0]->getNumberOfVar(); k++){
							dist += (evaluated[j]->getVar(k) - trial->getVar(k)) * (evaluated[j]->getVar(k) - trial->getVar(k));
						}
						if (dist < minDist){
							minDist = dist;
							objectiveValue = evaluated[j]->getObj(0);
						}
					}
					cout << "Cambia la t y hubiera mejorado!!!" << endl;
					cout << "El mas cercano tiene valor: " << objectiveValue << endl;
					cout << "El best: " << bestObjective << endl;
				}
				cout << "Cambia la t" << endl;
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
			} else {
				t = originalt;
			}
			double realDistance = t * dist;
			for (int j = 0; j < original->getNumberOfVar(); j++){
				trial->setVar(j, original->getVar(j) + v[j] * t);
				trial->setVar(j, max(trial->getVar(j), trial->getMinimum(j)));
				trial->setVar(j, min(trial->getVar(j), trial->getMaximum(j)));
			}
			eval(trial);
			//compare the trial vector with the current vector
			if ( ((trial->getOptDirection(0) == MINIMIZE) && (trial->getObj(0) < (*population)[candidate]->getObj(0))) ||
			     ((trial->getOptDirection(0) == MAXIMIZE) && (trial->getObj(0) > (*population)[candidate]->getObj(0))) ){
				delete (*population)[candidate];
				(*population)[candidate] = trial;
			} else {
				delete trial;
			}
		} else { //Not success (all collapsed)
			cout << "No tuvo exito" << endl;
			if (trial->getObj(0) < bestObjective){
				cout << "No tuvo éxito y hubiera mejorado!!!" << endl;
			}

			//cout << "k: " << k_slope << endl;
			delete trial;
			if (trials == 10){
				trials = 0;
				cerr << "No muta!!!: " << k_slope << endl;
				//k_slope *= 1.001;
			}
		}
	}
}

void MonoDE_Piyavskii::eval(Individual *ind){

	double minDist = DBL_MAX;
	double objectiveValue = 0;
	for (int j = 0; j < evaluated.size(); j++){
		double dist = 0;
		for (int k = 0; k < evaluated[0]->getNumberOfVar(); k++){
			dist += (evaluated[j]->getVar(k) - ind->getVar(k)) * (evaluated[j]->getVar(k) - ind->getVar(k));
		}
		if (dist < minDist){
			minDist = dist;
			objectiveValue = evaluated[j]->getObj(0);
		}
	}

	evaluate(ind);
	evaluated.push_back(ind->internalClone());
	updateKValuesWithLast();
	if ((optDirection == MINIMIZE) && (ind->getObj(0) < bestObjective)){
		bestObjective = ind->getObj(0);
		cout << "Mejora: " << bestObjective << endl;
		cout << "Evalua uno en el que el mas cercano vale: " << objectiveValue << " y mejora" << endl;
	} else if ((optDirection == MAXIMIZE) && (ind->getObj(0) > bestObjective)){
		bestObjective = ind->getObj(0);
	} else {
		cout << "Evalua uno en el que el mas cercano vale: " << objectiveValue << " y no mejora" << endl;
	}
}

//Execute a generation: variate (includes the parent selection, because depending on the type of combination the parents are
//selected in different ways), evaluate offspring, execute survivor selection
void MonoDE_Piyavskii::runGeneration(){
	k_slope /= 1.01;
	//cout << k_slope << endl;
	for(int candidate = 0; candidate < ps; candidate++){
		Rand1Exp(candidate);
	}
	printDiversity();
}

//Recombine and mutate
void MonoDE_Piyavskii::variation(){
	/*recombinateParents();
	mutateOffspring();*/
}

void MonoDE_Piyavskii::getSolution(MOFront *p){
	for (int i = 0; i < ps; i++){
		p->insert((*population)[i]);
	}
}

void MonoDE_Piyavskii::printInfo(ostream &os) const {
	os << "MonoObjective Differential Evolution with Piyavskii" << endl;
	os << "Population size: " << ps << endl;
	os << "F: " << F << endl;
	os << "CR: " << CR << endl;
}


