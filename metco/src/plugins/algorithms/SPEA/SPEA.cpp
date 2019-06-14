/***********************************************************************************
 * AUTHORS
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * DATE
 *   November 2007
 * *********************************************************************************/


#include "SPEA.h"


// Constructor
SPEA::SPEA () {
	archive = new vector<Individual*>;
}

// Destructor
SPEA::~SPEA (void) {
	for (unsigned int i = 0; i < archive->size(); i++)
		delete (*archive)[i];
	delete (archive);
}

// SPEA generation
void SPEA::runGeneration() {
	fitness();
	matingSelection();
	variation();
	evaluation ();
}

// Initialization: pm pc popSize aSize
bool SPEA::init(const vector<string> &params) {
	// Check for the algorithm proper parameters
	if (params.size() != 4) {
		cerr << "Error SPEA: Incorrect parameters (pm pc pSize aSize)" << endl;
		return false;
	}

	this->pm = atof(params[0].c_str());
	this->pc = atof(params[1].c_str());
	setPopulationSize(atoi(params[2].c_str()));
	this->aSize = atoi(params[3].c_str());
	return true;
}

// Obtain current solution
void SPEA::getSolution(MOFront *p) {
	for (unsigned int i = 0;  i < archive->size(); i++) {
		p->insert((*archive)[i]);
	}
}

// Print configuration
void SPEA::printInfo(ostream &os) const {
	os << "Genetic Algorithm SPEA" << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Mutation Probability = " << pm << endl;
	os << "Crossover Probability = " << pc << endl;
	os << "Population Size = " << getPopulationSize() << endl;
}

// Mating selection
void SPEA::matingSelection (void) {
	unsigned int index1, index2;
	Individual *ind1, *ind2;
	mating.clear();

	// Select |P| individuals
	for (int i = 0; i < getPopulationSize(); i++){
		// tournament selection 
		index1 = (unsigned int)((double)(archive->size() + getPopulationSize())*rand()/ (RAND_MAX + 1.0));
		index2 = (unsigned int)((double)(archive->size() + getPopulationSize())*rand()/ (RAND_MAX + 1.0));
	
		// Selección de los dos individuos
		if (index1 < archive->size())
			ind1 = (*archive)[index1];
		else 
			ind1 = (*population)[index1 - archive->size()];
		if (index2 < archive->size())
			ind2 = (*archive)[index2];
		else 
			ind2 = (*population)[index2 - archive->size()];
		
		if (ind1->getFitnessValue() < ind2->getFitnessValue())
			mating.push_back(ind1);
		else
			mating.push_back(ind2);
	}
}

// Evaluation of the individuals in the population
// Update external set
// Calculate fitness
void SPEA::evaluation (void) {
	// Evaluate population
	for (int i = 0; i < getPopulationSize(); i++){
		evaluate((*population)[i]);
		insertInArchive((*population)[i]);
	}

	// Detect non-dominated individuals in the population
	markNondominated();
	
	// Remove solutions within the archive which are covered by any other member of the archive, and
	// insert new non-dominated individuals
	updateExternalNondominatedSet();

	// If the number of externally stored nondominated solutions exceeds a given maximum aSize, 
	// prune the archive by means of clustering.
	if (archive->size() > ((unsigned int) aSize))
		clustering(archive, aSize);
	
}


// Variation from the mating pool to the new population
void SPEA::variation (void) {
	vector<Individual*> *oldPop = population;
	population = new vector<Individual*>;
	
	// Crossover and mutation
	for (int i = 0; i < getPopulationSize(); i = i + 2) {
		population->push_back(mating[i]->internalClone());
		if (i != getPopulationSize() - 1)
			population->push_back(mating[i+1]->internalClone());
		double vcross = rand() / (RAND_MAX + 1.0);

		// Crossover
		if (vcross < pc) {
			if (i != getPopulationSize() - 1) {
				(*population)[population->size()-2]->crossover((*population)[population->size()-1]);						  
			} else {
				Individual *tmp = mating[0]->internalClone();
				(*population)[population->size()-1]->crossover(tmp);
				delete(tmp);
			}
		}
		
		// Mutation
		(*population)[population->size()-1]->mutation(pm);
		if (i != getPopulationSize() - 1)
			(*population)[population->size()-2]->mutation(pm);
	}

	// Clear old population
	for (unsigned int i = 0; i < oldPop->size(); i++)
		delete((*oldPop)[i]);
	delete oldPop;
}


