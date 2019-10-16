/***********************************************************************************
 * AUTORES
 *   - Ofelia Gonz�lez P�rez
 *   - Gara Miranda Valladares
 *   - Carlos Segura Gonz�lez
 *
 * FECHA
 *    Noviembre 2007
 ************************************************************************************/

#include "Individual.h"
#include <dlfcn.h>
#include <float.h>
#include <math.h>
#include <sstream>
#include <string>

void Individual::mutation(const double pm) {
  if (mutOperator != NULL) {
    mutOperator->mutation(this, pm);
  } else {
    dependentMutation(pm);
  }
}

void Individual::crossover(Individual *ind) {
  if (crossOperator != NULL) {
    crossOperator->crossover(this, ind);
  } else {
    dependentCrossover(ind);
  }
}

void Individual::dependentMutation(const double pm) {
  cout << "Mutation internal specified, but dependentMutation method not "
          "reimplemented"
       << endl;
  exit(-1);
}

void Individual::dependentCrossover(Individual *ind) {
  cout << "Croosver internal specified, but dependentCrossover method not "
          "reimplemented"
       << endl;
  exit(-1);
}

// Change binary-flip (NUEVO)
void Individual::mutate_binary_flip(const double pm, int origin,
                                    int destination) {
  destination = (destination == -1) ? var.size() : destination;
  for (unsigned int i = origin; i < destination; i++) {
    double vmut = ((double)rand()) / (double)(RAND_MAX);
    if (vmut < pm) {
      if (var[i] <= 0.5)
        var[i] = 1;
      else
        var[i] = 0;
    }
  }
}

// Change one of the Individual variables randomly
void Individual::mutate_uniform_one(double pm) {
  double vmut = ((double)rand()) / (double)(RAND_MAX);
  if (vmut < pm) {
    int indElem = (int)((double)(var.size()) * rand() / (RAND_MAX + 1.0));
    var[indElem] = ((double)rand() / (double)RAND_MAX);
    var[indElem] = var[indElem] * (getMaximum(indElem) - getMinimum(indElem)) +
                   getMinimum(indElem);
  }
}

// Change randomly all the Individual variables
// NUEVO
void Individual::mutate_uniform(double pm, int origin, int destination) {
  destination = (destination == -1) ? var.size() : destination;
  for (unsigned int i = origin; i < destination; i++) {
    float vmut = ((double)rand()) / (double)(RAND_MAX);
    if (vmut < pm) {
      var[i] = ((double)rand() / (double)RAND_MAX);
      var[i] = var[i] * (getMaximum(i) - getMinimum(i)) + getMinimum(i);
    }
  }
}

// Change binary-coded
void Individual::mutate_binary(double pm) {
  for (unsigned int i = 0; i < var.size(); i++) {
    double vmut = ((double)rand()) / (double)(RAND_MAX);
    if (vmut < pm) {
      if (var[i] == 0)
        var[i] = 1;
      else
        var[i] = 0;
    }
  }
}

// Change randomly an interval of the Individual variables
void Individual::mutate_range(double pm) {
  // Generate interval to mutate
  int start = (int)((double)(var.size()) * rand() / (RAND_MAX + 1.0));
  int end = (int)((double)(var.size()) * rand() / (RAND_MAX + 1.0));
  if (end < start) {
    for (int i = 0; i < end; i++) {
      double vmut = ((double)rand()) / (double)(RAND_MAX);
      if (vmut < pm) {
        var[i] = ((double)rand() / (double)RAND_MAX);
        var[i] = var[i] * (getMaximum(i) - getMinimum(i)) + getMinimum(i);
      }
    }
    for (unsigned int i = start; i < var.size(); i++) {
      float vmut = ((double)rand()) / (double)(RAND_MAX);
      if (vmut < pm) {
        var[i] = ((double)rand() / (double)RAND_MAX);
        var[i] = var[i] * (getMaximum(i) - getMinimum(i)) + getMinimum(i);
      }
    }
  } else {
    // Mutate variables
    for (int i = start; i <= end; i++) {
      float vmut = ((double)rand()) / (double)(RAND_MAX);
      if (vmut < pm) {
        var[i] = ((double)rand() / (double)RAND_MAX);
        var[i] = var[i] * (getMaximum(i) - getMinimum(i)) + getMinimum(i);
      }
    }
  }
}

