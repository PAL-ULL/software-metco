/***********************************************************************************
 * AUTHORS
 *   Gara Miranda Valladares
 *   Carlos Segura González
 * 
 *   AFPProblem class breaks from the code of Franscisco Luna
 *   
 * DATE
 *   MARCH 2009
 ***********************************************************************************/

#include "AFP_memory.h"
#include "AFPOriginal.h"
#include "utils.h"

#include <float.h>
#include <limits.h>

/*************************************************************************************
 * AFP Members
 *************************************************************************************/

// Individual Initialization
// Parameters: instance_file mutationType mutationParams
bool AFP_memory::init (const vector<string> &params){
	//Check parameters
	if (params.size() < 2){
		initialization_error();
		return false;
	}
	cout << "Va a llamar al constructor" << endl << flush;
	if (firstInitialization)
		problem = new AFPProblem(params[0]);
	cout << "Sale del constructor" << endl << flush;
	mutationType = atoi(params[1].c_str());

	// Check correct parameters for the selected mutation
	switch(mutationType){
		case MUTATION_UNIFORM:
			if (params.size() != 2){
				initialization_error();
				return false;
			}
			break;
		case MUTATION_RENAME:
			if (params.size() != 2){
				initialization_error();
				return false;
			}
			break;
		case MUTATION_NEIGHBOURS:
			if (params.size() != 3){
				initialization_error();
				return false;
			}
			maxMutated = atoi(params[2].c_str());
			break;
		default:
			initialization_error();
			return false;
	}
	// Static variables to access problem information: only one initialization needed
	if (firstInitialization){
		cout << "2" << endl << flush;
		dependentSectors = problem->getDependentSectors();
		cout << "3" << endl << flush;
		independentSectors = problem->getIndependentSectors();
		cout << "4" << endl << flush;
		sectors2trxs = problem->getSectors2trxs();
		cout << "5" << endl << flush;
		trxs = problem->getTrxs();
		cout << "6" << endl << flush;
		adjCostBothSect = problem->getAdjCostBothSect();
		cout << "7" << endl << flush;
		coCostBothSect = problem->getCoCostBothSect();
		cout << "8" << endl << flush;
		victimOrInterfererTrxCost = problem->getVictimOrInterfererTrxCost();
		directInfluencedTrxs = problem->getDirectInfluencedTrxs();
		cout << "9" << endl << flush;
		sectorsInterfereOrVictim = problem->getSectorsInterfereOrVictim();
		cout << "10" << endl << flush;
		//calculateInfluencedTrxs();
		cout << "11" << endl << flush;
		firstInitialization = 0;
	}
	setNumberOfVar(trxs->size());
	setNumberOfObj(1);
	return true;
}

// Execute the corresponding mutation
void AFP_memory::dependentMutation(double pm){
	switch (mutationType){
		case MUTATION_UNIFORM:
			mutationUniform(pm);
			break;
		case MUTATION_RENAME:
			mutationRename(pm);
			break;
		case MUTATION_NEIGHBOURS:
			mutationNeighbours(pm);
			break;
		default:
			cout << "Internal error: No Mutation defined" << endl;
			break;
	}
	cout << "Antes: " << getObj(0) << endl;
	setCacheDataSize(problem->getIndexesInTable());
	cout << "Evaluacion" << endl << flush;
	problem->completeEvaluation(this);
	cout << "Despues: " << getObj(0) << endl;
}

// Each assignment is randomly changed with a probability pm 
// At the end, performs a localSearch
void AFP_memory::mutationUniform(double pm){
	MiSetInt *newSectorsToTry = new MiSetInt(problem->getHighestSector());
	for (unsigned short i = 0; i < trxs->size(); i++){
		if((1.0*rand()/(RAND_MAX+1.0)) < pm){
			vector<unsigned short> *trxFreq = (*trxs)[i].validFrequencies;
			removeAssignTRX(i, (*trxFreq)[generateRandom(0, trxFreq->size()-1)]);
			for (vector<unsigned short>::iterator it2 = (*directInfluencedTrxs)[i].begin(); it2 != (*directInfluencedTrxs)[i].end(); it2++){
				newSectorsToTry->insert((*trxs)[*it2].sectorID);
			}
		}
	}
	improveSectors(newSectorsToTry);
	delete newSectorsToTry;
}

//  Given the set of accepted frequencies F, make a random bijection f: F <--> F.
//  Then change each assignment freq by f(freq) with a probability pm.
//  At the end, performs a localSearch
//
//  Note: it suppose that every TRX support the same frequencies. 
void AFP_memory::mutationRename(double pm){
	vector<unsigned short> *freqs = (*trxs)[0].validFrequencies;
	vector<unsigned short> freqsReordered = *freqs;
	//Create a random bijection by randomly ordering the set of frequencies
	for (unsigned short i = 0; i < freqs->size(); i++){
		int selected = generateRandom(0, freqs->size() -1 -i);
		unsigned short tmp = freqsReordered[selected];
		freqsReordered[selected] = freqsReordered[freqsReordered.size() - 1 -i];
		freqsReordered[freqsReordered.size() - 1 -i ] = tmp;
	}
	//Each frequency value is changed for its corresponding in the
	//random order generated, with a probability pm
	map<unsigned short, unsigned short> rename;
	for (unsigned short i = 0; i < freqs->size(); i++){
		rename[(*freqs)[i]] = freqsReordered[i];
	}
	for (unsigned short i = 0; i < trxs->size(); i++){
		if((1.0*rand()/(RAND_MAX+1.0)) < pm){
			removeAssignTRX(i, rename[(unsigned short)getVar(i)]);
		}
	}
	localSearch();
}

// First it selects a random trx and randomly change its assignment. With a probability pm
// it changes the trx which produced some interference with the selected trx (in any 
// direction)
// This is repeated maxMutated times, but in the next steps the trx is selected among
// the ones that had interferences with the previously changed trxs.
// At the end, performs a localSearch
void AFP_memory::mutationNeighbours(double pm){
	MiSetInt *newSectorsToTry = new MiSetInt(problem->getHighestSector());
	vector<unsigned short> previouslyInfluencedSectors;
	set<unsigned short> interfPrevious;
	
	for (unsigned short h = 0; h < maxMutated; h++){// Repeat maxMutated times
		unsigned short selectedSector;
		if (previouslyInfluencedSectors.size() == 0){ // Select a random sector
			selectedSector = (*dependentSectors)[generateRandom(0, dependentSectors->size()-1)];
		} else{ // Select a sector influenced by a previously selected trx
			selectedSector = previouslyInfluencedSectors[generateRandom(0, previouslyInfluencedSectors.size()-1)];
		}
		// Selects a random trx from the sector
		unsigned short trx = (*sectors2trxs)[selectedSector][generateRandom(0, (*sectors2trxs)[selectedSector].size()-1)];

		// Randomly reassign the frequency of the trx
		vector<unsigned short> *trxFreq = (*trxs)[trx].validFrequencies;
		removeAssignTRX2(trx, (*trxFreq)[(generateRandom(0, trxFreq->size()-1))], interfPrevious);
		for (vector<unsigned short>::iterator it2 = (*directInfluencedTrxs)[trx].begin(); it2 != (*directInfluencedTrxs)[trx].end(); it2++){
			newSectorsToTry->insert((*trxs)[*it2].sectorID);
		}
		// Randomly reassign the frequency of the interfered trx with a probability pm
		for (set<unsigned short>::iterator it = interfPrevious.begin(); it != interfPrevious.end(); it++){
			if((1.0*rand()/(RAND_MAX+1.0)) < pm){
				removeAssignTRX(*it, (*(*trxs)[*it].validFrequencies)[generateRandom(0, (*trxs)[*it].validFrequencies->size()-1)]);
				// Store the possibly influenced sectors in previouslyInfluencedSectors
				for (vector<unsigned short>::iterator it2 = (*directInfluencedTrxs)[*it].begin(); it2 != (*directInfluencedTrxs)[*it].end(); it2++){
					newSectorsToTry->insert((*trxs)[*it2].sectorID);
					previouslyInfluencedSectors.push_back((*trxs)[*it2].sectorID);
				}
			}
		}
	}
	improveSectors(newSectorsToTry);
	delete newSectorsToTry;
}

// Fill the variable directInfluencedTrxs, indicating for each trx the set of trxs which
// has interferences with it (in any direction)
/*void AFP_memory::calculateInfluencedTrxs(){

	long metidos = 0;
	for (unsigned short trx = 0; trx < trxs->size(); trx++){
		directInfluencedTrxs.push_back(vector<unsigned short>());
		//Trxs in the sector
		for (unsigned short i = 0; i < (*sectors2trxs)[(*trxs)[trx].sectorID].size(); i++){
			directInfluencedTrxs[trx].push_back((*sectors2trxs)[(*trxs)[trx].sectorID][i]);
			metidos++;
		}
		//Trxs that are victim or interferer
		for (map<unsigned short, int>::iterator iter = (*victimOrInterfererTrxCost)[trx].begin(); iter != (*victimOrInterfererTrxCost)[trx].end(); iter++){
			directInfluencedTrxs[trx].push_back(iter->first);
			metidos++;
		}
	}

}*/

