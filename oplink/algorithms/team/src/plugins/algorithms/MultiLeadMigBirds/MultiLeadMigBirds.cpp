/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo Gonzalez
 * 
 * DATE
 *    April 2016
 *
 **********************************************************************************/
 
#include "MultiLeadMigBirds.h"

// Algorithm initialisation
bool MultiLeadMigBirds::init(const vector<string> &params){
	// Checks for the number of parameters
	if (params.size() != 3) {
		cout << "Parametros: n k x" << endl;
		return false;
	}

	// Main parameter initialisation
	setPopulationSize(atoi(params[0].c_str()));
  nob = getPopulationSize();
  non = atoi(params[1].c_str());
  olf = atoi(params[2].c_str());

  // Remaining parameters
  noi = getEvaluations();
  contLeaders = 0;
  contNoLeaders = 0;
  leaderImproves = true;

  leaders = vector< vector<int> >(nob, vector<int>(3, -1));
  noLeaders = vector<int>(nob, -1);
  neighborSets = vector< vector<Individual*> >(nob, vector<Individual*>());
  bests = vector<Individual*>(nob, NULL);

	return true;
}

// MultiLeadMigBirds generation
void MultiLeadMigBirds::runGeneration() {
  contLeaders = 0;
  contNoLeaders = 0;

  determineFlock();

  leaderImproves = true;
  double bestSol = lookForBestInd()->getObj(0);
  
  while ((getPerformedEvaluations() < getEvaluations()) && (leaderImproves) &&
          (((*population)[0]->getOptDirection(0) == MINIMIZE) && (bestSol <= lookForBestInd()->getObj(0)) ||
          (((*population)[0]->getOptDirection(0) == MAXIMIZE) && (bestSol >= lookForBestInd()->getObj(0))))) {

    flyFlock();
    //cout << "Number of evaluations performed: " << getPerformedEvaluations() << endl;
    //cout << "Best: " << lookForBestInd()->getObj(0) << endl;
    //cout << "Worst: " << lookForWorstInd()->getObj(0) << endl;
    //cout << endl;
  }
}

