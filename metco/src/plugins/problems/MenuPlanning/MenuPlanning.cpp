

/**
 *
 * author: Alejandro Marrero
 * contact: alejandro.marrero.10@ull.edu.es
 * Created on Sun Jul 05 2020
 *
 * Copyright (c) 2020 Universidad de La Laguna
 *
 * Summary:
 *
 *
 **/

#include "MenuPlanning.h"

#include <array>
#include <cmath>

// Constantes del problema
const int MenuPlanning::N_OBJS = 2;
const int MenuPlanning::MAX_INT = std::numeric_limits<int>::max();

// Variables estaticas del problema
int MenuPlanning::nDias;
int MenuPlanning::nParam;
vector<infoPlatos> MenuPlanning::v_primerosPlatos;
vector<infoPlatos> MenuPlanning::v_segundosPlatos;
vector<infoPlatos> MenuPlanning::v_postres;
vector<pair<int, int> > MenuPlanning::gruposAl;
vector<vector<vector<double> > > MenuPlanning::v3_compatibilidadPlatos;
vector<string> MenuPlanning::alergenosPlan;
vector<string> MenuPlanning::incompatibilidadesPlan;
vector<int> MenuPlanning::gruposAlPlan;
vector<double> MenuPlanning::infoNPlan;

/**
 *
 * Constructor por defecto de una instancia de MenuPlanning
 */
MenuPlanning::MenuPlanning() : infeasibilityDegree(0.0f) {}

/**
 * Metodo para inicializar un individuo MenuPlanning
 *
 * @param vector de parametros
 **/
bool MenuPlanning::init(const vector<string> &params) {
  if (params.size() != 1) {
    cout << "Error Menu Planning: numero incorrecto de parametros." << endl;
    cout << "N-days expected" << endl;
    return false;
  }
  nDias = atoi((params[0]).c_str());
  nParam = nDias * num_tipoPlato;  // Multiplicar nDias por tipos de platos (3)
  setNumberOfVar(nParam);
  setNumberOfObj(N_OBJS);
  set_gruposAl();
  set_Platos();
  set_vectorCompatibilidad();
  infoNPlan.assign(num_nutr, 0);
  alergenosPlan.assign(num_alerg, "0");
  incompatibilidadesPlan.assign(num_incomp, "0");
  infeasibilityDegree = 0.0f;
  restrictionsID.fill(0.0f);

  return true;
}

/**
 * Método para definir los grupos de alimentos
 */
void MenuPlanning::set_gruposAl(void) {
  pair<int, int> ga;
  ga.first = MAX_INT;
  ga.second = MAX_INT;
  gruposAl.assign(num_gAl, ga);
}

/**
 * Método para cargar los platos a partir de los datos
 * dentro del fichero que se recibe como parametro
 **/
void MenuPlanning::set_VectoresPlatos(const char *c_filename,
                                      vector<infoPlatos> &v_vecP) {
  ifstream ifs;
  int i;
  int cont = 1;

  ifs.open(c_filename, ifstream::in);
  if (ifs.is_open()) {
    while (ifs.good()) {
      string str;
      infoPlatos ip;

      getline(ifs, str, ',');
      ip.nombre = str;  // Nombre del plato
      getline(ifs, str, ',');
      ip.precio = atof(str.c_str());  // Precio
      getline(ifs, str, ',');
      ip.cantidad = atof(str.c_str());  // Cantidad
      ip.diasRep = std::numeric_limits<int>::max();

      for (i = 0; i < num_alerg; i++) {
        getline(ifs, str, ',');
        ip.alg.push_back(str);
      }  // Alergenos
      for (i = 0; i < num_incomp; i++) {
        getline(ifs, str, ',');
        ip.inc.push_back(str);
      }  // Incompatibilidades
      for (i = 0; i < num_nutr; i++) {
        getline(ifs, str, ',');
        ip.infoN.push_back(atof(str.c_str()));
      }  // Informacion nutricional

      getline(ifs, str, ',');
      while (str != "*" && !ifs.eof()) {
        ip.gruposAl.push_back(atoi(str.c_str()));
        getline(ifs, str, ',');

        for (int x = 0; x < str.size(); x++)
          if (str.at(x) == '\n') str = "*";
      }

      v_vecP.push_back(ip);
    }
    ifs.close();
  } else {
    cout << "\n\nError. No se han podido leer los archivos de platos.";
    cin.get();
    exit(0);
  }
}