// Change one using the polynomial probability distribution
// Sacado del software de M�laga: DEME
// NUEVO
void Individual::mutate_Pol(double pm, int origin, int destination) {
  destination = (destination == -1) ? var.size() : destination;
  const double n = 20;  // par�metro del mutation (distribution index)
  double delta, rnd, deltaq, mu, temp;

  for (unsigned int i = origin; i < destination; i++) {
    double vmut = ((double)rand()) / (double)(RAND_MAX);
    if (vmut < pm) {
      if ((var[i] - getMinimum(i)) < (getMaximum(i) - var[i]))
        delta = (var[i] - getMinimum(i)) / (getMaximum(i) - getMinimum(i));
      else
        delta = (getMaximum(i) - var[i]) / (getMaximum(i) - getMinimum(i));
      rnd = ((double)rand()) / (RAND_MAX);
      mu = 1.0 / (n + 1);
      if (rnd <= 0.5) {
        double xy = 1.0 - delta;
        temp = 2 * rnd + (1 - 2 * rnd) * (pow(xy, (n + 1)));
        deltaq = pow(temp, mu) - 1.0;
      } else {
        double xy = 1.0 - delta;
        temp = 2.0 * (1.0 - rnd) + 2.0 * (rnd - 0.5) * (pow(xy, (n + 1)));
        deltaq = 1.0 - (pow(temp, mu));
      }
      var[i] += deltaq * (getMaximum(i) - getMinimum(i));
      if (var[i] > getMaximum(i)) var[i] = getMaximum(i);
      if (var[i] < getMinimum(i)) var[i] = getMinimum(i);
    }
  }
}

// IMPLEMENTADO POR ANTONIO J. NEBRO (15/02/2008). Copiado de la version Java
// incluida en jMetal (http://neo.lcc.uma.es/metal)
// NUEVO
void Individual::mutate_Pol2(double pm, int origin, int destination) {
  destination = (destination == -1) ? var.size() : destination;
  const double distributionIndex = 20.0;
  double delta1, delta2, deltaq, mut_pow, rnd;

  for (int i = origin; i < destination; i++) {
    if ((double)rand() / RAND_MAX <= pm) {
      delta1 = (var[i] - getMinimum(i)) / (getMaximum(i) - getMinimum(i));
      delta2 = (getMaximum(i) - var[i]) / (getMaximum(i) - getMinimum(i));
      mut_pow = 1.0 / (distributionIndex + 1.0);
      rnd = (double)rand() / RAND_MAX;
      if (rnd <= 0.5) {
        double tmp;
        tmp = 2.0 * rnd +
              (1.0 - 2.0 * rnd) * pow(1.0 - delta1, distributionIndex + 1.0);
        deltaq = pow(tmp, mut_pow) - 1.0;
      }  // if
      else {
        double tmp;
        tmp = 2.0 * (1.0 - rnd) +
              2.0 * (rnd - 0.5) * pow(1.0 - delta2, distributionIndex + 1.0);
        deltaq = 1.0 - pow(tmp, mut_pow);
      }  // else
      var[i] += deltaq * (getMaximum(i) - getMinimum(i));
      if (var[i] < getMinimum(i)) var[i] = getMinimum(i);
      if (var[i] > getMaximum(i)) var[i] = getMaximum(i);
    }  // if
  }    // for
}

