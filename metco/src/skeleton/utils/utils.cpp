/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 * ********************************************************************************/

#include "float.h"
#include "utils.h"
#include <sys/time.h>
#include <iostream>
#include <sstream>
#include <strings.h>
#include <map>
#include <math.h>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <fstream>

using namespace std;

int objOrder;
// Separar una cadena en conjunto de cadenas usando como separador el espacio
vector<string> splitString(string s){
	vector<string> cadenas;
	int first = -1; 
	int next = 0;
	while(next != (int)string::npos){
		next = s.find(" ", first + 1);
		if ((next != (int)string::npos) && (next != first + 1)){
			cadenas.push_back(s.substr(first + 1, next - first - 1));
		}
		if ((next == (int)string::npos) && (first != (int)s.size() - 1)){
			cadenas.push_back(s.substr(first + 1, s.size() - first - 1));
		}
		first = next;
	}
	return cadenas;
}

// Buscar un string en un array de strings
int getIndex(string strToSearch, const string *array, int arraySize){
	for (int i = 0; i < arraySize; i++)
		if (strcasecmp(array[i].c_str(), strToSearch.c_str()) == 0)
			return (i);
	return -1;
}

// Inicializar la semilla para generacion de numeros aleatorios
void initRandomSeed(int mult){
	struct timeval t1;
	gettimeofday(&t1, NULL);
	srand ((unsigned int)(t1.tv_usec + t1.tv_sec) * mult);
}

void initFixedRandomSeed(int seed){
	srand (seed);
}

/****************************************************************************
*                                                                           *
* RandomUniform generates an equally distributed random number              *
* in the interval [0,1]. For further reference see Press,                   *
* W.H. et alii, Numerical Recipes in C, Cambridge University                *
* Press, 1992.                                                              *
*                                                                           *
*****************************************************************************/

double randomUniform(double minValue,double maxValue)
{
	long j;
	long k;
	static long idum;
	static long idum2=123456789;
	static long iy=0;
	static long iv[NTAB];
	double result;

	if (iy == 0)
		idum = SEED;

	if (idum <= 0)
	{
		if (-idum < 1)
			idum = 1;
		else
			idum = -idum;

		idum2 = idum;

		for (j=NTAB+7; j>=0; j--)
		{
			k = idum / IQ1;
			idum = IA1 * (idum - k*IQ1) - k*IR1;
			if (idum < 0) idum += IM1;
			if (j < NTAB) iv[j] = idum;
		}

		iy = iv[0];
	}

	k = idum / IQ1;
	idum = IA1 * (idum - k*IQ1) - k*IR1;

	if (idum < 0)
		idum += IM1;

	k = idum2 / IQ2;
	idum2 = IA2 * (idum2 - k*IQ2) - k*IR2;

	if (idum2 < 0)
		idum2 += IM2;

	j = iy / NDIV;
	iy = iv[j] - idum2;
	iv[j] = idum;

	if (iy < 1)
		iy += IMM1;

	result = AM * iy;

	if (result > RNMX)
		result = RNMX;

	result = minValue + result * (maxValue - minValue);
	return(result);
}

//Generacion de números aleatorios entre a y b sin repeticion
void generateRandomValues(const int a, const int b, const int n, vector<int> &values){
	vector<int> index;
	values.clear();
	if (n == 0)
		return;
	for (unsigned int i = a; i <= b; i++)
		index.push_back(i);

	if (n > index.size()){
		cerr << "Incorrect usage of generateRandomValues. Can not generate without repetitions" << endl;
		exit(-1);
	}
	for (int i = 0; i < n; i++) {
		int newIndex = (int) (((double)(b-a+1) - i)*rand()/(RAND_MAX+1.0));
		values.push_back(index[newIndex]);
		index[newIndex] = index[index.size()-i-1];
	}
}

int generateRandom(const int minValue, const int maxValue){
	return (int) (((double)(maxValue + 1 - minValue)*rand()/(RAND_MAX+1.0))) + minValue;
}

