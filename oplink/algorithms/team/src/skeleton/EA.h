/***********************************************************************************
 * AUTHORS
 *   - Ofelia Gonz�lez P�rez
 *   - Gara Miranda Valladares
 *   - Carlos Segura Gonz�lez
 * 
 * DATE
 *    November 2007
 *
 * DESCRIPTION
 *   Plugin base class to represent the algorithms
 *
 *   --> Steps for the creation of an individual
 *   	 - User must create a class inheriting from EA, defining the next
 *   	 methods: runGeneration, init, received, getSolution and printInfo.
 *
 *       +virtual void runGeneration()
 *          Este metodo debe ejecutar una generaci�n de b�squeda del algoritmo.
 *          La poblaci�n se encuentra en el vector population. Cuando se quiera 
 *          evaluar un individuo se debe llamar al m�todo evaluate de EA.
 *          No se debe llamar directamente al m�todo evaluate del individuo.
 *          Durante la primera generaci�n de cada ejecuci�n es posible que existan
 *          individuos que ya han sido evaluados (cuando un algoritmo empieza a 
 *          ejecutar en una isla, puede coger parte de su poblaci�n inicial de la
 *          soluci�n global encontrada actualmente). Llamando al m�todo 
 *          getPopEvaluatedIndividuals se obtiene cu�ntos individuos de los que hay 
 *          en la poblaci�n ya han sido evaluados, con lo que s�lo habr� que evaluar el 
 *          resto. Los individuos ya evaluados est�n colocados al inicio del vector 
 *          population. Lo mismo ocurre cuando se reciben individuos migrados.
 *
 *       +virtual bool initParams(const vector<double> &params)
 *          Este m�todo ser� invocado durante la inicializaci�n del algoritmo pas�ndole
 *          los par�metros que se hayan especificado en el fichero de configuraci�n del 
 *          equipo de algoritmos (ejecuciones paralelas) o en la l�nea de comandos
 *          (ejecuciones secuenciales).
 *          En esta inicializaci�n hay que fijar el tama�o de la poblaci�n llamando 
 *          a setPopulationSize.
 *      
 *       +virtual void getSolution(MOFront *solucion)
 *          Inserta en el frente solucion las soluciones encontradas por el EA.
 *          Para ello se debe llamar al metodo insert del objeto solucion (clase
 *          MOFront) pas�ndole cada uno de las soluciones a a�adir.
 *          
 *       +virtual void printInfo(ostream &os)
 *          Esta rutina debe mostrar informacion sobre el algoritmo: nombre y par�metros
 *          que se est�n usando (probabilidades de mutaci�n y crossover, tama�os de 
 *          poblaci�n, etc.)
 *          Esta informaci�n se mostrar� en el fichero de resultados de las ejecuciones
 *          secuenciales.
 *
 *     - Opcionalmente se pueden sobreescribir los siguientes m�todos:
 *
 *       +virtual void fillPopWithNewInds()
 *          Este m�todo tiene como funci�n, rellenar la porci�n de la poblaci�n inicial que no
 *          ha sido rellenada con individuos del frente global.
 *          Por defecto rellena la poblaci�n llamando al  m�todo create del individuo usado.
 *          Para saber cu�ntos individuos han sido obtenidos a partir del frente global
 *          se debe llamar al m�todo \texttt{getPopEvaluatedIndividuals()}
 *          Para hacer una generaci�n inicial de la poblaci�n que no sea llamando al m�todo
 *          create del individuo, se debe sobreescribir el m�todo.
 *
 *
 *      +virtual void received(vector<Individual*> &migrados)
 *          Este m�todo es invocado cuando se reciben individuos migrados desde otra isla. 
 *          El algoritmo debe sacrificar a algunos de los individuos actuales, 
 *          intercambi�ndolos por los nuevos individuos. La elecci�n de que individuos se 
 *          intercambian es dependiente del algoritmo.
 *          El conjunto de individuos que sean sacrificados (ya no se vayan a usar m�s)
 *          y los individuos del vector migrados que no se utilicen deben ser borrados
 *          mediante el uso del operador delete.
 *          Por defecto se intercambian los primeros individuos de la poblaci�n por
 *          los individuos que se han recibido
 ***********************************************************************************/


#ifndef __EA_H__
#define __EA_H__

#include <vector>
#include <string>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>

#include "Plugin.h"
#include "utils.h"
#include "Individual.h"
#include "MOFront.h"
#include "LocalSearch.h"
#include "LocalScoreAlgorithm.h" 
#include "OutputPrinter.h"
#include "MultiObjectivization.h"

#define N_CRIT_STOP 3 
const string CRIT_STOP[] = {"TIME", "EVALUATIONS", "QUALITY"};

#define ARCHIVE_VECTOR 10
#define ARCHIVE_INSO1 11
using namespace std;

class OutputPrinter;

class EA : public Plugin {

public:
	// Constructor
	EA ();

	// Destructor
	virtual ~EA (void);

	// M�todos p�blicos que tienen que ser sobreescritos
	virtual void getSolution(MOFront *p) = 0;
	virtual void printInfo(ostream &os) const = 0;

