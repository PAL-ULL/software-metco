/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo Gonzalez
 * 
 * DATE
 *    April 2016
 *
 **********************************************************************************/
 
#include "MigBirdsOptMod.h"

struct extIndType {
  int id;
  Individual* ind;
};

bool sortByObj(const Individual *i1, const Individual *i2){
	return (i1->getInternalOptDirection(0) == MINIMIZE)?(i1->getObj(0) < i2->getObj(0)):(i1->getObj(0) > i2->getObj(0));
}

void readParameter(map<string,string> &parameters, const string &parameter, double &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MigBirdsOptMod no definido" << endl;
		exit(-1);
	}
	value = atof(parameters[parameter].c_str());
	parameters.erase(parameter);
}

void readParameter(map<string,string> &parameters, const string &parameter, int &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MigBirdsOptMod no definido" << endl;
		exit(-1);
	}
	value = atoi(parameters[parameter].c_str());
	parameters.erase(parameter);
}

void readParameter(map<string,string> &parameters, const string &parameter, string &value){
	if (parameters.count(parameter) == 0){
		cerr << "Parametro " << parameter << " de MigBirdsOptMod no definido" << endl;
		exit(-1);
	}
	value = parameters[parameter];
	parameters.erase(parameter);
}

// Algorithm initialisation
bool MigBirdsOptMod::init(const vector<string> &params) {
	if ((params.size() % 2) != 0) {
		cerr << "Error en los parametros de MigBirdsOptMod. Deben ser multiplos de 2" << endl;
    cerr << "Current number of parameters: " << params.size() << endl;
		return false;
	}
	
  // Transforms parameter vector into a parameter map	
	map<string, string> parameters;
	for (int i = 0; i < params.size(); i += 2) {
		parameters[params[i]] = params[i + 1];
	}
	
  readParameter(parameters, "n", nob);
  // Checks if n < 3
  if (nob < 3) {
		cout << "El valor de n debe ser mayor o igual que 3" << endl;
		return false;
  }
	setPopulationSize(nob);
  
  readParameter(parameters, "k", non);
  readParameter(parameters, "x", olf);
  // Checks if k >= (2 * x) + 1
  if (non < (2 * olf + 1)) {
		cout << "El valor de k debe ser mayor o igual que (2 * x) + 1" << endl;
		return false;
  }

  readParameter(parameters, "m", nof);

  // Global search
  string globalSearchTypeStr;
  globalSearchRequired = false;
  readParameter(parameters, "GlobalSearchType", globalSearchTypeStr);
  if (globalSearchTypeStr == "No") {
    globalSearchType = GS_NO;
    globalSearchRequired= false;
  } else if (globalSearchTypeStr == "GlobalNeighbourhood") {
    globalSearchType = GS_GLOBAL_NEIGHBOURHOOD;
    globalSearchRequired = true;
  } else if (globalSearchTypeStr == "GlobalNeighbourhoodPathSearch") {
    globalSearchType = GS_GLOBAL_NEIGHBOURHOOD_PATH_SEARCH;
    globalSearchRequired = true;
  } else {
    cout << "Valor de GlobalSearchType no reconocido: " << globalSearchTypeStr << endl;
    exit(-1);
  }

  // Remaining parameters
  neighborSets = vector< vector<Individual*> >(nob, vector<Individual*>());
  bests = vector<Individual*>(nob, NULL);
  leftSide = true;
  rightSideFixed = true;
  currentLeaderSuccessor = 2;

  mut_f = 0.5;
  cross_cr = 0.9;

  nuF = 0.5;
  nuCR = 0.5;
  c = 0.1;
  fValues.insert(fValues.begin(), getPopulationSize(), 0);
  crValues.insert(crValues.begin(), getPopulationSize(), 0);

  neighbor_fValues = vector< vector<double> >(nob, vector<double>());
  neighbor_crValues = vector< vector<double> >(nob, vector<double>());

  p = 0.05;
  accepted = p * getPopulationSize();
	if (accepted < 1) {
		cout << "Error: the value of p is too small" << endl;
		exit(-1);
	}
  
	return true;
}

// MigBirdsOptMod generation
void MigBirdsOptMod::runGeneration() {

  for(int i = 0; i < nof; i++) {
    flyFlock();
    //updateNuF();
    updateNuCR();
  }

  replaceLeaderWithSuccessor12();

  if (globalSearchRequired) {
    globalSearch();
  }
}

