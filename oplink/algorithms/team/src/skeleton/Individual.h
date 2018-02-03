/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 *
 * DESCRIPCION
 *   Clase padre para la representación de individuos (problemas)
 *   
 *   --> OPERADORES DE MUTACION Y CROSSOVER
 *     Los siguientes operadores han sido implementados:
 *       - Mutación uniforme cambiando sólo uno de los genes con probabilidad pm
 *   	   - Mutación uniforme cambiando cambiando cada gen con probabilidad pm
 *   	   - Mutación binaria
 *   	   - Mutación uniforme en el que se cambia un rango de genes
 *   	   - Mutación polinómica cambiando todos los genes
 *   	   - Crossover uniforme
 *   	   - Crossover por invervales 
 *   	   - Simulated binary crossover
 *  
 *   --> Pasos en la creación de un individuo
 *     - El usuario al heredar debe implementar los métodos mutation, crossover,
 *       evaluate, init y clone
 *       
 *       +virtual void mutation (double pm)
 *         El método mutation debe realizar una mutación sobre el propio 
 *         individuo. Puede llamar a alguno de los métodos de mutación de la clase
 *         padre o implementar su propio método.
 *     
 *       +virtual void crossover (Individual *ind)
 *          El método de crossover permite la creación de 2 nuevos individuos
 *          a partir de otros 2 individuos. Los 2 individuos originales son
 *          el individuo que se pasa como parámetro y el propio individuo.
 *          Los 2 nuevos individuos generadores se "guardan" sobre el
 *          individuo que se pasó como parámetro y sobre el propio individuo.
 *  
 *       +virtual bool init(const vector<string> &params)
 *          Este método se encarga de inicializar el individuo a partir de los
 *          parámetros pasados. Los parámetros son los dados por el usuario
 *          en la línea de comandos (ejecución secuencial) o en el fichero
 *          de configuración (ejecución paralela).
 *          Además de las inicializaciones propias del individuos se deben 
 *          realizar las siguientes operaciones:
 *            - Fijar el número de variables(genes): setNumberOfVar(int)
 *            - Fijar el número de objetivos: setNumberOfObj(int)
 *            - Fijar las direcciones de optimización de cada objetivo (MINIMIZE
 *              o MAXIMIZE). Se puede hacer de 2 formas:
 *                * Llamando al método setOptDirection(const int optDir[]), 
 *                  pasándole un array de enteros que contiene MINIMIZE o 
 *                  MAXIMIZE para cada objetivo.
 *                * Llamar sucesivamente al método setNextOptDirection (una vez
 *                  por cada objetivo), pasándole cada vez MINIMIZE o MAXIMIZE
 *            - Fijar los rangos de valores válidos (minimos y máximos) para cada
 *              gen. Los valores minimos se pueden establecer de 2 formas:
 *                * LLamando al metodo setMinimumValues(const double minValues[]),
 *                  pasándole un array que contiene el valor mínimo para cada 
 *                  objetivo.
 *                * Llamando sucesivamente al metodo 
 *                setNextMinimumValue(const double value) (una vez por cada objetivo)
 *                pasándole cada vez el valor mínimo correspondiente al objetivo.
 *              Los valores máximos se tratan de la misma forma llamando a los métodos
 *              setMaximumValues o setNextMaximumValue
 *            - Fijar el número de datos adicionales del individuo, que quieren que
 *            sean sean migrados y clonados de forma automática. Sólo hace faltar fijarlo
 *            si es distinto de 0. Se realiza llamando al método setMigrationDataSize.
 *          Retorna un valor booleano:
 *          - True: la inicialización fue correcta.
 *          - False: error en los parámetros pasados. Se abortará la ejecución.
 *          En los casos en los que se retorne false, se deberá escribir un error
 *          en cerr
 *  
 *       +virtual void evaluate (void)
 *          Este método se encarga de calcular los valores objetivos a partir de
 *          los valores de las variables (genes) del individuo. Los valores de
 *          los genes están en el vector var. Los objetivos los debe almacenar
 *          en el vector obj.
 *  
 *       +virtual Individual* clone (void)
 *          Este método sirve para realizar la clonación de un individuo. El usuario
 *          debe crear un individuo de la clase hija y devolverlo.
 *          Los genes, valores objetivos y parámetros compartidos son copiados
 *          de forma automática.
 *   
 *
 *    - Opcionalmente se pueden sobreescribir los siguientes métodos:
 *
 *        +virtual void print(ostream &os)
 *           Este método por defecto imprime los valores de las variables del individuo
 *           seguido de los valores de los objetivos.
 *           En caso de querer mostrar por pantalla algún dato más del individuo o
 *           en otro formato distinto, se puede sobreescribir este método
 *        
 *        +virtual void restart()
 *           Este método se usa para crear nuevos individuos en la primera población. 
 *           Si no se sobreescribe, se genera un valor aleatorio para cada variable, 
 *           con una probabilidad uniforme dentro de su rango de valores.
 *
 *        +virtual void generationCode()
 *           Tras cada paso de generación en la que se use el individuo, este método 
 *           es instanciado. Si es necesario realizar alguna operación adicional tras cada 
 *           generación, se puede hacer en este método.
 *
 *    - Otras notas:
 *        En cualquiera de los método sobrecargados se pueden tener en cuenta los datos
 *        adicionales del individuo (datos a migrar). A estos datos se pueden acceder
 *        con el método:
 *        	double inline getMigrationData(const int i) 
 *        Los valores se pueden fijar con el método:
 *        	void inline setMigrationData(const int i, const double value)
 *
 *   *********************************************************************************/


