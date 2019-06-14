/***********************************************************************************
 * AUTHORS
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * DATE
 *   November 2007
 ***********************************************************************************/

#include "SPEA2_MultiObjectivized.h"

// Constructor
SPEA2_MultiObjectivized::SPEA2_MultiObjectivized () {
	archive = new vector<Individual*>;
}

// Destructor
SPEA2_MultiObjectivized::~SPEA2_MultiObjectivized (void) {
	for (unsigned int i = 0; i < archive->size(); i++)
		delete (*archive)[i];
	delete (archive);
}

// SPEA2_MultiObjectivized generation
void SPEA2_MultiObjectivized::runGeneration() {
	if (getGeneration() == 0){
		multiObjectivize(population);
	}
	fitness();
	selection();
	matingSelection();
	variation();
	evaluation();
}

// Initialization: pm pc pSize aSize
bool SPEA2_MultiObjectivized::init(const vector<string> &params) {

	// Check for the proper parameters
	if (params.size() != 4) {
		cerr << "Error SPEA2_MultiObjectivized: Incorrect parameters (pm pc pSize aSize)" << endl;
		return false;
	}

	// Initiate the parameters
	this->pm = atof(params[0].c_str());
	this->pc = atof(params[1].c_str());
	setPopulationSize(atoi(params[2].c_str()));
	this->aSize = atoi(params[3].c_str());
	k = (int)(sqrt(double(getPopulationSize() + aSize)));
	return true;
}

string SPEA2_MultiObjectivized::getParameterValue(int i) const{
	stringstream s;
	switch(i){
		case 0:
			s << pm;
			return s.str();
		case 1:
			s << pc;
			return s.str();
		case 2:
			s << getPopulationSize();
			return s.str();
		case 3:
			s << aSize;
			return s.str();
	}
	return "invalid";

}

string SPEA2_MultiObjectivized::getParameterName(int i) const{
	switch(i){
		case 0:
			return "pm";
		case 1:
			return "pc";
		case 2:
			return "pSize";
		case 3:
			return "aSize";
	}
	return "invalid";
}

int SPEA2_MultiObjectivized::getNumberOfParameters() const{
	return 4;
}


// Get solution from internal archive
void SPEA2_MultiObjectivized::getSolution(MOFront *p) {
	for (unsigned int i = 0;  i < archive->size(); i++) {
		p->insert((*archive)[i]);
	}
}

// Print Information
void SPEA2_MultiObjectivized::printInfo(ostream &os) const {
	os << "Genetic Algorithm SPEA2_MultiObjectivized" << endl;
	os << "Number of Evaluations = " << getPerformedEvaluations() << endl;
	os << "Mutation Probability = " << pm << endl;
	os << "Crossover Probability = " << pc << endl;
	os << "Population Size = " << getPopulationSize() << endl;				
}