/*--------------------------------------------*/
/*---------- COMPATIBILIDAD PLATOS -----------*/
/*--------------------------------------------*/
void MenuPlanning::set_vectorCompatibilidad(void) {
  // Vector de postres
  vector<double> sz(v_postres.size(), 0.0);
  // Plano de postres y segundos platos
  vector<vector<double> > sz2(v_segundosPlatos.size(), sz);
  // Prisma rectangular de primeros, segundos platos y postres
  v3_compatibilidadPlatos.assign(v_primerosPlatos.size(), sz2);
  vector<bool> gaElegidos;
  for (int y = 0; y < v_primerosPlatos.size(); y++)
    for (int x = 0; x < v_segundosPlatos.size(); x++)
      for (int z = 0; z < v_postres.size(); z++) {
        gaElegidos.clear();
        gaElegidos.assign(num_gAl, false);
        set_GAElegidos(v_primerosPlatos[y].gruposAl, gaElegidos);
        v3_compatibilidadPlatos[y][x][z] =
            set_penalizacionVC(v_segundosPlatos[x].gruposAl, gaElegidos);
        set_GAElegidos(v_segundosPlatos[x].gruposAl, gaElegidos);
        v3_compatibilidadPlatos[y][x][z] +=
            set_penalizacionVC(v_postres[z].gruposAl, gaElegidos);
      }
}

void MenuPlanning::set_GAElegidos(vector<int> gal, vector<bool> &galE) {
  for (int i = 0; i < gal.size(); i++) galE[gal[i]] = true;
}

double MenuPlanning::set_penalizacionVC(vector<int> &gal, vector<bool> galE) {
  double resultado = 0.0;
  for (int i = 0; i < gal.size(); i++) {
    if (galE[gal[i]]) switch (gal[i]) {
        case 0:
          resultado += penalizaciones[0];
          break;
        case 1:
          resultado += penalizaciones[1];
          break;
        case 2:
          resultado += penalizaciones[2];
          break;
        case 3:
          resultado += penalizaciones[3];
          break;
        case 4:
          resultado += penalizaciones[4];
          break;
        case 5:
          resultado += penalizaciones[5];
          break;
        case 6:
          resultado += penalizaciones[6];
          break;
        case 7:
          resultado += penalizaciones[7];
          break;
        case 8:
          resultado += penalizaciones[8];
          break;
        case 9:
          resultado += penalizaciones[9];
          break;
      }
  }
  return resultado;
}

/*--------------------------------------------------------*/
/*---------- GENERACION ALEATORIA DE INDIVIDUO -----------*/
/*--------------------------------------------------------*/
void MenuPlanning::restart(void) {
  for (int i = 0; i < nParam; i++)
    setVar(i, (double)(rand() % (int)getMaximum(i)));

  computeFeasibility();
  // repair();
}

/**
 * Clonacion del individuo
 **/
Individual *MenuPlanning::clone(void) const {
  MenuPlanning *mpp = new MenuPlanning();
  mpp->infeasibilityDegree = this->infeasibilityDegree;
  return mpp;
}

/*----------------------------------------*/
/*---------- OPERADOR DE CRUCE -----------*/
/*----------------------------------------*/
void MenuPlanning::dependentCrossover(Individual *ind) {
  crossover_uniform(ind);
  computeFeasibility();
  // ((MenuPlanning *)ind)->repair();
  // repair();
}

/*-------------------------------------------*/
/*---------- OPERADOR DE MUTACION -----------*/
/*-------------------------------------------*/
void MenuPlanning::dependentMutation(double pm) {
  int x;
  bool mod = false;
  for (int i = 0; i < nDias; i++) {
    x = rand() % 100;
    if (x < (pm * 100)) {
      setVar(i * num_tipoPlato, rand() % v_primerosPlatos.size());
      setVar(i * num_tipoPlato + 1, rand() % v_segundosPlatos.size());
      setVar(i * num_tipoPlato + 2, rand() % v_postres.size());
      mod = true;
    }
  }
  if (mod) computeFeasibility();
  // if (mod) repair();
}

/**
 * Método que se encarga de calcular la factibilidad de un
 * individuo con respecto a las restricciones de nutrientes
 *
 * Alejandro Marrero - alu0100825008@ull.edu.es
 **/