void MigBirdsOptMod::replaceLeaderWithSuccessorOrig() {
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

void MigBirdsOptMod::replaceLeaderWithSuccessor() {
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
      delete bests[i];
      bests[i] = (*population)[i]->internalClone();
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

void MigBirdsOptMod::replaceLeaderWithSuccessor2() {
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
      bests[nob - 1] = (*population)[0]->internalClone();
      bests[nob - 2] = (*population)[1]->internalClone();
    }
    // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
    else {
      delete bests[nob - 1];
      delete bests[nob - 2];
      bests[nob - 1] = (*population)[1]->internalClone();
      bests[nob - 2] = (*population)[0]->internalClone();
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
      bests[nob - 1] = (*population)[2]->internalClone();
      bests[nob - 2] = (*population)[0]->internalClone();
    }
    // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
    else {
      delete bests[nob - 1];
      delete bests[nob - 2];
      bests[nob - 1] = (*population)[0]->internalClone();
      bests[nob - 2] = (*population)[2]->internalClone();
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

void MigBirdsOptMod::replaceLeaderWithSuccessor3() {
  for (int i = 0; i < bests.size(); i++) {
    bests[i] = getSampleInd()->internalClone();
		bests[i]->restart();
	  if (bests[i]->getOptDirection(0) == MINIMIZE)
			bests[i]->setObj(0, DBL_MAX);
		else if (bests[i]->getOptDirection(0) == MAXIMIZE)
			bests[i]->setObj(0, DBL_MIN);
	}
	
	if ((rightSideFixed)) {
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
      leftSide = !leftSide;
  	}
  	else {
      delete bests[0];
      bests[0] = (*population)[2]->internalClone();
      for (int i = 1; i < nob - 2; i++) {
        delete bests[i];
        bests[i] = (*population)[i]->internalClone();
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
      leftSide = !leftSide;
      rightSideFixed = !rightSideFixed;
    }
  }
	else {
    if (leftSide) {
      delete bests[0];
	    bests[0] = (*population)[1]->internalClone();
      for (int i = 1; i < nob - 2; i++) {
        delete bests[i];
        bests[i] = (*population)[i]->internalClone();
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
      leftSide = !leftSide;
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
      leftSide = !leftSide;
      rightSideFixed = !rightSideFixed;
    }
  }

  // New updated population
  for (int i = 0; i < nob; i++) {
    delete (*population)[i];
    (*population)[i] = bests[i]->internalClone();
  }

  // Free memory
  for (int i = 0; i < bests.size(); i++)
    delete bests[i];
}

void MigBirdsOptMod::replaceLeaderWithSuccessor4() {
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
        // Herein population should be indexed by i
        bests[i] = (*population)[i + 2]->internalClone();
      } else {
        delete bests[i];
        // Herein population should be indexed by i + 2
        bests[i] = (*population)[i]->internalClone();
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

void MigBirdsOptMod::replaceLeaderWithSuccessor5() {
  for (int i = 0; i < bests.size(); i++) {
    bests[i] = getSampleInd()->internalClone();
		bests[i]->restart();
	  if (bests[i]->getOptDirection(0) == MINIMIZE)
			bests[i]->setObj(0, DBL_MAX);
		else if (bests[i]->getOptDirection(0) == MAXIMIZE)
			bests[i]->setObj(0, DBL_MIN);
	}
			
	if ((rightSideFixed)) {
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
      leftSide = !leftSide;
  	}
  	else {
      delete bests[0];
      bests[0] = (*population)[2]->internalClone();
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
      leftSide = !leftSide;
      rightSideFixed = !rightSideFixed;
    }
  }
  else {
    if (leftSide) {
      delete bests[0];
	    bests[0] = (*population)[1]->internalClone();
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
      leftSide = !leftSide;
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
      leftSide = !leftSide;
      rightSideFixed = !rightSideFixed;
    }
  }

  // New updated population
  for (int i = 0; i < nob; i++) {
    delete (*population)[i];
    (*population)[i] = bests[i]->internalClone();
  }

  // Free memory
  for (int i = 0; i < bests.size(); i++)
    delete bests[i];
}

void MigBirdsOptMod::replaceLeaderWithSuccessor6() {
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
      delete bests[i];
      bests[i] = (*population)[i]->internalClone();
    }
    // Poblacion impar -> Formacion en V alineada
    if ((nob % 2) == 1) {
      delete bests[nob - 1];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 1]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 1]->getObj(0))))
        bests[nob - 1] = (*population)[0]->internalClone();
      else
        bests[nob - 1] = (*population)[nob - 1]->internalClone();

      delete bests[nob - 2];
      bests[nob - 2] = (*population)[nob - 2]->internalClone();
    }
    // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
    else {
      delete bests[nob - 1];
      bests[nob - 1] = (*population)[nob - 1]->internalClone();

      delete bests[nob - 2];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 2]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 2]->getObj(0))))
        bests[nob - 2] = (*population)[0]->internalClone();
      else
        bests[nob - 2] = (*population)[nob - 2]->internalClone();
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

