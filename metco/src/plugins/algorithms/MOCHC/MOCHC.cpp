/***********************************************************************************
 * AUTHORS
 *   - Francisco Jose Morales de la Rosa
 * 
 * *********************************************************************************/


#include "MOCHC.h"
#include <float.h>


// Define una generación de búsqueda del algoritmo 
void MOCHC::runGeneration() {

//cout << " entrando en runGeneration " << getGeneration() << endl;
   
   // Evaluate the initial population
   if (getGeneration() == 0) {
//cout << " primera generacion " << endl;
      initialize();
//cout << " despues de initialize " << endl;
   } 
   
   // Seleccion aleatoria de los padres
//cout << " antes de create child pop " << endl;
   createChildPop();
      
   // Seleccion de la nueva generacion
//cout << " antes de rankcrowding " << endl;
   rankCrowdingPopulation();
      
   // Comprobar el umbral de incesto y provocar el reinicio
//cout << " antes de mutation " << endl;
   mutation();
//cout << " fin de rungeneration " << endl;
}


// Inicializa los parámetros iniciales del algoritmo
bool MOCHC::init(const vector<string> &params) {
   
   // Check for the algorithm parameters
   if (params.size() != 5) {
      cerr << "Error MOCHC: Incorrect parameters (pm pSize icc cv pp)" << endl;
      return false;
   }
   
   // Initiate the parameters
   this->pm = atof(params[0].c_str());
   setPopulationSize(atoi(params[1].c_str()));
   this->icc = atof(params[2].c_str());
   this->cv = atof(params[3].c_str());
   this->pp = atof(params[4].c_str());
   return true;
}


// Rellena un frente con las soluciones actuales
void MOCHC::getSolution(MOFront *p) {
   //cerr << "POBLACION" << endl;
   for (unsigned int i = 0;  i < population->size(); i++) {
   //   (*population)[i]->print(cerr);
      p->insert((*population)[i]);
   }
   //cerr << "FIN POBLACION" << endl;
}


// Muestra la información relativa al algoritmo
void MOCHC::printInfo(ostream &os) const {
   os << "Genetic Algorithm MOCHC" << endl;
   os << "Number of Evaluations = " << getEvaluations() << endl;
   os << "Mutation Probability = " << pm << endl;
   os << "Initial Convergence Count = " << icc << endl;
   os << "Convergence Value = " << cv << endl;
   os << "Preserved Population = " << pp << endl;
   os << "Population Size = " << getPopulationSize() << endl;
}


/*****************************************************************************************
 * Los siguientes métodos son específicos del algoritmo MOCHC y describen las operaciones
 * internas y el propio funcionamiento del algoritmo.
 * ***************************************************************************************/

// Se inicializa el valor de la distancia minima
void MOCHC::initialize() {   
   minimunDistance = (icc * (*population)[0]->getNumberOfVar());
//cout << " fin de initialize icc: " << icc << " numberofvar " << (*population)[0]->getNumberOfVar() << " minimundistance: " << minimunDistance << endl;
}




bool MOCHC::compareIndividual (Individual *ind) {
	bool encontrado = false;	

	set<Individual *>::iterator it = populationOld.begin();

	while (!encontrado && (it != populationOld.end())) {
//	for (it=populationOld.begin(); it != myset.end(); it++) {
		int j = 0;
		while (j < ind->getNumberOfVar()) {
			if (ind->getVar(j) != (*it)->getVar(j)) {
				break;
			} 
				
			if (j == (ind->getNumberOfVar()-1)) {
				encontrado = true;
			}
			j++;
		}
		it++;
	}

	return encontrado;	
}

/*************************************************************************
 * Create fronts using the non-domination rank until pSize elements have
 * been selected.
 * Each front is ordered using the local crowding distance.
 * In: population without any order with size = pSize * 2
 * Exit: population ordered using the crowded comparison operator with size pSize
 * ***********************************************************************/
void MOCHC::rankCrowdingPopulation() {
//cout << " rankcrowdingpopulation " << endl;
   int inserted = 0;
   
   vector < vector < Individual * > > fronts;
   vector < Individual *> notClassified;
   
   //Calculate fronts
   int differentsInFront0;
   FastNondominatedSort(population, fronts, notClassified, differentsInFront0, getPopulationSize());
   
//cout << " despues fastnondominatedsort " << endl;
   // Order each front using the local crowding distance 
   for (int i = 0; i < fronts.size(); i++)
      crowOrder(fronts[i]);

	if (!populationOld.empty()) {
		for (int i = 0; i < fronts[0].size(); i++) {
			if ((i < (unsigned int) getPopulationSize()) && !compareIndividual(fronts[0][i])) {
				changePopulation = true;
			}
		}
	}

	populationOld.clear();	
	for (int i = 0; i < fronts[0].size(); i++) {
		if (i < (unsigned int) getPopulationSize()) {
			populationOld.insert(fronts[0][i]);
		}
	}
//cout << " despues croworder " << endl;
   
   //Create the ordered population
   population->clear();
   for (int i = 0; i < fronts.size() - 1; i++){
 		for (int j = 0; j < fronts[i].size(); j++){
//cout << " dentro de bucle " << i << "," << j << " --> " << fronts[i][j] << endl;
////			if (!changePopulation && (!compareIndividual(fronts[i][j]))) {
////////	    		if (populationOld.count(fronts[i][j]) == 0) {
//cout << " no esta dentro de la poblacion antigua " << endl;
////					changePopulation = true;
////	    		}
    		population->push_back(fronts[i][j]);
 		}
	}
      
      //Last front
   for (int j = 0; j < fronts[fronts.size() - 1].size(); j++){
//cout << " bucle ultimo frente " << fronts[fronts.size()-1][j] << endl;
 		if (population->size() < (unsigned int) getPopulationSize()){
//cout << " se inserta " << endl;
/////////		      if (populationOld.count(fronts[fronts.size()-1][j]) == 0) {
////				if (!changePopulation && (!compareIndividual(fronts[fronts.size()-1][j]))) {
//cout << " no esta dentro de la poblacion antigua " << endl;
////					changePopulation = true;
////	   		}
    		population->push_back((fronts[fronts.size() - 1][j]));
		} else {
   		delete (fronts[fronts.size() - 1][j]);
 		}
	}
      
      //Delete not used individuals
      for (int i = 0; i < notClassified.size(); i++){
	 delete (notClassified[i]);
      }
                       
//cout << " fin rankcrowding changePopulation: " << changePopulation << endl;
      
}