double MenuPlanning::computeFeasibility() {
  double totalFeasibility = 0.0;
  // Reseteamos el array de ID por nutriente
  restrictionsID.fill(0.0f);

  std::array<double, num_nutr> infoNPlan;
  // Bucle para calcular did(S)
  for (int i = 0; i < nDias; i++) {
    int idx = i * num_tipoPlato;
    std::array<double, num_nutr> dayNutr;
    // Primero obtenemos la cantidad de nutrientes de cada dia
    for (unsigned int j = 0; j < num_nutr; j++) {
      dayNutr[j] += v_primerosPlatos[round(getVar(idx))].infoN[j];
      dayNutr[j] += v_segundosPlatos[round(getVar(idx + 1))].infoN[j];
      dayNutr[j] += v_postres[round(getVar(idx + 2))].infoN[j];
      infoNPlan[j] += dayNutr[j];
    }
    // Calculamos los nutrientes por dia
    for (int j = 0; j < FORCED_INDEXES_SIZE; j++) {
      int index = FORCED_INDEXES[j];
      if (dayNutr[index] < ingR[index] * FORCED_MIN[j]) {
        totalFeasibility +=
            pow((ingR[index] * (FORCED_MIN[j] - dayNutr[index])), 2);
      } else if (dayNutr[index] > ingR[index] * FORCED_MAX[j]) {
        totalFeasibility +=
            pow((dayNutr[index] - (ingR[index] * FORCED_MAX[j])), 2);
      }
    }
  }

  // Ahora calculamos los nutrientes globales del plan gid(S)
  for (unsigned int i = 0; i < num_nutr; i++) {
    if ((i == CALCIUM_INDEX) || (i == POTASIUM_INDEX) || (i == IRON_INDEX))
      continue;

    // En cualquier caso, guardamos la diferencia para cada nutriente
    restrictionsID[i] = (infoNPlan[i] - (ingR[i] * minReq[i] * nDias));
    if (infoNPlan[i] < (ingR[i] * minReq[i] * nDias)) {
      totalFeasibility =
          pow(((ingR[i] * minReq[i] * nDias) - infoNPlan[i]), 2) * 1e6;
    }
    if (infoNPlan[i] > (ingR[i] * maxReq[i] * nDias)) {
      totalFeasibility =
          pow((infoNPlan[i] - (ingR[i] * maxReq[i] * nDias)), 2) * 1e6;
    }
  }
  // devolvemos id(S) = did(S) + gid(S)
  infeasibilityDegree = totalFeasibility;
  return infeasibilityDegree;
}

/**
 * Metodo que evalua un individuo de la clase MenuPlanning
 * - Asigna los valores para los dos objetivos:
 *  - Precio
 *  - Nivel de Repeticion
 **/