void MigBirdsOptMod::replaceLeaderWithSuccessor7() {
  for (int i = 0; i < bests.size(); i++) {
    bests[i] = getSampleInd()->internalClone();
		bests[i]->restart();
	  if (bests[i]->getOptDirection(0) == MINIMIZE)
			bests[i]->setObj(0, DBL_MAX);
		else if (bests[i]->getOptDirection(0) == MAXIMIZE)
			bests[i]->setObj(0, DBL_MIN);
	}
		
  if (rightSideFixed) {
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
      leftSide = !leftSide;
  	}
  	else {
      delete bests[0];
      bests[0] = (*population)[2]->internalClone();
      for (int i = 1; i < nob - 2; i++) {
        delete bests[i];
        bests[i] = (*population)[i]->internalClone();
      }
      // Poblacion impar -> Formacion en V alineada
      if ((nob % 2) == 1) {
        delete bests[nob - 1];
        if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 1]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 1]->getObj(0))))
          bests[nob - 1] = (*population)[0]->internalClone();
        else
          bests[nob - 1] = (*population)[nob - 1]->internalClone();

        delete bests[nob - 2];
        bests[nob - 2] = (*population)[nob - 2]->internalClone();
      }
      // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
      else {
        delete bests[nob - 1];
        bests[nob - 1] = (*population)[nob - 1]->internalClone();

        delete bests[nob - 2];
        if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 2]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 2]->getObj(0))))
          bests[nob - 2] = (*population)[0]->internalClone();
        else
          bests[nob - 2] = (*population)[nob - 2]->internalClone();
      }
      leftSide = !leftSide;
      rightSideFixed = !rightSideFixed;
    }
  }
  else {
    if (leftSide) {
      delete bests[0];
	    bests[0] = (*population)[1]->internalClone();
      for (int i = 1; i < nob - 2; i++) {
        delete bests[i];
        bests[i] = (*population)[i]->internalClone();
      }
      // Poblacion impar -> Formacion en V alineada
      if ((nob % 2) == 1) {
        delete bests[nob - 1];
        bests[nob - 1] = (*population)[nob - 1]->internalClone();

        delete bests[nob - 2];
        if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 2]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 2]->getObj(0))))
          bests[nob - 2] = (*population)[0]->internalClone();
        else
          bests[nob - 2] = (*population)[nob - 2]->internalClone();

      }
      // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
      else {
        delete bests[nob - 1];
        if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 1]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 1]->getObj(0))))
          bests[nob - 1] = (*population)[0]->internalClone();
        else
          bests[nob - 1] = (*population)[nob - 1]->internalClone();

        delete bests[nob - 2];
        bests[nob - 2] = (*population)[nob - 2]->internalClone();
      }
      leftSide = !leftSide;
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
      leftSide = !leftSide;
      rightSideFixed = !rightSideFixed;
    }
  }
    
  // New updated population
  for (int i = 0; i < nob; i++) {
    delete (*population)[i];
    (*population)[i] = bests[i]->internalClone();
  }

  // Free memory
  for (int i = 0; i < bests.size(); i++)
    delete bests[i];
}