int getRandomInteger0_N(int n){
	return (int) ((n + 1.0)*rand()/(RAND_MAX+1.0));
}

//Nota se usa un valor truncado de pi porque en el extremo no está definido
double getRandomCauchy(double mean, double scale){
	double p = generateRandomDouble0_Max(1);
	return (mean + scale * tan(3.14159* (p - 0.5)));
}

// Incluido por Antonio J. Nebro
// Modificado por Carlos Segura
// Comprueba si ind1 domina a ind2
// ind1 domina a ind2 si todos sus objetivos son "mejores" (max-min)  
// o iguales y al menos uno de ellos es mejor
int dominanceTest (const Individual *ind1, const Individual *ind2) {
	int flag      ; 
	int dominate1 ;
	int dominate2 ;
	dominate1 = 0 ;
	dominate2 = 0 ;
	
	for(unsigned int i = 0; i < ind1->getNumberOfObj(); i++) {
		if (ind1->getInternalOptDirection(i) == MAXIMIZE){ 
			if (ind1->getObj(i) > ind2->getObj(i)) 
				flag = -1 ;
			else if (ind1->getObj(i) < ind2->getObj(i)) 
				flag = 1 ;
			else 
				flag = 0 ;
		} else {                          // Objetivo a minimizar
			if (ind1->getObj(i) < ind2->getObj(i)) 
				flag = -1;
			else if (ind1->getObj(i) > ind2->getObj(i))
				flag = 1 ;
			else
				flag = 0 ;
		}
		
		if (flag == -1)
			dominate1 = 1 ;
		if (flag == 1)
			dominate2 = 1 ;
	}
    
  if ((dominate1 == 0) && (dominate2 == 0)){//iguales
    return NON_DOMINANCES_EQUALS;  // Non dominated solutions
  }
	if ((dominate1 == 1) && (dominate2 == 1)){//no iguales
		return NON_DOMINANCES_NOT_EQUALS;
	}
  if (dominate1 == 1) {
    return FIRST_DOMINATES; // solution1 dominates
  }
  return SECOND_DOMINATES;    // solution2 dominates
}

void calculateRanks(const vector<Individual*> &pop, vector < vector <int > > &ranks){
	ranks.clear();
	vector<int> *indexesCurrentPop = new vector<int>;
	vector<Individual *> *currentPop = new vector<Individual*>;
	for (int i = 0; i < pop.size(); i++){
		currentPop->push_back(pop[i]);
		indexesCurrentPop->push_back(i);
	}
	vector<bool> dominated;
	do { //Until all individuals are ranked
		vector<Individual *> *newPop;
		vector<int> *indexesNewPop;
		dominated.clear();
		dominated.insert(dominated.begin(), currentPop->size(), false);
		ranks.push_back(vector<int>());
		for (int i = 0; i < currentPop->size(); i++) {
			if (dominated[i]) // i is dominated
				continue;
			for (int j = i+1; j < currentPop->size(); j++) {
				if (dominated[j])
					continue;
				int dominanceInformation = dominanceTest((*currentPop)[j], (*currentPop)[i]);
				if (dominanceInformation == FIRST_DOMINATES){
					dominated[i] = true;
					break;
				} else if (dominanceInformation == SECOND_DOMINATES){
					dominated[j] = true;
				}
			}
		}
		newPop = new vector<Individual*>;
		indexesNewPop = new vector<int>;
		for (int i = 0; i < currentPop->size(); i++){
			if (!dominated[i]){
				ranks[ranks.size()-1].push_back((*indexesCurrentPop)[i]);
			} else{
				newPop->push_back((*currentPop)[i]);
				indexesNewPop->push_back((*indexesCurrentPop)[i]);
			}
		}
		// Update the currentPop
		delete currentPop;
		delete indexesCurrentPop;
		currentPop = newPop;
		indexesCurrentPop = indexesNewPop;
	} while (currentPop->size() != 0);
	delete currentPop;
	delete indexesCurrentPop;
}

