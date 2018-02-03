/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo Gonzalez
 *   - Carlos Segura Gonzalez
 * 
 * DATE
 *    March 2011
 *
 **********************************************************************************/
 
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <math.h>

#include "MonoGA_GlobalSearch.h"

// Algorithm initialization
bool MonoGA_GlobalSearch::init(const vector<string> &params){
	// Check number of parameters
	if (params.size() != 5){
		cout << "Parametros: popSize pm pc survivalSelection globalSearchType" << endl;
		cout << "Survival Selection: " << endl;
		cout << "   0: Steady State" << endl;
		cout << "   1: Generational" << endl;
		cout << "   2: Replace worst from parent and offspring" << endl;
		cout << "Global Search Mechanism: " << endl;
		cout << "   0: Similarity-based Global Search" << endl;
		cout << "   1: Original Global Search" << endl;
		cout << "   2: No Global Search" << endl;
		return false;
	}

	// Only mono-objective optimization is supported
	if (getSampleInd()->getNumberOfObj() != 1){
		cout << "Multi-Objective not supported" << endl;
		return false;
	}

	// Parameters initialization
	setPopulationSize(atoi(params[0].c_str()));
	this->pm = atof(params[1].c_str());
	this->pc = atof(params[2].c_str());
	this->survivalSelectionType = atof(params[3].c_str());
	this->globalSearchType = atof(params[4].c_str());
  
  if ((globalSearchType == GLOBAL_SEARCH_SIMILARITY) || (globalSearchType == GLOBAL_SEARCH_ORIGINAL)) {
    globalSearchRequired = true;
  }
  else if (globalSearchType == GLOBAL_SEARCH_NO) {
    globalSearchRequired = false;
  }
  else {
    cerr << "MonoGA_GlobalSearch: globalSearchType not supported" << endl;
    exit(-1);
  }

  maxVarValues = 1000;
  varThreshold = 0.4;
  
  maxMeanDCN = DBL_MIN;
  maxMeanPairwise = DBL_MIN;
	
	return true;
}

void MonoGA_GlobalSearch::fillPopWithOppositionBasedLearning() {
	// Generates a total number of individuals equal to (getPopulationSize() - population->size()) * 2
	vector<Individual *> genPopulation;
	for (int i = 0; i < (getPopulationSize() - population->size()); i++) {
		Individual *ind = getSampleInd()->internalClone();
		Individual *opp_ind = getSampleInd()->internalClone();
		for (int j = 0; j < ind->getNumberOfVar(); j++) {
			// Generates a random population
			//double aux = (double) generator() / (double) generator.max();
      double aux = ((double) rand () / (double) RAND_MAX);
			ind->setVar(j, aux * (ind->getMaximum(j) - ind->getMinimum(j)) + ind->getMinimum(j)); 

			// Generates the oppositional population
			opp_ind->setVar(j, opp_ind->getMinimum(j) + opp_ind->getMaximum(j) - ind->getVar(j));
		}
		evaluate(ind);
		evaluate(opp_ind);
		genPopulation.push_back(ind);
		genPopulation.push_back(opp_ind);
	}

	sort(genPopulation.begin(), genPopulation.end(), sortByObj0);

	for (int i = 0; i < genPopulation.size() / 2; i++) {
		population->push_back(genPopulation[i]);
		delete genPopulation[genPopulation.size() - 1 - i];
	}

	genPopulation.clear();
}

void MonoGA_GlobalSearch::fillPopWithNewIndsAndEvaluate() {
  fillPopWithOppositionBasedLearning();
}

void MonoGA_GlobalSearch::evaluateOffspring(){
	for (int i = 0; i < offSpring.size(); i++){
		evaluate(offSpring[i]);
	}
}

// MonoGA generation
void MonoGA_GlobalSearch::runGeneration() {
	sortPopulation();//Se ordena cada vez porque el paralelo podria desordenarlo TODO: ordenar solo 1 vez
	createOffSprings();
	evaluateOffspring();
	survivalSelection();
  
  if (globalSearchRequired) {
    globalSearch();
  }

  //NelderMeadNoShrink();

  // Calculates different diversity metrics at each generation
  /*if (getGeneration() % 1000 == 0) {
    cout << "Generation: " << getGeneration() << ", ";
    cout << "Number of evaluations: " << getPerformedEvaluations() << ", ";
    cout << "Normalised Mean DCN: " << getNormalisedMeanDistanceToClosestNeighbour() << ", ";
    cout << "Normalised Mean Pairwise: " << getNormalisedMeanPairwiseDistanceAllIndividuals() << endl;
  }*/
}