// Simulated Binary Crossover
// Explicaci�n en "Simulated Binary Crossover for continuous search space"
// Kalyanmoy Deb and Ram Bhusan Agrawal
//
// The paper define beta and analyze the probability distribution for beta
// in the binary single point crossover
// SBX have a similar probability distribution for beta, but it is applied
// to real numbers.
//
// The explication of code is not in the paper
//
// When beta < 1 => contracting crossover: p(beta) = 0.5 (n+1) (beta**n)
// When beta > 1 => expanding crossover:   p(beta) = 0.5 (n+1) *
// (1/(beta**(n+2)))
//
// Integrating can be obtained:
//- contracting: F(beta) = 0.5*(beta**(n+1))
//- expanding:   F(beta) = [0-.5*(beta**(-n-1)) + 0.5]
//
// If u is calculated in function of beta in beta = pow((2*u),1/(v+1.0));
// and in beta = pow((1/(2*(1-u))), 1/(v+1.0)); we can see that they coincide
//
// Note: in the first case: real u = 2*u
//      in the second csae: real u = 2*u - 0.5
//
// In half cases a contracting crossover is performed and in the other half
// a expanding crossover is performed
//
// Once that beta has been calcalated 2 childs are generated.
// The average value of the alleles in the childrens is the same
// as the average value in the parents and the beta value is the
// one obatained.
//
// The generation of the childs is made with the next equations:
// 0.5[(1 + beta)P1_i + (1-beta)P2_i]
// 0.5[(1 - beta)P1_i + (1+beta)P2_i]
//
// Para aplicarlo a multivariable propone:
// - Aplicarlo en cada variable con probabilidad 0.5
// - Determinar una variable, aplicarlo sobre esa e intercambiar
// los alelos posteriores.
// - No lo propone en este articulo, pero otra gente lo aplica
// sobre todas las variables
//
// Estaa funci�n lo aplica en cada variable con probabilidad 0.5
// NUEVO
void Individual::crossover_SBX(Individual *ind, int origin, int destination) {
  destination = (destination == -1) ? var.size() : destination;
  const double v = 5;  // par�metro del crossover
  double u, beta, cross;

  for (unsigned int i = origin; i < destination; i++) {
    cross = ((double)rand()) / (RAND_MAX);
    if (cross >= 0.5) {
      u = ((double)rand()) /
          (RAND_MAX + 1.0);  // genera entre 0 y 1 (no incluid9
      if (u <= 0.5) {        // hacer un contracting crossver (beta < 1)
        beta = pow((2 * u), 1 / (v + 1.0));
      } else {  // hacer un expanding crossover (beta > 1)
        beta = pow((1 / (2 * (1 - u))), 1 / (v + 1.0));
      }
      var[i] = 0.5 * ((1 + beta) * var[i] + (1 - beta) * ind->var[i]);
      if (var[i] > getMaximum(i)) {
        var[i] = getMaximum(i);
      }
      if (var[i] < getMinimum(i)) {
        var[i] = getMinimum(i);
      }
      ind->var[i] = 0.5 * ((1 - beta) * var[i] + (1 + beta) * ind->var[i]);
      if (ind->var[i] > ind->getMaximum(i)) {
        ind->var[i] = ind->getMaximum(i);
      }
      if (ind->var[i] < ind->getMinimum(i)) {
        ind->var[i] = ind->getMinimum(i);
      }
    }
  }
}

// IMPLEMENTADO POR ANTONIO J. NEBRO (15/02/2008). Copiado de la version Java
// incluida en jMetal (http://neo.lcc.uma.es/metal)
void Individual::crossover_SBX2(Individual *ind) {
  const double EPSILON = 1e-10;
  const double distributionIndex = 20;
  double y1, y2, c1, c2;
  double alpha, beta, betaq;

  for (int i = 0; i < var.size(); i++) {
    if ((double)rand() / RAND_MAX <= 0.5) {
      if (var[i] - ind->var[i] > EPSILON) {
        if (var[i] < ind->var[i]) {
          y1 = var[i];
          y2 = ind->var[i];
        }  // if
        else {
          y1 = ind->var[i];
          y2 = var[i];
        }  // else
        double random;
        random = (double)rand() / RAND_MAX;
        beta = 1.0 + (2.0 * (y1 - getMinimum(i)) / (y2 - y1));
        alpha = 2.0 - pow(beta, -(distributionIndex + 1.0));
        if (random <= (1.0 / alpha))
          betaq = pow(random * alpha, 1.0 / (distributionIndex + 1.0));
        else
          betaq = pow(1.0 / (2.0 - random * alpha),
                      1.0 / (distributionIndex + 1.0));

        c1 = 0.5 * ((y1 + y2) - betaq * (y2 - y1));
        beta = 1.0 + (2.0 * (getMaximum(i) - y2) / (y2 - y1));
        alpha = 2.0 - pow(beta, -(distributionIndex + 1.0));

        if (random <= (1.0 / alpha))
          betaq = pow((random * alpha), (1.0 / (distributionIndex + 1.0)));
        else
          betaq = pow((1.0 / (2.0 - random * alpha)),
                      (1.0 / (distributionIndex + 1.0)));

        c2 = 0.5 * ((y1 + y2) + betaq * (y2 - y1));

        if (c1 < getMinimum(i)) c1 = getMinimum(i);
        if (c2 < getMinimum(i)) c2 = getMinimum(i);
        if (c1 > getMaximum(i)) c1 = getMaximum(i);
        if (c2 > getMaximum(i)) c2 = getMaximum(i);

        if ((double)rand() / RAND_MAX <= 0.5) {
          var[i] = c2;
          ind->var[i] = c1;
        }  // if
        else {
          var[i] = c1;
          ind->var[i] = c2;
        }  // else
      }    // if
    }      // if
    else {
      double tmp;
      tmp = var[i];
      var[i] = ind->var[i];
      ind->var[i] = tmp;
    }  // else
  }    // if
}  // crossover_SBX

