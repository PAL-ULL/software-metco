/***********************************************************************
* Autor: Carlos Segura González
* Fecha: 04/01/13
* Descripcion: clase que implementa la funcionalidad de una 
* estrategia evolutiva
************************************************************************/

#ifndef _MONO_ES_H_
#define _MONO_ES_H_

#include <map>
#include "Individual.h"
#include "EA.h"
#include "MutationES.h"
#include "Recombination.h"

using namespace std;

//Codes of Parents Type (for recombination)
#define GLOBAL_PARENTS 1
#define LOCAL_PARENTS 2

//Codes of survivor selection
#define COMMA_SELECTION 3
#define PLUS_SELECTION 4
#define DE_SELECTION 5

//Codes of initialization strategy parameters
#define FIXED 5
#define RANDOM_WITH_MAXIMUM 6
#define RANDOM_196 7
	
class MonoES : public EA {
	public:
		MonoES();
		bool init(const vector<string> &params);
		void initialize();
		void run();
		void getSolution(MOFront *p);
		void printInfo(ostream &os) const ;
		virtual ~MonoES();
	private:
		void fillPopWithNewIndsAndEvaluate();
		void checkSanity() const;//Check if the parameters are correct
		void printBestIndividual();
		void printDiversity();

		//Survivor selectors
		void survivorSelection();
		void convertSurvivorSelectionToCode(const string &survivorSelectionStr);
		void survivorSelectionComma();
		void survivorSelectionPlus();
		void DESelection();
		int survivorSelectionCode;

		//Variation
		void variation();
		void recombinateParents();
		void mutateOffspring();
		void evaluateOffspring();
		void convertParentsToCode(const string &parentsStr);
		void convertTypeToCode(const string &recombinationTypeStr, int &code);
		Recombination *getRecombinator(int parents);

		int variablesRecombinationTypeCode;
		int strategyRecombinationTypeCode;
		int recombinationParentsCode;
		MutationES *mutator;
		Recombination *recombinator;


		//Strategy parameters initialization
		void convertParameterInitializationTypeToCode(const string &parameterInitializationTypeStr);
		void initializeAdaptiveParameters(const int i);
		int parameterInitializationTypeCode;
		int numberOfStepSizes;
		double initSigma;
		double maxSigma;

		void runGeneration();


		vector<Individual*>* offspring;
		int mu;//Tam. poblacion
		int lambda;//Num. hijos
		int evaluations;//Stop Criterion
		int currentEvaluations;
		int printPeriod;
		int nextPrint;
		double epsilon;
		double tau;
		double tauPrim;

		map<string, string> parameters;//Lo almacenamos, porque al leerlo aun no tenemos la info del individuo, asi que retrasamos la inicializacion
};

#endif
