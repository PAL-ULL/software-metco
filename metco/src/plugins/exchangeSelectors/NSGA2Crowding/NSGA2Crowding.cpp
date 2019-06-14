/*****************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE 
 *    March 2009
 *****************************************************************************************************/
 
#include <algorithm>
#include "NSGA2Crowding.h"
#include "utils.h"
#include <float.h>

void rankCrowdingPopulation(vector<Individual*> *population, int populationSize) {
	int inserted = 0;

	vector < vector < Individual * > > fronts;
	vector < Individual *> notClassified;

	//Calculate fronts
	int differentsInFront0;
	FastNondominatedSort(population, fronts, notClassified, differentsInFront0, populationSize);

	// Order each front using the local crowding distance 
	if (fronts[fronts.size()-1].size() > 1){
		crowOrder(fronts[fronts.size()-1]);
	}

	//Create the ordered population
	population->clear();
	for (int i = 0; i < fronts.size() - 1; i++){
		for (int j = 0; j < fronts[i].size(); j++){
			population->push_back(fronts[i][j]);
		}
	}
	
	//Last front
	int borrados = 0;
	for (int j = 0; j < fronts[fronts.size() - 1].size(); j++){
		if (population->size() < (unsigned int) populationSize){
			population->push_back((fronts[fronts.size() - 1][j]));
		} else {
			borrados++;
			delete (fronts[fronts.size() - 1][j]);
		}
	}

	//Delete not used individuals
	for (int i = 0; i < notClassified.size(); i++){
		borrados++;
		delete (notClassified[i]);
	}
}

void NSGA2Crowding::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
	int cantidadIguales = 0;
	//Eliminar elementos migrados que ya estan en la poblacion
	for (int i = 0; i < migrated.size(); i++){
		for (int j = 0; j < population.size(); j++){
			bool iguales = true;
			for (int k = 0; k < migrated[i]->getNumberOfObj(); k++){
				if ((migrated[i]->getObj(k)) != (population[j]->getObj(k))){
					iguales = false;
					break;
				}
			}
			if (iguales){
				cantidadIguales++;
				delete migrated[i];
				migrated[i] = migrated[migrated.size() - 1];
				migrated.pop_back();
				i--;
				break;
			}
		}
	}
	for (int i = 0; i < migrated.size(); i++)
		population.push_back(migrated[i]);
	rankCrowdingPopulation(&population, population.size() - migrated.size());
	
//	vector< vector <int> > ranks;
//	calculateRanks(population, ranks);
//	vector<int> indexToRemove;
	//Remove elements from the worst ranks
	/*cout << "Quedaron: " << migrated.size() << endl;
	cout << "En el rango 0 hay: " << ranks[0].size() << endl;
	cout << "Numero de igualdades: " << cantidadIguales << endl;*/
//	for (int i = 0; i < migrated.size(); i++){
		//Select individual to remove
		/*vector<Individual*> tmpPopulation;
		for (int j = 0; j < ranks[ranks.size()-1].size(); j++){
			population[ranks[ranks.size()-1][j]]->setAuxDataSize(1);
			population[ranks[ranks.size()-1][j]]->setAuxData(0, j);
			tmpPopulation.push_back(population[ranks[ranks.size()-1][j]]);
		}
		crowOrder2(tmpPopulation);*/
		//int rnd = generateRandom(0, ranks[ranks.size()-1].size() - 1);
//		int rnd = tmpPopulation[tmpPopulation.size()-1]->getAuxData(0);
//		indexToRemove.push_back(ranks[ranks.size()-1][rnd]);

		//Erase used
//		ranks[ranks.size()-1][rnd] = ranks[ranks.size()-1][ranks[ranks.size()-1].size() - 1];
//		ranks[ranks.size()-1].pop_back();
//		if (ranks[ranks.size()-1].size() == 0)
//			ranks.pop_back();
//	}

	//Erase the worst individuals
//	sort(indexToRemove.begin(), indexToRemove.end());
//	for (int i = indexToRemove.size() - 1; i >= 0; i--){
//		delete (population[indexToRemove[i]]);
//		population[indexToRemove[i]] = population[population.size()-1];
//		population.pop_back();
//	}
}