//Se selecciona un numero de individuos de forma aleatoria
void MOCHC::selectInPopulation(vector<Individual*> pop, int numberIndividual, vector<Individual*> &selected){
//cout << " select in population " << numberIndividual << endl;
	vector<int> index;
	vector<Individual*> resultado;
	int generatedNumbers = (pop.size() > numberIndividual)?numberIndividual:pop.size();
	generateRandomValues(0, pop.size()-1, generatedNumbers, index);
	
	for(unsigned int i = 0; i < index.size(); i++) {
//cout << " añado los elementos seleccionados " << index[i] << " --> " << pop[index[i]] << endl;
//cout << " despues de clone " << endl;
	    resultado.push_back(pop[index[i]]->internalClone());
	}
	selected = resultado;
//cout << " fin de elementos" << endl;
}


//*******************************************************************************
//Se realiza la mutacion de parte de la poblacion en caso de superar el umbral.
//*******************************************************************************
void MOCHC::mutation() {
   
   //Se comprueba si se ha modificado la poblacion
//cout << "mutation " << changePopulation << endl;
   if (!changePopulation) {
      minimunDistance = minimunDistance - 1;
//cout << "minimunDistance " << minimunDistance << " " << -cv << endl;
      
      //Se comprueba si se supera el umbral de incesto
      if (minimunDistance <= -cv) {
	 
//cout << "se realiza la mutacion " << endl;
	 minimunDistance = 1 - (1.0/(*population)[0]->getNumberOfVar());
//cout << "minimunDistance nueva " << minimunDistance << endl;
	 //minimumDistance = (int) (1.0/size * (1 - 1.0/size) * size);
	 
	 //Se calcula el numero de elementos que se van a preservar
	 int preserve = floor(pp * population->size());
//cout << "preserve " << preserve  << " " << pp << " " << population->size() << endl;
	 vector<Individual*>* newPopulation = new vector<Individual*>;
	 
	 //Se copian el porcentaje de elementos que se quieren preservar
	 for (int i = 0; i < preserve; i++) {
//cout << "copio los preservados " << endl;
	    newPopulation->push_back((*population)[i]);
         }
        
//cout << "despues de preservar "  << endl;
	 //Se realiza la mutacion del resto para obtener el tamaño de la nueva poblacion
         for (int i = preserve; i < population->size(); i++) {
//cout << "antes de mutation " << i << endl;
	    (*population)[i]->mutation(pm);
//cout << "antes de evaluate " << endl;
	    evaluate((*population)[i]);
//cout << "antes de insert in archive " << endl;
	    insertInArchive((*population)[i]);
//cout << "antes de push_back " << endl;
	    newPopulation->push_back((*population)[i]);
//cout << "despues pushback " << endl;
	 }

//cout << "despues de bucle " << endl;
	 population = newPopulation;
      }
//cout << " fin mutation " << endl;
      
   }
}

/*******************************************************************************
 * Create a child population by applying:
 *  - Selection
 *  - Crossover
 * *****************************************************************************/
void MOCHC::createChildPop() {
   int tamPop = population->size();  
//cout << " tamPop: " << tamPop << endl;
   vector<Individual*> selected;
   changePopulation = false;
   /*populationOld.clear();
   
   for (int i = 0; i < tamPop; i++) {
      populationOld.insert((*population)[i]);
//cout << " insertando en population old " << (*population)[i] << endl;
   }*/
   
   for (int i = 0; i < (tamPop/2); i++){
      //Se seleccionan los padres mediante un RandomSelection      
      selectInPopulation (*population, 2, selected);
//cout << " despues de selectinpopulation: " << selected[0] << endl;
//cout << " despues de selectinpopulation: " << selected[1] << endl;
      
      // Crossover (HUX)      
      if (hammingDistance(selected[0], selected[1]) >= minimunDistance) {  
//cout << " hammingdistance >= distancia minima: " << minimunDistance << endl;
	 selected[0]->crossover(selected[1]);
//cout << " despues de crossover" << endl;
	 evaluate(selected[0]);
//cout << " despues evaluate " << endl;
	 insertInArchive(selected[0]);
//cout << " despues insertarchive0 " << endl;
	 evaluate(selected[1]);
//cout << " despues evaluate1 "  << endl;
	 insertInArchive(selected[1]);
//cout << " despues insertarchive1 " << endl;
	 population->push_back(selected[0]);
//cout << " despues push_back " << selected[0] << endl;
	 population->push_back(selected[1]);
//cout << " despues push_back " << selected[1] << endl;
      } else {
		delete selected[0];
		delete selected[1];
	}		
      
   }
     
}