// Calculate the fitness value of the all individuals
void SPEA::fitness (void) {
	// In P':
  //  f_i = Strength_i = n_i / (N+1), 
  //     n_i is the number of dominated solutions by i in P
  //     N = |P| 
	for (unsigned int i = 0; i < archive->size(); i++) {
		double count = 0.0;
		for (int j = 0; j < getPopulationSize(); j++) {
			int test = dominanceTest((*archive)[i], (*population)[j]);
			if (test == FIRST_DOMINATES)
				count++;
		}
		(*archive)[i]->setFitnessValue(count / (getPopulationSize() + 1));
	}	 

	// In P:
  //   f_i = 1 + (Strength_j foreach j in P', which dominantes i)
	for (int i = 0; i < getPopulationSize(); i++) {
		double sum = 0.0;
		for (unsigned int j = 0; j < archive->size(); j++) {
			int test = dominanceTest((*archive)[j], (*population)[i]);
			if (test == FIRST_DOMINATES)
				sum += (*archive)[j]->getFitnessValue();
		}
		(*population)[i]->setFitnessValue(1 + sum);
	}	  
}

// Marks all nondominated individuals according to the population with fitness 0 
// Individuals with fitness = 0, are non-dominated individuals (in the population)
void SPEA::markNondominated (void) {
	Individual *indI, *indJ;
	for (int i = getPopulationSize() - 1; i >= 0; i--)
		(*population)[i]->setFitnessValue(0);
	for (int i = getPopulationSize() - 1; i >= 0; i--) {
		indI = (*population)[i];
		if (indI->getFitnessValue() != 0)
			continue;
		for (int j = i - 1; j >= 0; j--) {
			indJ = (*population)[j];
			if (indJ->getFitnessValue() == 0) {
				int test = dominanceTest(indI, indJ);
				if (test == FIRST_DOMINATES)
					indJ->setFitnessValue(1);
				else if (test == SECOND_DOMINATES){
					indI->setFitnessValue(1);
					break;
				}
			}
		}
	}
} 

// Remove solutions within the external set which are covered by any other set member
// and insert new non-dominated individuals
void SPEA::updateExternalNondominatedSet (void) {
	Individual *ind, *nondomInd;
	int dominated;
	int j;

	for (int i = getPopulationSize() - 1; i >= 0; i--) {
		ind = (*population)[i];
		if (ind->getFitnessValue() == 0) {//non-dominated in the population
			dominated = false;
			for (j = 0; j < ((int) archive->size()); j++) {
	   		nondomInd = (*archive)[j];
				int test = dominanceTest(nondomInd, ind);
				if (test == FIRST_DOMINATES){
					dominated = true;
					break;
				} else if (test == SECOND_DOMINATES){
					delete (nondomInd);
					(*archive)[j] = ind->internalClone();
					break;
				}
			}
			if (dominated == false) { //remove remaining
				if (j < (((int) archive->size()) - 1)) {
					j++;  // Next position because ind is stored at position j 
					while (j <= ((int) (archive->size()) - 1)) {
						nondomInd = (*archive)[j];
						int test = dominanceTest(ind, nondomInd);
						if (test == FIRST_DOMINATES){
							delete (nondomInd);
							(*archive)[j] = (*archive)[archive->size() - 1];
							archive->pop_back();						
						} else  
							j++;
					}
				} else if (j == ((int) archive->size())) {
					// Candidate is a new solution in nondominatedSet 
					archive->push_back(ind->internalClone());
				}
			}
		}
	}
} 

// Calculate the distance between individuals
double SPEA::calcDistance(const Individual* i1, const Individual* i2) {
	double dist = 0;
	double distActual;
	
	for (unsigned int i = 0; i < i1->getNumberOfObj(); i++){
		distActual = i1->getObj(i) - i2->getObj(i);
		dist += distActual * distActual;
	}
	
	return (sqrt(dist));
}

