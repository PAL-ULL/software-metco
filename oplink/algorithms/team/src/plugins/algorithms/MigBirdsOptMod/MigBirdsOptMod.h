/***********************************************************************************
 * AUTHORS
 *  - Eduardo Segredo Gonzalez
 * 
 * DATE
 *    April 2016
 *
 **********************************************************************************/


#ifndef MBO_H
#define MBO_H

#include "Individual.h"
#include "EA.h"

#include <vector>
#include <map>
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <math.h>
#include <functional>

using namespace std;

// Variable globalSearchType
#define GS_NO 1
#define GS_GLOBAL_NEIGHBOURHOOD 2
#define GS_GLOBAL_NEIGHBOURHOOD_PATH_SEARCH 3

struct extIndType;

bool sortByObj(const Individual *i1, const Individual *i2);

void readParameter(map<string,string> &parameters, const string &parameter, double &value);
void readParameter(map<string,string> &parameters, const string &parameter, int &value);
void readParameter(map<string,string> &parameters, const string &parameter, string &value);

class MigBirdsOptMod : public EA {

	public:
		bool init(const vector<string> &params);
    void getSolution(MOFront *p);
		void printInfo(ostream &os) const;
	private:
		void runGeneration();
    void flyFlock();

    // MBO original
    void replaceLeaderWithSuccessorOrig();
    // Variante Lalla (Virolo 2): Va bien, pero un poco peor que Virolo original
	  void replaceLeaderWithSuccessor();
    // Variante Edu: Solo va bien en dos problemas
	  void replaceLeaderWithSuccessor2();
    // Variante Edu: Basado en el de Lalla (Virolo 2) pero alternando el ala fija. No va muy bien.
	  void replaceLeaderWithSuccessor3();
    // Virolo original: Es uno de los mejores que va. Resultados en PPSN2016/version2 y PPSN2016/version3
	  void replaceLeaderWithSuccessor4();
    // Virolo original, pero alternando el ala de la formacion: Va bien pero un poco peor que Virolo original
	  void replaceLeaderWithSuccessor5();
    // Virolo 2B: Es como Virolo 2, pero comprueba que la solucion que pase al final de la formacion
    // en el ala fija sea mejor que el que va a sustituir. Si es mejor la sustituye. En otro caso, 
    // la descarta. Va igual que Virolo original, incluso un poco mejor. Resultados en PPSN2016/version4
	  void replaceLeaderWithSuccessor6();
    // Virolo 2B, pero alternando el ala fija y el desplazamiento: Va bien, pero un poco peor que Virolo 2B
	  void replaceLeaderWithSuccessor7();
    // Virolo 2C: Es como Virolo 2B, pero va variando secuencialmente la solución del ala que pasa a ser lider
    // No va tan bien como Virolo 2B o Virolo original.
	  void replaceLeaderWithSuccessor8();
    // Virolo 2B, pero alternando el ala fija y sin desplazamiento en ambas alas. No va bien.
	  void replaceLeaderWithSuccessor9();
    // Virolo 2D: Es como Virolo 2C, pero en lugar de enviar la solucion que era lider al final de la formación,
    // la envía a la posición donde se encontraba la que ha pasado a ser líder. No va tan bien como Virolo 2B,
    // Virolo 2C o Virolo Original
	  void replaceLeaderWithSuccessor10();
    // Virolo 2E: El reemplazo de lider se lleva a cabo con un desplazamiento circular en la V, en el sentido de
    // las agujas del reloj. No va muy bien.
	  void replaceLeaderWithSuccessor11();
    // Virolo 2F: Para reemplazar el lider, se comprueba cual de los dos seguidores directos es mejor. Ese se
    // duplica y el que era lider pasa al final de la formacion, siempre y cuando sea mejor que el individuo
    // que va a reemplazar. En el otro ala, se lleva a cabo un desplazamiento, tras haber enviado al final de
    // la formacion al otro sucesor.
	  void replaceLeaderWithSuccessor12();

    Individual* lookForBestInd();
    Individual* lookForWorstInd();
    Individual* getBestIndividual0_N(int bestp);
		
    void generateF(int ind);
		void generateCR(int ind);
		void updateNuF();
		void updateNuCR();

    // Parameterisation
    bool globalSearchRequired;
    int globalSearchType;

    // Attributes
    int noi, nob, non, olf, nof;
    int currentLeaderSuccessor;
    int contLeaders, contNoLeaders;

    int accepted;
    double mut_f, cross_cr, p, c, nuF, nuCR;
    vector<double> successfullFs, successfullCRs, fValues, crValues;
    vector< vector<double> > neighbor_fValues, neighbor_crValues;

    bool leftSide, rightSideFixed;
    vector<int> noLeaders;
    vector< vector<int> > leaders;
    vector< vector<Individual*> > neighborSets;
    vector<Individual*> bests;

    // Methods for manipulating neighbor sets
    void createNeighborSet(int ind, int nongbr);
    Individual* neighborDE_Rand_Fixed(int ind);
    Individual* neighborDE_Rand_Adapt(int ind);
    Individual* neighborDE_CurrentToPBest_Adapt(int ind);
    Individual* neighborSphere(int ind);
    Individual* getBestNeighbor(int ind, bool remove, double &f, double &cr);
    void addNeighbor(int ind, Individual *aux, double f, double cr);

    void globalSearch();   
    // Global search original implementation 
    void globalSearchGlobalNeighbourhood();
    // Lineal descending function
    void globalSearchGlobalNeighbourhood8();
    
    // Sorts the population (extended individuals) in ascending order by considering the 
    // Euclidean distance in the decision space to the fittest individual
    bool sortByDistanceToBestExtInd(const extIndType *i1, const extIndType *i2, const Individual *best);

};

#endif