void MigBirdsOptMod::replaceLeaderWithSuccessor8() {
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
    bests[0] = (*population)[currentLeaderSuccessor]->internalClone();

    if (nob % 2 == 1)
      currentLeaderSuccessor = (currentLeaderSuccessor + 2) % (nob + 1);
    else
      currentLeaderSuccessor = (currentLeaderSuccessor + 2) % nob;

    if (currentLeaderSuccessor == 0)
      currentLeaderSuccessor = 2;

    for (int i = 1; i < nob - 2; i++) {
      delete bests[i];
      bests[i] = (*population)[i]->internalClone();
    }

    // Poblacion impar -> Formacion en V alineada
    if ((nob % 2) == 1) {
      delete bests[nob - 1];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 1]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 1]->getObj(0))))
        bests[nob - 1] = (*population)[0]->internalClone();
      else
        bests[nob - 1] = (*population)[nob - 1]->internalClone();

      delete bests[nob - 2];
      bests[nob - 2] = (*population)[nob - 2]->internalClone();
    }
    // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
    else {
      delete bests[nob - 1];
      bests[nob - 1] = (*population)[nob - 1]->internalClone();

      delete bests[nob - 2];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 2]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 2]->getObj(0))))
        bests[nob - 2] = (*population)[0]->internalClone();
      else
        bests[nob - 2] = (*population)[nob - 2]->internalClone();
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

void MigBirdsOptMod::replaceLeaderWithSuccessor9() {
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
      delete bests[i];
      bests[i] = (*population)[i]->internalClone();
    }
    // Poblacion impar -> Formacion en V alineada
    if ((nob % 2) == 1) {
      delete bests[nob - 1];
      bests[nob - 1] = (*population)[nob - 1]->internalClone();

      delete bests[nob - 2];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 2]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 2]->getObj(0))))
        bests[nob - 2] = (*population)[0]->internalClone();
      else
        bests[nob - 2] = (*population)[nob - 2]->internalClone();
    }
    // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
    else {
      delete bests[nob - 1];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 1]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 1]->getObj(0))))
        bests[nob - 1] = (*population)[0]->internalClone();
      else
        bests[nob - 1] = (*population)[nob - 1]->internalClone();
      
      delete bests[nob - 2];
      bests[nob - 2] = (*population)[nob - 2]->internalClone();
    }
	}
	else {
    delete bests[0];
    bests[0] = (*population)[2]->internalClone();
    for (int i = 1; i < nob - 2; i++) {
      delete bests[i];
      bests[i] = (*population)[i]->internalClone();
    }
    // Poblacion impar -> Formacion en V alineada
    if ((nob % 2) == 1) {
      delete bests[nob - 1];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 1]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 1]->getObj(0))))
        bests[nob - 1] = (*population)[0]->internalClone();
      else
        bests[nob - 1] = (*population)[nob - 1]->internalClone();

      delete bests[nob - 2];
      bests[nob - 2] = (*population)[nob - 2]->internalClone();
    }
    // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
    else {
      delete bests[nob - 1];
      bests[nob - 1] = (*population)[nob - 1]->internalClone();

      delete bests[nob - 2];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 2]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 2]->getObj(0))))
        bests[nob - 2] = (*population)[0]->internalClone();
      else
        bests[nob - 2] = (*population)[nob - 2]->internalClone();
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

void MigBirdsOptMod::replaceLeaderWithSuccessor10() {
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
    bests[0] = (*population)[currentLeaderSuccessor]->internalClone();

    for (int i = 1; i < nob; i++) {
      delete bests[i];
      bests[i] = (*population)[i]->internalClone();
    }

    // Checks if previous leader is better than the current one. If so, the former replaces the latter. Otherwise, the former is discarded.

    if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[currentLeaderSuccessor]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[currentLeaderSuccessor]->getObj(0)))) {
      delete bests[currentLeaderSuccessor];
      bests[currentLeaderSuccessor] = (*population)[0]->internalClone();
    }

    // Updates the position of the next generation leader
    if (nob % 2 == 1)
      currentLeaderSuccessor = (currentLeaderSuccessor + 2) % (nob + 1);
    else
      currentLeaderSuccessor = (currentLeaderSuccessor + 2) % nob;

    if (currentLeaderSuccessor == 0)
      currentLeaderSuccessor = 2;
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