// Mix parents variables by using different intervals number
void Individual::crossover_range(Individual *ind, const int points) {
  vector<int> index;
  // Generate positions where to cross variables
  for (int i = 0; i < points; i++)
    index.push_back(
        (int)((double)(var.size() - 1) * rand() / (RAND_MAX + 1.0)));
  sort(index.begin(), index.end());
  index.push_back(var.size());

  for (int i = 0; i < points; i = i + 2) {
    for (int j = index[i]; j < index[i + 1]; j++) {
      double tmp = ind->var[j];
      ind->var[j] = var[j];
      var[j] = tmp;
    }
  }
}

// MODIFICADO
// Cruza cada variable con la misma probabilidad
// NUEVO
void Individual::crossover_uniform(Individual *ind, int origin,
                                   int destination) {
  destination = (destination == -1) ? var.size() : destination;
  for (unsigned int i = origin; i < destination; i++) {
    if (rand() > (RAND_MAX / 2)) {
      double tmp = ind->var[i];
      ind->var[i] = var[i];
      var[i] = tmp;
    }
  }
}

// Se parten ambos individuos por el mismo punto de corte, generado �ste
// a partir de un numero aleatorio. Una vez se tiene el punto de corte
// se hace que la parte traceada mayor del individuo 1 se une con la
// con la parte troceada menor del individuo 2. Y lo mismo ocurre con
// el individuo dos, se coge su parte troceada mayor y se une con la
// parte troceada menor del individuo 1
void Individual::crossover_OPC(Individual *ind, int origin, int destination) {
  destination = (destination == -1) ? var.size() : destination;
  int size = destination - origin;
  int vcross = int((double)(origin + size * rand()) /
                   (RAND_MAX + 1.0));  // Valor [origin-destination]

  if (vcross >= (size / 2)) {
    // Union del primer trozo del individuo 1 con el segundo trozo
    // individuo 2
    for (unsigned int i = (vcross + 1); i < destination; i++) {
      double tmp1 = ind->var[i];
      double tmp2 = var[i];
      if ((tmp2 == 1) && (tmp1 == 0)) {
        ind->var[i] = 1;
        var[i] = 0;
        //((RND*)(ind))->addBs(i);
        // removeBs(i);
      } else if ((tmp2 == 0) && (tmp1 == 1)) {
        ind->var[i] = 0;
        var[i] = 1;
        //((RND*)(ind))->removeBs(i);
        // addBs(i);
      }
    }
  } else {
    // Union del segundo trozo del individuo 1 con el primer trozo
    // individuo 2
    for (unsigned int i = 0; i < vcross; i++) {
      double tmp1 = ind->var[i];
      double tmp2 = var[i];
      if ((tmp2 == 1) && (tmp1 == 0)) {
        ind->var[i] = 1;  //((RND*)(ind))->addBs(i);
        var[i] = 0;       // removeBs(i);
      } else if ((tmp2 == 0) && (tmp1 == 1)) {
        ind->var[i] = 0;  //((RND*)(ind))->removeBs(i);
        var[i] = 1;       // addBs(i);
      }
    }
  }
}