	// Metodos p�blicos que pueden ser sobreescritos
	virtual void received(vector<Individual*> &);
	virtual	double *getRestartInfo(){ return NULL; }
	virtual void setRestartInfo(double *){}
	virtual void exchangePerformed() {}
	
	// Run secuencial
	void run();

	// Inicializacion de la poblacion inicial con individuos del frente global
	void fillPopWithEvaluatedInds(const vector<Individual *> &);

	// Ejecucion de una generacion
	void runGenerationInc();

	// Criterio de parada
	bool hasFinished();

	// Resetear las condiciones de paradas (reiniciar tiempo, numero de evaluaciones)
	void resetStopConditions();

	// Evaluar un individuo
	void evaluate(Individual *ind);

	//Almacenar individuo en el archivo
	void insertInArchive(Individual *ind);

	// Inline Setters
	void setPopulationSize          (const int pSize)     { this->pSize = pSize;                                     }
	void setSampleInd               (Individual *ind)     { sampleInd = ind;                                         }
	void setOutputPrinter           (OutputPrinter *op)   { outputPrinter = op;                                      }
	void setPrintPeriod             (const int pp)        { printPeriod = nextPrint = pp;                            }
	void setMaxLocalFrontSize       (const int max)       { localFrontSize = max;                                    }
	void setIdConf                  (const int idConf)    { this->idConf = idConf;                                   }
	void setScoreAlgorithm          (ScoreAlgorithm *sc)  { scoreAlgorithm = (LocalScoreAlgorithm *)sc;              }
	void setLocalSearch             (LocalSearch *ls)     { this->ls = ls;                                           }
	void setMultiObjectivizationPlugins (const vector<MultiObjectivization*> &multi) { this->multiObjectivizationsPlugins = multi; }
	
	//Fijar archivo
	void setGenerateArchive         (const bool generate, const int type);

	// Non-Inline Setters
	void setStoppingCriterion(const int critStop, const double critStopValue);

	// Getters
	inline int getCritStop                     (void) const             { return critStop;                               }
	inline double getCritStopValue             (void) const             { return critStopValue;                          }
	inline int getPrintPeriod                  (void) const             { return printPeriod;                            }
	inline int getPopulationSize               (void) const             { return pSize;                                  }
	inline MOFront* getLocalSolution           (void) const             { return localSolution;                          }
	inline int getEvaluations                  (void) const             { return evaluations;                            }
	inline int getPerformedEvaluations         (void) const             { return evaluationActual;                       }
	inline int getGeneration                   (void) const             { return generationActual;                       }
	inline int getMaxLocalFrontSize            (void) const             { return localFrontSize;                         }
	inline int getIdConf                       (void) const             { return idConf;                                 }
	inline Individual *getSampleInd            (void) const             { return sampleInd;                              } 
	static inline string getGlobalTypeStopCrit (const int i)            { return CRIT_STOP[i];                           }
	static inline int getTypeStoppingCriterion (const string &cr)       { return getIndex(cr, CRIT_STOP, N_CRIT_STOP);   }
	inline bool isGeneratingArchive            (void) const             { return generateArchive;                        }
	inline int getNextPrint                    (void) const             { return nextPrint;                              }
	inline MultiObjectivization* getMultiObjectivizationPlugin (int index) const { return multiObjectivizationsPlugins[index];    }
	inline int getNumMultiObjectivizationPlugins(void) const            { return  multiObjectivizationsPlugins.size(); }
	double getElapsedTime() const;

	// Soporte de multiObjectivizacion
	virtual bool supportsMultiObjectivization() { return false; }

protected:
	vector<Individual*>* population;
	
	// Operador de torneo binario anadido por 
	// Antonio J. Nebro
	int binaryTournament(vector<Individual *>* pop);

private:
	Individual *sampleInd;
	OutputPrinter *outputPrinter;
	MOFront *localSolution;
	LocalScoreAlgorithm *scoreAlgorithm;
	LocalSearch *ls;
	int critStop, evaluations, evaluationActual, generations, generationActual;
	double quality;
	int localFrontSize;
	int pSize, myId, numProcs, initiated;
	double time, critStopValue;
	int idConf;
	double startTime;
	vector<MultiObjectivization*> multiObjectivizationsPlugins;

	// Metodos privados que tienen que ser sobreescritos
	virtual void runGeneration() = 0;

	// Metodos privados que pueden ser sobreescritos
	virtual void fillPopWithNewIndsAndEvaluate();

	
	// Impresiones intermedias
	void checkPrint();
	int printPeriod;
	int nextPrint;

	// Generar frente complete
	bool generateArchive;

	friend class ExecutionIsland;
};

typedef enum {
	LOCAL_TIME,
	LOCAL_EVALUATIONS
} local_stop_criterions_ids_t;


//Obtenci�n de individuo a partir de una librer�a
EA *getEA(const string &pluginPath, const string &pluginName, const vector<string> &args, bool printError, Individual *ind);

#endif
