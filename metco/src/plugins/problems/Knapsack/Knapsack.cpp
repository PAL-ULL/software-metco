/* -----------------------------------------------------------------------------
 *  One-dimensional Knapsack Problem implementation
 *  Implementacion del 0-1 Knapsack Problem unidimensional.
 *
 *
 *  @author: Alejandro Marrero Diaz (alu0100825008@ull.edu.es)
 *  @date: 2019
 *
 * -------------------------------------------------------------------------- */

#include "Knapsack.h"
#include <algorithm>

/**
 *  Variables estáticas que son comunes para todas las instancias del problema.
 *  - Numero de elementos de la instancia.
 *  - Beneficio de cada uno de los i elementos.
 *  - Peso de cada uno de los i elementos.
 *  - Capacidad maxima de la mochila.
 *  - Vector que almacena los elementos en funcion de su efficiencia.
 *
 **/
int Knapsack::nItems;
vector<float> Knapsack::profits;
vector<float> Knapsack::weights;
float Knapsack::capacity;
vector<item_efficiency> Knapsack::items_efficiency;
const int Knapsack::ARGS = 1;

// Problem Initialization
bool Knapsack::init(const vector<string> &params) {
  // Check for the problem parameters
  if (params.size() != ARGS) {
    cout << "Error Knapsack: Incorrect parameters " << endl;
    cout << "<instance>" << endl;
    return false;
  }
  string filename = params[0];
  setNumberOfObj(1);
  // Leemos las caracteristicas del problema del fichero de instancia.
  readFile(filename);
  computeEfficiency();
  return true;
}

/**
 *  Metodo empleado para parsear una instancia del KP.
 *  Este metodo espera un fichero con la siguiente estructura:
 *
 *  number_of_items
 *  capacity
 *
 *  (wi, pi) para los elementos i=0, number_of_items-1
 **/
void Knapsack::readFile(const string filename) {
  ifstream input(filename.c_str());
  if (input.fail()) {
    cerr << "Error while opening the instance file" << endl;
    exit(-1);
  }
  string readLine;
  input >> nItems;
  input >> capacity;
  profits.resize(nItems);
  weights.resize(nItems);
  setNumberOfVar(nItems);
  float weight, profit;

  for (int i = 0; i < nItems; i++) {
    input >> weight >> profit;
    weights[i] = weight;
    profits[i] = profit;
  }
  input.close();
}

/**
 *  Operador que realiza la mutacion binaria especifica para este problema.
 *  - Heredado.
 **/
void Knapsack::dependentMutation(double pm) {
  mutate_binary(pm);
  repair();
}

/**
 *  Generacion aleatoria de un nuevo individuo.
 *  Se establece un valor aleatorio en el rango [0, 1] y posteriormente
 *  se redondea al entero mas cercano.
 **/
void Knapsack::restart(void) {
  for (int i = 0; i < nItems; i++) {
    setVar(i, ((double)rand() / (double)RAND_MAX));
    setVar(i, getVar(i) * (getMaximum(i) - getMinimum(i)) + getMinimum(i));
    if (getVar(i) >= 0.5)
      setVar(i, 1.0);
    else
      setVar(i, 0.0);
  }
  repair();
}

/**
 *  Metodo para reparar una solucion cuando se ha violado la restriccion
 *
 *  Recorremos el array de pares donde estan ordenados los elementos por su
 *  eficiencia y cambiamos su valor de 1 a 0.
 **/
void Knapsack::repair(void) {
  int i = 0;
  // Mientras violemos la restriccion
  while (!checkCapacity()) {
    // Ponemos a 0 el elemento en la posicion i-esima del vector de efficiencia
    setVar(items_efficiency[i].first, 0.0);
    i++;
  }
  // Nueva version
  // Incluimos los elementos del final mientras no nos pasemos
  i = nItems - 1;
  while (checkCapacity()) {
    setVar(items_efficiency[i].first, 1.0);
    i--;
  }
  // Eliminamos el ultimo elemento incluido porque la capacidad ha sido
  // sobrepasada
  setVar(items_efficiency[i + 1].first, 0.0);
}

/**
 *  Comprobamos que la solucion sigue siendo factible
 *  Para ellos sumamos los pesos de los elementos incluidos dentro de la
 *solucion y verificamos que no excede la capacidad de la mochila
 **/
bool Knapsack::checkCapacity() {
  float packed = 0;
  for (int i = 0; i < nItems; i++) {
    packed += getWeight(i) * int(getVar(i));
  }
  return (packed > capacity) ? false : true;
}

/**
 *  Metodo que nos permite asegurarnos que nos mantenemos en
 *  una representacion binaria del problema
 **/
void Knapsack::roundVariables() {
  for (int i = 0; i < nItems; i++) {
    if (getVar(i) >= 0.5)
      setVar(i, 1.0);
    else
      setVar(i, 0.0);
  }
}

/**
 *  Metodo para evaluar un individuo de la poblacion. En primer lugar, aplicamos
 *el operador de reparacion que hace dos cosas:
 *  - Elimina los elementos mas eficientes que hacen que se sobrepase la
 *capacidad de la mochila.
 *
 * - Añade los elemenos menos eficientes que cuyos pesos no convierten a la
 *solucion en no-factible.
 *
 *  Por último evaluamos el individuo y asignamos su fitness.
 **/
void Knapsack::evaluate(void) {
  repair();
  // Ahora tenemos que evaluar el individuo
  float objective = 0;
  for (int j = 0; j < nItems; j++) {
    objective += profits[j] * int(getVar(j));
  }
  setObj(0, objective);
}

/**
 *  Metodo empleado para calcular y ordenar los elementos por su eficiencia.
 *  El concepto de eficiencia se define como el ratio pi/wi y por lo tanto
 *  los elementos con mayor eficiencia estarán en los primeros puestos.
 *  Ordenación descendente.
 **/
void Knapsack::computeEfficiency(void) {
  items_efficiency.resize(nItems);

  for (int j = 0; j < nItems; j++) {
    items_efficiency[j].first = j;
    items_efficiency[j].second = 0.0;
    for (int i = 0; i < 1; i++) {
      items_efficiency[j].second = getProfit(j) / getWeight(j);
    }
  }
  sort(items_efficiency.begin(), items_efficiency.end(), sortByEfficiencyDesc);
}

/**
 *  Funcion auxiliar para ordenar los elementos por su eficiencia
 *
 **/
bool sortByEfficiencyDesc(const pair<int, int> &firstPair,
                          const pair<int, int> &secondPair) {
  return firstPair.second > secondPair.second;
}

Individual *Knapsack::clone(void) const { return new Knapsack(); }