void markDominated(const vector<Individual*> &pop, vector<int> &dominatedIndexes){
	dominatedIndexes.clear();
	vector<bool> dominated(pop.size(), false);
	for (int i = 0; i < pop.size(); i++){
		if (dominated[i])
			continue;
		for (int j = i + 1; j < pop.size(); j++){
			if (dominated[j])
				continue;
			int dominance = dominanceTest(pop[i], pop[j]);
			if (dominance == FIRST_DOMINATES){
				dominated[j] = true;
				dominatedIndexes.push_back(j);
			} else if (dominance == SECOND_DOMINATES){
				dominated[i] = true;
				dominatedIndexes.push_back(i);
				break;
			}
		}
	}
}

void markNonDominated(const vector<Individual*> &pop, vector<int> &nonDominatedIndexes){
	nonDominatedIndexes.clear();
	vector<bool> dominated(pop.size(), false);
	for (int i = 0; i < pop.size(); i++){
		if (dominated[i])
			continue;
		for (int j = i + 1; j < pop.size(); j++){
			if (dominated[j])
				continue;
			int dominance = dominanceTest(pop[i], pop[j]);
			if (dominance == FIRST_DOMINATES){
				dominated[j] = true;
			} else if (dominance == SECOND_DOMINATES) {
				dominated[i] = true;
				break;
			}
		}
	}
	for (int i = 0; i < pop.size(); i++){
		if (dominated[i] == false){
			nonDominatedIndexes.push_back(i);
		}
	}
}

//Clasify at least (front1Size + front1Size*incr)  
void FastNondominatedSort2(vector<Individual *> *p, vector < vector<Individual*> > &fronts, vector<Individual *> &notClassified, int &differentsInFront0, double incr){

	differentsInFront0 = 0;
	int classified = 0;
	fronts.clear();
	notClassified.clear();
	vector < vector < int > > dominates(p->size(), vector<int>());//dominates[i] --> individuals dominated by individual i
	vector < int > dominatedBy(p->size(), 0); //dominatedBy[i] --> i is dominated by such number of individuals
	vector < bool > inserted(p->size(), false);//inserted[i] --> true: individual i has been inserted in a front

	//Assign Identifier to Individuals
	bool spaceAssigned = false;
	int spaceAssignedForId = 0;
	for (int i = 0; i < p->size(); i++){
		int space = (*p)[i]->requestAlgorithmData();
		if (!spaceAssigned){
			spaceAssigned = true;
			spaceAssignedForId = space;
		} else if (space != spaceAssignedForId) {
			cerr << "Internal Error: different space assigned: " << space << " " << spaceAssignedForId << endl;
		}
		(*p)[i]->setAlgorithmData(spaceAssignedForId, i);
	}

	//Calculate F1
	vector<int> equals(p->size(), 1);
	fronts.push_back(vector <Individual *> ());
	for (int i = 0; i < p->size(); i++){
		for (int j = i + 1; j < p->size(); j++){
			int dominanceInfo = dominanceTest((*p)[i], (*p)[j]);
			if (dominanceInfo == FIRST_DOMINATES){
				dominates[i].push_back(j);
				dominatedBy[j]++;
			} else if (dominanceInfo == SECOND_DOMINATES){
				dominates[j].push_back(i);
				dominatedBy[i]++;
			} else if (dominanceInfo == NON_DOMINANCES_EQUALS){
				equals[i]++;
				equals[j]++;
			}
		}
		if (dominatedBy[i] == 0){
			fronts[0].push_back((*p)[i]);
			inserted[i] = true;
			classified++;
		}
	}

	int minClassified = fronts[0].size() * incr;

	//Calculate diffent elements
	map<int, int> count;//cuantos hay que indiquen q tiene n iguales
	for (int i = 0; i < fronts[0].size(); i++){
		count[equals[fronts[0][i]->getAlgorithmData(spaceAssignedForId)]]++;
	}
	//Dividiendo el numero de elementos que indican que tienen n iguales, por n
	//y sumando obtenemos el numero de elementos diferentes
	for (map<int, int>::iterator i = count.begin(); i != count.end(); i++){
		differentsInFront0 += (i->second / i->first);
	}

	
	//Calculate remaining F
	while((fronts[fronts.size() - 1].size() != 0) && (classified < minClassified)){
		fronts.push_back(vector <Individual *>() );
		vector<Individual *> &fi = fronts[fronts.size() - 2];
		for (int i = 0; i < fi.size(); i++){
			for (int j = 0; j < dominates[fi[i]->getAlgorithmData(spaceAssignedForId)].size(); j++){
				dominatedBy[dominates[fi[i]->getAlgorithmData(spaceAssignedForId)][j]]--;
				if (dominatedBy[dominates[fi[i]->getAlgorithmData(spaceAssignedForId)][j]] == 0){
					fronts[fronts.size() - 1].push_back((*p)[dominates[fi[i]->getAlgorithmData(spaceAssignedForId)][j]]);
					inserted[dominates[fi[i]->getAlgorithmData(spaceAssignedForId)][j]] = true;
					classified++;
				}
			}
		}
	}
	if (fronts[fronts.size() - 1].size() == 0){
		fronts.pop_back();
	}
	for (int i = 0; i < p->size(); i++){
		if (inserted[i] == false){
			notClassified.push_back((*p)[i]);
		}
		(*p)[i]->releaseAlgorithmData();
	}
}