void MigBirdsOptMod::replaceLeaderWithSuccessor11() {
  for (int i = 0; i < bests.size(); i++) {
    bests[i] = getSampleInd()->internalClone();
		bests[i]->restart();
	  if (bests[i]->getOptDirection(0) == MINIMIZE)
			bests[i]->setObj(0, DBL_MAX);
		else if (bests[i]->getOptDirection(0) == MAXIMIZE)
			bests[i]->setObj(0, DBL_MIN);
	}

  delete bests[0];
  bests[0] = (*population)[1]->internalClone();
  
  for (int i = 1; i < nob - 2; i++) {
    delete bests[i];
    if ((i % 2) == 1)
      bests[i] = (*population)[i + 2]->internalClone();
    else
      bests[i] = (*population)[i - 2]->internalClone();
  }

  // Poblacion impar -> Formacion en V alineada
  if ((nob % 2) == 1) {
    delete bests[nob - 1];
    delete bests[nob - 2];
    bests[nob - 1] = (*population)[nob - 3]->internalClone();
    bests[nob - 2] = (*population)[nob - 1]->internalClone();
  }
  // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
  else {
    delete bests[nob - 1];
    delete bests[nob - 2];
    bests[nob - 1] = (*population)[nob - 2]->internalClone();
    bests[nob - 2] = (*population)[nob - 4]->internalClone();
  }

  // New updated population
  for (int i = 0; i < nob; i++) {
    delete (*population)[i];
    (*population)[i] = bests[i]->internalClone();
  }

  // Free memory
  for (int i = 0; i < bests.size(); i++)
    delete bests[i];
}

void MigBirdsOptMod::replaceLeaderWithSuccessor12() {
  for (int i = 0; i < bests.size(); i++) {
    bests[i] = getSampleInd()->internalClone();
		bests[i]->restart();
	  if (bests[i]->getOptDirection(0) == MINIMIZE)
			bests[i]->setObj(0, DBL_MAX);
		else if (bests[i]->getOptDirection(0) == MAXIMIZE)
			bests[i]->setObj(0, DBL_MIN);
	}

  // Looks for the best direct successor:
  // Left successor is better
  if ((((*population)[1]->getOptDirection(0) == MINIMIZE) && ((*population)[1]->getObj(0) < (*population)[2]->getObj(0))) || (((*population)[1]->getOptDirection(0) == MAXIMIZE) && ((*population)[1]->getObj(0) > (*population)[2]->getObj(0)))) {
    // Left side
    delete bests[0];
	  bests[0] = (*population)[1]->internalClone();

    // Right Side
    for (int i = 1; i < nob - 2; i++) {
      if (i % 2 == 1) {
        delete bests[i];
        bests[i] = (*population)[i]->internalClone();
      } else {
        delete bests[i];
        bests[i] = (*population)[i + 2]->internalClone();
      }
    }

    // Latest individuals
    // Poblacion impar -> Formacion en V alineada
    if ((nob % 2) == 1) {
      delete bests[nob - 1];
      bests[nob - 1] = (*population)[2]->internalClone();

      delete bests[nob - 2];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 2]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 2]->getObj(0))))
        bests[nob - 2] = (*population)[0]->internalClone();
      else
        bests[nob - 2] = (*population)[nob - 2]->internalClone();
    }
    // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
    else {
      delete bests[nob - 1];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 1]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 1]->getObj(0))))
        bests[nob - 1] = (*population)[0]->internalClone();
      else
        bests[nob - 1] = (*population)[nob - 1]->internalClone();

      delete bests[nob - 2];
      bests[nob - 2] = (*population)[2]->internalClone();
    }
  }
  // Right successor is better
  else {
    // Right side
    delete bests[0];
	  bests[0] = (*population)[2]->internalClone();

    // Left Side
    for (int i = 1; i < nob - 2; i++) {
      if (i % 2 == 1) {
        delete bests[i];
        bests[i] = (*population)[i + 2]->internalClone();
      } else {
        delete bests[i];
        bests[i] = (*population)[i]->internalClone();
      }
    }

    // Latest individuals
    // Poblacion impar -> Formacion en V alineada
    if ((nob % 2) == 1) {
      delete bests[nob - 1];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 1]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 1]->getObj(0))))
        bests[nob - 1] = (*population)[0]->internalClone();
      else
        bests[nob - 1] = (*population)[nob - 1]->internalClone();
      
      delete bests[nob - 2];
      bests[nob - 2] = (*population)[1]->internalClone();
    }
    // Poblacion par -> Formacion en V con un ind. mas por la izda. al final de la misma
    else {
      delete bests[nob - 1];
      bests[nob - 1] = (*population)[1]->internalClone();

      delete bests[nob - 2];
      if ((((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[0]->getObj(0) < (*population)[nob - 2]->getObj(0))) || (((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[0]->getObj(0) > (*population)[nob - 2]->getObj(0))))
        bests[nob - 2] = (*population)[0]->internalClone();
      else
        bests[nob - 2] = (*population)[nob - 2]->internalClone();
    }
  }

  // New updated population
  for (int i = 0; i < nob; i++) {
    delete (*population)[i];
    (*population)[i] = bests[i]->internalClone();
  }

  // Free memory
  for (int i = 0; i < bests.size(); i++)
    delete bests[i];
}