void MonoGA_GlobalSearch::sortPopulation(){
	sort(population->begin(), population->end(), sortByObj0);
}

bool MonoGA_GlobalSearch::sortByDistanceToBest(const Individual *i1, const Individual *i2, const Individual *best) {
	return (i1->getEuclideanDistanceDecisionSpace(best) < i2->getEuclideanDistanceDecisionSpace(best));
}

bool MonoGA_GlobalSearch::sortByDistanceToBestReversed(const Individual *i1, const Individual *i2, const Individual *best) {
	return (i1->getEuclideanDistanceDecisionSpace(best) > i2->getEuclideanDistanceDecisionSpace(best));
}

void MonoGA_GlobalSearch::binaryTournamentSelection(int &p1, int &p2){//Supone que population esta ordenada
	int opt1 = (int) (((double)(getPopulationSize()))*rand()/(RAND_MAX+1.0));
	int opt2 = (int) (((double)(getPopulationSize()))*rand()/(RAND_MAX+1.0));
	p1 = min(opt1, opt2);
	opt1 = (int) (((double)(getPopulationSize()))*rand()/(RAND_MAX+1.0));
	opt2 = (int) (((double)(getPopulationSize()))*rand()/(RAND_MAX+1.0));
	p2 = min(opt1, opt2);
}

void MonoGA_GlobalSearch::parentSelection(int &p1, int &p2){
	binaryTournamentSelection(p1, p2);
}

void MonoGA_GlobalSearch::createOffSpring(){//Crear 2 hijos
	int p1, p2;
	parentSelection(p1, p2);
	Individual *of1 = (*population)[p1]->internalClone();
	Individual *of2 = (*population)[p2]->internalClone();
	double vcross = rand() / (RAND_MAX + 1.0);
	if (vcross < pc) {  
		of1->crossover(of2);
	}
	of1->mutation(pm);
	of2->mutation(pm);
	offSpring.push_back(of1);
	offSpring.push_back(of2);
}

void MonoGA_GlobalSearch::createOffSprings(){
	offSpring.clear();
	if (survivalSelectionType == SURVIVAL_SELECTION_SS){//Crear un unico hijo
		createOffSpring();
		delete(offSpring[1]);
		offSpring.pop_back();
	} else if ((survivalSelectionType == SURVIVAL_SELECTION_GENERATIONAL) || (survivalSelectionType == SURVIVAL_SELECTION_REPLACEWORST_FROMPARENTANDOFFSPRING)){
		if (survivalSelectionType == SURVIVAL_SELECTION_GENERATIONAL){//Elitism
			offSpring.push_back((*population)[0]->internalClone());
		}
		while(offSpring.size() < getPopulationSize()){
			createOffSpring();
		}
	}
	if (offSpring.size() > getPopulationSize()){
		delete(offSpring[offSpring.size()-1]);	
		offSpring.pop_back();
	}
}

void MonoGA_GlobalSearch::survivalSelection(){
	if (survivalSelectionType == SURVIVAL_SELECTION_SS){
		if ((*population)[0]->getInternalOptDirection(0) == MINIMIZE){
			if ((*population)[getPopulationSize() - 1]->getObj(0) > offSpring[0]->getObj(0)){
				delete (*population)[getPopulationSize() - 1];
				(*population)[getPopulationSize() - 1] = offSpring[0];//Ojo: Queda desornado
			} else {
				delete offSpring[0];
			}
		} else {//Maximizar
			if ((*population)[getPopulationSize() - 1]->getObj(0) < offSpring[0]->getObj(0)){
				delete (*population)[getPopulationSize() - 1];
				(*population)[getPopulationSize() - 1] = offSpring[0];//Ojo: Queda desornado
			} else {
				delete offSpring[0];
			}
		}
	} else if (survivalSelectionType == SURVIVAL_SELECTION_GENERATIONAL){
		for (int i = 0; i < getPopulationSize(); i++){
			delete (*population)[i];
			(*population)[i] = offSpring[i];
		}
	} else if (survivalSelectionType == SURVIVAL_SELECTION_REPLACEWORST_FROMPARENTANDOFFSPRING){
		//Nota este replace worst no es el que esta explicado en el libro de Eiben
		//Aqui nos estamos "cargando" los peores entre padres e hijos
		//En el libro de Eiben dice que se generan nu hijos (< n) y se borran los nu peores
		//padres.
		//Habria que hacer otro operador mas y a este llamarlo RPELACEWORSTFROMPARENTSANDOFFSPRING
		//Al otro lo podemos llamar GENITOR
		//Nota: en la pagina 27 si llama replace worst a un esquema en que se eligen los mejores entre
		//padres e hijos, pero puede llevar a confusion...
		for (int i = 0; i < offSpring.size(); i++){
			population->push_back(offSpring[i]);
		}
		sortPopulation();
		for (int i = 0; i < offSpring.size(); i++){
			delete ((*population)[population->size()-1]);
			population->pop_back();
		}
	}
}