//Clasify p in fronts. When at least minN has been classified, the remaining are not classified
void FastNondominatedSort(vector<Individual *> *p, vector < vector<Individual*> > &fronts, vector<Individual *> &notClassified, int &differentsInFront0, int minClassified){

	differentsInFront0 = 0;
	int classified = 0;
	fronts.clear();
	notClassified.clear();
	vector < vector < int > > dominates(p->size(), vector<int>());//dominates[i] --> individuals dominated by individual i
	vector < int > dominatedBy(p->size(), 0); //dominatedBy[i] --> i is dominated by such number of individuals
	vector < bool > inserted(p->size(), false);//inserted[i] --> true: individual i has been inserted in a front

	//Assign Identifier to Individuals
	bool spaceAssigned = false;
	int spaceAssignedForId = 0;
	for (int i = 0; i < p->size(); i++){
		int space = (*p)[i]->requestAlgorithmData();
		if (!spaceAssigned){
			spaceAssigned = true;
			spaceAssignedForId = space;
		} else if (space != spaceAssignedForId) {
			cerr << "Internal Error: different space assigned: " << space << " " << spaceAssignedForId << endl;
		}
		(*p)[i]->setAlgorithmData(spaceAssignedForId, i);
	}

	//Calculate F1
	vector<int> equals(p->size(), 1);
	fronts.push_back(vector <Individual *> ());
	for (int i = 0; i < p->size(); i++){
		for (int j = i + 1; j < p->size(); j++){
			int dominanceInfo = dominanceTest((*p)[i], (*p)[j]);
			if (dominanceInfo == FIRST_DOMINATES){
				dominates[i].push_back(j);
				dominatedBy[j]++;
			} else if (dominanceInfo == SECOND_DOMINATES){
				dominates[j].push_back(i);
				dominatedBy[i]++;
			} else if (dominanceInfo == NON_DOMINANCES_EQUALS){
				equals[i]++;
				equals[j]++;
			}
		}
		if (dominatedBy[i] == 0){
			fronts[0].push_back((*p)[i]);
			inserted[i] = true;
			classified++;
		}
	}

	//Calculate diffent elements
	map<int, int> count;//cuantos hay que indiquen q tiene n iguales
	for (int i = 0; i < fronts[0].size(); i++){
		count[equals[fronts[0][i]->getAlgorithmData(spaceAssignedForId)]]++;
	}
	//Dividiendo el numero de elementos que indican que tienen n iguales, por n
	//y sumando obtenemos el numero de elementos diferentes
	for (map<int, int>::iterator i = count.begin(); i != count.end(); i++){
		differentsInFront0 += (i->second / i->first);
	}

	
	//Calculate remaining F
	while((fronts[fronts.size() - 1].size() != 0) && (classified < minClassified)){
		fronts.push_back(vector <Individual *>() );
		vector<Individual *> &fi = fronts[fronts.size() - 2];
		for (int i = 0; i < fi.size(); i++){
			for (int j = 0; j < dominates[fi[i]->getAlgorithmData(spaceAssignedForId)].size(); j++){
				dominatedBy[dominates[fi[i]->getAlgorithmData(spaceAssignedForId)][j]]--;
				if (dominatedBy[dominates[fi[i]->getAlgorithmData(spaceAssignedForId)][j]] == 0){
					fronts[fronts.size() - 1].push_back((*p)[dominates[fi[i]->getAlgorithmData(spaceAssignedForId)][j]]);
					inserted[dominates[fi[i]->getAlgorithmData(spaceAssignedForId)][j]] = true;
					classified++;
				}
			}
		}
	}
	if (fronts[fronts.size() - 1].size() == 0){
		fronts.pop_back();
	}
	for (int i = 0; i < p->size(); i++){
		if (inserted[i] == false){
			notClassified.push_back((*p)[i]);
		}
		(*p)[i]->releaseAlgorithmData();
	}
}

