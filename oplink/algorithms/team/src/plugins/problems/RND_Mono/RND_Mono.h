#ifndef __RND_Mono_H__
#define __RND_Mono_H__

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <map>

#include "Individual.h"
#include "ProblemRND.h"

using namespace std;

class Position;
class ProblemRND;

#define CROSSOVER_OPC 0
#define CROSSOVER_GEOGRAPHIC_CIRCULAR 1
#define CROSSOVER_GEOGRAPHIC_RECTANGULAR 2

class RND_Mono : public Individual {
	public:
		// Constructores y Destructor
		RND_Mono(){};
		~RND_Mono(); //{};

    // Individual initialization
    bool init (const vector<string> &params);
		
		// Es un operador de copia
		Individual* clone() const;

    void deserialized();

		// Individual evaluation and selection
    void evaluate() {};  // Esta vacio, dado que ya se tienen los objetivos

		// Getters
		unsigned int getOptDirection(const int i) const; 
		unsigned int getUsedBs() const       { return (int)getObj(0); }
		unsigned int getCoverage() const     { return (int)getObj(1); }
		unsigned int getBsId(unsigned int i, unsigned int j) const  {return posiciones[i][j]; }

		// NUEVO BUSQUEDA LOCAL
		unsigned int getNeighbourhoodSize() const;
		Individual* getNeighbour(const int i) const;
		void getNeighbourObjectives(const int i, vector<double> &objectives) const;
		
		// Ranges
		double inline getMaximum(const int i) const { return 1; }
		double inline getMinimum(const int i) const { return 0; }

		// Operador de salida por pantalla
		void print() const;

		// Inicializacion de las estaciones bases que se encuentran dentro del radio de cada
		// una de las ubicaciones
		void calculateRegion();
		void calculateRegionRect();
		 
		void updateFitness();
		
		// Elimina/ Incluye de la solucion la antena con identificador bsId
		void removeBs(const unsigned int bsId);
		void addBs(const unsigned int bsId);
		//Funcion que imprime los valores del Gen
		void imprimir();
		void restart(void);
		virtual void printInfo(ostream &os) const { os << "instanceDefinitionFile=\"" << instanceFile << "\""; }

		//Info de la mutacion
		string getMutationName() const;
		//Info del crossover
		string getCrossoverName() const;
		int getCrossoverNumberOfParameters() const;
		string getCrossoverParameterName(int i) const;
		string getCrossoverParameterValue(int i) const;
		//Info de los objetivos
		string getObjName(int i) const; 
		void maintainOnlyObj();

		int getIndexObj() const { return 0; }
		int getIndexObjMin() const { return 0; }
		int getIndexObjMax() const { return problem->getBsLocationsSize(); }

	private:
		// Genetic Operators
    void dependentMutation(double pm);
		void crossover_opc(Individual* ind);
		void crossover_geographic_circular(Individual* ind);
		void crossover_geographic_rectangular(Individual* ind);
		void dependentCrossover(Individual* ind);
	                                                  
		
		//Variables
		vector < vector <int> > grid;            // Almacena el cubrimiento de cada punto del grid
		vector<int> antenas;                     // Antenas que caen dentro de la region del rectangulo
		// Las variables estaticas se inicializaran una sola vez y seran compartidas por todos los individuos
		static ProblemRND *problem;              // Instancia del problema a resolver
		
		unsigned int type_crossover;
		unsigned int dim_min;                   // Dimension minima del rectangulo
		unsigned int dim_max;                   // Dimension maximo del rectangulo   
		unsigned int radio_region;
		unsigned int coveredPositions;
		unsigned int usedBs;

		static unsigned int type_instance;
		static map < int, vector< vector <int> > > bsRegion;// Conjunto de puntos que conforman cada region (crossover circular)
		static vector< vector <int> > posiciones;//Almacena las posiciones donde se localizan las antenas
		static vector< vector< vector< vector <int> > > > RegionBs;//Conjunto de puntos que conforman cada region (crossover rectangular)
		static string instanceFile;

		static vector < string > crossParamNames;
		static vector < string > crossParamValues;
};

#endif