void MonoGA_GlobalSearch::getSolution(MOFront *p) {
	sortPopulation();
	p->insert((*population)[0]);
}

void MonoGA_GlobalSearch::printInfo(ostream &os) const {
	os << "MonoObjective Genetic Algorithm"  << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Mutation Probability = " << pm << endl;
	os << "Crossover Probability = " << pc << endl;
	os << "Population Size = " << getPopulationSize() << endl;
	os << "Survival Selection Type = " << survivalSelectionType << endl;
	os << "Global Search Type = " << globalSearchType << endl;
}

void MonoGA_GlobalSearch::globalSearch() {
  if (globalSearchType == GLOBAL_SEARCH_SIMILARITY) {
    GlobalSearchGlobalNeighbourhood12();
  }
  else if (globalSearchType == GLOBAL_SEARCH_ORIGINAL) {
    GlobalSearchGlobalNeighbourhood();
  }
}

// Implementation of the original proposal
void MonoGA_GlobalSearch::GlobalSearchGlobalNeighbourhood() {
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

  // Randomly selects three mutually exclusive integers belonging to
  // the set {0, 1, ..., NP - 1}
	int k, r1, r2;

  k = getRandomInteger0_N(getPopulationSize() - 1);

  do {
    r1 = getRandomInteger0_N(getPopulationSize() - 1);
  } while (r1 == k);

  do {
    r2 = getRandomInteger0_N(getPopulationSize() - 1);
  } while ((r2 == r1) || (r2 == k));

  // Determines the best individual in the population
  Individual *best = (*population)[0];
  for (int i = 1; i < getPopulationSize(); i++) {
    if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
        (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
      best = (*population)[i];
    }
  }

  // Applies global neighbourhood search strategy
  Individual *v = getSampleInd()->internalClone();
  for (int i = 0; i < v->getNumberOfVar(); i++) {
    v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

    // Checks lower and upper limits of variables
    if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
      double r = ((double) rand () / (double) RAND_MAX);
      v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
    }
    v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
    v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
  }

  // Evaluates the new individual
  evaluate(v);

  // Selects the best individual between v and k
  if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
      ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
    delete (*population)[k];
    (*population)[k] = v;
  } else {
    delete v;
	}
}