void crowOrder(vector<Individual*> &pop) {
	double difference;
	
	// Clear the fitness of the individuals
	for (unsigned int i = 0; i < pop.size(); i++)
		(pop)[i]->setFitnessValue(0);
	
	// Calculate the distances (for each objective)
	for (unsigned int i = 0; i < (pop)[0]->getNumberOfObj(); i++) {
		objOrder = i;
		sort(pop.begin(), pop.end(), orderByObjective);
		//En los limities, en caso de igualdad ponemos el que mayor valor actual tenga de fitness
		//Es para tratar un poco mejor las repeticiones 
		//De todas formas, el algoritmo no se comporta bien frente a las repeticiones
		//En algunos articulos se elimina de uno en uno, y no todoas a la vez, para evitar
		//estos problemas (aunque eso conllevaria mas computo)
		for (int j = 1; j < pop.size(); j++){
			if (pop[j]->getObj(i) != pop[0]->getObj(i)){
				break;
			}
			if (pop[j]->getFitnessValue() > pop[0]->getFitnessValue()){
				Individual *tmp = pop[0];
				pop[0] = pop[j];
				pop[j] = tmp;
			}
		}

		for (int j = pop.size() - 2; j >= 0; j--){
			if (pop[j]->getObj(i) != pop[pop.size() - 1]->getObj(i)){
				break;
			}
			if (pop[j]->getFitnessValue() > pop[pop.size()-1]->getFitnessValue()){
				Individual *tmp = pop[pop.size()-1];
				pop[pop.size()-1] = pop[j];
				pop[j] = tmp;
			}
		}
		
		difference = (pop)[pop.size()-1]->getObj(i) - (pop)[0]->getObj(i);
		if (difference == 0)
			difference = 1;
		(pop)[0]->setFitnessValue(DBL_MAX);
		(pop)[pop.size()-1]->setFitnessValue(DBL_MAX);
		for (unsigned int j = 1; j < pop.size()-1; j++) {
			if ((pop)[j]->getFitnessValue() != DBL_MAX) {
				(pop)[j]->setFitnessValue((((pop)[j+1]->getObj(i) - (pop)[j-1]->getObj(i))/(difference)) + (pop)[j]->getFitnessValue() );
			}
		}
	}
	
	// Ordenamos por el valor calculado
	sort(pop.begin(), pop.end(), ordByFitness);
}

// Función auxiliar para ordenación por objetivos
bool orderByObjective(Individual *i1, Individual *i2) {
	return (i1->getObj(objOrder) < i2->getObj(objOrder));
}

void changeFirst(string &str, int n, string &result){
	stringstream newStr;
	bool changed = false;
	for (int i = 0; i < str.size(); i++){
		if ((changed) || (str[i] != '$')){
			newStr << str[i];
		} else {
			changed = true;
			newStr << n;
		}
	}
	result = newStr.str();
}

