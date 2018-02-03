/***********************************************************************************************
 * AUTHORS
 * - Gara Miranda Valladares
 * - Carlos Segura González
 *
 * DATE 
 *   February 2008 
 ***********************************************************************************************/
 

#include "Conversion.h"

#include <dlfcn.h>
#include <string>
#include <vector>
#include <sstream>

bool Conversion::makeTableTranslation(const vector < pair <string, string> > &pairsConv){
	if (pairsConv.size() != configFilePairs.size()){
		cerr << "Error creating the translation table" << endl;
		return false;
	}
	translation.resize(pairsConv.size());
	for (int i = 0; i < configFilePairs.size(); i++){
		int j;
		for (j = 0; j < pairsConv.size(); j++){
			if ( (configFilePairs[i].first == pairsConv[j].first) && (configFilePairs[i].second == pairsConv[j].second) ){
				translation[i] = j;
				break;
			}
		}
		if (j == pairsConv.size()){
			cerr << "Error creating the translation table: (" << configFilePairs[i].first << ", " << configFilePairs[i].second << ") not found" << endl;
			return false;
		}
	}
}

bool Conversion::makeTableTranslation(const vector < string > &alg){
	translation.resize(configFilePairs.size());
	for (int i = 0; i < configFilePairs.size(); i++){
		int j;
		for (j = 0; j < alg.size(); j++){
			if (configFilePairs[i].first == alg[j]){
				translation[i] = j;
				break;
			}
		}
		if (j == alg.size()){
			cerr << "Error creating the translation table: (" << configFilePairs[i].first << ", " << configFilePairs[i].second << ") not found" << endl;
			return false;
		}
	}
}

Conversion *getConversion(const string &name, const vector<string> &convArgs, const vector< pair < string, string > > &algIndPairs, bool printError){
	void *hndl;
	stringstream libFich;
	libFich << name << ".so";
	hndl = dlopen(libFich.str().c_str(), RTLD_NOW);
	if(hndl == NULL){
		if (printError){
			char *error;
			if ((error = dlerror()) != NULL)  {
		  	cerr << error << endl;	
			} 
			cerr << "Conversion file could not be opened: " << libFich.str() << endl;
		}
		return NULL;
	}
	Conversion* (*mkr)() = (Conversion* (*)())dlsym(hndl, "maker");
	if (mkr == NULL){
		if (printError)
			cerr << "maker function in Conversion do no exist"<< endl;
		return NULL;
	}
	Conversion *newConv = (*mkr)();
	newConv->setAlgIndPairs(algIndPairs);
	if (!newConv->init(convArgs)){
		if (printError)
			cerr << "Conversion initialization failed" << endl;
		return NULL;
	}
	return newConv;
}