void MonoGA_GlobalSearch::globalNeighbourhoodPathSearch() {
  sortPopulation();
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1, r2;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));

      // Applies global neighbourhood search strategy
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * (*population)[0]->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sortPopulation();

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoGA_GlobalSearch::GlobalSearchGlobalNeighbourhood3_2() {
  sortPopulation();
  Individual *best = (*population)[0];
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1, r2;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));

      // Applies global neighbourhood search strategy
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;

        // Checks if the new individual v is the best one
        if ((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < best->getObj(0)))
          best = v;
        else if ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > best->getObj(0)))
          best = v;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sortPopulation();

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoGA_GlobalSearch::GlobalSearchGlobalNeighbourhood6() {
  sortPopulation();
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  // Calculates the centroid of the current population
  Individual *c = getSampleInd()->internalClone();
  for (int i = 0; i < c->getNumberOfVar(); i++) {
    double sum = 0;
    for (int j = 0; j < getPopulationSize(); j++) {
      sum += (*population)[j]->getVar(i);
    }
    c->setVar(i, sum / getPopulationSize());
  }

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	/*do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));*/

      // Applies global neighbourhood search strategy considering the centroid of the population
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * c->getVar(i) + a3 * ((*population)[0]->getVar(i) - (*population)[r1]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sortPopulation();

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoGA_GlobalSearch::GlobalSearchGlobalNeighbourhood6_2() {
  sortPopulation();
  Individual *best = (*population)[0];
  vector<bool> explored (getPopulationSize(), false);
  int numberImp;
  int numberExp = 0;

  for (int i = 0; i < getPopulationSize(); i++)
    explored[i] = false;

  // Calculates the centroid of the current population
  Individual *c = getSampleInd()->internalClone();
  for (int i = 0; i < c->getNumberOfVar(); i++) {
    double sum = 0;
    for (int j = 0; j < getPopulationSize(); j++) {
      sum += (*population)[j]->getVar(i);
    }
    c->setVar(i, sum / getPopulationSize());
  }

  do {
    bool improvement = false;
    numberImp = 0;
    int k;

    do {
      k = getRandomInteger0_N(getPopulationSize() - 1);
    } while (explored[k]);

    explored[k] = true;
    numberExp++;

    do {
      // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
      // The condition a1 + a2 + a3 == 1 must be satisfied
      double a1, a2, a3;

      do {
        a1 = ((double) rand () / (double) RAND_MAX);
        a2 = ((double) rand () / (double) RAND_MAX);
        a3 = 1.0 - a1 - a2;
      } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

      int r1;

     	do {
    		r1 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while (r1 == k);

    	/*do {
    		r2 = getRandomInteger0_N(getPopulationSize() - 1);
    	} while ((r2 == r1) || (r2 == k));*/

      // Applies global neighbourhood search strategy considering the centroid of the population
      Individual *v = getSampleInd()->internalClone();
      for (int i = 0; i < v->getNumberOfVar(); i++) {
        v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * c->getVar(i) + a3 * (best->getVar(i) - (*population)[r1]->getVar(i)));

        // Checks lower and upper limits of variables
        if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
          double r = ((double) rand () / (double) RAND_MAX);
          v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
        }
        v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
        v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
      }

      // Evaluates the new individual
      evaluate(v);

      // Selects the best individual between v and k
      if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
        //delete (*population)[k];
        population->push_back((*population)[k]);
        (*population)[k] = v;
        improvement = true;
        numberImp++;

        // Checks if the new individual v is the best one
        if ((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < best->getObj(0)))
          best = v;
        else if ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > best->getObj(0)))
          best = v;
      } else {
        delete v;
        improvement = false;
  	  }
    } while (improvement);
  } while ((numberImp > 0) && (numberExp < getPopulationSize()));

  // Selects the best individuals from the current population and the global neighbourhood path
  sortPopulation();

	for (int i = 0; i < (population->size() - getPopulationSize()); i++) {
		delete (*population)[population->size() - 1];
    population->pop_back();
	}
}

void MonoGA_GlobalSearch::GlobalSearchGlobalNeighbourhood8() {
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

  bool improvement = false;
  int k;

  k = getRandomInteger0_N(getPopulationSize() - 1);

  // Calculate the number of elegible individuals for r1 and r2 depending on the
  // current moment of the search (given by the number of evaluations performed)
  int min_pop = max((int)(round(getPopulationSize() * 0.1)), 4);
  int max_pop = getPopulationSize();
  double m = (double)(min_pop - max_pop) / (double)getEvaluations();
  int y = (int)(round(m * getPerformedEvaluations() + max_pop));

  do {
    // Determines the best individual in the population
    Individual *best = (*population)[0];
    for (int i = 1; i < getPopulationSize(); i++) {
      if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
          (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
        best = (*population)[i];
      }
    }
    
    // Sort the population in ascending order in terms of the distance with respect to
    // the fittest individual
    sort(population->begin(), population->end(), bind(&MonoGA_GlobalSearch::sortByDistanceToBest, this, std::placeholders::_1, std::placeholders::_2, best));

    int r1, r2;

   	do {
  		r1 = getRandomInteger0_N(y - 1);
  	} while (r1 == k);

  	do {
  		r2 = getRandomInteger0_N(y - 1);
  	} while ((r2 == r1) || (r2 == k));

    // Applies global neighbourhood search strategy
    Individual *v = getSampleInd()->internalClone();
    for (int i = 0; i < v->getNumberOfVar(); i++) {
      v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

      // Checks lower and upper limits of variables
      if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
        double r = ((double) rand () / (double) RAND_MAX);
        v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
      }
      v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
      v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
    }

    // Evaluates the new individual
    evaluate(v);

    // Selects the best individual between v and k
    if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
        ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
      delete (*population)[k];
      (*population)[k] = v;
      improvement = true;
    } else {
      delete v;
      improvement = false;
	  }
  } while (improvement);
}