void MenuPlanning::evaluate(void) {
  double precioTotal = 0;
  double valPP, valSP, valP, valTabla, valGAFirst;
  double numPP = penalizaciones[15];
  double numSP = penalizaciones[16];
  double numP = penalizaciones[17];

  double valTotal = 0;
  // Vector que guarda los grupos alimenticios pertenecientes a los platos
  // elegidos
  vector<int> gaElegidos;
  // Vector que guarda los ga pertenecientes al menu de la iteracion anterior
  vector<int> gaElegidosAnterior;
  vector<vector<int> > ultimos5GA;
  int x = 0;

  for (int i = 0; i < nDias; i++) {
    x = i * num_tipoPlato;
    // Coste del menu
    precioTotal += v_primerosPlatos[round(getVar(x))].precio +
                   v_segundosPlatos[round(getVar(x + 1))].precio +
                   v_postres[round(getVar(x + 2))].precio;
    // Nutrientes
    for (int j = 0; j < infoNPlan.size(); j++)
      infoNPlan[j] += v_primerosPlatos[round(getVar(x))].infoN[j] +
                      v_segundosPlatos[round(getVar(x + 1))].infoN[j] +
                      v_postres[round(getVar(x + 2))].infoN[j];
  }
  // Nivel de repeticion
  for (int i = 0; i < nDias; i++) {
    x = i * num_tipoPlato;

    // PRIMER PLATO
    // Numero de dias desde que se repitio el plato seleccionado
    valPP = setValorPP(getVar(x));
    // Numero de dias desde que se repitio el grupo alimenticio
    for (int j = 0; j < v_primerosPlatos[getVar(x)].gruposAl.size(); j++) {
      // comprueba si ya habia aparecido en el menu el grupo alimenticio j, si
      // no lo a�ade al vector gaElegidos
      if (gaElegidosPorIteracion(gaElegidos,
                                 v_primerosPlatos[getVar(x)].gruposAl[j]))
        gaElegidos.push_back(v_primerosPlatos[getVar(x)].gruposAl[j]);
    }

    // SEGUNDO PLATO
    valSP = setValorSP(getVar(x + 1));
    for (int k = 0; k < v_segundosPlatos[getVar(x + 1)].gruposAl.size(); k++) {
      if (gaElegidosPorIteracion(gaElegidos,
                                 v_segundosPlatos[getVar(x + 1)].gruposAl[k]))
        gaElegidos.push_back(v_segundosPlatos[getVar(x + 1)].gruposAl[k]);
    }

    // POSTRE
    valP = setValorP(getVar(x + 2));
    for (int l = 0; l < v_postres[getVar(x + 2)].gruposAl.size(); l++) {
      if (gaElegidosPorIteracion(gaElegidos,
                                 v_postres[getVar(x + 2)].gruposAl[l]))
        gaElegidos.push_back(v_postres[getVar(x + 2)].gruposAl[l]);
    }

    // Obtener el valor de la tabla de platos de compatibilidad
    // entre primeros y segundos platos
    valTabla = v3_compatibilidadPlatos[getVar(x)][getVar(x + 1)][getVar(x + 2)];
    valGAFirst = set_ValorGAFirstAlternativa(ultimos5GA, gaElegidos);
    // Obtener el valor total del numero de dias
    // desde que se repitieron grupos alimenticios
    valTotal +=
        valTabla + numPP / valPP + numSP / valSP + numP / valP + valGAFirst;

    // Suma los valores de platos y grupos alimenticios
    // elegidos para el siguiente dia
    sumValorPP();
    sumValorSP();
    sumValorP();
    sumValorGA();

    set_ultimos5GA(ultimos5GA, gaElegidos);
    gaElegidosAnterior = gaElegidos;
    gaElegidos.clear();
  }

  ultimos5GA.clear();
  gaElegidosAnterior.clear();
  // Asignamos el valor de los objetivos
  computeFeasibility();
  setObj(0, precioTotal + infeasibilityDegree);
  setObj(1, valTotal + infeasibilityDegree);
}

#ifdef __MPP_FEASIBILITY_DEBUG__
/**
 * En esta version nos centramos únicamente en mostrar los
 * valores de cada nutriente para comprobar la factiblidad de los
 * individuos resultantes
 */
void MenuPlanning::print(ostream &os) const {
  for (unsigned int i = 0; i < num_nutr; i++) {
    os << restrictionsID[i] << " ";
  }
  os << this->infeasibilityDegree;
  os << endl;
}
#endif

/*----------------------------------------------------------------------*/
/*---------- METODOS PARA EL CALCULO DEL GRADO DE REPETICION -----------*/
/*----------------------------------------------------------------------*/

bool MenuPlanning::gaElegidosPorIteracion(vector<int> vec, int valor) {
  bool resultado = true;
  for (int i = 0; i < vec.size(); i++)
    if (vec[i] == valor) resultado = false;
  return resultado;
}

int MenuPlanning::setValorPP(int id) {
  int valor = MAX_INT;  // Tengo que retornar el numero de dias desde que se
                        // eligio el plato por ultima vez
  // Si el numero de dias es MAX_INT, significa que nunca se ha
  // elegido, por lo que retorno 0
  if (v_primerosPlatos[id].diasRep != MAX_INT)
    valor = v_primerosPlatos[id].diasRep;

  // Si el numero de dias es distinto a MAX_INT,
  // retorno el valor y reseteo el numero de dias a 0
  v_primerosPlatos[id].diasRep = 0;

  return valor;
}

int MenuPlanning::setValorSP(int id) {
  int valor = MAX_INT;
  if (v_segundosPlatos[id].diasRep != MAX_INT)
    valor = v_segundosPlatos[id].diasRep;
  v_segundosPlatos[id].diasRep = 0;

  return valor;
}

