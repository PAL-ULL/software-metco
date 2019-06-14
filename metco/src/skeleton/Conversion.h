/***********************************************************************************************
 * AUTHORS
 * - Gara Miranda Valladares
 * - Carlos Segura González
 *
 * DATE 
 *   February 2008 
 *
 * DESCRIPTION
 *   Plugin Base class to represent conversions between individuals. 
 *   The method convert must be implemented to specify the transformation. The method convert
 *   receive an individual of the type confFrom, and must be converted to an individual of the
 *   type confTo. 
 *   In order to know the name of the algorithm associated to confTo and confFrom,
 *   the vector configFilePairs can be used. Also, a translation table can be created by using
 *   makeTableTranslation and getTranslation. Usually, the translation table is created in
 *   the method init, and used in the method convert.
 * ******************************************************************************************/


#ifndef CONVERSION_H
#define CONVERSION_H

#include <string>
#include <vector>

#include "Individual.h"
#include "Plugin.h"

class Conversion : public Plugin {
	public:
		virtual ~Conversion(){}
		virtual Individual* convert(const Individual *ind, const int confFrom, const int confTo) = 0;
		void setAlgIndPairs(const vector< pair <string,string> > &pairs) { configFilePairs = pairs; }
		bool makeTableTranslation(const vector < pair <string, string> > &pairsConv);
		bool makeTableTranslation(const vector < string > &alg);
		inline int getTranslation(int alg) { return translation[alg]; }
		void setExampleIndividuals(vector<Individual*> &examples) { exampleIndividuals = examples; }
	protected:
		vector<Individual*> exampleIndividuals;
		vector< pair <string, string> > configFilePairs;
		vector<int> translation;
}; 

Conversion *getConversion(const string &convName, const vector<string> &convArgs, const vector < pair <string, string > > &algIndPairs, bool printError);

#endif