// Remove the assignment to the trx, and make the new assigment freq
// Update the objective value
void AFP_memory::removeAssignTRX(unsigned short trx, unsigned short freq){
	short dif;
	if ((unsigned short)getVar(trx) == freq){
		return;
	}
	// Check each trx with possible interferences
	for (vector<unsigned short>::iterator it = (*directInfluencedTrxs)[trx].begin(); it != (*directInfluencedTrxs)[trx].end(); it++){
		int cachePos = (*victimOrInterfererTrxCost)[MENOR(trx, *it)][MAYOR(trx, *it)];
		setObj(0, getObj(0) - getCacheData(cachePos));
		if ((unsigned short)getVar(*it) == freq){
			setObj(0, getObj(0) + (*coCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
			setCacheData(cachePos, (*coCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
		} else {
			dif = (unsigned short)getVar(*it) - freq;
			if ((dif == 1) || (dif == -1)){
				setObj(0, getObj(0) + (*adjCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
				setCacheData(cachePos, (*adjCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
			} else {
				setCacheData(cachePos, 0);
			}
		}
	}
	setVar(trx, freq);
}

// Remove the assignment to the trx, and make the new assigment freq
// Update the objective value
// Save in interfPrevious the set of trxs which had interferences with the
// trx in the previous assigment
void AFP_memory::removeAssignTRX2(unsigned short trx, unsigned short freq, set<unsigned short> &interfPrevious){
	int dif;
	interfPrevious.clear();
	if ((unsigned short)getVar(trx) == freq){
		return;
	}
	// Check each trx with possible interferences
	for (vector<unsigned short>::iterator it = (*directInfluencedTrxs)[trx].begin(); it != (*directInfluencedTrxs)[trx].end(); it++){
		int cachePos = (*victimOrInterfererTrxCost)[MENOR(trx, *it)][MAYOR(trx, *it)];
		setObj(0, getObj(0) - getCacheData(cachePos));
		if (getCacheData(cachePos) != 0){
			interfPrevious.insert(*it);	
		}
		if ((unsigned short)getVar(*it) == freq){
			setObj(0, getObj(0) + (*coCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
			setCacheData(cachePos, (*coCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
		} else {
			dif = (unsigned short)getVar(*it) - freq;
			if ((dif == 1) || (dif == -1)){
				setObj(0, getObj(0) + (*adjCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
				setCacheData(cachePos, (*adjCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
			} else {
				setCacheData(cachePos, 0);
			}
		}
	}
	setVar(trx, freq);
}

//assign to each trx one frequency among its valid frequencies
//It doesn't update objectives value or cache table
void AFP_memory::randomAssignment(){
	for (unsigned short i = 0; i < (*trxs).size(); i++){
		vector<unsigned short> *trxFreq = (*trxs)[i].validFrequencies;
		setVar(i, (*trxFreq)[generateRandom(0, trxFreq->size()-1)]);
	}
}

// Creation of initial individuals
// - Generate them randomly
// - Evaluate
// - Perform a local search
void AFP_memory::restart(){
	cout << "Restart" << endl << flush;
	randomAssignment();
	cout << "Random Assignment" << endl << flush;
	setCacheDataSize(problem->getIndexesInTable());
	cout << "Evaluacion" << endl << flush;
	problem->completeEvaluation(this);
	print(cout);
	cout << "local search" << endl << flush;
	localSearch();
	cout << "fin localsearch" << endl << flush;
}

// Generate a new individual cloning it
Individual *AFP_memory::clone() const{ 
	AFP_memory *newInd = new AFP_memory();
	//copy the current costs
	newInd->cacheData = cacheData;
	newInd->maxMutated = maxMutated;
	newInd->mutationType = mutationType;
	return newInd;
}

void AFP_memory::cloned(){
	//Calculamos la tabla cache
	setCacheDataSize(problem->getIndexesInTable());
	problem->completeEvaluation(this);
}

void AFP_memory::deserialized(){
	//Calculamos la tabla cache
	setCacheDataSize(problem->getIndexesInTable());
	problem->completeEvaluation(this);
}

/*
The aim of the designed local search is to optimize the assignment of
frequencies to TRXs in a given sector, without changing the remaining
network assignments. The neighbours of a candidate solution are obtained by
replacing the frequencies in the TRXs of each sector. Therefore, the
neighbourhood size is the number of sectors in the network. The
reassignment of frequencies within a sector is performed in the
following way: first, the available frequencies for the sector are
sorted by their involved cost. Then, two possibilities are
considered, either assign the frequency with lowest associated cost
to a TRX that is allowed to use that frequency, or assign its two
adjacent frequencies to two different TRXs (if they are allowed to
use these frequencies). For each of the newly generated partial
solutions the same process is repeated until all TRXs in the sector
have been assigned a frequency. The complete solution with lowest
associated cost is considered as the new neighbour.
The order in which neighbours are analyzed is determined in a random
way, but trying to avoid the generation of neighbours that are not going 
to improve the current solution. For such purpose a set called 
newSectorsToTry (inside the method improveSectors) containing the sectors 
that might improve the current solution is maintained. 
Initially, all sectors are introduced in sectorsToTry. For the generation 
of a new neighbour, a sector sec is randomly extracted from sectorsToTry
and its frequencies reassigned. The local search moves to the first new
generated neighbour that improves the current solution,
adding all the sectors that interfere or are interfered by $sec$ to
the set of the next sectors (newSectorsToTry) to consider. Each time 
sectorsToTry set gets empty, sectors in the newSectorsToTry are 
transferred to the current set and the newSectorsToTry set is cleared. 
The local search stops when none of the neighbours improves the current solution.
*/
void AFP_memory::localSearch(){
	MiSetInt *allSectors = new MiSetInt(problem->getHighestSector());
	for (vector<unsigned short>::iterator i = dependentSectors->begin(); i != dependentSectors->end(); i++){
		allSectors->insert(*i);
	}
	for (vector<unsigned short>::iterator i = independentSectors->begin(); i != independentSectors->end(); i++){
		allSectors->insert(*i);
	}
	//Initially all sectors must be considered
	improveSectors(allSectors);
	delete allSectors;
}

//Perform the localSearch considering the sectors in newSectorsToTry
void AFP_memory::improveSectors(MiSetInt *newSectorsToTry){
	MiSetInt *sectorsToTry = new MiSetInt(problem->getHighestSector());
	MiSetInt *freeDelayed = sectorsToTry;
	while(newSectorsToTry->size() != 0){ // while there exists sectors to try 
		MiSetInt *tmpSet = sectorsToTry; //sectorsToTry here is empty
		sectorsToTry = newSectorsToTry;
		newSectorsToTry = tmpSet;
		while(sectorsToTry->size()){ // for each sector that could be improved 
			unsigned short indexSector = generateRandom(0, sectorsToTry->size()-1); //select one random sector
			unsigned short sector = sectorsToTry->get(indexSector);
			if (improveSector(sector)){ // The new assigment is better
				// The sectors which has interferences with it (in any direction) must be checked 
				for (vector<unsigned short>::iterator iter = (*sectorsInterfereOrVictim)[sector].begin(); iter != (*sectorsInterfereOrVictim)[sector].end(); iter++){
					newSectorsToTry->insert(*iter);
				}
			}
			// Erase the optimized sector
			sectorsToTry->remove(sector);
		}
	}
	delete freeDelayed;
}

//Move to the neighbuour that changes the sector selectedSector
//One sector is changed, considering that the rest of the network can not be modified.
//For this purpose the cost of using each frequency is calculated. In principle we will want to use
//the best, but it could happen that it is better to don't use the best frequency, but the
//2 adjacent frequencies. Each posibility is tried in this way, and the best found configuration
//constitutes the neighbour.
//It return a bool indicating if any change in the sector has been performed
bool AFP_memory::improveSector(unsigned short selectedSector){
	double initialCost = getObj(0);
	if ((*sectors2trxs)[selectedSector].size() == 0)
		return false;
	vector< vector<unsigned short> > selectedFrequencies(1, vector<unsigned short>());
	vector< multiset<unsigned short> > possibleFrequencies(1, multiset<unsigned short>());
	vector< set<unsigned short> > notAssignedTrxs (1, set<unsigned short>());
	vector< vector<unsigned short> > assignedTrxs(1, vector<unsigned short>());
	set<unsigned short> initialFreqs;

	//remove assignment to trxs in the sector
	for (vector<unsigned short>::iterator trx = (*sectors2trxs)[selectedSector].begin(); trx != (*sectors2trxs)[selectedSector].end(); trx++){//Each trx of the selected sector
		notAssignedTrxs[0].insert(*trx);
		initialFreqs.insert((unsigned short)getVar(*trx));
		removeTrx(*trx);
	}

	//calculate cost of assignment of each frequency
	vector<double> frequencyCosts(problem->getMaxFrequency() - problem->getMinFrequency() + 3, 0);
	unsigned short trxExample = *((*sectors2trxs)[selectedSector].begin());
	
	//At the beginning any frequency of the trxs is valid 
	for (unsigned short i = 0; i < (*sectors2trxs)[selectedSector].size(); i++){// for each trx in the sector 
		unsigned short trxIndex = (*sectors2trxs)[selectedSector][i];
		for (unsigned short j = 0; j < (*trxs)[trxIndex].validFrequencies->size(); j++){// add each valid frequency 
			possibleFrequencies[0].insert((*(*trxs)[trxIndex].validFrequencies)[j]);
		}
	}

	// get the cost associated to each frequency
	getAssignmentCost(trxExample, frequencyCosts);
	
	//A frequency for each trx in the selected sector must be assigned
	unsigned short frequenciesToSelect = (*sectors2trxs)[selectedSector].size();
	
	unsigned short actual = 0;
	while(actual < selectedFrequencies.size()){ //Until each option has been explored
		double minValue = DBL_MAX;
		unsigned short minFreq;
		if (selectedFrequencies[actual].size() == frequenciesToSelect){//all trxs assigned for this option
			actual++;//explore next option
			continue;
		}
		for (set<unsigned short>::iterator iter = possibleFrequencies[actual].begin(); iter != possibleFrequencies[actual].end(); iter++){//search the mimimum cost 
			if (frequencyCosts[*iter - (problem->getMinFrequency()-1)] < minValue){
				minValue = frequencyCosts[*iter - (problem->getMinFrequency()-1)]; 
				minFreq = *iter;
			}
		}
		if ((possibleFrequencies[actual].count(minFreq+1)) && (possibleFrequencies[actual].count(minFreq-1)) && (selectedFrequencies[actual].size() <= frequenciesToSelect - 2)){
			//do not use the best frequency, but the 2 adjacent frequencies
			//search one trx that support an adjacent frequency

			//Copy current status to the next
			selectedFrequencies.push_back(selectedFrequencies[actual]);
			notAssignedTrxs.push_back(notAssignedTrxs[actual]);
			assignedTrxs.push_back(assignedTrxs[actual]);
			possibleFrequencies.push_back(possibleFrequencies[actual]);
			
			if (!assignTrxToFrequency(notAssignedTrxs[notAssignedTrxs.size()-1], minFreq-1, possibleFrequencies[possibleFrequencies.size()-1], assignedTrxs[assignedTrxs.size()-1])){//Assign the first: should never fail
				cerr << "A frequency could not be assigned: internal error" << endl;
				exit(-1);
			}
			if (assignTrxToFrequency(notAssignedTrxs[notAssignedTrxs.size()-1], minFreq+1, possibleFrequencies[possibleFrequencies.size()-1], assignedTrxs[assignedTrxs.size()-1])){//Assign the second: correct
				selectedFrequencies[selectedFrequencies.size()-1].push_back(minFreq-1);
				selectedFrequencies[selectedFrequencies.size()-1].push_back(minFreq+1);
				possibleFrequencies[possibleFrequencies.size()-1].erase(minFreq+1);
				possibleFrequencies[possibleFrequencies.size()-1].erase(minFreq-1);
				possibleFrequencies[possibleFrequencies.size()-1].erase(minFreq);
				possibleFrequencies[possibleFrequencies.size()-1].erase(minFreq+2);
				possibleFrequencies[possibleFrequencies.size()-1].erase(minFreq-2);
			} else { //Assign the second: it could fail
				selectedFrequencies.pop_back();
				notAssignedTrxs.pop_back();
				assignedTrxs.pop_back();
				possibleFrequencies.pop_back();
			}
		}
		// Use the best frequency
		if (!assignTrxToFrequency(notAssignedTrxs[actual], minFreq, possibleFrequencies[actual], assignedTrxs[actual])){
			cerr << "A frequency could not be assigned: internal error" << endl;
			exit(-1);
		}
		selectedFrequencies[actual].push_back(minFreq);
		possibleFrequencies[actual].erase(minFreq);
		possibleFrequencies[actual].erase(minFreq+1);
		possibleFrequencies[actual].erase(minFreq-1);
	}
	//select the best option
	double minCost = DBL_MAX;
	unsigned short bestOption = 0;
	for (unsigned short i = 0; i < selectedFrequencies.size(); i++){
		double cost = 0;
		for (vector<unsigned short>::iterator iter = selectedFrequencies[i].begin(); iter != selectedFrequencies[i].end(); iter++){
			cost += frequencyCosts[*iter- (problem->getMinFrequency()-1)];
		}
		if (cost < minCost){
			minCost = cost;
			bestOption = i;
		}
	}
	unsigned short i = 0;
	//make the changes in the frequencyPlan
	for(vector<unsigned short>::iterator freq = selectedFrequencies[bestOption].begin(); freq != selectedFrequencies[bestOption].end(); freq++, i++){
		unsigned short trx = assignedTrxs[bestOption][i];
		setVar(trx, *freq);
		initialFreqs.erase(*freq);
		assignTRX(trx, *freq);
	}
	double finalCost = getObj(0);
	return ((initialFreqs.size() != 0) && (fabs(finalCost - initialCost)> 0.01));
	//Se comprueba que sea mayor que 0.01 para evitar bloqueos por errores numericos
}

//assign the frequency to one of the trx, and decrease the possibleFrequencies
bool AFP_memory::assignTrxToFrequency(set<unsigned short> &notAssignedTrxs, unsigned short freq, multiset<unsigned short> &possibleFrequencies, vector<unsigned short> &assignedTrxs){
	for (set<unsigned short>::iterator i = notAssignedTrxs.begin(); i != notAssignedTrxs.end(); i++){ //Entre los trxs no asignados
		if ((*trxs)[*i].validFrequenciesSet->count(freq)){ // Buscamos uno que pueda usar la frecuencia
			assignedTrxs.push_back(*i);
			for (vector<unsigned short>::iterator j = (*trxs)[*i].validFrequencies->begin(); j != (*trxs)[*i].validFrequencies->end(); j++){
				//Borramos del multiset de frecuencias posibles
				multiset<unsigned short>::iterator elementToErase = possibleFrequencies.find(*j);
				if (elementToErase != possibleFrequencies.end())
					possibleFrequencies.erase(elementToErase);
			}
			notAssignedTrxs.erase(i);
			return true;
		} 
	}
	return false;
}

//assign a frequency to a trx
void AFP_memory::assignTRX(unsigned short trx, unsigned short freq){
	int dif;
	for (vector<unsigned short>::iterator it = (*directInfluencedTrxs)[trx].begin(); it != (*directInfluencedTrxs)[trx].end(); it++){
		int cachePos = (*victimOrInterfererTrxCost)[MENOR(trx, *it)][MAYOR(trx, *it)];
		if ((unsigned short)getVar(*it) == freq){
			setObj(0, getObj(0) + (*coCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
			setCacheData(cachePos, (*coCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
		} else {
			dif = (unsigned short)getVar(*it) - freq;
			if ((dif == 1) || (dif == -1)){
				setObj(0, getObj(0) + (*adjCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
				setCacheData(cachePos, (*adjCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID]);
			} else {
				setCacheData(cachePos, 0);
			}
		}
	}
	setVar(trx, freq);
}

//remove the assignment of frequency to a trx
void AFP_memory::removeTrx(unsigned short trx) {
	for (vector<unsigned short>::iterator it = (*directInfluencedTrxs)[trx].begin(); it != (*directInfluencedTrxs)[trx].end(); it++){
		int cachePos = (*victimOrInterfererTrxCost)[MENOR(trx, *it)][MAYOR(trx, *it)];
		setObj(0, getObj(0) - getCacheData(cachePos));
		setCacheData(cachePos, 0);
	}
	setVar(trx, 0);
}

//get the cost of assigning the different frequencies to a trx
void AFP_memory::getAssignmentCost(unsigned short trx, vector<double> &assignmentCosts) const{
	for (vector<unsigned short>::iterator it = (*directInfluencedTrxs)[trx].begin(); it != (*directInfluencedTrxs)[trx].end(); it++){
		if ((unsigned short)getVar(*it) == 0)
			continue;
		assignmentCosts[(unsigned short)getVar(*it) - (problem->getMinFrequency()-1)] += (*coCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID];
		assignmentCosts[(unsigned short)getVar(*it)+1 - (problem->getMinFrequency()-1)] += (*adjCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID];
		assignmentCosts[(unsigned short)getVar(*it)-1 - (problem->getMinFrequency()-1)] += (*adjCostBothSect)[(*trxs)[trx].sectorID][(*trxs)[*it].sectorID];
	}
}

//print a solution: assignment of frequencies to each trx and fitness: calcule the fitness with the original code
void AFP_memory::print(ostream &os) const {
	// When printing evaluate with the reference code, because usually there is a small difference (< 0.001) between them
	if (!origCodeLoaded){
		loadOptions((problem->getBaseFile() + ".opt.txt").c_str());
		loadTrxs((problem->getBaseFile() + ".trx.txt").c_str()); 
		loadSectors((problem->getBaseFile() + ".sector.txt").c_str());
		loadIM((problem->getBaseFile() + ".IM.txt").c_str());
		loadSites((problem->getBaseFile() + ".site.txt").c_str());
		loadNeighbors((problem->getBaseFile() + ".1-hop.neighbors.txt").c_str());
		loadSecondNeighbors((problem->getBaseFile() + ".2-hop.neighbors.txt").c_str());
		origCodeLoaded = true;
	}

	IndividualAFP ind;
	ind.plan = new vector<unsigned short>(trxs->size());
	ind.planCC = new vector<double>(trxs->size());

	for (unsigned short i = 0; i < problem->getTrxSize(); i++){
		(*ind.plan)[i] = (unsigned short)getVar(i);
	}
	double fitness2 = interferenceBasedFitness(&ind);
	delete(ind.plan);
	delete(ind.planCC);
	for (unsigned short i = 0; i < problem->getTrxSize(); i++){
		os << (unsigned short)getVar(i) << " ";
	}
	os << getObj(0) << " ";
	os << fitness2 << endl;
}

// Add a value to the cache table
void AFP_memory::addVictimInterfererCostValue(int index, double cost){
	setCacheData(index, getCacheData(index) + cost);
}

// Print intialization error
void AFP_memory::initialization_error(){
	cout << "Incorrect AFP initalization: instance_file mutationType mutationParams" << endl;
	cout << "Mutation types:" << endl;
	cout << "  " << MUTATION_UNIFORM << ": Mutation_Uniform - No parameters" << endl;
	cout << "  " << MUTATION_RENAME << ": Mutation_Rename - No parameters" << endl;
	cout << "  " << MUTATION_NEIGHBOURS << ": Mutation_Neighbours - Parameters: maxMutated" << endl;
}

// Evaluate
// It doesn't make anything because it is auto-evaluated each time
// it is changed, i.e. the operators automatically change the objective
// value
void AFP_memory::evaluate(){
}

//Static members of AFP class
AFPProblem *AFP_memory::problem = NULL;
vector<unsigned short> *AFP_memory::dependentSectors;
vector<unsigned short> *AFP_memory::independentSectors;
vector< vector<unsigned short> > *AFP_memory::sectors2trxs;
vector<TRX> *AFP_memory::trxs;
vector< vector <double> > *AFP_memory::adjCostBothSect;
vector< vector <double> > *AFP_memory::coCostBothSect;
vector<map <unsigned short, int> > *AFP_memory::victimOrInterfererTrxCost;
vector< vector <unsigned short> > *AFP_memory::sectorsInterfereOrVictim;
vector< vector <unsigned short> > *AFP_memory::directInfluencedTrxs;
bool AFP_memory::origCodeLoaded = false;
unsigned short AFP_memory::firstInitialization = 1;



/*************************************************************************************
 * AFPProblem Members
 *************************************************************************************/

//Constructor: load each associated file
AFPProblem::AFPProblem(const string &baseFile){
	minFrequency = USHRT_MAX;
	maxFrequency = 0;
	this->baseFile = baseFile;
	loadOptions(baseFile + ".opt.txt");
	loadTrxs(baseFile + ".trx.txt"); 
	loadSectors(baseFile + ".sector.txt");
	loadIM(baseFile + ".IM.txt");
	loadSites(baseFile + ".site.txt");
	loadNeighbors(baseFile + ".1-hop.neighbors.txt");
	loadSecondNeighbors(baseFile + ".2-hop.neighbors.txt");
	cout << "Generando cost trx matrix" << endl << flush;
	generateCostTrxMatrix();
	cout << "fillCostIndexes" << endl << flush;
	fillCostIndexes();
	cout << "Analyze network" << endl << flush;
	analyzeNetwork();
}

// Detect all possible interferences and assign a cache index to each one
void AFPProblem::fillCostIndexes(){
	indexesInTable = 0;
	victimOrInterfererTrxCost.insert(victimOrInterfererTrxCost.begin(), trxs.size(), map<unsigned short, int>());
	
	long metidos = 0;
	for (unsigned short trx = 0; trx < trxs.size(); trx++){
		directInfluencedTrxs.push_back(vector<unsigned short>());
		//Trxs in the sector
		for (unsigned short i = 0; i < (sectors2trxs)[(trxs)[trx].sectorID].size(); i++){
			if ((sectors2trxs)[(trxs)[trx].sectorID][i] != trx){
				directInfluencedTrxs[trx].push_back((sectors2trxs)[(trxs)[trx].sectorID][i]);
				metidos++;
			}
		}
	}

	//between different sectors
	for (map<unsigned short, map<unsigned short, pair<double,double> > >::iterator iteri = interferenceMatrix.begin(); iteri != interferenceMatrix.end(); iteri++){
		unsigned short victimSector = iteri->first;
		if (sectors2trxs[victimSector].size() == 0)
			continue;
		for (map<unsigned short, pair<double,double> >::iterator iterj = iteri->second.begin(); iterj != iteri->second.end(); iterj++){
			unsigned short interfererSector = iterj->first;
			if (sectors2trxs[interfererSector].size() == 0)
				continue;
			for (vector<unsigned short>::iterator iterk = sectors2trxs[victimSector].begin(); iterk != sectors2trxs[victimSector].end(); iterk++){
				unsigned short victimTRX = *iterk;
				for (vector<unsigned short>::iterator iterl = sectors2trxs[interfererSector].begin(); iterl != sectors2trxs[interfererSector].end(); iterl++){
					unsigned short interfererTRX = *iterl;
					if (victimOrInterfererTrxCost[MENOR(victimTRX, interfererTRX)].count(MAYOR(victimTRX, interfererTRX)) == 0){
						victimOrInterfererTrxCost[MENOR(victimTRX, interfererTRX)][MAYOR(victimTRX,interfererTRX)] = indexesInTable;
						indexesInTable++;
						directInfluencedTrxs[victimTRX].push_back(interfererTRX);
						directInfluencedTrxs[interfererTRX].push_back(victimTRX);
						metidos += 2;
					}
				}
			}
		}
	}
	//con los del propio sector
	for (unsigned short victimTRX = 0; victimTRX < trxs.size(); victimTRX++){//para cada TRX
		for (vector<unsigned short>::iterator interfererTRX = sectors2trxs[trxs[victimTRX].sectorID].begin(); interfererTRX != sectors2trxs[trxs[victimTRX].sectorID].end(); interfererTRX++){//trx del sector afectado
			if (*interfererTRX == (unsigned short)victimTRX)
				continue;
			if (victimOrInterfererTrxCost[MENOR(victimTRX, *interfererTRX)].count(MAYOR(victimTRX, *interfererTRX)) == 0){
				victimOrInterfererTrxCost[MENOR(victimTRX, *interfererTRX)][MAYOR(victimTRX, *interfererTRX)] = indexesInTable;
				indexesInTable++;
			}
		}
	}
	cout << "Indices: " << indexesInTable << endl;
	cout << "Metidos: " << metidos << endl;
}



void AFPProblem::completeEvaluation(AFP_memory *ind){
	unsigned short i;
	double cost = 0;
  //compute cost between different sectors Method1
	for (map<unsigned short, map<unsigned short, pair<double,double> > >::iterator victim = interferenceMatrix.begin(); victim != interferenceMatrix.end(); victim++ ) { //Each victim sector
		for (map<unsigned short, pair<double,double> >::iterator interferer = (*victim).second.begin(); interferer != (*victim).second.end(); interferer++) { //Each interferer sector
			unsigned short victimSector = (*victim).first;
			unsigned short interfererSector = (*interferer).first;
			for(vector<unsigned short>::iterator victimTrx = sectors2trxs[victimSector].begin(); victimTrx != sectors2trxs[victimSector].end(); victimTrx ++) { //Each trx in the victim sector
				for (vector<unsigned short>::iterator interfererTrx = sectors2trxs[interfererSector].begin(); interfererTrx != sectors2trxs[interfererSector].end(); interfererTrx ++) { //Each trx in the interferer sector
					if ((*victimTrx < *interfererTrx) || (interferenceMatrix[interfererSector].count(victimSector) == 0)){
						int diff = (unsigned short)ind->getVar(*victimTrx) - (unsigned short)ind->getVar(*interfererTrx);
						if (abs(diff) == 1){
							ind->addVictimInterfererCostValue(victimOrInterfererTrxCost[MENOR(*victimTrx, *interfererTrx)][MAYOR(*victimTrx, *interfererTrx)], (*adjCostBothSect)[victimSector][interfererSector]); 
							cost += (*adjCostBothSect)[victimSector][interfererSector];
						} else if (diff == 0){
							ind->addVictimInterfererCostValue(victimOrInterfererTrxCost[MENOR(*victimTrx, *interfererTrx)][MAYOR(*victimTrx, *interfererTrx)], (*coCostBothSect)[victimSector][interfererSector]); 
							cost += (*coCostBothSect)[victimSector][interfererSector];
						}
					}
				}
		  } 
    } 
  }
	
	//Costs inside sectors
	i = 0;
	for (unsigned short i = 0; i < trxs.size(); i++){//foreach TRX
		for (vector<unsigned short>::iterator iterk = sectors2trxs[trxs[i].sectorID].begin(); iterk != sectors2trxs[trxs[i].sectorID].end(); iterk++){//trxs in the sector 
			if (i < *iterk){
				unsigned short freqi = (unsigned short)ind->getVar(i);
				if (((freqi) == (unsigned short)(ind->getVar(*iterk))) ){
					cost += (*coCostBothSect)[trxs[i].sectorID][trxs[i].sectorID];
					ind->addVictimInterfererCostValue(victimOrInterfererTrxCost[MENOR(i, *iterk)][MAYOR(i, *iterk)], (*coCostBothSect)[trxs[i].sectorID][trxs[i].sectorID]);
				} else if (abs( freqi - (short)ind->getVar(*iterk)) == 1){
					cost += (*adjCostBothSect)[trxs[i].sectorID][trxs[i].sectorID];
					ind->addVictimInterfererCostValue(victimOrInterfererTrxCost[MENOR(i, *iterk)][MAYOR(i, *iterk)], (*adjCostBothSect)[trxs[i].sectorID][trxs[i].sectorID]);
				}
			}
		}
	}
	ind->setObj(0, cost);
}

void AFPProblem::generateCostTrxMatrix(){
	coCostBothSect = new vector< vector <double> > (sectors2trxs.size(), vector<double>(sectors2trxs.size(), 0));
	adjCostBothSect = new vector< vector <double> > (sectors2trxs.size(), vector<double>(sectors2trxs.size(), 0));

	double coInterferenceValue;
	double adjInterferenceValue;
	//interferences between different sectors
  for (map<unsigned short, map<unsigned short, pair<double,double> > >::iterator victim = interferenceMatrix.begin(); victim != interferenceMatrix.end(); victim++ ) { //for each victim sector
    for (map<unsigned short, pair<double,double> >::iterator interferer = (*victim).second.begin(); interferer != (*victim).second.end(); interferer++) { //for each interferer sectors
			unsigned short victimSector = victim->first;
			unsigned short interfererSector = interferer->first;
			computeDifferentSectorsCost(interferer->second.first, interferer->second.second, coInterferenceValue, adjInterferenceValue);
			(*coCostBothSect)[victimSector][interfererSector] += coInterferenceValue;
			(*coCostBothSect)[interfererSector][victimSector] += coInterferenceValue;
			(*adjCostBothSect)[victimSector][interfererSector] += adjInterferenceValue;
			(*adjCostBothSect)[interfererSector][victimSector] += adjInterferenceValue;
    }   
  } 

	cout << "Rellenando interferencias dentro del mismo sector" << endl << flush;
	//interferences inside a sector
	for(unsigned short sector = 0; sector < sectors2trxs.size(); sector++){
		//OJO: el codigo supone que las interferencias entre trxs dentro de un mismo sector son todas iguales, de todas
		//formas por si se intenta usar una instancia que no cumpla esto, se comprueba y en caso que no sea asi se da un error
		//y sale avisando!
		bool first = true;
		for (vector<unsigned short>::iterator victimTRX = sectors2trxs[sector].begin(); victimTRX != sectors2trxs[sector].end(); victimTRX++){
			for (vector<unsigned short>::iterator interfererTRX = sectors2trxs[sector].begin(); interfererTRX != sectors2trxs[sector].end(); interfererTRX++){
				if (*victimTRX != *interfererTRX){
					double co, adj;
					computeSameSectorCost(*victimTRX, *interfererTRX, co, adj);
					if (first){
						(*coCostBothSect)[sector][sector] += co;
						(*coCostBothSect)[sector][sector] += co;
						(*adjCostBothSect)[sector][sector] += adj;
						(*adjCostBothSect)[sector][sector] += adj;
						first = false;
					} else {
						double tmp1, tmp2;
						if ((co + co != (*coCostBothSect)[sector][sector]) || (adj + adj != (*adjCostBothSect)[sector][sector])){
							cout << "Error: codigo no soporta valores distintos en interferencias dentro de un mismo sector" << endl << flush;
							exit(-1);
						}
					}
				} 
			}
		}
	}
}

void AFPProblem::computeSameSectorCost(unsigned short victimTrx, unsigned short interfererTrx, double &coCost, double &adjCost){
	if ((trxs[victimTrx].type == "BCCH") || (trxs[interfererTrx].type == "BCCH")){
		coCost = sameSectorCoChBcch;
		adjCost = sameSectorAdjChBcch;
	} else{
		coCost = sameSectorCoChTch;
		adjCost = sameSectorAdjChTch;
	}
}

void AFPProblem::computeDifferentSectorsCost(double mean, double std, double &coCost, double &adjCost){
	coCost = 100.0*(1.0 -  0.5*erfcc((signallingThreshold - mean)/ (std*sqrt(2.0f))));
	adjCost = 100.0*(1.0 - 0.5*erfcc((signallingThreshold - mean - adjChannelRejection)/(std*sqrt(2.0f))));
}


/*
 * Detect the sets of dependent TRXs
 */
/*void AFPProblem::analyzeNetwork(){
	vector< pair<int,int> > usedRange;
	vector<int> sector2group(getSectorsSize(), 0);
	vector< vector<int> > groups(1, vector<int>());
	set<int> groupIds;
	int actualGroup = 1;
	for (int i = sortCosts.size() - 1; i >= 0; i--){
		if (sector2group[sortCosts[i].getVictim()]){
			if (sector2group[sortCosts[i].getInterferer()]){//both have group: join
				int groupSmall = sector2group[sortCosts[i].getVictim()];
				int groupBig = sector2group[sortCosts[i].getInterferer()];
				if (groupSmall != groupBig){
					if (groups[groupSmall].size() >= groups[groupBig].size()){
						int tmp = groupBig;
						groupBig = groupSmall;
						groupSmall = tmp;
					}
					for (unsigned int j = 0; j < groups[groupSmall].size(); j++){
						groups[groupBig].push_back(groups[groupSmall][j]);
						sector2group[groups[groupSmall][j]] = groupBig;
					}
					groups[groupSmall].clear();
					groupIds.erase(groupSmall);
				}
			} else {//only one have group
				sector2group[sortCosts[i].getInterferer()] = sector2group[sortCosts[i].getVictim()];
				groups[sector2group[sortCosts[i].getInterferer()]].push_back(sortCosts[i].getInterferer());
			}
		} else {
			if (sector2group[sortCosts[i].getInterferer()]){//only one have group
				sector2group[sortCosts[i].getVictim()] = sector2group[sortCosts[i].getInterferer()];
				groups[sector2group[sortCosts[i].getVictim()]].push_back(sortCosts[i].getVictim());
			} else {//both have not group
				sector2group[sortCosts[i].getInterferer()] = actualGroup;
				sector2group[sortCosts[i].getVictim()] = actualGroup;
				groups.push_back(vector<int>());
				groups[actualGroup].push_back(sortCosts[i].getInterferer());
				groups[actualGroup].push_back(sortCosts[i].getVictim());
				groupIds.insert(actualGroup);
				actualGroup++;
			}
		}
	}
	map<int, int> group2finalGroup;
	unsigned int i = 0;
	for (set<int>::iterator iteri = groupIds.begin(); iteri != groupIds.end(); iteri++){
		group2finalGroup[*iteri] = i;
		i++;
	}
	//store the groups
	relatedSectors.insert(relatedSectors.begin(), groupIds.size(), vector<int>());
	for (i = 0; i < sector2group.size(); i++){
		if (sectors2trxs[i].size() != 0){
			if (sector2group[i] == 0){
				independentSectors.push_back(i);
			} else {
				relatedSectors[group2finalGroup[sector2group[i]]].push_back(i);
			}
		}
	}
	for (unsigned int i = 0; i < relatedSectors.size(); i++){
		for (unsigned int j = 0; j < relatedSectors[i].size(); j++){
			dependentSectors.push_back(relatedSectors[i][j]);
		}
	}
}*/

// No analiza, usa todos los sectores siempre
void AFPProblem::analyzeNetwork(){
	for (unsigned short i = 0; i < sectors2trxs.size(); i++){
		dependentSectors.push_back(i);
	}
}

// Load project options from file
void AFPProblem::loadOptions(const string &filename) {
	ifstream file(filename.c_str(),ios_base::in);
  
	if (!file.good()) {
		cout << "Error loading file " << filename << endl;
		exit(-1);
	} 
  
	//get the first line
	char line[256];
	file.getline(line,256);
  
	string key;
  
	//read the first key
	file >> key;
	while (!file.eof()) {
		if (key == "Sector_Co_Ch_TCH") 
			file >> sameSectorCoChTch;
		else if (key == "Sector_Co_Ch_BCCH")
			file >> sameSectorCoChBcch;
		else if (key == "Sector_Adj_Ch_TCH")
			file >> sameSectorAdjChTch;
		else if (key == "Sector_Adj_Ch_BCCH")
			file >> sameSectorAdjChBcch;
		else if (key == "Sector_Ch_Sep")
			file >> sameSectorChSep;
		else if (key == "Site_Co_Ch_BCCH_BCCH")
			file >> sameSiteCoBcchBcch;
		else if (key == "Site_Co_Ch_BCCH_TCH")
			file >> sameSiteCoBcchTch;
		else if (key == "Site_Co_Ch_TCH_TCH")
			file >> sameSiteCoTchTch;
		else if (key == "Site_Adj_Ch_BCCH_BCCH")
			file >> sameSiteAdjBcchBcch;
		else if (key == "Site_Adj_Ch_BCCH_TCH")
			file >> sameSiteAdjBcchTch;
		else if (key == "Site_Adj_Ch_TCH_TCH")
			file >> sameSiteAdjTchTch;
		else if (key == "Site_Ch_Sep")
			file >> sameSiteChSep;
		else if (key == "Nbr_Co_Ch_BCCH_BCCH")
			file >> nbrCoBcchBcch;
		else if (key == "Nbr_Co_Ch_BCCH_TCH")
			file >> nbrCoBcchTch;
		else if (key == "Nbr_Co_Ch_TCH_TCH")
			file >> nbrCoTchTch;
		else if (key == "Nbr_Adj_Ch_BCCH_BCCH")
			file >> nbrAdjBcchBcch;
		else if (key == "Nbr_Adj_Ch_BCCH_TCH")
			file >> nbrAdjBcchTch;
		else if (key == "Nbr_Adj_Ch_TCH_TCH")
			file >> nbrAdjTchTch;
		else if (key == "Nbr2_Co_Ch_BCCH_BCCH")
			file >> nbr2CoBcchBcch;
		else if (key == "Nbr2_Co_Ch_BCCH_TCH")
			file >> nbr2CoBcchTch;
		else if (key == "Nbr2_Co_Ch_TCH_TCH")
			file >> nbr2CoTchTch;
		else if (key == "Adjacent_Channel_Interference_Rejection")
			file >> adjChannelRejection;
		else if (key == "Signalling_Threshold")
			file >> signallingThreshold;
		else {
			cout << "Wrong key " << key << endl;
			exit(-1);
		} 
		file >> key;
	} //while  
	file.close();
};

// Load TRXs information stored in filename
void AFPProblem::loadTrxs(const string &filename){
	ifstream file(filename.c_str(),ios_base::in);
  
	if (!file.good()) {
		cout << "Error loading file " << filename << endl;
		exit(-1);
	}
  
	char line[256];
	file.getline(line,256);

	//create TRX
	TRX * t = new TRX();
	//read trx ID
	file >> t->trxID;
  
	while (!file.eof()) {
		//read type
		file >> t->type;
		//read sector ID
 		file >> t->sectorID;
		//read site ID
		file >> t->siteID;
		//read valid frequencies
		unsigned short numberOfFrequencies;
		unsigned short frequency;
		file >> numberOfFrequencies;
		vector<unsigned short> * freq = new vector<unsigned short>();
		set<unsigned short> * freqSet = new set<unsigned short>();
		for (unsigned short i = 0; i < numberOfFrequencies; i++) {
			file >> frequency;
			freq->push_back(frequency);
			freqSet->insert(frequency);
			if (frequency > maxFrequency)
				maxFrequency = frequency;
			if (frequency < minFrequency)
				minFrequency = frequency;
		}  
		t->validFrequencies = freq;
		t->validFrequenciesSet = freqSet;
		//store TRX
		trxs.push_back(*t);
		t = new TRX();
		file >> t->trxID;
	}
	delete(t);
	file.close();
};   

// Load sector information from file
void AFPProblem::loadSectors(const string &filename) {
	ifstream file(filename.c_str(),ios_base::in);
  	
	highestSector = 0;
	if (!file.good()) {
		cout << "Error loading file " << filename << endl;
		exit(-1);
	}
  
	//get the first line
	char line[256];
	file.getline(line,256);
  
	unsigned short sector;
	int constraint;
	file >> sector;

	while (!file.eof()) {
		if (sector > highestSector)
			highestSector = sector;
		while(sectors2trxs.size() <= (unsigned short)sector)
			sectors2trxs.push_back(vector<unsigned short>());
		//get the constraint value for the sector
		file >> constraint;
		sectorConstraintSep[sector] = constraint;
    
		//read the list of trxs
		unsigned short numberOfTrxs, trx;
		file >> numberOfTrxs;
		for (unsigned short i = 0; i < numberOfTrxs; i++) {
			file >> trx;
			sectors2trxs[sector].push_back(trx);
		} 
    
		//read new sector
		file >> sector;
	}  
	file.close();
};


// Load interference matrix from file
void AFPProblem::loadIM(const string &filename) {
	ifstream file(filename.c_str(),ios_base::in);
  
	if (!file.good()) {
		cout << "Error loading file " << filename << endl;
		exit(-1);
	} 
  
	vector< set <unsigned short> > sectorsInterfereOrVictimSetTmp;
	sectorsInterfereOrVictimSetTmp.insert(sectorsInterfereOrVictimSetTmp.begin(), sectors2trxs.size(), set<unsigned short>());

	char line[256];
	unsigned short victim;
	unsigned short interferer;
	double mean, std;
	file.getline(line,256);
  
	//read victim
	file >> victim;
	unsigned short previousVictim = victim;
	map<unsigned short, pair<double,double> > * node = new map<unsigned short, pair<double,double> >();
 
	while (!file.eof()) {
		//read interferer
		file >> interferer;
		sectorsInterfereOrVictimSetTmp[interferer].insert(victim);
		sectorsInterfereOrVictimSetTmp[victim].insert(interferer);
		//read mean and std
		file >> mean;
		file >> std;
    
		(*node)[interferer] = pair<double,double>(mean,std);

		//read another victim
		previousVictim = victim;
		file >> victim;
 
		if (victim != previousVictim) {
			interferenceMatrix[previousVictim] = *node; 
			delete(node);
			node = new map<unsigned short, pair<double,double> >();
		}  
		else if (file.eof()){
			interferenceMatrix[victim] = *node;
			delete(node);
		}
	}   
	file.close();

	sectorsInterfereOrVictim.insert(sectorsInterfereOrVictim.begin(), sectors2trxs.size(), vector<unsigned short>());
	for (unsigned int i = 0; i < sectorsInterfereOrVictim.size(); i++){
		for (set<unsigned short>::iterator iter = sectorsInterfereOrVictimSetTmp[i].begin(); iter != sectorsInterfereOrVictimSetTmp[i].end(); iter++){
			sectorsInterfereOrVictim[i].push_back(*iter);
		}
	}
};   

// Load sites from file
void AFPProblem::loadSites(const string &filename) {
	//open file
	ifstream file(filename.c_str(),ios_base::in);

	if (!file.good()) {
		cout << "Error loading file " << filename << endl;
		exit(-1);
	} 
  
	//get the first line
	char line[256];
	file.getline(line,256);
  
	unsigned short site, numberOfSectors, sector;
 
	file >> site;
	while (!file.eof()) {
		//read separation constraint
		file >> siteConstraintSep[site] ;
  	
		//read number of sectors
		file >> numberOfSectors;
  	
		//read sectors
		for (unsigned short i = 0; i < numberOfSectors; i++) {
			file >> sector;
			site2sectors[site].push_back(sector);
		} 
		file >> site;
	} 
	file.close();
};

// Load neighbors from file
void AFPProblem::loadNeighbors(const string &filename) {
	//open file
	ifstream file(filename.c_str(),ios_base::in);
	
	if (!file.good()) {
		cout << "Error loading file " << filename << endl;
		exit(-1);
	} 
  
	//get the first line
	char line[256];
	file.getline(line,256);
  
	unsigned short sector, numberOfNeighbors, neighbor;
 
	file >> sector;
	while (!file.eof()) {
		//read neighbors
		file >> numberOfNeighbors;
		for (unsigned short i = 0; i < numberOfNeighbors; i++) {
			file >> neighbor;
			neighbors[sector].push_back(neighbor);
		}
  	
		//read new sector
		file >> sector;
	} 
	file.close();
};

// Load neighbors from file
void AFPProblem::loadSecondNeighbors(const string &filename) {
	//open file
	ifstream file(filename.c_str(),ios_base::in);
	
	if (!file.good()) {
		cout << "Error loading file " << filename << endl;
		exit(-1);
	} //if
  
	//get the first line
	char line[256];
	file.getline(line,256);

	unsigned short sector, numberOfNeighbors, neighbor;
  
	file >> sector;
	while (!file.eof()) {
		//read neighbors
	 	file >> numberOfNeighbors;
	  	for (unsigned short i = 0; i < numberOfNeighbors; i++) {
			file >> neighbor;
	  		secondNeighbors[sector].push_back(neighbor);
 	 	} 
  	
 	 	//read new sector
	  	file >> sector;
	} 
	file.close();
};

// Numerical method for computing function Q
float AFPProblem::erfcc(float x) {
  float t, z, ans;
  
  z = (float)fabs(x);
  t = 1.0F / (1.0F + 0.5F * z);
  ans = (float)(t * exp(-z * z - 1.26551223F + t *
		(1.00002368F + t * 
		 (0.37409196F + t *
		  (0.09678418F + t *
		   (-0.18628806F + t *
		    (0.27886807F + t * 
		     (-1.13520398F + t *
		      (1.48851587F + t * 
		       (-0.82215223F + t * 0.17087277F)
		      )
		     )
		    )
		   )
		  )
		 )
		)
		));
  
  return  x >= 0.0F ? ans : 2.0F - ans;

}; //erfcc

//Static Members of AFPProblem
vector< map <unsigned short, int> > AFPProblem::victimOrInterfererTrxCost;
vector< vector <unsigned short> > AFPProblem::directInfluencedTrxs;
int AFPProblem::indexesInTable;


//Computing methods of Francisco Luna (slightly changed but obtains the same results)
//It is neccesary because the code in the classes has been changed and does not calculate
//exactly the same fitness (usually error < 0.001) but in order to avoid problems, at
//the end the fitness is calculated with this code

/*******************************************************
 * Numerical method for computing function Q
 *******************************************************/
float erfcc(float x) {
  float t, z, ans;
  
  z = (float)fabs(x);
  t = 1.0F / (1.0F + 0.5F * z);
  ans = (float)(t * exp(-z * z - 1.26551223F + t *
		(1.00002368F + t * 
		 (0.37409196F + t *
		  (0.09678418F + t *
		   (-0.18628806F + t *
		    (0.27886807F + t * 
		     (-1.13520398F + t *
		      (1.48851587F + t * 
		       (-0.82215223F + t * 0.17087277F)
		      )
		     )
		    )
		   )
		  )
		 )
		)
		));
  return  x >= 0.0F ? ans : 2.0F - ans;
}; //erfcc

/****************************************************************************
 * Compute interferences between a victim TRX and an interferer TRX
 ****************************************************************************/
double computeChannelInterference(vector<unsigned short> * plan, vector<double> * planningCC, unsigned short victimTrx, unsigned short interfererTrx) {
	double cost = 0.0;
	unsigned short victimFrequency     = plan->at(victimTrx);
	unsigned short interfererFrequency = plan->at(interfererTrx);
	
	
	if (_trxs[victimTrx].sectorID == _trxs[interfererTrx].sectorID) {
	  if (victimFrequency == interfererFrequency)
	    if ((_trxs[victimTrx].type == "BCCH") || (_trxs[interfererTrx].type == "BCCH"))
	      cost += projectParameters.sameSectorCoChBcch;
	    else
	      cost += projectParameters.sameSectorCoChTch;
	  else if (abs(victimFrequency - interfererFrequency) == 1)
	    if ((_trxs[victimTrx].type == "BCCH") || (_trxs[interfererTrx].type == "BCCH"))
	      cost += projectParameters.sameSectorAdjChBcch;
	    else
	      cost += projectParameters.sameSectorAdjChTch; //*/
	 } //if
	 else {
	   double mean = _interferenceMatrix[_trxs[victimTrx].sectorID][_trxs[interfererTrx].sectorID].first;
	   double std  = _interferenceMatrix[_trxs[victimTrx].sectorID][_trxs[interfererTrx].sectorID].second;
	   //only co-channel and adj-channel are considered
  	 if (victimFrequency == interfererFrequency) {
  		 cost += 100.0*(1.0 
  		                - 
  		                0.5*erfcc(
  		                          (projectParameters.signallingThreshold - mean)
  		                          /
  		                          (std*sqrt(2.0f))
  		                         )
  		                );
  	 } //if
  	 else if (abs(victimFrequency - interfererFrequency) == 1) {
  	 	cost += 100.0*(1.0 
  	 	               - 
  	 	               0.5*erfcc(
  	 	                         (projectParameters.signallingThreshold - mean - projectParameters.adjChannelRejection)
  	 	                         /
  	 	                         (std*sqrt(2.0f))
  	 	                        )
  	 	              );
  	 } //if
	 } //else
	
  //update the component cost
  planningCC->at(victimTrx) = cost;
  return cost;
	
}; //computeChanneInterference

/******************************************************************************
 * Compute interferences between a victim sector and an interferer sector
 ******************************************************************************/
double signalingCost(vector<unsigned short> * plan, vector<double> * planCC, unsigned short victimSector, unsigned short interfererSector) {
	double cost = 0.0;
	
	for(vector<unsigned short>::iterator victimTrx = _sectors2trxs[victimSector].begin();
		  victimTrx != _sectors2trxs[victimSector].end();
		  victimTrx ++) {
		  	for (vector<unsigned short>::iterator interfererTrx = _sectors2trxs[interfererSector].begin();
		  		interfererTrx != _sectors2trxs[interfererSector].end();
		  		interfererTrx ++) {
		  			if (*victimTrx != *interfererTrx) 
    				  cost += computeChannelInterference(plan, planCC,*victimTrx,*interfererTrx);
		  		} //for
		  } //for
		  
  return cost;
}; //signalingCost
 
/*****************************************************************************
 * C/I cost
 *****************************************************************************/
 double c2iCost(vector<unsigned short> * plan, vector<double> * planCC) {
 	double cost = 0.0;

  //firstly, add the cost provoked by the interference within each sector
  for (map<unsigned short,vector<unsigned short> >::iterator it = _sectors2trxs.begin(); 
       it != _sectors2trxs.end(); 
       it++) {
    cost += signalingCost(plan,planCC,(*it).first,(*it).first);
  }
  
  //compute sector based cost
  //for each victim
  for (map<unsigned short, map<unsigned short, pair<double,double> > >::iterator victim = _interferenceMatrix.begin();
       victim != _interferenceMatrix.end();
       victim++ ) {
        
    //traverse all the interferer sectors
    for (map<unsigned short, pair<double,double> >::iterator interferer = (*victim).second.begin();
       interferer != (*victim).second.end();
       interferer++) {
       //cost for all the interferences between TRXs
       cost += signalingCost(plan, planCC, (*victim).first, (*interferer).first);
    } //for  
  } //for
  return cost;
 }; //c2iCost

/*****************************************************************************
 * Load interference matrix from file
 *****************************************************************************/
void loadIM(const char * filename) {
  ifstream file(filename,ios_base::in);
  
  if (!file.good()) {
    cout << "Error loading file " << filename << endl;
    exit(-1);
  } //if
  
  char line[256];
  unsigned short victim;
  unsigned short interferer;
  double mean, std;
  file.getline(line,256);
  
  //read victim
  file >> victim;
  unsigned short previousVictim = victim;
  map<unsigned short, pair<double,double> > * node = new map<unsigned short, pair<double,double> >();
  
  while (!file.eof()) {
    //read interferer
    file >> interferer;
    _interfereTo[interferer].push_back(victim);
    
    //read mean and std
    file >> mean;
    file >> std;
    
    //cout << victim << "," << interferer << "->" << mean << "," << std << endl;
    
    (*node)[interferer] = pair<double,double>(mean,std);
   
    //read another victim
    previousVictim = victim;
    file >> victim;
    
    if (victim != previousVictim) {
      _interferenceMatrix[previousVictim] = *node;  
      node = new map<unsigned short, pair<double,double> >();
    } //if  
    else if (file.eof())
      _interferenceMatrix[victim] = *node;
  } //while  
  file.close();
}; //loadIM  

/*****************************************************************************
 * Load sector information from file
 *****************************************************************************/
void loadSectors(const char * filename) {
  ifstream file(filename,ios_base::in);
  
  if (!file.good()) {
    cout << "Error loading file " << filename << endl;
    exit(-1);
  } //if
  
  //get the first line
  char line[256];
  file.getline(line,256);
  
  unsigned short sector;
  unsigned short constraint;
  file >> sector;
  
  while (!file.eof()) {
  	//get the constraint value for the sector
  	file >> constraint;
    _sectorConstraintSep[sector] = constraint;
    
    //read the list of trxs
   unsigned short numberOfTrxs, trx;
    file >> numberOfTrxs;
    for (unsigned short i = 0; i < numberOfTrxs; i++) {
    	file >> trx;
    	_sectors2trxs[sector].push_back(trx);
    } //for
    
    //read new sector
    file >> sector;
  } //while  
  file.close();
 
}; //loadSectors

/*****************************************************************************
 * Load sites from file
 *****************************************************************************/
void loadSites(const char * filename) {
	//open file
	ifstream file(filename,ios_base::in);
	
	if (!file.good()) {
    cout << "Error loading file " << filename << endl;
    exit(-1);
  } //if
  
  //get the first line
  char line[256];
  file.getline(line,256);
  
  unsigned short site, numberOfSectors, sector;
  
  file >> site;
  while (!file.eof()) {
  	//read separation constraint
  	file >> _siteConstraintSep[site] ;
  	
  	//read number of sectors
  	file >> numberOfSectors;
  	
  	//read sectors
  	for (unsigned short i = 0; i < numberOfSectors; i++) {
  		file >> sector;
  		_site2sectors[site].push_back(sector);
  	} //for
  	
  	file >> site;
  } //while
  file.close();
  
  
}; //loadSites

/*****************************************************************************
 * Load neighbors from file
 *****************************************************************************/
void loadNeighbors(const char * filename) {
	//open file
	ifstream file(filename,ios_base::in);
	
	if (!file.good()) {
    cout << "Error loading file " << filename << endl;
    exit(-1);
  } //if
  
  //get the first line
  char line[256];
  file.getline(line,256);
  
  unsigned short sector, numberOfNeighbors, neighbor;
  
  file >> sector;
  while (!file.eof()) {
  	//read neighbors
  	file >> numberOfNeighbors;
  	for (unsigned short i = 0; i < numberOfNeighbors; i++) {
  		file >> neighbor;
  		_neighbors[sector].push_back(neighbor);
  	} //for
  	
  	//read new sector
  	file >> sector;
  } //while
  file.close();
  
}; //loadNeighbors

/*****************************************************************************
 * Load neighbors from file
 *****************************************************************************/
void loadSecondNeighbors(const char * filename) {
	//open file
	ifstream file(filename,ios_base::in);
	
	if (!file.good()) {
    cout << "Error loading file " << filename << endl;
    exit(-1);
  } //if
  
  //get the first line
  char line[256];
  file.getline(line,256);
  
  unsigned short sector, numberOfNeighbors, neighbor;
  
  file >> sector;
  while (!file.eof()) {
  	//read neighbors
  	file >> numberOfNeighbors;
  	for (unsigned short i = 0; i < numberOfNeighbors; i++) {
  		file >> neighbor;
  		_secondNeighbors[sector].push_back(neighbor);
  	} //for
  	
  	//read new sector
  	file >> sector;
  } //while
  file.close();
}; //loadSecondNeighbors

// Load project options from file
void loadOptions(const char * filename) {
  ifstream file(filename,ios_base::in);
  
  if (!file.good()) {
    cout << "Error loading file " << filename << endl;
    exit(-1);
  } //if
  
  //get the first line
  char line[256];
  file.getline(line,256);
  
  string key;
  
  //read the first key
  file >> key;
  while (!file.eof()) {
  	if (key == "Sector_Co_Ch_TCH") 
  		file >> projectParameters.sameSectorCoChTch;
  	else if (key == "Sector_Co_Ch_BCCH")
  		file >> projectParameters.sameSectorCoChBcch;
  	else if (key == "Sector_Adj_Ch_TCH")
  		file >> projectParameters.sameSectorAdjChTch;
  	else if (key == "Sector_Adj_Ch_BCCH")
  		file >> projectParameters.sameSectorAdjChBcch;
  	else if (key == "Sector_Ch_Sep")
  		file >> projectParameters.sameSectorChSep;
  	else if (key == "Site_Co_Ch_BCCH_BCCH")
  		file >> projectParameters.sameSiteCoBcchBcch;
  	else if (key == "Site_Co_Ch_BCCH_TCH")
  		file >> projectParameters.sameSiteCoBcchTch;
  	else if (key == "Site_Co_Ch_TCH_TCH")
  		file >> projectParameters.sameSiteCoTchTch;
  	else if (key == "Site_Adj_Ch_BCCH_BCCH")
  		file >> projectParameters.sameSiteAdjBcchBcch;
  	else if (key == "Site_Adj_Ch_BCCH_TCH")
  		file >> projectParameters.sameSiteAdjBcchTch;
  	else if (key == "Site_Adj_Ch_TCH_TCH")
  		file >> projectParameters.sameSiteAdjTchTch;
  	else if (key == "Site_Ch_Sep")
  		file >> projectParameters.sameSiteChSep;
  	else if (key == "Nbr_Co_Ch_BCCH_BCCH")
  		file >> projectParameters.nbrCoBcchBcch;
  	else if (key == "Nbr_Co_Ch_BCCH_TCH")
  		file >> projectParameters.nbrCoBcchTch;
  	else if (key == "Nbr_Co_Ch_TCH_TCH")
  		file >> projectParameters.nbrCoTchTch;
  	else if (key == "Nbr_Adj_Ch_BCCH_BCCH")
  		file >> projectParameters.nbrAdjBcchBcch;
  	else if (key == "Nbr_Adj_Ch_BCCH_TCH")
  		file >> projectParameters.nbrAdjBcchTch;
  	else if (key == "Nbr_Adj_Ch_TCH_TCH")
  		file >> projectParameters.nbrAdjTchTch;
  	else if (key == "Nbr2_Co_Ch_BCCH_BCCH")
  		file >> projectParameters.nbr2CoBcchBcch;
  	else if (key == "Nbr2_Co_Ch_BCCH_TCH")
  		file >> projectParameters.nbr2CoBcchTch;
  	else if (key == "Nbr2_Co_Ch_TCH_TCH")
  		file >> projectParameters.nbr2CoTchTch;
  	else if (key == "Adjacent_Channel_Interference_Rejection")
  		file >> projectParameters.adjChannelRejection;
  	else if (key == "Signalling_Threshold")
  		file >> projectParameters.signallingThreshold;
  	else {
  		cout << "Wrong key " << key << endl;
  		exit(-1);
  	} //else
  	file >> key;
  } //while
  
  file.close();
}; //loadOptions

// Load TRXs information stored in filename
void loadTrxs(const char * filename) {
  ifstream file(filename,ios_base::in);
  
  if (!file.good()) {
    cout << "Error loading file " << filename << endl;
    exit(-1);
  } //if
  
  char line[256];
  file.getline(line,256);

  TRXOriginal * t = new TRXOriginal();
  //read trx ID
  file >> t->trxID;
  
  while (!file.eof()) {

    //read type
    file >> t->type;
    //read sector ID
    file >> t->sectorID;
    //read site ID
    file >> t->siteID;
    //read valid frequencies
    unsigned short numberOfFrequencies;
    unsigned short frequency;
    file >> numberOfFrequencies;
    vector<unsigned short> * freq = new vector<unsigned short>();
    for (unsigned short i = 0; i < numberOfFrequencies; i++) {
      file >> frequency;
      freq->push_back(frequency);
    } //for  
    t->validFrequencies = freq;
    //store TRX
    _trxs.push_back(*t);
    t = new TRXOriginal();
    file >> t->trxID;
  } //while
  file.close();
}; //loadTrxs  



/******************************************************************************
 * Interference based fitness
 ******************************************************************************/
double interferenceBasedFitness(IndividualAFP * solution) {
  return c2iCost(solution->plan,solution->planCC);
}; //interferenceBasedFitness

//variables of original code
vector<TRXOriginal>                       _trxs                ; //transceivers 
map<unsigned short, map<unsigned short, pair<double,double> > > _interferenceMatrix  ; //IM
map<unsigned short, vector<unsigned short> >                    _interfereTo         ; //sectors to which each sector interfere
map<unsigned short, vector<unsigned short> >                    _sectors2trxs        ; //map sectors to trxs
map<int, int>                             _sectorConstraintSep ; //sector constraint separation
map<unsigned short, vector<unsigned short> >                    _site2sectors        ; //map sites to sectors
map<int, int>                             _siteConstraintSep   ; //site constraint separation
map<unsigned short, vector<unsigned short> >                    _neighbors           ; //1-hop neighbors
map<unsigned short, vector<unsigned short> >                    _secondNeighbors     ; //2-hop neighbors