void MonoGA_GlobalSearch::GlobalSearchGlobalNeighbourhood9() {
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

  bool improvement = false;
  int k;

  k = getRandomInteger0_N(getPopulationSize() - 1);

  // Calculate the number of elegible individuals for r1 and r2 depending on the
  // current moment of the search (given by the number of evaluations performed)
  int min_pop = max((int)(round(getPopulationSize() * 0.1)), 4);
  int max_pop = getPopulationSize();
  double m = (double)(max_pop - min_pop) / (double)getEvaluations();
  int y = (int)(round(m * getPerformedEvaluations() + min_pop));

  do {
    // Determines the best individual in the population
    Individual *best = (*population)[0];
    for (int i = 1; i < getPopulationSize(); i++) {
      if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
          (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
        best = (*population)[i];
      }
    }
    
    // Sort the population in ascending order in terms of the distance with respect to
    // the fittest individual
    sort(population->begin(), population->end(), bind(&MonoGA_GlobalSearch::sortByDistanceToBest, this, std::placeholders::_1, std::placeholders::_2, best));

    int r1, r2;

   	do {
  		r1 = getRandomInteger0_N(y - 1);
  	} while (r1 == k);

  	do {
  		r2 = getRandomInteger0_N(y - 1);
  	} while ((r2 == r1) || (r2 == k));

    // Applies global neighbourhood search strategy
    Individual *v = getSampleInd()->internalClone();
    for (int i = 0; i < v->getNumberOfVar(); i++) {
      v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

      // Checks lower and upper limits of variables
      if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
        double r = ((double) rand () / (double) RAND_MAX);
        v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
      }
      v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
      v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
    }

    // Evaluates the new individual
    evaluate(v);

    // Selects the best individual between v and k
    if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
        ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
      delete (*population)[k];
      (*population)[k] = v;
      improvement = true;
    } else {
      delete v;
      improvement = false;
	  }
  } while (improvement);
}

void MonoGA_GlobalSearch::GlobalSearchGlobalNeighbourhood10() {
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);

  bool improvement = false;
  int k;

  k = getRandomInteger0_N(getPopulationSize() - 1);

  do {
    // Calculates the mean value of each decision variable of the current population
    vector<double> means((*population)[0]->getNumberOfVar(), 0);

    for (int i = 0; i < (*population)[0]->getNumberOfVar(); i++) {
      double sum = 0;
      for (int j = 0; j < getPopulationSize(); j++) {
        sum += (*population)[j]->getVar(i);
      }
      means[i] = sum / getPopulationSize();
    }
   
    // Calculates the diversity of the current population 
		double var = 0;
    for (int i = 0; i < getPopulationSize(); i++) {
      double sum = 0;
      for (int j = 0; j < (*population)[0]->getNumberOfVar(); j++) {
        double aux = (*population)[i]->getVar(j) - means[j];
        sum += (aux * aux);
      }
      var += sqrt(sum);
    }
    var /= getPopulationSize();
    
    //cout << "var (no norm): " << var << endl;

    // Inserts the new variance value into the variance value vector
    if (varValues.size() < maxVarValues) {
      varValues.push_back(var);
    }
    else {
      varValues.erase(varValues.begin());
      varValues.push_back(var);
    }

    // Normalises the value of the variance taking into account the
    // different values of the variance value vector
    double maxVar = DBL_MIN;
    for (int i = 0; i < varValues.size(); i++) {
      if (varValues[i] > maxVar) {
        maxVar = varValues[i];
      }
    }
    var /= maxVar;

    //cout << "var (norm): " << var << endl;
    //cout << "varThreshold: " << varThreshold << endl;

    // Determines the best individual in the population
    Individual *best = (*population)[0];
    for (int i = 1; i < getPopulationSize(); i++) {
      if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
          (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
        best = (*population)[i];
      }
    }
    
    // Sort the population in ascending order in terms of the distance with respect to
    // the fittest individual
    sort(population->begin(), population->end(), bind(&MonoGA_GlobalSearch::sortByDistanceToBest, this, std::placeholders::_1, std::placeholders::_2, best));
    
    // Calculate the range of elegible individuals for r1 and r2 depending on the
    // current moment of the search (given by the variance of the population)
    int minRange, maxRange;
    int minPop = max((int)(round(getPopulationSize() * 0.1)), 4);

    // We should promote exploitation
    if (var >= varThreshold) {
      minRange = 0;
      maxRange = max((int)(round((1 - var) * getPopulationSize()) - 1), minPop - 1);
    }
    // we should promote diversification
    else {
      minRange = min((int)(round((1 - var) * getPopulationSize()) - 1), getPopulationSize() - minPop - 1);
      maxRange = getPopulationSize() - 1;
    }

    //cout << "minRange: " << minRange << endl;
    //cout << "maxRange: " << maxRange << endl;

    int r1, r2;

   	do {
  		r1 = generateRandom(minRange, maxRange);
  	} while (r1 == k);

  	do {
  		r2 = generateRandom(minRange, maxRange);
  	} while ((r2 == r1) || (r2 == k));

    //cout << "r1: " << r1 << endl;
    //cout << "r2: " << r2 << endl;

    // Applies global neighbourhood search strategy
    Individual *v = getSampleInd()->internalClone();
    for (int i = 0; i < v->getNumberOfVar(); i++) {
      v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

      // Checks lower and upper limits of variables
      if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
        double r = ((double) rand () / (double) RAND_MAX);
        v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
      }
      v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
      v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
    }

    // Evaluates the new individual
    evaluate(v);

    // Selects the best individual between v and k
    if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
        ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
      delete (*population)[k];
      (*population)[k] = v;
      improvement = true;
    } else {
      delete v;
      improvement = false;
	  }
  } while (improvement);
}