// Calculate the distance between two individuals
int hammingDistance(const Individual* i1, const Individual* i2) {
   int dist = 0;
   for (unsigned int i = 0; i < i1->getNumberOfVar(); i++) {
      if (i1->getVar(i) != i2->getVar(i)) {
	 dist++;
      }				
   }
   return dist;
}

double generateRandomDouble0_Max(double maxValue){
	return (double)(rand()) / RAND_MAX * maxValue;
}

double generateRandomDoubleMinMax(double min, double max){
	return ((double)(rand()) / RAND_MAX * (max - min)) + min;
}

double z2, lastMean, lastDesv;
bool z2Generated = false;

double generateRandomNormal(double mean, double desv){
	double v1, v2, w, z1;
	if ((z2Generated) && (mean == lastMean) && (desv == lastDesv)){/* use value from previous call */
		z1 = z2;
		z2Generated = false;
	} else {
			do {
				v1 = ((2.0*rand()/(RAND_MAX))) - 1.0;
				v2 = ((2.0*rand()/(RAND_MAX))) - 1.0;
				w = v1 * v1 + v2 * v2;
			} while (( w >= 1.0 ) || (w == 0));

			w = sqrt( (-2.0 * log( w ) ) / w );
			z1 = v1 * w;
			z2 = v2 * w;
			z2Generated = true;
		}
	lastMean = mean;
	lastDesv = desv;
	return( mean + z1 * desv );
}

// ----- SOBOL POINTS GENERATOR BASED ON GRAYCODE ORDER -----------------
// INPUT: 
//   N         number of points  (cannot be greater than 2^32)
//   D         dimension  (make sure that the data file contains enough data!!)      
//   dir_file  the input file containing direction numbers
//
// OUTPUT:
//   A 2-dimensional array points, where
//     
//     points[i][j] = the jth component of the ith point,
//   
//   with i indexed from 0 to N-1 and j indexed from 0 to D-1
//
// Received from http://web.maths.unsw.edu.au/~fkuo/sobol/
//
// ----------------------------------------------------------------------

double **sobol_points(unsigned N, unsigned D, const char *dir_file)
{
  ifstream infile(dir_file,ios::in);
  if (!infile) {
    cout << "Input file containing direction numbers cannot be found!\n";
    exit(1);
  }
  char buffer[1000];
  infile.getline(buffer,1000,'\n');
  
  // L = max number of bits needed 
  unsigned L = (unsigned)ceil(log((double)N)/log(2.0)); 

  // C[i] = index from the right of the first zero bit of i
  unsigned *C = new unsigned [N];
  C[0] = 1;
  for (unsigned i=1;i<=N-1;i++) {
    C[i] = 1;
    unsigned value = i;
    while (value & 1) {
      value >>= 1;
      C[i]++;
    }
  }
  
  // points[i][j] = the jth component of the ith point
  //                with i indexed from 0 to N-1 and j indexed from 0 to D-1
  double **points = new double * [N];
  for (unsigned i=0;i<=N-1;i++) points[i] = new double [D];
  for (unsigned j=0;j<=D-1;j++) points[0][j] = 0; 

  // ----- Compute the first dimension -----
  
  // Compute direction numbers V[1] to V[L], scaled by pow(2,32)
  unsigned *V = new unsigned [L+1]; 
  for (unsigned i=1;i<=L;i++) V[i] = 1 << (32-i); // all m's = 1

  // Evalulate X[0] to X[N-1], scaled by pow(2,32)
  unsigned *X = new unsigned [N];
  X[0] = 0;
  for (unsigned i=1;i<=N-1;i++) {
    X[i] = X[i-1] ^ V[C[i-1]];
    points[i][0] = (double)X[i]/pow(2.0,32); // *** the actual points
    //        ^ 0 for first dimension
  }
  
  // Clean up
  delete [] V;
  delete [] X;
  
  
  // ----- Compute the remaining dimensions -----
  for (unsigned j=1;j<=D-1;j++) {
    
    // Read in parameters from file 
    unsigned d, s;
    unsigned a;
    infile >> d >> s >> a;
    unsigned *m = new unsigned [s+1];
    for (unsigned i=1;i<=s;i++) infile >> m[i];
    
    // Compute direction numbers V[1] to V[L], scaled by pow(2,32)
    unsigned *V = new unsigned [L+1];
    if (L <= s) {
      for (unsigned i=1;i<=L;i++) V[i] = m[i] << (32-i); 
    }
    else {
      for (unsigned i=1;i<=s;i++) V[i] = m[i] << (32-i); 
      for (unsigned i=s+1;i<=L;i++) {
	V[i] = V[i-s] ^ (V[i-s] >> s); 
	for (unsigned k=1;k<=s-1;k++) 
	  V[i] ^= (((a >> (s-1-k)) & 1) * V[i-k]); 
      }
    }
    
    // Evalulate X[0] to X[N-1], scaled by pow(2,32)
    unsigned *X = new unsigned [N];
    X[0] = 0;
    for (unsigned i=1;i<=N-1;i++) {
      X[i] = X[i-1] ^ V[C[i-1]];
      points[i][j] = (double)X[i]/pow(2.0,32); // *** the actual points
      //        ^ j for dimension (j+1)
   }
    
    // Clean up
    delete [] m;
    delete [] V;
    delete [] X;
  }
  delete [] C;
  
  return points;
}