void Individual::crossover_TPC(Individual *ind, int origin, int destination) {
  destination = (destination == -1) ? var.size() : destination;
  int size = destination - origin;
  int vcross1 = int((double)(origin + size * rand()) /
                    (RAND_MAX + 1.0));  // Valor [origin-destination]
  int vcross2 = int((double)(origin + size * rand()) /
                    (RAND_MAX + 1.0));  // Valor [origin-destination]

  if (vcross1 <= vcross2) {
    // Intercambia [vcross1-vcross2] entre el individuo1 y el individuo2
    for (unsigned int i = (vcross1 + 1); i < vcross2; i++) {
      double tmp1 = ind->var[i];
      double tmp2 = var[i];
      if ((tmp2 == 1) && (tmp1 == 0)) {
        ind->var[i] = 1;
        var[i] = 0;
      } else if ((tmp2 == 0) && (tmp1 == 1)) {
        ind->var[i] = 0;
        var[i] = 1;
      }
    }
  } else {
    // if (vcross1 > vcross2){
    // Intercambia [vcross2-vcross1] entre el individuo2 y el individuo1
    for (unsigned int i = vcross2; i < vcross1; i++) {
      double tmp1 = ind->var[i];
      double tmp2 = var[i];
      if ((tmp2 == 1) && (tmp1 == 0)) {
        ind->var[i] = 1;
        var[i] = 0;
      } else if ((tmp2 == 0) && (tmp1 == 1)) {
        ind->var[i] = 0;
        var[i] = 1;
      }
    }
    //}
  }
}

// Crea un nuevo individuo generando valores aleatorios para las variables del
// mismo
void Individual::restart(void) {
  for (unsigned int i = 0; i < var.size(); i++) {
    var[i] = ((double)rand() / (double)RAND_MAX);
    var[i] = var[i] * (getMaximum(i) - getMinimum(i)) + getMinimum(i);
  }
}

/**
 * 	Metodo de reparacion de individuos
 * 	Añadido por Alejandro Marrero - 2019
 * 	Para poder invocar al método desde el algoritmo
 **/
void Individual::repair(void) { return; }

// Operador de salida: por defecto imprime variables y objetivos
void Individual::print(ostream &os) const {
  for (unsigned int i = 0; i < var.size(); i++) os << var[i] << " ";
  for (unsigned int i = 0; i < obj.size(); i++) os << obj[i] << " ";
  //	for (unsigned int i = 0; i < auxData.size(); i++){
  //		os << auxData[i] << " ";
  //	}
  os << endl;
}

// Clone operators
Individual *Individual::internalClone(void) const {
  Individual *nuevo = clone();
  nuevo->internalClone(this);
  return nuevo;
}

void Individual::internalClone(const Individual *ind) {
  fitnessValue = ind->fitnessValue;
  mutOperator = ind->mutOperator;
  crossOperator = ind->crossOperator;
  multiObjectivizationsPlugins = ind->multiObjectivizationsPlugins;
  internalCloneData(ind);
}

void Individual::internalCloneData(const Individual *ind) {
  // Variables
  setNumberOfVar(ind->getNumberOfVar());
  for (unsigned int i = 0; i < ind->var.size(); i++) var[i] = ind->var[i];
  // Objetivos
  setNumberOfObj(ind->getNumberOfObj());
  for (unsigned int i = 0; i < ind->obj.size(); i++) obj[i] = ind->obj[i];
  // Otros par�metros a migrar
  setMigrationDataSize(ind->migrationData.size());
  for (unsigned int i = 0; i < ind->migrationData.size(); i++)
    migrationData[i] = ind->migrationData[i];
  // Otros datos auxiliares
  setAuxDataSize(ind->auxData.size());
  for (unsigned int i = 0; i < ind->auxData.size(); i++)
    auxData[i] = ind->auxData[i];
  owner = ind->owner;
}

double Individual::getEuclideanDistance(const Individual *i2) const {
  double dist = 0;
  for (int i = 0; i < getNumberOfObj(); i++) {
    dist += (i2->getObj(i) - getObj(i)) * (i2->getObj(i) - getObj(i));
  }
  return sqrt(dist);
}