void MonoGA_GlobalSearch::GlobalSearchGlobalNeighbourhood12() {
  // Three random numbers (a1, a2, a3) belonging to the range [0, 1] are obtained
  // The condition a1 + a2 + a3 == 1 must be satisfied
  double a1, a2, a3;

  /*do {
    a1 = ((double) rand () / (double) RAND_MAX);
    a2 = ((double) rand () / (double) RAND_MAX);
    a3 = 1.0 - a1 - a2;
  } while  (abs(a1) + abs(a2) + abs(a3) != 1.0);*/
  
  a1 = ((double) rand () / (double) RAND_MAX);
  a2 = 1.0 - a1;

  //a1 = ((double) getPerformedEvaluations() / (double) getEvaluations());
  //a2 = 1.0 - a1;

  //cout << "a1: " << a1 << ", a2: " << a2 << ", a3: " << a3 << endl;

  int k = getRandomInteger0_N(getPopulationSize() - 1);

  // Calculate the number of elegible individuals for r1 depending on the
  // current moment of the search (given by the number of evaluations performed)
  int min_pop = max((int)(round(getPopulationSize() * 0.1)), 5) - 1;
  int max_pop = getPopulationSize() - 1;
  double m = (double)(max_pop - min_pop) / (double)getEvaluations();
  int upper = (int)(round(m * getPerformedEvaluations() + min_pop));
  int lower = upper - min_pop;

  //cout << "lower: " << lower << ", upper: " << upper << endl;

  // Determines the best individual in the population
  Individual *best = (*population)[0];
  for (int i = 1; i < getPopulationSize(); i++) {
    if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
        (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
      best = (*population)[i];
    }
  }

  // Sorts the population in descending order in terms of the Euclidean distance in the decision space
  // with respect to the fittest individual
  sort(population->begin(), population->end(), bind(&MonoGA_GlobalSearch::sortByDistanceToBestReversed, this, std::placeholders::_1, std::placeholders::_2, best));

  //for (int i = 0; i < getPopulationSize(); i++)
  // cout << "i: " << i << " Fitness: " << (*population)[i]->getObj(0) << " Distance to best: " << getEuclideanDistance(best, (*population)[i]) << endl;

  int r1, r2;

 	do {
    //r1 = getRandomInteger0_N(upper - 1);
    r1 = generateRandom(lower, upper);
 	} while (r1 == k);

  /*do {
  	r2 = generateRandom(lower, upper);
  } while ((r2 == r1) || (r2 == k));*/
  
  //cout << "k: " << k << ", r1: " << r1 << ", r2: " << r2 << endl;

  // Applies global neighbourhood search strategy
  Individual *v = getSampleInd()->internalClone();
  for (int i = 0; i < v->getNumberOfVar(); i++) {
    //v->setVar(i, (*population)[k]->getVar(i) + a1 * ((*population)[r1]->getVar(i) - (*population)[k]->getVar(i)));
    v->setVar(i, (*population)[k]->getVar(i) + a1 * (best->getVar(i) - (*population)[k]->getVar(i)) + a2 * ((*population)[r1]->getVar(i) - (*population)[k]->getVar(i)));
    //v->setVar(i, a1 * (*population)[k]->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

    // Checks lower and upper limits of variables
    if ((v->getVar(i) < v->getMinimum(i)) || (v->getVar(i) > v->getMaximum(i))) {
      double r = ((double) rand () / (double) RAND_MAX);
      v->setVar(i, r * (v->getMaximum(i) - v->getMinimum(i)) + v->getMinimum(i));
    }
    v->setVar(i, max(v->getVar(i), v->getMinimum(i)));
    v->setVar(i, min(v->getVar(i), v->getMaximum(i)));
  }

  // Evaluates the new individual
  evaluate(v);

  //cout << "Fitness del nuevo individuo generado: " << v->getObj(0) << " Distancia al mejor: " << getEuclideanDistance(best, v) << endl;

  // 1. Looks for the best individual's closest neighbour in order to replace it by the novel individual
  /*int index_closest = getPopulationSize() - 1;

  while ((getEuclideanDistance(best, (*population)[index_closest]) == 0) && (index_closest > 0))
    index_closest--;

  //cout << "Individuo mas cercano al best: " << index_closest << endl;

  delete (*population)[index_closest];
  (*population)[index_closest] = v;*/
  
  // 2. Looks for the best individual's most far away neighbour in order to replace it by the novel individual
  /*int index_far = 0;
  delete (*population)[index_far];
  (*population)[index_far] = v;*/
  
  // 3. Selects the best individual between v and k
  if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
      ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
    delete (*population)[k];
    (*population)[k] = v;
  } else {
    delete v;
	}
}

