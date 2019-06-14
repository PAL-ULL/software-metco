#include "NonDominated.h"
#include "SimpleIndividual.h"
#include "utils.h"

#define ORIGINAL 2
#define SIMPLE 3
void NonDominated::localSearch(vector <Individual*> *p, MOFront *currentSolution){
	double rLS = (double(rand()))/(RAND_MAX + 1.0); //[0, 1)
	if (pLS <= rLS)
		return;
	int nObj = (*p)[0]->getNumberOfObj();
	int desiredSize = p->size();

	//Copy population
	vector<Individual*> *pCurrent = new vector < Individual* >();
	vector<Individual*> *pNext = NULL;
	for (int i = 0; i < p->size(); i++){
		pCurrent->push_back((*p)[i]);
	}

	//Calculate fronts
	vector < vector < Individual* > > fronts;
	vector < Individual * > notClassified;
	int differentsInFront0;
	FastNondominatedSort(pCurrent, fronts, notClassified, differentsInFront0, desiredSize);
	int f0Size = differentsInFront0;
	bool spaceAssigned = false;
	int assignedSpaceId;

	//Assign front to each individual as an internal value
	for (int i = 0; i < fronts.size(); i++){
		for (int j = 0; j < fronts[i].size(); j++){
			int assignedSpaceIdTmp = fronts[i][j]->requestAlgorithmData();
			if (!spaceAssigned){
				spaceAssigned = true;
				assignedSpaceId = assignedSpaceIdTmp;
			} else if (assignedSpaceIdTmp != assignedSpaceId){
				cerr << "Error interno: assignedSpaceId no concuerda" << endl;
			}
			fronts[i][j]->setAlgorithmData(assignedSpaceId, i);//Number of front
			fronts[i][j]->setAlgorithmData(fronts[i][j]->requestAlgorithmData(), ORIGINAL);//Type: Original or Created
			fronts[i][j]->setAlgorithmData(fronts[i][j]->requestAlgorithmData(), 0);//Has been used to generate neighbours?
		}
	}

	bool improved = true;
	int iterations = 0;
	while((improved) && (iterations < maxIterations)){
		iterations++;
		vector <double> objectives(nObj, 0);
		//Generate neighbours
		for (int i = 0; i < desiredSize; i++){
			if ((*pCurrent)[i]->getAlgorithmData(assignedSpaceId + 2) == 0){
				(*pCurrent)[i]->setAlgorithmData(assignedSpaceId + 2, 1);//Used
				for (int j = 0; j < (*pCurrent)[i]->getNeighbourhoodSize(); j++){
					(*pCurrent)[i]->getNeighbourObjectives(j, objectives);
					SimpleIndividual *newNeigh = new SimpleIndividual(0, nObj);
					for (int k = 0; k < objectives.size(); k++){
						newNeigh->setObj(k, objectives[k]);			
						newNeigh->setOptDirection(k, (*pCurrent)[0]->getInternalOptDirection(k));
					}
					if (assignedSpaceId != newNeigh->requestAlgorithmData()){
						cerr << "Error interno: assignedSpaceId no concuerda" << endl;
					}
					newNeigh->setAlgorithmData(assignedSpaceId, -1);//Front
					newNeigh->setAlgorithmData(newNeigh->requestAlgorithmData(), SIMPLE);//Type
					newNeigh->setAlgorithmData(newNeigh->requestAlgorithmData(), 0);//Not Used
					//Informacion de como crear el individuo
					newNeigh->setAuxDataSize(2);
					newNeigh->setAuxData(0, i);
					newNeigh->setAuxData(1, j);
					pCurrent->push_back(newNeigh);
				}
			} 
		}

		//Calculate fronts
		FastNondominatedSort(pCurrent, fronts, notClassified, differentsInFront0, desiredSize);
		//Croworder last front
		crowOrder(fronts[fronts.size()-1]);

		//Check if an individual of the previous front 0, has been dominated
		improved = false;
	
		int newF0Size = (differentsInFront0 > desiredSize)?desiredSize:differentsInFront0;
		if (newF0Size > f0Size){
			improved = true;
		}
		f0Size = newF0Size;
		//Buscamos alguno que antes perteneciera al frente 0 y ahora no
		if (!improved){
			for (int i = 1; i < fronts.size(); i++){
				for (int j = 0; j < fronts[i].size(); j++){
					if (fronts[i][j]->getAlgorithmData(assignedSpaceId) == 0){
						improved = true;
						break;
					}
				}
				if (improved)
					break;
			}
		}
		if (!improved){
			for (int i = 0; i < notClassified.size(); i++){
				if (notClassified[i]->getAlgorithmData(assignedSpaceId) == 0){
					improved = true;
					break;
				}
			}
		}


		//Select desiredSize individuals, assigning its front
		pNext = new vector<Individual*>();
		int inserted = 0;
		for (int i = 0; i < fronts.size(); i++){
			for (int j = 0; j < fronts[i].size(); j++){
				fronts[i][j]->setAlgorithmData(assignedSpaceId, i);
				if (inserted < desiredSize){
					pNext->push_back(fronts[i][j]);
					inserted++;
				} else {
					notClassified.push_back(fronts[i][j]);
				}
			}
		}

		//Convert Individuals of pnext
		for (int i = 0; i < pNext->size(); i++){
			if ((*pNext)[i]->getAlgorithmData(assignedSpaceId + 1) == SIMPLE){
				int indFrom = (*pNext)[i]->getAuxData(0);
				int nei = (*pNext)[i]->getAuxData(1);
				Individual *newInd = (*pCurrent)[indFrom]->getNeighbour(nei);
				if (currentSolution != NULL){
					currentSolution->insert(newInd);
				} 
				newInd->setAlgorithmData(newInd->requestAlgorithmData(), (*pNext)[i]->getAlgorithmData(assignedSpaceId));
				newInd->setAlgorithmData(newInd->requestAlgorithmData(), ORIGINAL);
				newInd->setAlgorithmData(newInd->requestAlgorithmData(), 0);
				delete (*pNext)[i];
				(*pNext)[i] = newInd;		
			}
		}

		//Delete nos used individuals
		for (int i = 0; i < notClassified.size(); i++){
			delete (notClassified[i]);
		}
		delete pCurrent;
		pCurrent = pNext;

	}

	p->clear();
	for (int i = 0; i < pCurrent->size(); i++){
		(*pCurrent)[i]->releaseAlgorithmData();
		(*pCurrent)[i]->releaseAlgorithmData();
		(*pCurrent)[i]->releaseAlgorithmData();
		p->push_back((*pCurrent)[i]);
	}
	delete pCurrent;
}
//Parameters: MaxIterations_per_individual pLS
bool NonDominated::init(const vector<string> &params){
	if (params.size() != 2){
		cout << "Error, NonDominated parameters --> MaxIterations pLS" << endl;
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
