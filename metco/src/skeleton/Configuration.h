/***********************************************************************************
 * AUTHORS
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 *   - Eduardo Manuel Segredo González
 * 
 * DATE
 *    November 2007 
 *
 * DESCRIPTION
 * 		For the parallel executions, it provides access to the parameters fixed in 
 * 		the configuration file.
 ***********************************************************************************/

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "MigrationSelector.h"
#include "ExchangeSelector.h"
#include "ScoreAlgorithm.h"
#include "MigrationTopology.h"
#include "InitPopIslandLoader.h"
#include "Mutation.h"
#include "Crossover.h"
#include "Conversion.h"
#include "LocalSearch.h"
#include "MultiObjectivization.h"
#include "EA.h"
#include "Individual.h"
#include "Definitions.h"
#include <string>
#include <vector>

using namespace std;

// Tipo algoritmo
typedef struct TAlgorithm {
	TAlgorithm();
	int critStop;
	double valueSC;
	string individualName;
	string individualParams;
	string migrationSelectorName;
	string exchangeSelectorName;
	string mutationName;
	string crossoverName;
	string localSearchName;
  string multiObjectivizationName;
	MigrationSelector *migrationSelector;
	ExchangeSelector *exchangeSelector;
	Mutation *mutation;
	Crossover *crossover;
	LocalSearch *localSearch;
	vector<string> localSearchParams;
  MultiObjectivization *multiObjectivization;
  vector<string> multiObjectivizationParams;
	int solutionSource;
	Individual *ind;
	string algorithmStr;
	int maxLocalFrontSize;
  int sharingGroup;
	vector< vector <string> > configs; 
} TAlgorithm;

// Tipo Modelo de ejecucion
typedef struct {
	int stopCrit;
	double valueStopCrit;
	int typeSelection;
} TExecutionModel;

class Configuration{
	public:
		Configuration();
		// Carga del fichero
		void load(const string &confFile, const int myId);

		// Traduccion indices/cadenas
		static int getTypeSelection (const string &typeSelect);
		static int getGlobalTypeStoppingCriterion (const string &crit);
		static int getAlgorithm (const string &alg);
		static int getSolutionSource(const string &typeSolutionSource);
		static inline string getTypeSelection (const int i) { return TYPE_SELECTION[i]; }
		static inline string getGlobalTypeStoppingCriterion (const int i) { return GLOBAL_CRIT_STOP[i]; }
		static inline string getAlgorithm (const int i){ return ALGORITHMS[i]; }

		//getters
		inline const vector<TExecutionModel>& getExecutionModel           () const { return executionModel;               }
		inline double                         getMigrationProbability     () const { return migrationProbability;         }
		inline int                            getNumberIndMigrate         () const { return numberOfIndividualsToMigrate; }
		inline int                            getMaxGlobalFrontSize       () const { return maxGlobalFrontSize;           }
		inline int                            getMaxFinalSolutionSize     () const { return maxFinalSolutionSize;         }
		inline double                         getInitPercentOfIndividuals () const { return initPercentOfIndividuals;     }
		inline int                            getErrorParsing             () const { return errorParsing;                 }
		inline const string&                  getErrorParsingStr          () const { return errorParsingStr;              }
		inline const vector<TAlgorithm>&      getTeamAlgorithms           () const { return teamAlgorithms;               }
		inline const string&                  getBaseDirectory            () const { return baseDirectory;                }
		inline bool                           getSendResultsPerGeneration () const { return sendResultsPerGen;            }
		inline Conversion*                    getConversion               () const { return conversion;                   }
		inline ScoreAlgorithm*                getScoreAlgorithm           () const { return scoreAlgorithm;               }
		inline MigrationTopology*             getMigrationTopology        () const { return migrationTopology;            }
		inline InitPopIslandLoader*           getInitPopIslandLoader      () const { return initPopIslandLoader;          }
		inline const string&                  getConversionStr            () const { return conversionStr;                }
		inline const string&                  getScoreStr                 () const { return scoreStr;                     }
		inline const string&                  getMigrationTopologyStr     () const { return migrationTopologyStr;         }
		inline const string&                  getInitPopIslandLoaderStr   () const { return initPopIslandLoaderStr;       }
		inline const vector<string>&          getConversionParams         () const { return conversionParams;             }
		inline const vector<string>&          getScoreParams              () const { return scoreParams;                  }
		inline const vector<string>&          getMigrationTopologyParams  () const { return migrationTopologyParams;      }
		inline const vector<string>&          getInitPopIslandLoaderParams() const { return initPopIslandLoaderParams;    }
		inline const string&                  getOutputPath               () const { return outputPath;                   }
		inline const string&                  getPluginPath               () const { return pluginPath;                   }
		inline int                            getNumberParallelExecutions () const { return numberParallelExecutions;     }

