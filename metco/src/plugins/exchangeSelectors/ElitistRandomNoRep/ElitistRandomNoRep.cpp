/***************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    2008
 ***************************************************************************************/   
 

#include "utils.h"
#include "ElitistRandomNoRep.h"

void ElitistRandomNoRep::exchange(vector<Individual*> &migrated, vector<Individual*> &population){
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
	//Detect dominated individuals
	vector<int> dominated;
	markDominated(population, dominated);
	//Select random dominated individuals
	int copied = 0;
	while((dominated.size() != 0) && (copied < indCopy)){
		int selected = generateRandom(0, dominated.size()-1);
		delete (population)[dominated[selected]];
		(population)[dominated[selected]] = migrated[copied];
		copied++;
		dominated[selected] = dominated[dominated.size()-1];
		dominated.pop_back();
	}
	for (unsigned int i = copied; i < migrated.size(); i++){
		delete(migrated[i]);
	}
}