void MonoGA_GlobalSearch::NelderMeadNoShrink() {

  int dimension = (*population)[0]->getNumberOfVar() + 1;
  vector<Individual *> nelderPop;
  
  if (population->size() >= dimension) {
    for (int i = 0; i < dimension; i++) {
      Individual* ind = (*population)[i]->internalClone();
      nelderPop.push_back(ind);
    }
  }
  else {
    for (int i = 0; i < population->size(); i++) {
      Individual* ind = (*population)[i]->internalClone();
      nelderPop.push_back(ind);
    }
    for (int i = 0; i < (dimension - population->size()); i++) {
  	  int k = getRandomInteger0_N(population->size() - 1);
      Individual *ind = (*population)[k]->internalClone();
      nelderPop.push_back(ind);
    }
  }
  bool shrink = false;
  while(!shrink)
  {
  	sort(nelderPop.begin(), nelderPop.end(), sortByObj0);
	  Individual *xc = getSampleInd()->internalClone();
	  
	  for (int i = 0; i < xc->getNumberOfVar(); i++) {
	    double sum = 0;
	    for (int j = 0; j < dimension - 1; j++) {
	      sum += nelderPop[j]->getVar(i);
	    }
	    xc->setVar(i, sum / (dimension - 1)); //Calculamos el centroide de toda la poblaci\F3n excepto del peor
	  }
	  
	  Individual *xr = getSampleInd()->internalClone(); // punto reflejado
	  for (int i = 0; i < xr->getNumberOfVar(); i++) {
	  	double valorCalculado=xc->getVar(i) + NM1 * (xc->getVar(i) - nelderPop[dimension - 1]->getVar(i));
	  	xr->setVar(i, valorCalculado);	
	  }
	  evaluate(xr);	
	  
	  if(xr->getObj(0) < nelderPop[0]->getObj(0)) // Si el reflejado mejora la mejor soluci\F3n
	  {
	  	Individual *xe = getSampleInd()->internalClone(); // punto extendido
	  	for (int i = 0; i < xe->getNumberOfVar(); i++) {
	  		double valorCalculado = xc->getVar(i) + NM2 * (xr->getVar(i) - xc->getVar(i));
	  		xe->setVar(i, valorCalculado);	
	  	}
	  	evaluate(xe);

	    if(xe->getObj(0) < nelderPop[0]->getObj(0))
	    {
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xe; // agregamos el nuevo
		  }
		else
		{
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xr; // agregamos el nuevo	
		}
	  }
	  
	  else if((xr->getObj(0) >= nelderPop[0]->getObj(0)) && (xr->getObj(0) < nelderPop[dimension-1]->getObj(0)))
	  {
         delete nelderPop[dimension - 1];
	    	 nelderPop[dimension - 1] = xr; // agregamos el nuevo	
	  }
	  
	  else if(xr->getObj(0) >= nelderPop[dimension - 1]->getObj(0))
	  {
	   	 if(xr->getObj(0) < nelderPop[dimension - 1]->getObj(0))
	   	 {
		   	 	Individual *xcf = getSampleInd()->internalClone(); // punto de expansi\F3n
		     	for (int i = 0; i < xcf->getNumberOfVar(); i++) {
		    		double valorCalculado = xc->getVar(i) + NM3 * (xr->getVar(i) - xc->getVar(i));
		    		xcf->setVar(i, valorCalculado);	
		    	}
		  		evaluate(xcf);
			    if(xcf->getObj(0) < nelderPop[0]->getObj(0))
				  {
             delete nelderPop[dimension - 1];
		    		 nelderPop[dimension - 1] = xcf; // agregamos el nuevo
				  }
			    else
			    {
				 	  shrink = true;
			    }
		  }
		  else
		  {
			  	Individual *xcd = getSampleInd()->internalClone(); // punto de contracci\F3n
		   	   	for (int i = 0; i < xcd->getNumberOfVar(); i++) {
		  			double valorCalculado=xc->getVar(i) - NM3*(xc->getVar(i)-nelderPop[dimension - 1]->getVar(i));
		  			xcd->setVar(i, valorCalculado);	
		  		}
		  		evaluate(xcd);
			    if(xcd->getObj(0) < nelderPop[0]->getObj(0))
				{
             delete nelderPop[dimension - 1];
		    		 nelderPop[dimension - 1] = xcd; // agregamos el nuevo	
				}
				else
				{
					shrink = true;
				}
	    }
	  }
  }
  // Replaces the current population with the Nelder population
  if (population->size() >= dimension) {
    for (int i = 0; i < dimension; i++) {
      delete (*population)[i];
      (*population)[i] = nelderPop[i];
    }
  }
  else {
    for (int i = 0; i < population->size(); i++) {
      delete (*population)[i];
      (*population)[i] = nelderPop[i];
    }
  }
}