// Calculate the distance between clusters 
double SPEA::clusterDistance (vector<Individual*> *nonDominatedSet, vector<int> *clusters, vector<int> *clusterList, 
					 					int cluster1, int cluster2) {
	double sum = 0;
	int numOfPairs = 0;
	int c1 = (*clusters)[cluster1];
				    
	while (c1 >= 0) {
		int c2 = (*clusters)[cluster2];
		while (c2 >= 0) {
			sum += calcDistance((*nonDominatedSet)[c1], (*nonDominatedSet)[c2]);
			numOfPairs++;
			c2 = (*clusterList)[c2];
     }
     c1 = (*clusterList)[c1];
	}
	return (sum / double(numOfPairs));
} 

// Join two clusters
void SPEA::joinClusters(vector<int> *clusters, vector<int> *clusterList, int cluster1, int  cluster2, int& numOfClusters) {
	int c1 = (*clusters)[cluster1];
	
	while ((*clusterList)[c1] >= 0)
		c1 = (*clusterList)[c1];
	(*clusterList)[c1] = (*clusters)[cluster2];
	numOfClusters--;
	(*clusters)[cluster2] = (*clusters)[numOfClusters];
} 

// Calculate the point with minimal average distance to all other points in the cluster
int SPEA::clusterCentroid(vector<Individual*> *nonDominatedSet, vector<int> *clusters, vector<int> *clusterList, int cluster) {
	double minSum = -1;
	int minIndex = 0;
	int c1 = (*clusters)[cluster];
				   
	while (c1 >= 0) {
		int c2 = (*clusters)[cluster];
		double sum = 0;
		while (c2 >= 0) {
			sum += calcDistance((*nonDominatedSet)[c1], (*nonDominatedSet)[c2]);
			c2 = (*clusterList)[c2];
		}
		if ((sum < minSum) || (minSum < 0)) {
			minSum = sum;
			minIndex = c1;
		}
		c1 = (*clusterList)[c1];
	}
	return (minIndex);
}


// Reduce the archive size, using the clustering technique
void SPEA::clustering (vector<Individual*> *nonDominatedSet, const int maxSize) {
	vector<int> *clusters;
	vector<int> *clusterList;      
	vector<int> selectedSolutions;      

	clusters = new vector<int>;
	clusterList = new vector<int>;
	
	// Firstly, define one-element-clusters 
	int  numOfClusters = nonDominatedSet->size();
	for (int i = 0 ; i < numOfClusters; i++) {
		clusters->push_back(i);
		clusterList->push_back(-1);
	}

	// Join clusters using average linkage method 
	while (numOfClusters > maxSize) {
		// Find cluster pair with minimal distance 
		double minDist = -1;
		int join1 = 0, join2 = 0;	
		for (int c = 0; c < numOfClusters; c++) {
			for (int d = c + 1; d < numOfClusters; d++) {
				double dist = clusterDistance(nonDominatedSet, clusters, clusterList, c, d);
				if ((dist < minDist) || (minDist < 0)) {
					minDist = dist;
					join1 = c;
					join2 = d;
				}
			}
		}
		// Join cluster pair
		joinClusters(clusters, clusterList, join1, join2, numOfClusters);
	}
	
	// Choose for each cluster representative solution (centroid) 
	for (unsigned int i = 0; i < nonDominatedSet->size(); i++)
		selectedSolutions.push_back(false);
	for (int i = 0; i < numOfClusters; i++)
		selectedSolutions[clusterCentroid(nonDominatedSet, clusters, clusterList, i)] = true;

	unsigned int counter = 0;
	for (unsigned int i = 0; i < nonDominatedSet->size(); i++) {
		if (selectedSolutions[i]) {
			if (counter != i) {
				(*nonDominatedSet)[counter] = (*nonDominatedSet)[i];
				(*nonDominatedSet)[i] = NULL;
			}
			counter++;
		} else {
			delete ((*nonDominatedSet)[i]);
			(*nonDominatedSet)[i] = NULL;
		}
	}
	nonDominatedSet->resize(aSize);
	delete clusters;
	delete clusterList;
}