double Individual::getEuclideanDistanceDecisionSpace(
    const Individual *i2) const {
  double dist = 0;
  for (int i = 0; i < getNumberOfVar(); i++) {
    dist += (i2->getVar(i) - getVar(i)) * (i2->getVar(i) - getVar(i));
  }
  return sqrt(dist);
}

Individual *Individual::deserialize(const int nVar, const int nObj,
                                    const int nMig, const long double *buffer,
                                    int &count) {
  Individual *nuevo = clone();
  count = 0;

  // Variables
  nuevo->setNumberOfVar(nVar);
  for (int i = 0; i < nVar; i++) nuevo->var[i] = buffer[count++];

  // Objetivos
  nuevo->setNumberOfObj(nObj);
  for (int i = 0; i < nObj; i++) nuevo->obj[i] = buffer[count++];

  // Otros par�metros a migrar
  nuevo->setMigrationDataSize(nMig);
  for (int i = 0; i < nMig; i++) nuevo->migrationData[i] = buffer[count++];

  // Inicializacion del vector de datos auxiliares
  nuevo->setAuxDataSize(getAuxDataSize());

  // Operador de mutacion
  nuevo->mutOperator = mutOperator;
  nuevo->crossOperator = crossOperator;
  nuevo->multiObjectivizationsPlugins = multiObjectivizationsPlugins;
  nuevo->deserialized();
  return nuevo;
}

void Individual::serialize(long double *buffer, int &index) const {
  index = 0;
  for (unsigned int i = 0; i < var.size(); i++) buffer[index++] = var[i];
  for (unsigned int i = 0; i < obj.size(); i++) buffer[index++] = obj[i];
  for (unsigned int i = 0; i < migrationData.size(); i++)
    buffer[index++] = migrationData[i];
}

string Individual::getMutationName() const {
  if (mutOperator != NULL) {
    return mutOperator->getName();
  }
  return "Internal_operator";
}

int Individual::getMutationNumberOfParameters() const {
  if (mutOperator != NULL) {
    return mutOperator->getNumberOfParameters();
  }
  return 0;
}

string Individual::getMutationParameterName(int i) const {
  if (mutOperator != NULL) {
    return mutOperator->getParameterName(i);
  }
  return "";
}

string Individual::getMutationParameterValue(int i) const {
  if (mutOperator != NULL) {
    return mutOperator->getParameterValue(i);
  }
  return "";
}

string Individual::getCrossoverName() const {
  if (crossOperator != NULL) {
    return crossOperator->getName();
  }
  return "Internal_operator";
}

int Individual::getCrossoverNumberOfParameters() const {
  if (crossOperator != NULL) {
    return crossOperator->getNumberOfParameters();
  }
  return 0;
}

string Individual::getCrossoverParameterName(int i) const {
  if (crossOperator != NULL) {
    return crossOperator->getParameterName(i);
  }
  return "";
}

string Individual::getCrossoverParameterValue(int i) const {
  if (crossOperator != NULL) {
    return crossOperator->getParameterValue(i);
  }
  return "";
}

// it is not enough with make var.clear() (for freeing the space)
void Individual::deleteVars() {
  var.clear();
  vector<double>(var).swap(var);
}

unsigned int Individual::getInternalOptDirection(const int i) const {
  // Chequear si es del individuo o es ficticio
  int realObjectives = getNumberOfObj() - multiObjectivizationsPlugins.size();
  if (i < realObjectives) {
    return getOptDirection(i);
  } else {
    return multiObjectivizationsPlugins[i - realObjectives]->getOptDirection();
  }
}

// Auxiliar functions
bool cmpIndividual(Individual *ind1, Individual *ind2) {
  return (*ind1 < *ind2);
}

bool ordByFitness(Individual *i1, Individual *i2) {
  return (i1->getFitnessValue() > i2->getFitnessValue());
}

bool reverseOrdByFitness(Individual *i1, Individual *i2) {
  return (i1->getFitnessValue() < i2->getFitnessValue());
}

// Library functions
void closeInd(void *hndl) { dlclose(hndl); }

// Obtenci�n de individuo a partir de una librer�a
Individual *getInd(const string &pluginPath, const string &name,
                   const vector<string> &indArgs, bool printError) {
  return ((Individual *)(loadPlugin("Problem", pluginPath, name, indArgs,
                                    printError, true)));
}