// Update archive
void SPEA2_MultiObjectivized::selection (void) {
	vector<Individual*>* newArchive = new vector<Individual*>;
		
	sort(archive->begin(), archive->end(), cmpIndividual);
	sort(population->begin(), population->end(), cmpIndividual);	

	// Selection of best individuals from the archive	
	unsigned int i = 0;
	while ((i < archive->size()) && ((*archive)[i]->getFitnessValue() < 1)) {
		newArchive->push_back((*archive)[i]);
		i++;
	}
						
	// Selection of best individuals from the population	
	unsigned int j = 0;
	while ((j < population->size()) && ((*population)[j]->getFitnessValue() < 1)) {
		newArchive->push_back((*population)[j]);
		j++;
	}

	// Complete with individuals from archive and population
	int newArchiveSize = (population->size() + archive->size())> aSize? aSize: getPopulationSize() + archive->size();
	for (unsigned int z = newArchive->size(); z < newArchiveSize; z++) {
		if (i >= archive->size()) {
			newArchive->push_back((*population)[j++]);
			continue;
		}
		if (j >= getPopulationSize()) {
			newArchive->push_back((*archive)[i++]);
			continue;
		}
		if ( (*population)[j]->getFitnessValue() < (*archive)[i]->getFitnessValue() ) {
			newArchive->push_back((*population)[j++]);
		} else {
			newArchive->push_back((*archive)[i++]);
		}
	}

	// Liberar los individuos que van a desaparecer
	for (unsigned int i2 = i; i2 < archive->size(); i2++)
		delete ((*archive)[i2]);
	for (unsigned int j2 = j; j2 < population->size(); j2++)
		delete ((*population)[j2]);

	// Truncar el nuevo archivo si habian demasiados buenos individuos
	if (newArchive->size() > aSize) {
//		truncate (newArchive, aSize);
		// Looks for the best individual (Obj0) in the archive
		long double value;
		if ((*newArchive)[0]->getOptDirection(0) == MAXIMIZE) {
			value = LDBL_MIN;
		}
		else if ((*newArchive)[0]->getOptDirection(0) == MINIMIZE) {
			value = LDBL_MAX;
		}
		Individual *best = (*newArchive)[0];
		int count = 0;
		for (int i = 0; i < newArchive->size(); i++) {
			if (((best->getOptDirection(0) == MINIMIZE) && ((*newArchive)[i]->getObj(0) < value)) ||
					((best->getOptDirection(0) == MAXIMIZE) && ((*newArchive)[i]->getObj(0) > value))) {
				best = (*newArchive)[i];
				value = (*newArchive)[i]->getObj(0);
				count = i;
			}
		}
		// Extracts the best individual from the archive
		Individual *aux = (*newArchive)[newArchive->size() - 1];
		(*newArchive)[newArchive->size() - 1] = best;
		(*newArchive)[count] = aux;
		newArchive->pop_back();
		// Truncates the archive
		truncate (newArchive, aSize - 1);
		// Inserts the best individual into the archive
		newArchive->push_back(best);
	}
	delete (archive);
	archive = newArchive;
}

// Mating selection
// Following the PISA implementation
void SPEA2_MultiObjectivized::matingSelection (void) {
	int ind1, ind2;
	
	// Reset the mating pool
	mating.clear();

	// Fill the mating pool	
	for (int i = 0; i < getPopulationSize(); i++) {
		// Elegimos 2 individuos al azar para realizar el torneo: de 0 a aSize -1
		ind1 = (int)((double)(archive->size())*rand()/ (RAND_MAX + 1.0));
		ind2 = (int)((double)(archive->size())*rand()/ (RAND_MAX + 1.0));
		if ((*archive)[ind1]->getFitnessValue() < (*archive)[ind2]->getFitnessValue())
			mating.push_back((*archive)[ind1]);
		else
			mating.push_back((*archive)[ind2]);
	}
}

// Evaluation of the individuals in the population
void SPEA2_MultiObjectivized::evaluation (void) {
	for (int i = 0; i < offSpring.size(); i++){
		evaluate((offSpring)[i]);
	}
	multiObjectivize(&offSpring);
	population->clear();
	for (int i = 0; i < offSpring.size(); i++){
		insertInArchive((offSpring)[i]);
		population->push_back((offSpring)[i]);
	}
}

// Create new population by variating the manting pool 
void SPEA2_MultiObjectivized::variation (void) {
	// clear offspring 
	offSpring.clear();
	
	// Crossover and mutation
	for (int i = 0; i < getPopulationSize(); i = i + 2) {
		offSpring.push_back(mating[i]->internalClone());
		if (i != getPopulationSize() - 1)
			offSpring.push_back(mating[i+1]->internalClone());
		// Do crossover 
		double vcross = rand() / (RAND_MAX + 1.0);
		if (vcross < pc) {  
			if (i != getPopulationSize() - 1) {
				(offSpring)[offSpring.size()-2]->crossover((offSpring)[offSpring.size()-1]);						  
			} else {
				Individual *tmp = mating[0]->internalClone();
				(offSpring)[offSpring.size()-1]->crossover(tmp);
				delete(tmp);
			}
		}
		// Do mutation 
		(offSpring)[offSpring.size()-1]->mutation(pm);
		if (i != getPopulationSize() - 1)
			(offSpring)[offSpring.size()-2]->mutation(pm);
	}
}