int MenuPlanning::setValorP(int id) {
  int valor = MAX_INT;
  if (v_postres[id].diasRep != MAX_INT) valor = v_postres[id].diasRep;
  v_postres[id].diasRep = 0;

  return valor;
}

void MenuPlanning::sumValorPP(void) {
  for (int i = 0; i < v_primerosPlatos.size(); i++)
    if (v_primerosPlatos[i].diasRep != MAX_INT) v_primerosPlatos[i].diasRep++;
}

void MenuPlanning::sumValorSP(void) {
  for (int i = 0; i < v_segundosPlatos.size(); i++)
    if (v_segundosPlatos[i].diasRep != MAX_INT) v_segundosPlatos[i].diasRep++;
}

void MenuPlanning::sumValorP(void) {
  for (int i = 0; i < v_postres.size(); i++)
    if (v_postres[i].diasRep != MAX_INT) v_postres[i].diasRep++;
}

void MenuPlanning::sumValorGA(void) {
  for (int i = 0; i < gruposAl.size(); i++) {
    if (gruposAl[i].first != MAX_INT) gruposAl[i].first++;
    gruposAl[i].second = MAX_INT;
  }
}

// METODO PARA ANIADIR LOS GA DE UN DIA EN EL VECTOR DE GAS DE LOS ULTIMOS 5
// DIAS
void MenuPlanning::set_ultimos5GA(vector<vector<int> > &ultimos5GA,
                                  vector<int> vec) {
  if (ultimos5GA.size() < 5)
    ultimos5GA.push_back(vec);
  else {
    ultimos5GA.erase(ultimos5GA.begin());
    ultimos5GA.push_back(vec);
  }
}

double MenuPlanning::set_ValorGAFirstAlternativa(
    vector<vector<int> > &ultimos5GA, vector<int> vec) {
  /* 0 Otros, 1 Carne, 2 Cereal, 3 Fruta, 4 Lacteo, 5 Legumbre, 6 Marisco, 7
   * Pasta, 8 Pescado, 9 Verdura */
  double penalizacionPorGA[10] = {penalizaciones[0], penalizaciones[1],
                                  penalizaciones[2], penalizaciones[3],
                                  penalizaciones[4], penalizaciones[5],
                                  penalizaciones[6], penalizaciones[7],
                                  penalizaciones[8], penalizaciones[9]};
  double penalizacionPorDias[5] = {penalizaciones[10], penalizaciones[11],
                                   penalizaciones[12], penalizaciones[13],
                                   penalizaciones[14]};
  bool pen[5] = {false, false, false, false, false};
  double resultado = 0;

  if (ultimos5GA.size() > 0) {
    for (int i = 0; i < vec.size(); i++) {
      for (int j = 0; j < ultimos5GA.size(); j++)
        for (int k = 0; k < ultimos5GA[j].size(); k++) {
          if (vec[i] == ultimos5GA[j][k]) {
            pen[j] = true;
            resultado += penalizacionPorGA[vec[i]];
          }
        }
    }
    for (int x = 0; x < 5; x++)
      if (pen[x]) {
        resultado += penalizacionPorDias[x];
        pen[x] = false;
      }
  }

  return resultado;
}

