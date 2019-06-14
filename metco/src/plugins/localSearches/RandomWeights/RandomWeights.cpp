#include "RandomWeights.h"

void RandomWeights::localSearch(vector <Individual*> *poblacion, MOFront *currentSolution){
	double rLS = (double(rand()))/(RAND_MAX + 1.0); //[0, 1)
	if (pLS <= rLS)
		return;

	vector <double> objectives((*poblacion)[0]->getNumberOfObj(), 0);
	for (unsigned int i = 0; i < poblacion->size(); i++) {
		vector <double> randomWeights;
		//La funcion de fitness w0 * f0 + w1 * f1 +... es a maximizar
		for (int j = 0; j < (*poblacion)[i]->getNumberOfObj(); j++){
			if ((*poblacion)[i]->getInternalOptDirection(j) == MAXIMIZE){
				randomWeights.push_back((double)(rand()) / RAND_MAX);
			} else {
				randomWeights.push_back((-(double)(rand())) / RAND_MAX);
			}
		}
		
		int iterations = 0;
		bool changed = true;
		int bestNeighbour = 0;
		while ((changed) && (iterations <= maxIterations)) {
			changed = false;
			iterations++;
			double currentFitness = calculateF((*poblacion)[i], randomWeights);
			for (unsigned int j = 0; j < (*poblacion)[i]->getNeighbourhoodSize(); j++){
				(*poblacion)[i]->getNeighbourObjectives(j, objectives);
				double tmpFitness = calculateF(objectives, randomWeights);
				if (tmpFitness > currentFitness){
					currentFitness = tmpFitness;
					bestNeighbour = j; 
					changed = true;
				} 
			}
			if (changed){
				Individual *tmp = (*poblacion)[i]->getNeighbour(bestNeighbour);
				if (currentSolution != NULL){
					currentSolution->insert(tmp);
				} 
				delete (*poblacion)[i];
				(*poblacion)[i] = tmp;
			}
		}
	}
}

double RandomWeights::calculateF( Individual *ind, vector <double> &weights){
	double fitness = 0;
	for (int i = 0; i < ind->getNumberOfObj(); i++){
		fitness += (ind->getObj(i) * weights[i]);
	}
	return fitness;
}

double RandomWeights::calculateF( vector <double> &objectives, vector <double> &weights){
	double fitness = 0;
	for (int i = 0; i < objectives.size(); i++){
		fitness += (objectives[i] * weights[i]);
	}
	return fitness;
}


//Parameters: MaxIterations_per_individual pLS
bool RandomWeights::init(const vector<string> &params){
	if (params.size() != 2){
		cout << "Error, RandomWeights parameters --> MaxIterations pLS" << endl;
		return false;				
	}
	maxIterations = atoi(params[0].c_str());
	if (maxIterations < 1){
		cout << "Error, maxIterations < 1" << endl;
		return false;
	}
	pLS = atof(params[1].c_str());
	if ((pLS < 0) || (pLS > 1)){
		cout << "Error, pLS must be in [0, 1]" << endl;
	}
	return true;
}