// Calculate the distance between two individuals
double SPEA2_MultiObjectivized::calcDistance(const Individual* i1, const Individual* i2) {
	double dist = 0;
	for (unsigned int i = 0; i < i1->getNumberOfObj(); i++) {
		double distActual;
		distActual = i1->getObj(i) - i2->getObj(i);
		dist += distActual*distActual;
	}
	return sqrt(dist);
}

// Calculate the fitness value of the all individuals
void SPEA2_MultiObjectivized::fitness (void) {
	//Calculate Strength for each individual as the number of dominated
	//individuals in P + P'
	vector<int> Sp(population->size(), 0), Sa(archive->size(), 0);
	
	for (int i = 0; i < population->size(); i++){
		for (int j = i + 1; j < population->size(); j++){
			int test = dominanceTest((*population)[i], (*population)[j]);
			if (test == FIRST_DOMINATES)
				Sp[i]++;
			else if (test == SECOND_DOMINATES)
				Sp[j]++;
		}
	}

	for (int i = 0; i < population->size(); i++){
		for (int j = 0; j < archive->size(); j++){
			int test = dominanceTest((*population)[i], (*archive)[j]);
			if (test == FIRST_DOMINATES)
				Sp[i]++;
			else if (test == SECOND_DOMINATES)
				Sa[j]++;
		}
	}

	for (int i = 0; i < archive->size(); i++){
		for (int j = i + 1; j < archive->size(); j++){
			int test = dominanceTest((*archive)[i], (*archive)[j]);
			if (test == FIRST_DOMINATES)
				Sa[i]++;
			else if (test == SECOND_DOMINATES)
				Sa[j]++;
		}
	}

	//Calculate raw fitness and distances
	vector< vector < double> > distancesPopulation(population->size(), vector<double> ());
	vector< vector < double> > distancesArchive(archive->size(), vector<double> ());
	for (int i = 0; i < population->size(); i++){
		(*population)[i]->setFitnessValue(0);
	}
	for (int i = 0; i < archive->size(); i++){
		(*archive)[i]->setFitnessValue(0);
	}
	// Population - Population
	for (int i = 0; i < population->size(); i++){
		for (int j = i + 1; j < population->size(); j++){
			int test = dominanceTest((*population)[i], (*population)[j]);
			if (test == FIRST_DOMINATES)
				(*population)[j]->setFitnessValue((*population)[j]->getFitnessValue() + Sp[i]);
			else if (test == SECOND_DOMINATES)
				(*population)[i]->setFitnessValue((*population)[i]->getFitnessValue() + Sp[j]);
			double dist = calcDistance((*population)[i], (*population)[j]);
			distancesPopulation[i].push_back(dist);
			distancesPopulation[j].push_back(dist);
		}
	}

	// Population - Archive
	for (int i = 0; i < population->size(); i++){
		for (int j = 0; j < archive->size(); j++){
			int test = dominanceTest((*population)[i], (*archive)[j]);
			if (test == FIRST_DOMINATES)
				(*archive)[j]->setFitnessValue((*archive)[j]->getFitnessValue() + Sp[i]);
			else if (test == SECOND_DOMINATES)
				(*population)[i]->setFitnessValue((*population)[i]->getFitnessValue() + Sa[j]);
			double dist = calcDistance((*population)[i], (*archive)[j]);
			distancesPopulation[i].push_back(dist);
			distancesArchive[j].push_back(dist);
		}
	}

	// Archive - Archive
	for (int i = 0; i < archive->size(); i++){
		for (int j = i + 1; j < archive->size(); j++){
			int test = dominanceTest((*archive)[i], (*archive)[j]);
			if (test == FIRST_DOMINATES)
				(*archive)[j]->setFitnessValue((*archive)[j]->getFitnessValue() + Sa[i]);					
			else if (test == SECOND_DOMINATES)
				(*archive)[i]->setFitnessValue((*archive)[i]->getFitnessValue() + Sa[j]);
			double dist = calcDistance((*archive)[i], (*archive)[j]);
			distancesArchive[i].push_back(dist);
			distancesArchive[j].push_back(dist);
		}
	}

	// Sum density estimation
	for (int i = 0; i < population->size(); i++){
		sort(distancesPopulation[i].begin(), distancesPopulation[i].end());
		(*population)[i]->setFitnessValue((*population)[i]->getFitnessValue() + (1.0 / (distancesPopulation[i][k] + 2.0)));
	}
	for (int i = 0; i < archive->size(); i++){
		sort(distancesArchive[i].begin(), distancesArchive[i].end());
		(*archive)[i]->setFitnessValue((*archive)[i]->getFitnessValue() + (1.0 / (distancesArchive[i][k] + 2.0)));
	}
}