void MenuPlanning::mostrarPlatos(void) {
  cout << "\n\nPRIMEROS PLATOS";
  for (int i = 0; i < v_primerosPlatos.size(); i++) {
    cout << "\nNombre: " << v_primerosPlatos[i].nombre;
    cout << "\ndiasrep: " << v_primerosPlatos[i].diasRep;
    cout << "\nprecio: " << v_primerosPlatos[i].precio;
    cout << "\ncantidad: " << v_primerosPlatos[i].cantidad;
    cout << "\ngrupos Al:";
    for (int x = 0; x < v_primerosPlatos[i].gruposAl.size(); x++) {
      cout << "\n- " << v_primerosPlatos[i].gruposAl[x];
    }
    cout << "\nInfo Nut:";
    for (int x = 0; x < v_primerosPlatos[i].infoN.size(); x++) {
      cout << "\n- " << v_primerosPlatos[i].infoN[x];
    }
    cout << "\nAlergenos:";
    for (int x = 0; x < v_primerosPlatos[i].alg.size(); x++) {
      cout << "\n- " << v_primerosPlatos[i].alg[x];
    }

    cout << "\nIncompatibilidades:";
    for (int x = 0; x < v_primerosPlatos[i].inc.size(); x++) {
      cout << "\n- " << v_primerosPlatos[i].inc[x];
    }
    cin.get();
  }

  cout << "\n\nSEGUNDOS PLATOS";
  for (int i = 0; i < v_segundosPlatos.size(); i++) {
    cout << "\nNombre: " << v_segundosPlatos[i].nombre;
    cout << "\ndiasrep: " << v_segundosPlatos[i].diasRep;
    cout << "\nprecio: " << v_segundosPlatos[i].precio;
    cout << "\ncantidad: " << v_segundosPlatos[i].cantidad;
    cout << "\ngrupos Al:";
    for (int x = 0; x < v_segundosPlatos[i].gruposAl.size(); x++) {
      cout << "\n- " << v_segundosPlatos[i].gruposAl[x];
    }
    cout << "\nInfo Nut:";
    for (int x = 0; x < v_segundosPlatos[i].infoN.size(); x++) {
      cout << "\n- " << v_segundosPlatos[i].infoN[x];
    }
    cout << "\nAlergenos:";
    for (int x = 0; x < v_segundosPlatos[i].alg.size(); x++) {
      cout << "\n- " << v_segundosPlatos[i].alg[x];
    }

    cout << "\nIncompatibilidades:";
    for (int x = 0; x < v_segundosPlatos[i].inc.size(); x++) {
      cout << "\n- " << v_segundosPlatos[i].inc[x];
    }
    cin.get();
  }

  cout << "\n\nPOSTRES";
  for (int i = 0; i < v_postres.size(); i++) {
    cout << "\nNombre: " << v_postres[i].nombre;
    cout << "\ndiasrep: " << v_postres[i].diasRep;
    cout << "\nprecio: " << v_postres[i].precio;
    cout << "\ncantidad: " << v_postres[i].cantidad;
    cout << "\ngrupos Al:";
    for (int x = 0; x < v_postres[i].gruposAl.size(); x++) {
      cout << "\n- " << v_postres[i].gruposAl[x];
    }
    cout << "\nInfo Nut:";
    for (int x = 0; x < v_postres[i].infoN.size(); x++) {
      cout << "\n- " << v_postres[i].infoN[x];
    }
    cout << "\nAlergenos:";
    for (int x = 0; x < v_postres[i].alg.size(); x++) {
      cout << "\n- " << v_postres[i].alg[x];
    }

    cout << "\nIncompatibilidades:";
    for (int x = 0; x < v_postres[i].inc.size(); x++) {
      cout << "\n- " << v_postres[i].inc[x];
    }
    cin.get();
  }
}

/*void MenuPlanning::repair(void) {
  bool mod = false;

  while (!checkInfoN2()) {
    for (int i = 0; i < nDias; i++) {
      setVar(i * num_tipoPlato, rand() % v_primerosPlatos.size());
      setVar(i * num_tipoPlato + 1, rand() % v_segundosPlatos.size());
      setVar(i * num_tipoPlato + 2, rand() % v_postres.size());
    }
    mod = true;
  }

  //	if(mod)
  //		evaluate();
}

bool MenuPlanning::checkInfoN(const int i) {
  double aux;
  for (int j = 0; j < num_nutr; j++) {
    aux = v_primerosPlatos[getVar(i * num_tipoPlato)].infoN[j] +
          v_segundosPlatos[getVar(i * num_tipoPlato + 1)].infoN[j] +
          v_postres[getVar(i * num_tipoPlato + 2)].infoN[j];
    if (aux < ingRecomendada[j].first || aux > ingRecomendada[j].second)
      return false;
  }
  return true;
}

bool MenuPlanning::checkInfoN2(void) {
  double aux;
  for (int j = 0; j < num_nutr; j++) {
    aux = 0;
    for (int i = 0; i < nDias; i++) {
      aux += v_primerosPlatos[getVar(i * num_tipoPlato)].infoN[j] +
             v_segundosPlatos[getVar(i * num_tipoPlato + 1)].infoN[j] +
             v_postres[getVar(i * num_tipoPlato + 2)].infoN[j];
    }
    if (aux < ingRecomendada[j].first || aux > ingRecomendada[j].second)
      return false;
  }
  return true;
}*/