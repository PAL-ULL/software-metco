#include "PreviousEndCheckingMonoWorsening.h"

#include <stdlib.h>
#include <math.h>

void PreviousEndCheckingMonoWorsening::loadIslandInitPop(const int previousAlg, const int previousConf, const int newAlg, const int newConf, const vector< Individual* > *previousInit, const vector< Individual* > *previousEnd, const vector< Individual* > *global, vector < Individual* > &newPopulation, vector<int> &origin) const {
	if (previousInit->size() != previousEnd->size()){
		cerr << "Unimplemented. PreviousInit->size() != PreviousEnd->size(). Plugin: PreviousEndCheckingMonoWorsening" << endl;
		exit(-1);
	}
	for (int i = 0; i < previousEnd->size(); i++){
		double worsening;
		//Calculate absolute worsening
		if ((*previousEnd)[i]->getInternalOptDirection(0) == MINIMIZE){
			worsening = (*previousEnd)[i]->getObj(0) - (*previousInit)[i]->getObj(0);
		} else {
			worsening = (*previousInit)[i]->getObj(0) - (*previousEnd)[i]->getObj(0) ;
		}
		//Calculate percentage
		if (worseningType == PERCENTAGE){
			worsening = worsening / fabs((*previousInit)[i]->getObj(0)) * 100;
		}
		if (worsening > acceptedWorsening){//use init
			newPopulation.push_back((*previousInit)[i]);
			origin.push_back(PREVIOUSINIT);
			delete (*previousEnd)[i];
		} else {//use end
			newPopulation.push_back((*previousEnd)[i]);
			origin.push_back(PREVIOUSEND);
			delete (*previousInit)[i];
		}
	}
}

//Parameters: Type( 0: percentage, 1: Absolute value) Percentage or absolute value of accepted worsening
//OJO: el de porcentaje no se deberia usar si el 0 es un posible valor objetivo, pues haria una division por 0
bool PreviousEndCheckingMonoWorsening::init(const vector<string> &params){
	if (params.size() != 2){
		worseningType = atoi(params[0].c_str());
		acceptedWorsening = atof(params[1].c_str());
	}
}