void MigBirdsOptMod::flyFlock() {
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

  double f, cr;

	delete bests[0];
	bests[0] = getBestNeighbor(0, true, f, cr);
  fValues[0] = f;
  crValues[0] = cr;

	for(int i = 0; i < 2 * olf; i++) {
		if(i % 2 == 0) {
      Individual *ind = getBestNeighbor(0, true, f, cr);
			addNeighbor(1, ind, f, cr);
		}
		else {
      Individual *ind = getBestNeighbor(0, true, f, cr);
			addNeighbor(2, ind, f, cr);
		}
	}

	for(int i = 1; i < nob - 2; i++) {
    delete bests[i];
		bests[i] = getBestNeighbor(i, true, f, cr);
    fValues[i] = f;
    crValues[i] = cr;
		for(int j = 0 ; j < olf; j++) {
      Individual *ind = getBestNeighbor(i, true, f, cr);
			addNeighbor(i + 2, ind, f, cr);
		}
	}

  delete bests[nob - 2];
  delete bests[nob - 1];
	bests[nob - 2] = getBestNeighbor(nob - 2, true, f, cr);
  fValues[nob - 2] = f;
  crValues[nob - 2] = cr;
	bests[nob - 1] = getBestNeighbor(nob - 1, true, f, cr);
  fValues[nob - 1] = f;
  crValues[nob - 1] = cr;
		
	for (int i = 0; i < nob; i++) {
    if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) > bests[i]->getObj(0))) || (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) < bests[i]->getObj(0)))) {
      delete (*population)[i];
      (*population)[i] = bests[i]->internalClone();
      successfullFs.push_back(fValues[i]);
      successfullCRs.push_back(crValues[i]);
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

  for (int i = 0; i < neighbor_fValues.size(); i++)
    neighbor_fValues[i].clear();
  for (int i = 0; i < neighbor_crValues.size(); i++)
    neighbor_crValues[i].clear();
}

void MigBirdsOptMod::createNeighborSet(int ind, int nongbr) {
	for (int i = 0; i < nongbr; i++) {
    generateF(ind);
    generateCR(ind);

		neighborSets[ind].push_back(neighborDE_Rand_Adapt(ind));
    neighbor_fValues[ind].push_back(fValues[ind]);
    neighbor_crValues[ind].push_back(crValues[ind]);
	}
}

