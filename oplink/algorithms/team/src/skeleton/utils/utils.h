/***********************************************************************************
 * AUTORES
 *   - Ofelia Gonz·lez PÈrez
 *   - Gara Miranda Valladares
 *   - Carlos Segura Gonz·lez
 * 
 * FECHA
 *    Noviembre 2007
 *
 * DESCRIPCION
 *    Funciones de uso general
 * ********************************************************************************/
 
#ifndef __UTILS_H__
#define __UTILS_H__

#include "Individual.h"
#include <vector>
#include <string>

// Constantes necesarias para randomUniform
#define SEED 3
#define IM1 2147483563
#define IM2 2147483399
#define AM (1.0/IM1)
#define IMM1 (IM1-1)
#define IA1 40014
#define IA2 40692
#define IQ1 53668
#define IQ2 52774
#define IR1 12211
#define IR2 3791
#define NTAB 32
#define NDIV (1+IMM1/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)


using namespace std;

// Separar una cadena en conjunto de cadenas usando como separador el espacio
vector<string> splitString(string s);

// Buscar un string en un array de strings
int getIndex(string strToSearch, const string *array, int arraySize);

// Inicializar la semilla para generacion de numeros aleatorios
void initRandomSeed(int mult = 1);
void initFixedRandomSeed(int seed);

// Incluido por Mar√≠a Dolores Jara√≠z Sim√≥n
// Uniform pseudorandom number generator 
double randomUniform(double minValue,double maxValue);

//Generacion de n˙meros aleatorios entre a y b sin repeticion
void generateRandomValues(const int a, const int b, const int n, vector<int> &values);

//Generar numero aleatorio [minValue, maxValue] con dist. uniforme
int generateRandom(const int minValue, const int maxValue);

double generateRandomDouble0_Max(double maxValue);

double generateRandomDoubleMinMax(double min, double max);//get random double [min, max]

double generateRandomNormal(double mean, double desv);

double getRandomCauchy(double mean, double scale);

int getRandomInteger0_N(int n);//get random integer [0, n]

// Incluido por Eduardo Segredo (Sobol set)
double **sobol_points(unsigned N, unsigned D, const char *dir_file);

// Incluido por Eduardo Segredo (Good Lattice Points)
long double f2 ( long double x );
long double f20_s ( int dim_num, long double x[] );
int *find_z20 ( int dim_num, int m );
long double lattice ( int dim_num, int m, int z[], long double f ( int dim_num, long double x[] ) );
long double r8_huge ( );

// Devuelve -1 si ind1 domina a ind2
// Devuelve 0 si ind1 e ind2 no se dominan mutuamente
// Devuelve 1 si ind1 es dominado por ind2
//
#define FIRST_DOMINATES -1
#define SECOND_DOMINATES 1
#define NON_DOMINANCES_EQUALS 2
#define NON_DOMINANCES_NOT_EQUALS 3
// Incluido por Antonio J. Nebro. 
int dominanceTest (const Individual *ind1, const Individual *ind2);

void markDominated(const vector<Individual*> &pop, vector<int> &dominatedIndexes);
void markNonDominated(const vector<Individual*> &pop, vector<int> &dominatedIndexes);
void calculateRanks(const vector<Individual*> &pop, vector < vector <int > > &ranks);
void FastNondominatedSort(vector<Individual *> *p, vector < vector<Individual*> > &fronts, vector<Individual *> &notClassified, int &differentsInFront0, int minClassified);
void FastNondominatedSort2(vector<Individual *> *p, vector < vector<Individual*> > &fronts, vector<Individual *> &notClassified, int &differentsInFront0, double incr);
void crowOrder(vector<Individual*> &pop);
bool orderByObjective(Individual *i1, Individual *i2);
void changeFirst(string &str, int n, string &result);
int hammingDistance(const Individual* i1, const Individual* i2);

#endif