// Truncate operator
void SPEA2_MultiObjectivized::truncate (vector<Individual*> *v, const int size) {
	vector < vector<double> > dists (v->size(), vector<double>(v->size(), HUGE_VAL));
	vector < vector<double> > distsOrdered;
	vector<int> eliminate;

	// Calculate distances between all individuals pairs
	// Distance (i, i) = HUGE_VAL
	for (unsigned int i = 0; i < v->size(); i++) {
		for (unsigned int j = i + 1; j < v->size(); j++) {
			dists[i][j] = dists[j][i] = calcDistance((*v)[i],(*v)[j]);
		}
	}
	
	// Order each vector
	distsOrdered = dists;
	for (unsigned int i = 0; i < distsOrdered.size(); i++) 
		sort (distsOrdered[i].begin(), distsOrdered[i].end());

	int s = v->size() - aSize; // Number of elements to eliminate
	vector<int> *activeInd = new vector<int>(); // when an individual is droped, we eliminate it from here
	vector<int> *minInd; // where to search the minimums
	vector<int> *minIndNext; // next individuals to consider
	double minValue;

	//Initially all elements are active
	for (unsigned int i = 0; i < distsOrdered.size(); i++) 
		activeInd->push_back(i);
	
	for (int i = 0; i < s; i++) { // For each element to eliminate
		minInd = activeInd;
		// dimensions from breaking the ties
		for (unsigned int searchDim = 0; searchDim < distsOrdered[0].size(); searchDim++) {
			minIndNext = new vector<int>();
			minValue = HUGE_VAL;
			for (unsigned int j = 0; j < minInd->size(); j++) { // Search the minimums in current dimension 
				if (distsOrdered[(*minInd)[j]][searchDim] < minValue) {
					minIndNext->clear();
					minIndNext->push_back((*minInd)[j]);
					minValue = distsOrdered[(*minInd)[j]][searchDim];
				} else if (distsOrdered[(*minInd)[j]][searchDim] == minValue) { // Tie
					minIndNext->push_back((*minInd)[j]);
				}	
			}
			if (minInd != activeInd)
				delete minInd;
			if (minIndNext->size() == 1){ // Found
				break;
			} else { // Next distance must be considered
				minInd = minIndNext;
			}
		}
	
		// Element to eliminate have been selected (index = minIndNext[0])
		// eliminate from activeInd 
		// TODO: change algorithm with a binary search
		eliminate.push_back ((*minIndNext)[0]);
		activeInd->erase (find(activeInd->begin(), activeInd->end(), (*minIndNext)[0]));
		
		// Eliminate the distances with that element in each vector
		// TODO: change algorithm with a binary search
		for (unsigned int j = 0; j < distsOrdered.size(); j++)
			distsOrdered[j].erase(find(distsOrdered[j].begin(), distsOrdered[j].end(), dists[(*minIndNext)[0]][j]));
		delete minIndNext;
	}
	delete activeInd;
	// Perform eliminations
	sort (eliminate.begin(), eliminate.end());
	for (int i = ((int) eliminate.size()) - 1; i >= 0; i--) {
	 	delete((*v)[eliminate[i]]);
		(*v)[eliminate[i]] = (*v)[v->size()-1];
		v->pop_back();
	}
}

void SPEA2_MultiObjectivized::multiObjectivize(vector <Individual*> *updatePop){
	int start = getSampleInd()->getNumberOfObj() - getNumMultiObjectivizationPlugins();
	for (int i = start; i < getSampleInd()->getNumberOfObj(); i++){
		getMultiObjectivizationPlugin(i - start)->convertToMultiObjective(archive, updatePop, NULL, i, getGeneration());
	}
}

void SPEA2_MultiObjectivized::exchangePerformed(){
	multiObjectivize(population);	
}