#ifndef __INDIVIDUAL_H__
#define __INDIVIDUAL_H__

#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdlib.h>
 
#include "MultiObjectivization.h"
#include "Plugin.h"
#include "Definitions.h"
#include "Mutation.h"
#include "Crossover.h"

using namespace std;

typedef pair < int, int > Owner;

class Mutation;
class Crossover;
class MultiObjectivization;

class Individual : public Plugin {

public:
	// Constructor
	Individual (void) { mutOperator = NULL; crossOperator = NULL; }

	// Destructor
	virtual ~Individual (void) {};

	// Initialization function
	//virtual bool init(const vector<string> &params) = 0;

	// Mutation Operators
	void mutate_binary_flip(double pm, int origin = 0, int destination = -1);
	void mutate_uniform_one (double pm);
	void mutate_uniform (double pm, int origin = 0, int destination = -1);
	void mutate_binary (double pm);
	void mutate_range (double pm);
	void mutate_Pol (double pm, int origin = 0, int destination = -1);
  	void mutate_Pol2 (double pm, int origin = 0, int destination = -1);

	// Crossover Operators
	void crossover_range (Individual *ind, const int interval);
	void crossover_uniform (Individual *ind, int origin = 0, int destination = -1);
	void crossover_SBX (Individual *ind, int origin = 0, int destination = -1);
	void crossover_SBX2 (Individual *ind);
	void crossover_OPC(Individual *ind, int origin = 0, int destination = -1);
	void crossover_TPC(Individual *ind, int origin = 0, int destination = -1);

	// Genetic operators
	void mutation (double pm);
	void crossover (Individual *ind);

	// Freeing space
	void deleteVars();

	// Evaluate
	virtual void evaluate (void) = 0;

	// Getters
	unsigned int inline   getNumberOfVar      (void)        const { return var.size();           }
	double inline         getVar              (const int i) const { return var[i];               }
	unsigned int inline   getNumberOfObj      (void)        const { return obj.size();           }
	long double inline    getObj              (const int i) const { return obj[i];               }
	unsigned int inline   getNumberOfMigData  (void)        const { return migrationData.size(); }
	unsigned int inline   getNumberOfAuxData  (void)        const { return auxData.size();       }
	double inline         getMigrationData    (const int i) const { return migrationData[i];     }
	double inline         getAuxData          (const int i) const { return auxData[i];           }
	unsigned int inline   getAuxDataSize      (void)        const { return auxData.size();       }
	double inline         getAlgorithmData    (const int i) const { return algorithmData[i];     }
	double inline         getAlgorithmDataSize(void)        const { return algorithmData.size(); }
	double inline         getFitnessValue     (void)        const { return fitnessValue;         }
	Owner inline          getOwner            (void)        const { return owner;                }
	unsigned int getInternalOptDirection      (const int i) const;
	virtual double        getMaximum          (const int i) const = 0;
	virtual double        getMinimum          (const int i) const = 0;
	inline MultiObjectivization* getMultiObjectivizationPlugin (int index) const { return multiObjectivizationsPlugins[index]; }
	inline int getNumMultiObjectivizationPlugins(void) const            { return  multiObjectivizationsPlugins.size(); }

	// Inline Setters
	void inline setObj              (const int i, const long double value) { obj[i] = value;       }
	void inline setVar              (const int i, const double value) { var[i] = value;            }
	void inline setFitnessValue     (const double value)              { fitnessValue = value;      }
	void inline setMigrationData    (const int i, const double value) { migrationData[i] = value;  }
	void inline setAuxData          (const int i, const double value) { auxData[i] = value;        }
	void inline setNumberOfVar      (const int n)                     { var.resize(n);             }
	void inline setNumberOfObj      (const int n)                     { obj.resize(n);             }
	void inline setAlgorithmData    (const int i, const double value) { algorithmData[i] = value;  }
	void inline setAuxDataSize      (const int n)                     { auxData.resize(n);         }
	void inline setMigrationDataSize(const int n)                     { migrationData.resize(n);   }
	void inline setOwner            (const Owner own)                 { owner = own;               }
	void inline setMutOperator      (Mutation *mut)                   { mutOperator = mut;         }
	void inline setCrossOperator    (Crossover *cross)                { crossOperator = cross;     }
	void setMultiObjectivizationPlugins (const vector<MultiObjectivization*> &multi) { this->multiObjectivizationsPlugins = multi; }

