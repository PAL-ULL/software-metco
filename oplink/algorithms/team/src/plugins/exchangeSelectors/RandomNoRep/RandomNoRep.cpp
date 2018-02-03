/*********************************************************************************
 * AUTHORS
 *    Carlos Segura Gonzalez
 *
 * DATE
 *    November 2008
 *********************************************************************************/
 
#include "RandomNoRep.h"
#include "utils.h"

void RandomNoRep::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
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
				delete migrated[i];
				migrated[i] = migrated[migrated.size() - 1];
				migrated.pop_back();
				i--;
				break;
			}
		}
	}

	unsigned int indCopy = (migrated.size() < population.size())?migrated.size():population.size();
	for (unsigned int i = 0; i < indCopy; i++){
		int selected = generateRandom(0, population.size()-1);
		delete (population)[selected];
		(population)[selected] = migrated[i];
	}
	for (unsigned int i = indCopy; i < migrated.size(); i++){
		delete(migrated[i]);
	}
}
