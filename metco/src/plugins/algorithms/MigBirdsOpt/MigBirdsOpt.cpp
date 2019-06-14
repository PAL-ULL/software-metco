/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo Gonzalez
 * 
 * DATE
 *    April 2016
 *
 **********************************************************************************/
 
#include "MigBirdsOpt.h"

// Algorithm initialisation
bool MigBirdsOpt::init(const vector<string> &params){
	// Checks for the number of parameters
	if (params.size() != 4) {
		cout << "Parametros: n k x m" << endl;
		return false;
	}

	// Main parameter initialisation
  setPopulationSize(atoi(params[0].c_str()));
  nob = getPopulationSize();
  non = atoi(params[1].c_str());
  olf = atoi(params[2].c_str());
  nof = atoi(params[3].c_str());

  // Checks if n < 3
  if (nob < 3) {
		cout << "El valor de n debe ser mayor o igual que 3" << endl;
		return false;
  }

  // Checks if k >= (2 * x) + 1
  if (non < (2 * olf + 1)) {
		cout << "El valor de k debe ser mayor o igual que (2 * x) + 1" << endl;
		return false;
  }

  // Remaining parameters
  noi = getEvaluations();
  neighborSets = vector< vector<Individual*> >(nob, vector<Individual*>());
  bests = vector<Individual*>(nob, NULL);
  leftSide = true;
	return true;
}

// MigBirdsOpt generation
void MigBirdsOpt::runGeneration() {
  double bestSol = lookForBestInd()->getObj(0);

  for(int i=0;i<nof;i++) {
    flyFlock();
  }

  replaceLeaderWithSuccessor();
}

void MigBirdsOpt::replaceLeaderWithSuccessor() {
  for (int i = 0; i < bests.size(); i++) {
    bests[i] = getSampleInd()->internalClone();
		bests[i]->restart();
	  if (bests[i]->getOptDirection(0) == MINIMIZE)
			bests[i]->setObj(0, DBL_MAX);
		else if (bests[i]->getOptDirection(0) == MAXIMIZE)
			bests[i]->setObj(0, DBL_MIN);
	}
			
  if (leftSide) {
    delete bests[0];
	  bests[0] = (*population)[1]->internalClone();
    for (int i = 1; i < nob - 2; i++) {
      if (i % 2 == 1) {
        delete bests[i];
        bests[i] = (*population)[i + 2]->internalClone();
      } else {
        delete bests[i];
        bests[i] = (*population)[i]->internalClone();
      }
    }
    // Poblacion impar -> Formacion en V alineada
    if ((nob % 2) == 1) {
      delete bests[nob - 1];
      delete bests[nob - 2];
      bests[nob - 1] = (*population)[nob - 1]->internalClone();
      bests[nob - 2] = (*population)[0]->internalClone();
    }
    // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
    else {
      delete bests[nob - 1];
      delete bests[nob - 2];
      bests[nob - 1] = (*population)[0]->internalClone();
      bests[nob - 2] = (*population)[nob - 2]->internalClone();
    }
	}
	else {
    delete bests[0];
    bests[0] = (*population)[2]->internalClone();
    for (int i = 1; i < nob - 2; i++) {
      if (i % 2 == 1) {
        delete bests[i];
        bests[i] = (*population)[i]->internalClone();
      } else {
        delete bests[i];
        bests[i] = (*population)[i + 2]->internalClone();
      }
    }
    // Poblacion impar -> Formacion en V alineada
    if ((nob % 2) == 1) {
      delete bests[nob - 1];
      delete bests[nob - 2];
      bests[nob - 1] = (*population)[0]->internalClone();
      bests[nob - 2] = (*population)[nob - 2]->internalClone();
    }
    // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
    else {
      delete bests[nob - 1];
      delete bests[nob - 2];
      bests[nob - 1] = (*population)[nob - 1]->internalClone();
      bests[nob - 2] = (*population)[0]->internalClone();
    }
  }

  // New updated population
  for (int i = 0; i < nob; i++) {
    delete (*population)[i];
    (*population)[i] = bests[i]->internalClone();
  }

  leftSide = !leftSide;

  // Free memory
  for (int i = 0; i < bests.size(); i++)
    delete bests[i];
}

