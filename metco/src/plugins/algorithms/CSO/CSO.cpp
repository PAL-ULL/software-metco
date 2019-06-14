#include "CSO.h"

//#define DEBUG

// Algorithm initialization
bool CSO::init(const vector<string>& params) {
	// Check number of parameters
	if (params.size() != 3) {
		cout << "Parametros: gamma popSize globalSearchType" << endl;
		cout << "Global Search Mechanism: " << endl;
		cout << "   0: Similarity-based Global Search" << endl;
		cout << "   1: Original Global Search" << endl;
		cout << "   2: No Global Search" << endl;
		return false;
	} 

  // Only mono-objective optimization is supported
	if (getSampleInd()->getNumberOfObj() != 1) {
		cout << "Multi-Objective not supported by CSO" << endl;
		return false;
	}

  setGamma(atof(params[0].c_str()));
	setPopulationSize(atoi(params[1].c_str()));
	globalSearchType = atof(params[2].c_str());
  
  if ((globalSearchType == GLOBAL_SEARCH_SIMILARITY) || (globalSearchType == GLOBAL_SEARCH_ORIGINAL)) {
    globalSearchRequired = true;
  }
  else if (globalSearchType == GLOBAL_SEARCH_NO) {
    globalSearchRequired = false;
  }
  else {
    cerr << "CSO: globalSearchType not supported" << endl;
    exit(-1);
  }
  
  maxVarValues = 1000;
  varThreshold = 0.4;

	velocity = vector<vector<double>>(getPopulationSize(), vector<double>(getSampleInd()->getNumberOfVar(), 0.0));
  mean = getSampleInd()->internalClone();

	return true;
}

void CSO::runGeneration() {
  compete();
  
  if (globalSearchRequired) {
    globalSearch();
  }
}

void CSO::shufflePop() {
  randomPop.clear();
  for (int i = 0; i < getPopulationSize(); i++)
    randomPop.push_back(i);
  std::random_shuffle(randomPop.begin(), randomPop.end());
}

void CSO::calculateMean() {
	const int dim = (*population)[0]->getNumberOfVar();
	const int pop = getPopulationSize();

	for (int j = 0; j < dim; j++) {
		mean->setVar(j, 0.0);
		for (int i = 0; i < pop; i++) {
			mean->setVar(j, mean->getVar(j) + (*population)[i]->getVar(j));
		}
		mean->setVar(j, mean->getVar(j) / (double)pop);
	}
}

void CSO::compete() {
	const int pop = getPopulationSize();
  shufflePop();
  calculateMean();

  /*for (int i = 0; i < pop; i++) {
    cout << "randomPop[" << i << "]: " << randomPop[i] << endl;
  }*/

	for (int i = 0; i < pop; i += 2) {
		int r1 = randomPop[i];
		int r2 = randomPop[i + 1];

    if ((((*population)[r1]->getInternalOptDirection(0) == MINIMIZE) && ((*population)[r1]->getObj(0) <= (*population)[r2]->getObj(0))) ||
        (((*population)[r1]->getInternalOptDirection(0) == MAXIMIZE) && ((*population)[r1]->getObj(0) >= (*population)[r2]->getObj(0))))
      update(r1, r2);
    else
      update(r2, r1);
	}
}

void CSO::update(int winner, int loser) {

  //cout << "w: " << winner << " l: " << loser << endl;

	//setGamma((0.7 - 0.2) * (getEvaluations() - getPerformedEvaluations()) / getEvaluations() + 0.2);
  //cout << "gamma: " << getGamma() << endl;

  const int dimension = (*population)[0]->getNumberOfVar();

	for (int j = 0; j <  dimension; j++) {
		double coeff1 = ((double) rand () / (double) RAND_MAX);
		double coeff2 = ((double) rand () / (double) RAND_MAX);
		double coeff3 = ((double) rand () / (double) RAND_MAX);

    //cout << "coef1: " << coeff1 << endl;
    //cout << "coef2: " << coeff2 << endl;
    //cout << "coef3: " << coeff3 << endl;

		velocity[loser][j] = coeff1 * velocity[loser][j] +
                         coeff2 * ((*population)[winner]->getVar(j) - (*population)[loser]->getVar(j)) +
		                     coeff3 * getGamma() * (mean->getVar(j) - (*population)[loser]->getVar(j));

		(*population)[loser]->setVar(j, (*population)[loser]->getVar(j) + velocity[loser][j]);

		if (((*population)[loser]->getVar(j) > (*population)[loser]->getMaximum(j)) || ((*population)[loser]->getVar(j) < (*population)[loser]->getMinimum(j))) {
			double aux = ((double) rand () / (double) RAND_MAX);
			(*population)[loser]->setVar(j, aux * ((*population)[loser]->getMaximum(j) - (*population)[loser]->getMinimum(j)) + (*population)[loser]->getMinimum(j));
    }
	}

	evaluate((*population)[loser]);
}

void CSO::globalSearch() {
  if (globalSearchType == GLOBAL_SEARCH_SIMILARITY) {
    GlobalSearchGlobalNeighbourhood8();
  }
  else if (globalSearchType == GLOBAL_SEARCH_ORIGINAL) {
    GlobalSearchGlobalNeighbourhood();
  }
}

// Implementation of the original proposal
void CSO::GlobalSearchGlobalNeighbourhood() {
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

void CSO::globalNeighbourhoodPathSearch() {
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

void CSO::GlobalSearchGlobalNeighbourhood8() {
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
    sort(population->begin(), population->end(), bind(&CSO::sortByDistanceToBest, this, std::placeholders::_1, std::placeholders::_2, best));

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

void CSO::GlobalSearchGlobalNeighbourhood9() {
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
    sort(population->begin(), population->end(), bind(&CSO::sortByDistanceToBest, this, std::placeholders::_1, std::placeholders::_2, best));

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

void CSO::GlobalSearchGlobalNeighbourhood10() {
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
    sort(population->begin(), population->end(), bind(&CSO::sortByDistanceToBest, this, std::placeholders::_1, std::placeholders::_2, best));
    
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

bool CSO::sortByDistanceToBest(const Individual *i1, const Individual *i2, const Individual *best) {
	return (i1->getEuclideanDistanceDecisionSpace(best) < i2->getEuclideanDistanceDecisionSpace(best));
}

void CSO::sortPopulation() {
	sort(population->begin(), population->end(), sortByObj0);
}

void CSO::getSolution(MOFront* p) {
	sortPopulation();
	p->insert((*population)[0]);
}

void CSO::printInfo(ostream& os) const {
	os << "Competitive Swarm Optimiser (CSO)"  << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Population Size = " << getPopulationSize() << endl;
  os << "Gamma = " << gamma << endl;
	os << "GlobalSearchType = " << globalSearchType << endl;
}

bool sortByObj0(const Individual* i1, const Individual* i2) {
	return (i1->getInternalOptDirection(0) == MINIMIZE) ?
	       (i1->getObj(0) < i2->getObj(0)) : (i1->getObj(0) > i2->getObj(0));
}