Individual* MigBirdsOptMod::neighborDE_Rand_Fixed(int ind) {
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
		if ((i == jrand) || (generateRandomDouble0_Max(1) < cross_cr)) {
		  trial->setVar(i, (*population)[r1]->getVar(i) + mut_f * ((*population)[r2]->getVar(i) - (*population)[r3]->getVar(i)));

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

Individual* MigBirdsOptMod::neighborDE_Rand_Adapt(int ind) {
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
		if ((i == jrand) || (generateRandomDouble0_Max(1) < crValues[ind])) {
		  trial->setVar(i, (*population)[r1]->getVar(i) + fValues[ind] * ((*population)[r2]->getVar(i) - (*population)[r3]->getVar(i)));

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
    
Individual* MigBirdsOptMod::neighborDE_CurrentToPBest_Adapt(int ind) {
  // Gets one of the best accepted (p * popSize) individuals at random
  Individual* randomBest = getBestIndividual0_N(getRandomInteger0_N(accepted - 1));

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
		if ((i == jrand) || (generateRandomDouble0_Max(1) < crValues[ind])) {
		  trial->setVar(i, (*population)[ind]->getVar(i) + fValues[ind] * (randomBest->getVar(i) - (*population)[ind]->getVar(i)) + fValues[ind] * ((*population)[r1]->getVar(i) - (*population)[r2]->getVar(i)));

	    //check boundary
	    if ((trial->getVar(i) < trial->getMinimum(i)) || (trial->getVar(i) > trial->getMaximum(i))) {
			  double r = ((double) rand () / (double) RAND_MAX);
			  trial->setVar(i, r * (trial->getMaximum(i) - trial->getMinimum(i)) + trial->getMinimum(i));
		  }

	    trial->setVar(i, max(trial->getVar(i), trial->getMinimum(i)));
	    trial->setVar(i, min(trial->getVar(i), trial->getMaximum(i)));
		}
	}

  delete randomBest;
  evaluate(trial);
  return trial;
}

Individual* MigBirdsOptMod::neighborSphere(int ind) {
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

Individual* MigBirdsOptMod::getBestNeighbor(int ind, bool remove, double &f, double &cr) {
//  cout << "Entrando en getBestNeighbor" << endl;
//  cout << "ind: " << ind << endl;
//  cout << "remove: " << remove << endl;

//  for (int i = 0; i < neighborSets[ind].size(); i++) {
//    cout << neighborSets[ind][i]->getObj(0) << " ";
//  }
//  cout << endl;

  Individual* best;
  double bestValue;
  int index = 0;

	if (getSampleInd()->getOptDirection(0) == MINIMIZE)
	  bestValue = DBL_MAX;
	else if (getSampleInd()->getOptDirection(0) == MAXIMIZE)
	  bestValue = DBL_MIN;

  for (int i = 0; i < neighborSets[ind].size(); i++) {
    if (((neighborSets[ind][i]->getOptDirection(0) == MINIMIZE) && (neighborSets[ind][i]->getObj(0) < bestValue)) || ((neighborSets[ind][i]->getOptDirection(0) == MAXIMIZE) && (neighborSets[ind][i]->getObj(0) > bestValue))) {
      bestValue = neighborSets[ind][i]->getObj(0);
      index = i;
    }
  }

  if (remove) {
    f = neighbor_fValues[ind][index];
    cr = neighbor_crValues[ind][index];
    best = neighborSets[ind][index]->internalClone();

    delete neighborSets[ind][index];
    neighborSets[ind].erase(neighborSets[ind].begin() + index);
    neighbor_fValues[ind].erase(neighbor_fValues[ind].begin() + index);
    neighbor_crValues[ind].erase(neighbor_crValues[ind].begin() + index);
  }
  else {
    f = neighbor_fValues[ind][index];
    cr = neighbor_crValues[ind][index];
    best = neighborSets[ind][index];
  }

//  for (int i = 0; i < neighborSets[ind].size(); i++) {
//    cout << neighborSets[ind][i]->getObj(0) << " ";
//  }
//  cout << endl;
//  cout << "best: " << best->getObj(0) << endl;
//  cout << "F: " << f << endl;
//  cout << "CR: " << cr << endl;
//  cout << "Saliendo de getBestNeighbor" << endl;
  return best;
}

void MigBirdsOptMod::addNeighbor(int ind, Individual *aux, double f, double cr) {
//  cout << "Entrando en addNeighbor" << endl;
//  cout << "ind: " << ind << endl;
//  cout << "aux: " << aux->getObj(0) << endl;
//  cout << "F: " << f << endl;
//  cout << "CR: " << cr << endl;

//  for (int i = 0; i < neighborSets[ind].size(); i++) {
//    cout << neighborSets[ind][i]->getObj(0) << " ";
//  }
//  cout << endl;

  neighborSets[ind].push_back(aux);
  neighbor_fValues[ind].push_back(f);
  neighbor_crValues[ind].push_back(cr);
  
//  for (int i = 0; i < neighborSets[ind].size(); i++) {
//    cout << neighborSets[ind][i]->getObj(0) << " ";
//  }
//  cout << endl;
//  cout << "Saliendo de addNeighbor" << endl;
}

void MigBirdsOptMod::getSolution(MOFront *p) { 
  p->insert(lookForBestInd());
}

void MigBirdsOptMod::printInfo(ostream &os) const {
	os << "Migrating Birds Optimisation"  << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Flock Size (n) = " << nob << endl;
  os << "Number of Neighbors (k) = " << non << endl;
  os << "Overlap factor (x) = " << olf << endl;
  os << "Number of flights (m) = " << nof << endl;
}

Individual* MigBirdsOptMod::lookForBestInd() {
  Individual *best = (*population)[0];
  for (int i = 1; i < getPopulationSize(); i++) {
    if (((best->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) || ((best->getOptDirection(0) == MAXIMIZE && ((*population)[i]->getObj(0) > best->getObj(0)))))
      best = (*population)[i];
  }
  return best;
}

Individual* MigBirdsOptMod::lookForWorstInd() {
  Individual *worst = (*population)[0];
  for (int i = 1; i < getPopulationSize(); i++) {
    if (((worst->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) > worst->getObj(0))) || ((worst->getOptDirection(0) == MAXIMIZE && ((*population)[i]->getObj(0) < worst->getObj(0)))))
      worst = (*population)[i];
  }
  return worst;
}
  
Individual* MigBirdsOptMod::getBestIndividual0_N(int bestp) {
  vector<Individual*> aux;
  for (int i = 0; i < getPopulationSize(); i++)
    aux.push_back((*population)[i]);

  sort(aux.begin(), aux.end(), sortByObj);

  Individual* bestInd = aux[bestp]->internalClone();
  aux.clear();

  return bestInd;
}

void MigBirdsOptMod::generateF(int ind) {
  do {
    fValues[ind] = getRandomCauchy(nuF, 0.1);
  } while (fValues[ind] < 0);
  fValues[ind] = min(fValues[ind], 1.0);
}

void MigBirdsOptMod::generateCR(int ind) {
  crValues[ind] = generateRandomNormal(nuCR, 0.1);
  crValues[ind] = min(crValues[ind], 1.0);
  crValues[ind] = max(crValues[ind], 0.0);
}

void MigBirdsOptMod::updateNuF() {
	if (successfullFs.size() > 0) {
		//Calculate Lehmer mean
		double num = 0;
		double denom = 0;
		for (int i = 0; i < successfullFs.size(); i++) {
			num += (successfullFs[i] * successfullFs[i]);
			denom += successfullFs[i];
		}
		double lMean = num / denom;
		nuF = (1 - c) * nuF + c * lMean;
		successfullFs.clear();
  }
}

void MigBirdsOptMod::updateNuCR() {
  if (successfullCRs.size() > 0) {
    double num = 0;
		double denom = 0;
		for (int i = 0; i < successfullCRs.size(); i++){
			num += (successfullCRs[i]);
		}
		double aMean = num / successfullCRs.size();
		nuCR = (1 - c) * nuCR + c * aMean;
		successfullCRs.clear();
	}
}

// Global search selector
void MigBirdsOptMod::globalSearch() {
  if (globalSearchType == GS_GLOBAL_NEIGHBOURHOOD) {
    globalSearchGlobalNeighbourhood();
  }
  else if (globalSearchType == GS_GLOBAL_NEIGHBOURHOOD_PATH_SEARCH) {
    globalSearchGlobalNeighbourhood8();
  }
}

// Implementation of the original proposal
void MigBirdsOptMod::globalSearchGlobalNeighbourhood() {
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

  // Selects the best individual between v and k
  if (((v->getOptDirection(0) == MINIMIZE) && (v->getObj(0) < (*population)[k]->getObj(0))) ||
      ((v->getOptDirection(0) == MAXIMIZE) && (v->getObj(0) > (*population)[k]->getObj(0)))) {
    delete (*population)[k];
    (*population)[k] = v;
  } else {
    delete v;
	}
}

void MigBirdsOptMod::globalSearchGlobalNeighbourhood8() {
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
    Individual *best = lookForBestInd();
    
    // Auxiliary vector with the population sorted by the Euclidean distance in the decision
    // space with respect to the best individual 
    vector<extIndType*> *auxPop = new vector<extIndType*>;
    for (int i = 0; i < getPopulationSize(); i++) {
      extIndType *auxInd = new extIndType;
      auxInd->id = i;
      auxInd->ind = (*population)[i];
      auxPop->push_back(auxInd);
    }
    sort(auxPop->begin(), auxPop->end(), bind(&MigBirdsOptMod::sortByDistanceToBestExtInd, this, std::placeholders::_1, std::placeholders::_2, best));

    int r1, r2;

   	do {
  		r1 = (*auxPop)[getRandomInteger0_N(y - 1)]->id;
  	} while (r1 == k);

  	do {
  		r2 = (*auxPop)[getRandomInteger0_N(y - 1)]->id;
  	} while ((r2 == r1) || (r2 == k));
  
    // Frees the memory of the auxiliary vector with the population 
    for (int i = 0; i < getPopulationSize(); i++) {
      delete (*auxPop)[i];
    }
    delete auxPop;

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

bool MigBirdsOptMod::sortByDistanceToBestExtInd(const extIndType *i1, const extIndType *i2, const Individual *best) {
	return (i1->ind->getEuclideanDistanceDecisionSpace(best) < i2->ind->getEuclideanDistanceDecisionSpace(best));
}