void MultiLeadMigBirds::determineFlock() {
  contLeaders = 0;
  contNoLeaders = 0;

  for (int i = 0; i < nob; i++) {
    leaders[i][0] = -1;
    leaders[i][1] = -1;
    leaders[i][2] = -1;
  }
  // Minimisation
  if ((*population)[0]->getOptDirection(0) == MINIMIZE) {
    if ((*population)[0]->getObj(0) < (*population)[1]->getObj(0)) {
      leaders[contLeaders][0] = 0;
      leaders[contLeaders][1] = 0;
    } else if ((*population)[0]->getObj(0) == (*population)[1]->getObj(0)) {
      noLeaders[contNoLeaders] = 0;
      contNoLeaders++;
      leaders[contLeaders][1] = 1;
    } else if ((*population)[0]->getObj(0) > (*population)[1]->getObj(0)) {
      leaders[contLeaders][1] = 0;
    }

    for (int i = 1; i < nob - 1; i++) {
      if ((*population)[i]->getObj(0) < (*population)[i + 1]->getObj(0) && ((*population)[i]->getObj(0) < (*population)[i - 1]->getObj(0))) {
        leaders[contLeaders][0] = i;
      } else if (((*population)[i]->getObj(0) < (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) == (*population)[i + 1]->getObj(0)) {
        leaders[contLeaders][2] = i;
        leaders[contLeaders][0] = i;
        contLeaders++;
      } else if (((*population)[i]->getObj(0) == (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) < (*population)[i + 1]->getObj(0)) {
        leaders[contLeaders][0] = i;
        leaders[contLeaders][1] = i;
      } else if (((*population)[i]->getObj(0) > (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) > (*population)[i + 1]->getObj(0)) {
        leaders[contLeaders][2] = i;
        contLeaders++;
        leaders[contLeaders][1] = i;
      } else if (((*population)[i]->getObj(0) == (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) > (*population)[i + 1]->getObj(0)) {
        leaders[contLeaders][1] = i;
      } else if (((*population)[i]->getObj(0) > (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) == (*population)[i + 1]->getObj(0)) {
        leaders[contLeaders][2] = i;
        contLeaders++;
      } else if (((*population)[i]->getObj(0) == (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) == (*population)[i + 1]->getObj(0)) {
        noLeaders[contNoLeaders] = i;
        contNoLeaders++;
      }
    }

    if ((*population)[nob - 2]->getObj(0) < (*population)[nob - 1]->getObj(0)) {
      leaders[contLeaders][2] = nob - 1;
    } else if ((*population)[nob - 2]->getObj(0) == (*population)[nob - 1]->getObj(0)) {
      noLeaders[contNoLeaders] = nob - 1;
      contNoLeaders++;
      contLeaders--;
    }

    if ((*population)[nob - 2]->getObj(0) > (*population)[nob - 1]->getObj(0)) {
      leaders[contLeaders][0] = nob - 1;
      leaders[contLeaders][2] = nob - 1;
    }

  // Maximisation  
  } else if ((*population)[0]->getOptDirection(0) == MAXIMIZE) {
    if ((*population)[0]->getObj(0) > (*population)[1]->getObj(0)) {
      leaders[contLeaders][0] = 0;
      leaders[contLeaders][1] = 0;
    } else if ((*population)[0]->getObj(0) == (*population)[1]->getObj(0)) {
      noLeaders[contNoLeaders] = 0;
      contNoLeaders++;
      leaders[contLeaders][1] = 1;
    } else if ((*population)[0]->getObj(0) < (*population)[1]->getObj(0)) {
      leaders[contLeaders][1] = 0;
    }

    for (int i = 1; i < nob - 1; i++) {
      if ((*population)[i]->getObj(0) > (*population)[i + 1]->getObj(0) && ((*population)[i]->getObj(0) > (*population)[i - 1]->getObj(0))) {
        leaders[contLeaders][0] = i;
      } else if (((*population)[i]->getObj(0) > (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) == (*population)[i + 1]->getObj(0)) {
        leaders[contLeaders][2] = i;
        leaders[contLeaders][0] = i;
        contLeaders++;
      } else if (((*population)[i]->getObj(0) == (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) > (*population)[i + 1]->getObj(0)) {
        leaders[contLeaders][0] = i;
        leaders[contLeaders][1] = i;
      } else if (((*population)[i]->getObj(0) < (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) < (*population)[i + 1]->getObj(0)) {
        leaders[contLeaders][2] = i;
        contLeaders++;
        leaders[contLeaders][1] = i;
      } else if (((*population)[i]->getObj(0) == (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) < (*population)[i + 1]->getObj(0)) {
        leaders[contLeaders][1] = i;
      } else if (((*population)[i]->getObj(0) < (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) == (*population)[i + 1]->getObj(0)) {
        leaders[contLeaders][2] = i;
        contLeaders++;
      } else if (((*population)[i]->getObj(0) == (*population)[i - 1]->getObj(0)) && (*population)[i]->getObj(0) == (*population)[i + 1]->getObj(0)) {
        noLeaders[contNoLeaders] = i;
        contNoLeaders++;
      }
    }

    if ((*population)[nob - 2]->getObj(0) > (*population)[nob - 1]->getObj(0)) {
      leaders[contLeaders][2] = nob - 1;
    } else if ((*population)[nob - 2]->getObj(0) == (*population)[nob - 1]->getObj(0)) {
      noLeaders[contNoLeaders] = nob - 1;
      contNoLeaders++;
      contLeaders--;
    }

    if ((*population)[nob - 2]->getObj(0) < (*population)[nob - 1]->getObj(0)) {
      leaders[contLeaders][0] = nob - 1;
      leaders[contLeaders][2] = nob - 1;
    }
  }
}

void MultiLeadMigBirds::flyFlock() {
  //Creates neighbors of each solution
  for (int i = 0; i < bests.size(); i++) {
    bests[i] = getSampleInd()->internalClone();
    bests[i]->restart();
    if (bests[i]->getOptDirection(0) == MINIMIZE)
      bests[i]->setObj(0, DBL_MAX);
    else if (bests[i]->getOptDirection(0) == MAXIMIZE)
      bests[i]->setObj(0, DBL_MIN);
  }

  //add neighbors of leader to its immediate followers
  for (int i = 0; i <= contLeaders; i++) {
    createNeighborSet(leaders[i][0], non);

    for (int j = leaders[i][0] - 1; j >= leaders[i][1]; j--) {
      createNeighborSet(j, non - olf);
    }

    for (int j = leaders[i][0] + 1; j <= leaders[i][2]; j++) {
      createNeighborSet(j, non - olf);
    }
  }

  for (int i = 0; i < contNoLeaders; i++) {
    createNeighborSet(noLeaders[i], non);
  }
  
  for (int i = 0; i <= contLeaders; i++) {
    delete bests[leaders[i][0]];
    bests[leaders[i][0]] = getBestNeighbor(leaders[i][0], true);
  
    for (int j = leaders[i][0] - 1; j >= leaders[i][1]; j--) {
      for (int k = 0; k < olf; k++) {
        addNeighbor(j, getBestNeighbor(j + 1, true));
      }

      if (((bests[j]->getOptDirection(0) == MINIMIZE) && (getBestNeighbor(j, false)->getObj(0) < bests[j]->getObj(0))) || ((bests[j]->getOptDirection(0) == MAXIMIZE) && (getBestNeighbor(j, false)->getObj(0) > bests[j]->getObj(0)))) {
        delete bests[j];
        bests[j] = getBestNeighbor(j, true);
      }
    }

    for (int j = leaders[i][0] + 1; j <= leaders[i][2]; j++) {
      for (int k = 0; k < olf; k++) {
        addNeighbor(j, getBestNeighbor(j - 1, true));
      }
  
      if (((bests[j]->getOptDirection(0) == MINIMIZE) && (getBestNeighbor(j, false)->getObj(0) < bests[j]->getObj(0))) || ((bests[j]->getOptDirection(0) == MAXIMIZE) && (getBestNeighbor(j, false)->getObj(0) > bests[j]->getObj(0)))) {
          delete bests[j];
          bests[j] = getBestNeighbor(j, true);
      }
    }
  }

  //added worsening movements
  bool enter = false;
  for (int i = 0; i < contNoLeaders; i++) {
    delete bests[noLeaders[i]];
    bests[noLeaders[i]] = getBestNeighbor(noLeaders[i], true);
    delete (*population)[noLeaders[i]];
    (*population)[noLeaders[i]] = bests[noLeaders[i]]->internalClone();
  }

  //replace the current birds with their best neighbors, if they are better
  for (int i = 0; i < nob; i++) {
    if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) > bests[i]->getObj(0))) || (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) < bests[i]->getObj(0)))) {
      for (int j = 0; j <= contLeaders; j++)
        if (i == leaders[j][0])
          enter = true;
      delete (*population)[i];
      (*population)[i] = bests[i]->internalClone();
    }
  }
  
  if (enter == false) {
    leaderImproves = false;
  } else {
    leaderImproves = true;
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

void MultiLeadMigBirds::createNeighborSet(int ind, int nongbr) {
	for (int i = 0; i < nongbr; i++) {
		neighborSets[ind].push_back(neighborDE(ind));
	}
}

Individual* MultiLeadMigBirds::neighbor(int ind) {
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

Individual* MultiLeadMigBirds::neighbor2(int ind) {

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

Individual* MultiLeadMigBirds::neighborDE(int ind) {
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

Individual* MultiLeadMigBirds::neighborDE2(int ind) {
  double cr = 0.9;
  double f = 0.5;

  // Randomly selects three mutually exclusive integers belonging to
  // the set {0, 1, ..., NP - 1}
	int r1, r2;

 	do {
 		r1 = getRandomInteger0_N(getPopulationSize() - 1);
 	} while (r1 == ind);

 	do {
 		r2 = getRandomInteger0_N(getPopulationSize() - 1);
 	} while ((r2 == r1) || (r2 == ind));

  // Looks for the best individual in the current population
 	Individual *best = lookForBestInd();

	//Create trial
	Individual *trial = (*population)[ind]->internalClone();

	int jrand = getRandomInteger0_N(trial->getNumberOfVar() - 1);

	for(int i = 0; i < trial->getNumberOfVar(); i++) {
		if ((i == jrand) || (generateRandomDouble0_Max(1) < cr)) {
      trial->setVar(i, best->getVar(i) + f * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

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

Individual* MultiLeadMigBirds::neighborSphere(int ind) {
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

Individual* MultiLeadMigBirds::getBestNeighbor(int ind, bool remove) {
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

void MultiLeadMigBirds::addNeighbor(int ind, Individual *aux) {
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

void MultiLeadMigBirds::getSolution(MOFront *p) { 
  p->insert(lookForBestInd());
}

void MultiLeadMigBirds::printInfo(ostream &os) const {
	os << "Multi-leader Migrating Birds Optimisation"  << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Flock Size (n) = " << nob << endl;
  os << "Number of Neighbors (k) = " << non << endl;
  os << "Overlap factor (x) = " << olf << endl;
}

Individual* MultiLeadMigBirds::lookForBestInd() {
  Individual *best = (*population)[0];
  for (int i = 1; i < getPopulationSize(); i++) {
    if (((best->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) || ((best->getOptDirection(0) == MAXIMIZE && ((*population)[i]->getObj(0) > best->getObj(0)))))
      best = (*population)[i];
  }
  return best;
}

Individual* MultiLeadMigBirds::lookForWorstInd() {
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
