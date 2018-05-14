
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
/** \brief Plugin base class to represent the algorithms 
 *
 * */
class EA : public Plugin {

public:
	//! Constructor
	EA ();
	//! Destructor
	virtual ~EA (void);
	/**
	 * This methods must copy all the solution into the Pareto's Front.
	 * @param: MOFront* Empty Pareto's Front
	 **/
	virtual void getSolution(MOFront *p) = 0;
	/**
	 * This method must be overwritten in order to print the information of the algorithm
	 * @param: Stream where the information will be printed.
	 **/
	virtual void printInfo(ostream &os) const = 0;
	virtual void received(vector<Individual*> &);
	virtual	double *getRestartInfo(){ return NULL; }
	virtual void setRestartInfo(double *){}
	virtual void exchangePerformed() {}
	/**
	 * Run the algorithm
	 **/
	void run();
	/**
	 * Initialize the initial population with individuals from Global Front.
	 * @param Vector<Individual *> Evaluated individuals
	 */
	void fillPopWithEvaluatedInds(const vector<Individual *> &);
	/**
	 * Increments the generation counter
	 **/
	void runGenerationInc();
	/**
	 * Checks if the algorithm has finished yet. The stopping criteria has been reached.
	 **/
	bool hasFinished();
	/**
	 * Restars the stopping condition
	 *  - Restart time
	 *  - Number of evaluations
	 **/
	void resetStopConditions();
	/**
	 * Evaluate the given individual with its evaluation function
	 * This methods must call the evaluate method from Individual class.
	 * @param Individual to be evaluated
	 **/
	void evaluate(Individual *ind);
	//Almacenar individuo en el archivo
	void insertInArchive(Individual *ind);

	/**
	 * Set the population size
	 * @param Integer that defines the population size
	 **/
	void setPopulationSize          (const int pSize)     { this->pSize = pSize;                                     }
	/**
	 * Set a sample Individual. Useful for copy a Individual without its evaluation.
	 * @param Individual
	 **/
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