	int  inline requestAlgorithmData(void)                            { algorithmData.push_back(0); return algorithmData.size() - 1; }
	void inline releaseAlgorithmData(void)                            { algorithmData.pop_back();  }

	// Clone of individuals 
	Individual *internalClone(void) const;
	void internalClone(const Individual *);
	void internalCloneData(const Individual *);

	// Serializacion
	void serialize(long double *buffer, int &count) const;
	Individual *deserialize(const int nVar, const int nObj, const int nMig, const long double *buffer, int &count);
	virtual void deserialized() { }
	virtual void cloned() { };

	// Otros
	virtual void maintainOnlyObj() {
		maintainOnlyObjInternal();
	}

	void maintainOnlyObjInternal() {
		vector<double> v1, v2, v3;
		var.swap(v1); 
		migrationData.swap(v2); 
		auxData.swap(v3);
	}

	
	// Salida por pantalla de los individuos
	virtual void print(ostream &os) const;
	virtual void printInfo(ostream &os) const { }

	//Info de la mutacion
	virtual string getMutationName() const; 
	virtual int getMutationNumberOfParameters() const; 
	virtual string getMutationParameterName(int i) const; 
	virtual string getMutationParameterValue(int i) const; 

	//Info del crossover
	virtual string getCrossoverName() const; 
	virtual int getCrossoverNumberOfParameters() const;
	virtual string getCrossoverParameterName(int i) const; 
	virtual string getCrossoverParameterValue(int i) const; 

	//Info de los objetivos
	virtual string getObjName(int i) const { stringstream s; s << "obj" << i; return s.str(); }

	// Uso de MOFrontBinaryInteger
	virtual int getIndexObj() const { return -1; }
	virtual int getIndexObjMin() const { return 0; }
	virtual int getIndexObjMax() const { return 0; }

	// Comparacion de individuos (por defecto por el campo fitnessValue)
	bool operator<(const Individual& ind) const { return (fitnessValue < ind.fitnessValue); }
	bool operator==(const Individual& ind) const { return (fitnessValue == ind.fitnessValue); }

	// Inicializacion aleatoria de un individuo
	virtual void restart(void);

	// Codigo llamado tras cada generacion 
	virtual void generationCode() const{}

	// Distancia euclidea
	double getEuclideanDistance(const Individual *) const;
	double getEuclideanDistanceDecisionSpace(const Individual *) const;

	// LocalSearch
	virtual unsigned int getNeighbourhoodSize() const { cout << "Funcion getNeighbourhoodSize llamada sin implementar" << endl; exit(-1); }
	virtual Individual* getNeighbour(const int i) const { cout << "Funcion getNeighbour llamada sin implementar" << endl; exit(-1); }
	virtual void getNeighbourObjectives(const int i, vector< double > &objectives) const { cout << "Funcion getNeighbourObjectives llamada sin implementar" << endl; exit(-1); }

	virtual unsigned int  getOptDirection     (const int i) const = 0;

private:
	
	// Operador de clonación: debe ser reimplementado en la clase hija
	virtual Individual* clone (void) const = 0;
	vector<MultiObjectivization*> multiObjectivizationsPlugins;
	
	// Genetic operators to implement
	virtual void dependentMutation (double pm);
	virtual void dependentCrossover (Individual *ind);

	// genes
	vector<double> var;

	// objetivos
	vector<long double> obj;

	// datos a migrar
	vector<double> migrationData;
	
	// datos auxiliares (que no se migran)
	vector<double> auxData;

	//vector en que puede guardar datos un algoritmo (no se migran)
	vector<double> algorithmData;

	// Valor de fitness del individuo
	double fitnessValue;

	Mutation *mutOperator;
	Crossover *crossOperator;

	// Algoritmo que lo genero (necesario para hacer conversiones)
	Owner owner;
};

// Auxiliar functions
bool cmpIndividual (Individual *ind1, Individual *ind2);
bool ordByFitness(Individual *i1, Individual *i2);
bool reverseOrdByFitness(Individual *i1, Individual *i2);

// Library functions
Individual *getInd(const string &pluginPath, const string &pluginName, const vector<string> &indArgs, bool printError);
void closeInd(void *hndl);
#endif