double MonoGA_GlobalSearch::getEuclideanDistance(Individual *ind1, Individual *ind2) {
  double dist = 0.0;
  for (int i = 0; i < ind1->getNumberOfVar(); i++) {
    dist += ((ind1->getVar(i) - ind2->getVar(i)) * (ind1->getVar(i) - ind2->getVar(i)));
  }
  return sqrt(dist);
}

double MonoGA_GlobalSearch::getDistanceToClosestNeighbour(Individual *ind) {
  double minDist = DBL_MAX;
  for (int i = 0; i < getPopulationSize(); i++) {
    if (ind != (*population)[i]) {
      double dist = getEuclideanDistance(ind, (*population)[i]);
      if (dist < minDist)
        minDist = dist;
    }
  }
  return minDist;
}

double MonoGA_GlobalSearch::getMeanDistanceToClosestNeighbour() {
  double sumDist = 0.0;

  for (int i = 0; i < getPopulationSize(); i++) {
    sumDist += getDistanceToClosestNeighbour((*population)[i]);
  }

  double meanDCN = sumDist / (double)getPopulationSize();
  if (meanDCN > maxMeanDCN)
    maxMeanDCN = meanDCN;
  return meanDCN;
}

double MonoGA_GlobalSearch::getNormalisedMeanDistanceToClosestNeighbour() {
  return getMeanDistanceToClosestNeighbour() / maxMeanDCN;
}

double MonoGA_GlobalSearch::getMeanPairwiseDistanceAllIndividuals() {
  double sumDist = 0.0;

  for (int i = 0; i < getPopulationSize(); i++) {
    for (int j = i + 1; j < getPopulationSize(); j++) {
      sumDist += getEuclideanDistance((*population)[i], (*population)[j]);
    }
  }

  double meanPairwise = sumDist / (double)(getPopulationSize() * (getPopulationSize() - 1) / 2.0);
  if (meanPairwise > maxMeanPairwise)
    maxMeanPairwise = meanPairwise;
  return meanPairwise;
}

double MonoGA_GlobalSearch::getNormalisedMeanPairwiseDistanceAllIndividuals() {
  return getMeanPairwiseDistanceAllIndividuals() / maxMeanPairwise;
}

bool sortByObj0(const Individual *i1, const Individual *i2){
	return (i1->getInternalOptDirection(0) == MINIMIZE)?(i1->getObj(0) < i2->getObj(0)):(i1->getObj(0) > i2->getObj(0));
}