//****************************************************************************80

int *find_z20 ( int dim_num, int m )

//****************************************************************************80
//
//  Purpose:
//
//    FIND_Z20 finds the appropriate Z vector to minimize P2(QS).
//
//  Discussion:
//
//    For the method of good lattice points, a number of points M, and
//    a single generator vector Z is chosen.  The integrand is assumed
//    to be periodic of period 1 in each argument, and is evaluated at
//    each of the points X(I)(1:DIM_NUM) = I * Z(1:DIM_NUM) / M, 
//    for I = 0 to M-1.  The integral is then approximated by the average
//    of these values.
//
//    Assuming that DIM_NUM and M are known, and that the integrand is not
//    known beforehand, the accuracy of the method depends entirely
//    on the choice of Z.  One method of choosing Z is to search for
//    the Z among all candidates which minimizes a particular quantity
//    P_ALPHA(QS).
//
//    We only need to look at vectors Z of the form
//    (1, L, L^2, ..., L^(DIM_NUM-1)),
//    for L = 1 to M/2.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    23 November 2008
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Ian Sloan, Stephen Joe,
//    Lattice Methods for Multiple Integration,
//    Oxford, 1994,
//    ISBN: 0198534728,
//    LC: QA311.S56
//
//  Parameters:
//
//    Input, int DIM_NUM, the spatial dimension.
//
//    Input, int M, the number of points to be used.
//
//    Output, int FIND_Z20[DIM_NUM], the optimal vector.
//
{
  int dim;
  int i;
  long double q0;
  long double q0_min;
  int value;
  int *z;
  int *z_min;

  z = new int[dim_num];
  z_min = new int[dim_num];

  q0_min = r8_huge ( );

  for ( i = 1; i <= m / 2; i++ )
  {
    value = 1;
    for ( dim = 0; dim < dim_num; dim++ )
    {
      z[dim] = value;
      value = ( value * i ) % m;
    }
//
//  Use this Z and the lattice integral method Q0 of order M,
//  to approximate the integral of P2.
//
    q0 = lattice ( dim_num, m, z, f20_s );
//
//  If this result is the smallest so far, save the corresponding Z.
//
    if ( q0 < q0_min )
    {
      q0_min = q0;
      for ( dim = 0; dim < dim_num; dim++ )
      {
        z_min[dim] = z[dim];
      }
    }
  }
  delete [] z;
//
//  Return the best Z.
//
  return z_min;
}

//****************************************************************************80

long double lattice ( int dim_num, int m, int z[], 
  long double f ( int dim_num, long double x[] ) )

