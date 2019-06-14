/*****************************************************************************************************
 *	Authors
 *		Eduardo Segredo Gonzalez
 *		Carlos Segura Gonzalez
 *
 * 	Description
 * 		Clase que representa a un individuo para resolver el problema 2DPack. La representacion vienen
 * 		dada por un cromosoma bidimensional (vector < vector < int > >). Cada posicion representa el
 * 		numero que se asignaria.
 * 		El unico operador evolutivo que existe es la mutacion, y no toma ventaja de que sea un cromosoma
 * 		bidimensional, asi que se puede considerar que esta linealizado.
 *****************************************************************************************************/


#ifndef TWODPACK_H
#define TWODPACK_H

#include "Individual.h"

class AssignationCost{
	public:
		AssignationCost(const int assignation, const int v, const int vUpper) { this->assignation = assignation; this->v = v; this->vUpper = vUpper; this->upperBound = v + vUpper; }
		inline int getV() const { return v; }
		inline int getVUpper() const { return vUpper; }
		inline int getAssignation() const { return assignation; }
		inline int getUpperBound() const { return upperBound; }
	private:
		int assignation;
		int v;
		int vUpper;
		int upperBound;
};

struct PairValueStruct {
  int pairA;
  int pairB;
  int value;
};

bool cmpV(const AssignationCost *c1, const AssignationCost *c2);
bool cmpBestPartner(const AssignationCost *c1, const AssignationCost *c2);
bool cmpUpperBound(const AssignationCost *c1, const AssignationCost *c2);
bool cmpPairValueStruct (const PairValueStruct &a, const PairValueStruct &b);

class Position{
	public:
		Position(const int x, const int y);
		inline int getX() const { return x; }
		inline int getY() const { return y; }
		inline void setX(const int x) { this->x = x; } 
		inline void setY(const int y) { this->y = y; } 
	private:
		int x;
		int y;
};

class TwoDPackingProblem{
	public:
		TwoDPackingProblem(const string &fileName);
		int inline getValue(const int val1, const int val2) const { return partnersValues[val1][val2]; }
		int inline getXSize() const { return xSize; }
		int inline getYSize() const { return ySize; }
		int inline getMaxNumber() const { return maxNumber; }
    int inline getPair2Obj(const int &v1, const int &v2) const { return pair2Obj[v1][v2]; }
		const vector<int> &getPartners(const int i) const { return partners[i]; }
		int getBestValue(const int i) const { return bestValues[i]; }
		int getRandomPartner(const int value) const { return partners[value][(int) (partners[value].size()*rand()/(RAND_MAX+1.0))]; }
	private:
		int xSize, ySize; // grid size
		int maxNumber; // cantidad de numeros que se pueden usar en la grid
		vector< vector< int > > partnersValues; // parternsValues[x,y] es el valor asociado a la aparicion de la pareja (x,y)
		vector< vector< int > > partners; // partners[x] es un vector con todos los y para los que v(x,y) no es 0 
		vector< int > bestValues; // bestValues[x] es el mayor valor asociado a una pareja que en x aparece
    vector< PairValueStruct > orderedPairs; // Pairs ordered by value
    vector< vector <int> > pair2Obj; // Specifies which objective is considered for the pair
};

class TwoDPack_MultiObjectivized : public Individual{
	public:
		// Constructores y destructores
		TwoDPack_MultiObjectivized() { nInds++; }
		virtual ~TwoDPack_MultiObjectivized();

		// Métodos de Individual
		bool init (const vector<string> &params);
		void evaluate (void);
		void restart(void);
		void dependentMutation( double pm );

		unsigned int inline getOptDirection(const int i) const { return MAXIMIZE; }
		double inline getMinimum(const int i) const { return 0; }
		double inline getMaximum(const int i) const { return twoDPackingProblem->getMaxNumber(); }
		Individual* clone() const;

		// Imprimir el individuo
		friend ostream& operator<< (ostream &os, const TwoDPack_MultiObjectivized &twoDPackingIndividual);

	private:
		// Generacion aleatoria de un individuo
		void generateRandom();

		// Obtenencion de numero asignado a una posicion
		inline int getGrid(const int x, const int y) const { return (int) getVar(y * twoDPackingProblem->getXSize() + x); }
		inline void setGrid(const int x, const int y, const int value) { setVar(y * twoDPackingProblem->getXSize() + x, value); }

		// Obtencion de la cantidad de veces que una pareja aparece en la solucion actual
		inline int getAppearance(const int x, const int y) const { return appearance[x][y]; }
		
		// Actualiza el fitness y num. de aparicion de parajes, considerando los valores en
		// las casillas (x1,y1) y (x2,y2)
		void update(const int x1, const int y1, const int x2, const int y2);
		void updatePartial(const int x1, const int y1, const int x2, const int y2);

		// Busquedas locales
		void localSearch(); // Este metodo simplemente tiene una llamada a localSearch1 o 2, en funcion de la que queramos usar
		
		// Busqueda local basada en cambiar 1 unico elemento cada vez
		void initLocalSearch1();
		void localSearch1(); 
		void endLocalSearch1();
		bool improvePosition(const int x, const int y);

		// Busqueda local basada en cambiar 2 elementos contiguos cada vez
		void initLocalSearch2();
		void localSearch2(); 
		void endLocalSearch2();
		bool improvePosition(const Position &p1, const Position &p2);
		bool improvePosition(const Position &p1, const Position &p2, const Position &p3);

		// Mutacion uniforme: cambia cada elemento con una determinada probabilidad
		void mutationUniform();
		void mutationUniformValid();
		// Mutacion basada en ventanas
		void mutationWindow();
		//Reordenacion: no hay cambio de valores
		void mutationReorder();

		//Crossover
		void subStringCrossover(TwoDPack_MultiObjectivized *ind);
		void dependentCrossover(Individual *ind);

		// Obtencion del coste asociado a hacer 1 asignacion
		int getAssignationCost(const int x, const int y, const int value);

		// Obtencion del coste asociado a hacer 2 asignaciones
		int getAssignationCost(const int x1, const int y1, const int v1, const int x2, const int y2, const int v2);
		int getAssignationCost(const int x1, const int y1, const int v1, const int x2, const int y2, const int v2, const int x3, const int y3, const int v3);

		// Obtencion de coste asociado a cualquier asignacion de 1 casilla
		void getAssignationCosts(const int x, const int y, vector<int> &costValues);
		
		// Suprimir una asignacion
		int removeAssignation(const int x, const int y);

		// Hacer una asignacion
		void makeAssignation(const int x, const int y, const int value);

		// Calcular el fitness
		void calculateFitness();
		void calculatePartialFitness(const int initX, const int endX, const int initY, const int endY);

		int pFitness;
		vector < vector < int > > appearance; // Cantidad de veces que una pareja aparece
		vector < vector < int > > pAppearance; // Cantidad de veces que una pareja aparece (en las ventanas)
		static const TwoDPackingProblem *twoDPackingProblem; // problema asociado
		static vector< pair<int, int> * > *neighboursToExplore; // vecinos a explorar (localSearch1)
		static vector< pair<Position, Position> * > *partnersToExplore; // vecinos a explorar (localSearch2)
		double mutProbLow, mutProbHigh, mutProbWLow, mutProbWHigh; // Probabilidad de mutacion
		int typeMutation, typeLocalSearch;
		static int nInds;
		char *initIndFich;
};


#endif