void MigBirdsOpt::flyFlock() {
  for (int i = 0; i < bests.size(); i++) {
		bests[i] = getSampleInd()->internalClone();
		bests[i]->restart();
    if (bests[i]->getOptDirection(0) == MINIMIZE)
      bests[i]->setObj(0, DBL_MAX);
    else if (bests[i]->getOptDirection(0) == MAXIMIZE)
      bests[i]->setObj(0, DBL_MIN);
	}

	createNeighborSet(0, non);

	for(int i = 1; i < nob; i++) {
		createNeighborSet(i, non - olf);
	}

	delete bests[0];
	bests[0] = getBestNeighbor(0, true);

	for(int i = 0; i < 2 * olf; i++) {
		if(i % 2 == 0) {
			addNeighbor(1, getBestNeighbor(0, true));
		}
		else {
			addNeighbor(2, getBestNeighbor(0, true));
		}
	}
		
	for(int i = 1; i < nob - 2; i++) {
    delete bests[i];
		bests[i] = getBestNeighbor(i, true);
		for(int j = 0 ; j < olf; j++) {
			addNeighbor(i + 2, getBestNeighbor(i, true));
		}
	}

  delete bests[nob - 2];
  delete bests[nob - 1];
	bests[nob - 2] = getBestNeighbor(nob - 2, true);
	bests[nob - 1] = getBestNeighbor(nob - 1, true);
		
	for (int i = 0; i < nob; i++) {
    if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) > bests[i]->getObj(0))) || (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) < bests[i]->getObj(0)))) {
      delete (*population)[i];
      (*population)[i] = bests[i]->internalClone();
    }
  }

  // Free memory
  for (int i = 0; i < bests.size(); i++)
    delete bests[i];

  for (int i = 0; i < neighborSets.size(); i++) {
    for (int j = 0; j < neighborSets[i].size(); j++)
      delete neighborSets[i][j];
    neighborSets[i].clear();
  }
}

void MigBirdsOpt::createNeighborSet(int ind, int nongbr) {
	for (int i = 0; i < nongbr; i++) {
		neighborSets[ind].push_back(neighborDE(ind));
	}
}

Individual* MigBirdsOpt::neighbor(int ind) {
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

 	k = ind;

 	do {
 		r1 = getRandomInteger0_N(getPopulationSize() - 1);
 	} while (r1 == k);

 	do {
 		r2 = getRandomInteger0_N(getPopulationSize() - 1);
 	} while ((r2 == r1) || (r2 == k));

  // Looks for the best individual in the current population
 	Individual *best = lookForBestInd();

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

  return v;
}

Individual* MigBirdsOpt::neighbor2(int ind) {

  Individual *aux = (*population)[ind]->internalClone();
  bool improvement = true;
  double bestObj;

  if (aux->getOptDirection(0) == MINIMIZE)
    bestObj = DBL_MAX;
  else if (aux->getOptDirection(0) == MAXIMIZE)
    bestObj = DBL_MIN;

  do {
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

 	  k = ind;

 	  do {
 	  	r1 = getRandomInteger0_N(getPopulationSize() - 1);
 	  } while (r1 == k);

 	  do {
 	  	r2 = getRandomInteger0_N(getPopulationSize() - 1);
 	  } while ((r2 == r1) || (r2 == k));

    // Looks for the best individual in the current population
 	  Individual *best = lookForBestInd();

    // Applies global neighbourhood search strategy
    Individual *v = getSampleInd()->internalClone();
    for (int i = 0; i < v->getNumberOfVar(); i++) {
      v->setVar(i, a1 * aux->getVar(i) + a2 * best->getVar(i) + a3 * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));
  
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

    if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < bestObj)) || ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > bestObj))) {
      improvement = true;
      bestObj = v->getObj(0);
      delete aux;
      aux = v;
    }
    else {
      improvement = false;
      delete v;
    }
  } while (improvement);

  return aux;
}