//****************************************************************************80
//
//  Purpose:
//
//    LATTICE applies a lattice integration rule.
//
//  Discussion:
//
//    Because this is a standard lattice rule, it is really only suited
//    for functions which are periodic, of period 1, in both X and Y.
//
//    For a suitable F, and a given value of M (the number of lattice points),
//    the performance of the routine is affected by the choice of the
//    generator vector Z.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    20 November 2008
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Ian Sloan, Stephen Joe,
//    Lattice Methods for Multiple Integration,
//    Oxford, 1994,
//    ISBN: 0198534728,
//    LC: QA311.S56
//
//  Parameters:
//
//    Input, int DIM_NUM, the spatial dimension.
//
//    Input, int M, the order (number of points) of the rule.
//
//    Input, int Z[DIM_NUM], the generator vector.  Typically, the elements
//    of Z satisfy 1 <= Z[*] < M, and are relatively prime to M.
//    This is easy to guarantee if M is itself a prime number.
//
//    Input, double F ( int DIM_NUM, double X[] ), the name of the 
//    user-supplied routine which evaluates the function.
//
//    Output, double LATTICE, the estimated integral.
//
{
  int i;
  int j;
  long double quad;
  long double *x;

  x = new long double[dim_num];

  quad = 0.0;

  for ( j = 0; j <= m - 1; j++ )
  {
    for ( i = 0; i < dim_num; i++ )
    {
      x[i] = fmod ( ( long double ) ( j * z[i] ) / ( long double ) ( m ), 1.0 );
    }
    quad = quad + f ( dim_num, x );
  }

  quad = quad / ( long double ) ( m );

  delete [] x;

  return quad;
}

//****************************************************************************80

long double f20_s ( int dim_num, long double x[] )

//****************************************************************************80
//
//  Purpose:
//
//    F20_S evaluates a function of a vector used in defining P2(Q).
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    19 November 2008
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Ian Sloan, Stephen Joe,
//    Lattice Methods for Multiple Integration,
//    Oxford, 1994,
//    ISBN: 0198534728,
//    LC: QA311.S56
//
//  Parameters:
//
//    Input, int DIM_NUM, the spatial dimension.
//
//    Input, double X[DIM_NUM], the value of the argument.
//
//    Output, double F20_S, the value of F20_S(X).
//
{
  int i;
  long double value;

  value = 1.0;
  for ( i = 0; i < dim_num; i++ )
  {
    value = value * ( 1.0 + ( f2 ( x[i] ) - 1.0 ) );
  }
  value = value - 1.0;

  return value;
}

//****************************************************************************80

long double f2 ( long double x )

//****************************************************************************80
//
//  Purpose:
//
//    F2 evaluates a function of a scalar used in defining P2(Q).
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    19 November 2008
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Ian Sloan, Stephen Joe,
//    Lattice Methods for Multiple Integration,
//    Oxford, 1994,
//    ISBN: 0198534728,
//    LC: QA311.S56
//
//  Parameters:
//
//    Input, double X, the value of the argument.
//
//    Output, double F2, the value of F2(X).
//
{
  long double pi = 3.141592653589793;
  long double value;

  value = 1.0 + 2.0 * pi * pi * ( x * x - x + 1.0 / 6.0 );

  return value;
}

//****************************************************************************80

long double r8_huge ( )

//****************************************************************************80
//
//  Purpose:
//
//    R8_HUGE returns a "huge" R8.
//
//  Discussion:
//
//    The value returned by this function is NOT required to be the
//    maximum representable R8.  This value varies from machine to machine,
//    from compiler to compiler, and may cause problems when being printed.
//    We simply want a "very large" but non-infinite number.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license. 
//
//  Modified:
//
//    06 October 2007
//
//  Author:
//
//    John Burkardt
//
//  Parameters:
//
//    Output, double R8_HUGE, a "huge" R8 value.
//
{
  long double value;

  //value = 1.0E+30;
  value = LDBL_MAX;

  return value;
}