		//setters
		inline void setExecutionModel               (const vector<TExecutionModel> &execModel) { executionModel = execModel; }
  	inline void setMigrationProbability         (const double migProb)                     { migrationProbability = migProb; }
		inline void setNumberOfIndividualsToMigrate (const int n)                              { numberOfIndividualsToMigrate = n; }
		inline void setMaxGlobalFrontSize           (const int n)                              { maxGlobalFrontSize = n; }
		inline void setMaxFinalSolutionSize         (const int n)                              { maxFinalSolutionSize = n; }
		inline void setInitPercentOfIndividuals     (const double n)                           { initPercentOfIndividuals = n; }
		inline void setErrorParsingStr              (const string &str)                        { if (!errorParsing){ errorParsing = 1; errorParsingStr = str;} }
		inline void setTeamAlgorithms               (const vector<TAlgorithm> &teamAlg )       { teamAlgorithms = teamAlg; }
		inline void setBaseDirectory                (const string &bd)                         { baseDirectory = bd; }
		inline void setSendResultsPerGeneration     (const bool &send)                         { sendResultsPerGen = send; }
		inline void setConversion                   (Conversion *conv)                         { conversion = conv; }
		inline void setScore                        (ScoreAlgorithm *sc)                       { scoreAlgorithm = sc; }
		inline void setMigrationTopology            (MigrationTopology *migTop)                { migrationTopology = migTop; }
		inline void setInitPopIslandLoader          (InitPopIslandLoader *ipil)                { initPopIslandLoader = ipil; }
		inline void setConversionStr                (const string &str)                        { conversionStr = str; }
		inline void setScoreStr                     (const string &str)                        { scoreStr = str; }
		inline void setMigrationTopologyStr         (const string &str)                        { migrationTopologyStr = str; }
		inline void setInitPopIslandLoaderStr       (const string &str)                        { initPopIslandLoaderStr = str; }
		inline void setConversionParams             (const vector<string> &params)             { conversionParams = params; }
		inline void setMigrationTopologyParams      (const vector<string> &params)             { migrationTopologyParams = params; }
		inline void setInitPopIslandLoaderParams    (const vector<string> &params)             { initPopIslandLoaderParams = params; }
		inline void setScoreParams                  (const vector<string> &params)             { scoreParams = params; }
		inline void setOutputPath                   (const string &str)                        { outputPath = str; }
		inline void setPluginPath                   (const string &str)                        { pluginPath = str; }
		inline void setNumberParallelExecutions     (const int n)                              { numberParallelExecutions = n; }
		
	private:
		static const string TYPE_SELECTION[];
		static const string GLOBAL_CRIT_STOP[];
		static const string ALGORITHMS[];
		static const string SOLUTION_SOURCES[];

		vector<TExecutionModel> executionModel;
 		double  migrationProbability; 
		int numberOfIndividualsToMigrate; 
		int maxGlobalFrontSize;
		int maxFinalSolutionSize;
		int numberParallelExecutions;
		bool sendResultsPerGen;
		double  initPercentOfIndividuals;
		int errorParsing;
		string errorParsingStr;
		Conversion *conversion;
		ScoreAlgorithm *scoreAlgorithm;
		MigrationTopology *migrationTopology;
		InitPopIslandLoader *initPopIslandLoader;
		string conversionStr;
		string scoreStr;
		string migrationTopologyStr;
		string initPopIslandLoaderStr;
		string outputPath;
		string pluginPath;
		vector<string> conversionParams;
		vector<string> scoreParams;
		vector<string> migrationTopologyParams;
		vector<string> initPopIslandLoaderParams;
		vector<TAlgorithm> teamAlgorithms; 	// Conjunto de configuraciones
		string baseDirectory;
};


typedef enum {
	GLOBAL_TIME,
	GLOBAL_EXECUTIONS,
	GLOBAL_EVALUATIONS,
	GLOBAL_CLEAR_STATISTICS,
	GLOBAL_QUALITY
} global_stop_criterions_ids_t;

typedef enum {
	SOURCE_ALGORITHM,
	SOURCE_ARCHIVE
} solution_sources_ids_t;


#endif