Individual* MigBirdsOpt::neighborSphere(int ind) {
  //parametros: radio
  double radio = 1e-6;
  int a1 = 0;
  int k = ind;
  Individual *v = getSampleInd()->internalClone();
  double angulo[v->getNumberOfVar()];

  do {
    angulo[a1] = ((double) rand () / (double) RAND_MAX) * 3.1415;
    a1++;
  } while (a1 != v->getNumberOfVar() - 1);

  angulo[v->getNumberOfVar() - 1] = ((double) rand () / (double) RAND_MAX) * 2 * 3.1415;

  for (int i = 0; i < v->getNumberOfVar(); i++) {
  	double value = radio;

	  for (int j = 0; j < i; j++) {
      value *= sin(angulo[j]);
	  } 
	  value*= cos(i);
	   
    v->setVar(i, value + (*population)[k]->getVar(i));

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

  return v;
}

Individual* MigBirdsOpt::neighborDE(int ind) {
  double f = 0.5;
  double cr = 0.9;

	//Prepare values for any of the trial vector generation strategies
	int r1, r2, r3;

	do{
		r1 = getRandomInteger0_N(getPopulationSize() - 1);
	} while(r1==ind);

	do {
		r2 = getRandomInteger0_N(getPopulationSize() - 1);
	} while((r2== r1) || (r2==ind));

	do {
		r3 = getRandomInteger0_N(getPopulationSize() - 1);
	} while((r3== r2) || (r3 == r1) || (r3==ind));

	//Create trial
	Individual *trial = (*population)[ind]->internalClone();

	int jrand = getRandomInteger0_N(trial->getNumberOfVar() - 1);

	for(int i = 0; i < trial->getNumberOfVar(); i++) {
		if ((i == jrand) || (generateRandomDouble0_Max(1) < cr)) {
		  trial->setVar(i, (*population)[r1]->getVar(i) + f * ((*population)[r2]->getVar(i) - (*population)[r3]->getVar(i)));

	    //check boundary
	    if ((trial->getVar(i) < trial->getMinimum(i)) || (trial->getVar(i) > trial->getMaximum(i))){
			  double r = ((double) rand () / (double) RAND_MAX);
			  trial->setVar(i, r * (trial->getMaximum(i) - trial->getMinimum(i)) + trial->getMinimum(i));
		  }

	    trial->setVar(i, max(trial->getVar(i), trial->getMinimum(i)));
	    trial->setVar(i, min(trial->getVar(i), trial->getMaximum(i)));
		}
	}
	
  evaluate(trial);
  return trial;
}

Individual* MigBirdsOpt::getBestNeighbor(int ind, bool remove) {
  //cout << "Entrando en getBestNeighbor" << endl;
  //cout << "ind: " << ind << endl;
  //cout << "remove: " << remove << endl;
  sort(neighborSets[ind].begin(), neighborSets[ind].end(), sortByObj);

  //for (int i = 0; i < neighborSets[ind].size(); i++) {
  //  cout << neighborSets[ind][i]->getObj(0) << " ";
  //}
  //cout << endl;

  Individual* best;

  if (remove) {
    best = neighborSets[ind][0]->internalClone();
    delete neighborSets[ind][0];
    neighborSets[ind].erase(neighborSets[ind].begin());
  }
  else {
    best = neighborSets[ind][0];
  }

  //for (int i = 0; i < neighborSets[ind].size(); i++) {
  //  cout << neighborSets[ind][i]->getObj(0) << " ";
  //}
  //cout << endl;
  //cout << "best: " << best->getObj(0) << endl;
  //cout << "Saliendo de getBestNeighbor" << endl;
  return best;
}

void MigBirdsOpt::addNeighbor(int ind, Individual *aux) {
  //cout << "Entrando en addNeighbor" << endl;
  //cout << "ind: " << ind << endl;
  //cout << "aux: " << aux->getObj(0) << endl;

  //for (int i = 0; i < neighborSets[ind].size(); i++) {
  //  cout << neighborSets[ind][i]->getObj(0) << " ";
  //}
  //cout << endl;

  neighborSets[ind].push_back(aux);
  
  //for (int i = 0; i < neighborSets[ind].size(); i++) {
  //  cout << neighborSets[ind][i]->getObj(0) << " ";
  //}
  //cout << endl;
  //cout << "Saliendo de addNeighbor" << endl;
}

void MigBirdsOpt::getSolution(MOFront *p) { 
  p->insert(lookForBestInd());
}

void MigBirdsOpt::printInfo(ostream &os) const {
	os << "Migrating Birds Optimisation"  << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Flock Size (n) = " << nob << endl;
  os << "Number of Neighbors (k) = " << non << endl;
  os << "Overlap factor (x) = " << olf << endl;
  os << "Number of flights (m) = " << olf << endl;
}

Individual* MigBirdsOpt::lookForBestInd() {
  Individual *best = (*population)[0];
  for (int i = 1; i < getPopulationSize(); i++) {
    if (((best->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) || ((best->getOptDirection(0) == MAXIMIZE && ((*population)[i]->getObj(0) > best->getObj(0)))))
      best = (*population)[i];
  }
  return best;
}

Individual* MigBirdsOpt::lookForWorstInd() {
  Individual *worst = (*population)[0];
  for (int i = 1; i < getPopulationSize(); i++) {
    if (((worst->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) > worst->getObj(0))) || ((worst->getOptDirection(0) == MAXIMIZE && ((*population)[i]->getObj(0) < worst->getObj(0)))))
      worst = (*population)[i];
  }
  return worst;
}

bool sortByObj(const Individual *i1, const Individual *i2){
	return (i1->getInternalOptDirection(0) == MINIMIZE)?(i1->getObj(0) < i2->getObj(0)):(i1->getObj(0) > i2->getObj(0));
}
