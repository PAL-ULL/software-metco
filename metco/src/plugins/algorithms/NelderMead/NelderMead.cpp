/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo Gonzalez
 * 
 * DATE
 *    March 2016
 *
 **********************************************************************************/
 
#include <cstdlib>
#include <iostream>
#include <signal.h>
#include <math.h>

#include "NelderMead.h"

// Algorithm initialization
bool NelderMead::init(const vector<string> &params){
	// Check number of parameters
	if (params.size() != 5){
		cout << "Parametros: popSize nm1 nm2 nm3 nm4" << endl;
		return false;
	}
	// Only mono-objective optimization is supported
	if (getSampleInd()->getNumberOfObj() != 1){
		cout << "Multi-Objective not supported" << endl;
		return false;
	}

	// Parameters initialization
	setPopulationSize(atoi(params[0].c_str()));
	this->nm1 = atof(params[1].c_str());
	this->nm2 = atof(params[2].c_str());
	this->nm3 = atof(params[3].c_str());
	this->nm4 = atof(params[4].c_str());
  this->dimension = getPopulationSize();
	
	return true;
}

// NelderMead generation
void NelderMead::runGeneration() {
  sortPopulation();

  //Calculamos el centroide de toda la población excepto del peor
  Individual *xc = getSampleInd()->internalClone();
  bool shrink = false;

  for (int i = 0; i < xc->getNumberOfVar(); i++) {
    double sum = 0;
    for (int j = 0; j < dimension - 1; j++) {
      sum += (*population)[j]->getVar(i);
    }
    xc->setVar(i, sum / (dimension - 1));
  }
	  
  Individual *xr = getSampleInd()->internalClone(); // punto reflejado
  for (int i = 0; i < xr->getNumberOfVar(); i++) {
  	double valorCalculado=xc->getVar(i) + nm1 * (xc->getVar(i) - (*population)[dimension - 1]->getVar(i));
  	xr->setVar(i, valorCalculado);	
  }
  evaluate(xr);	
	  
	if(xr->getObj(0) < (*population)[0]->getObj(0)) // Si el reflejado mejora la mejor solución
	{
	 	Individual *xe = getSampleInd()->internalClone(); // punto extendido
	 	for (int i = 0; i < xe->getNumberOfVar(); i++) {
	 		double valorCalculado = xc->getVar(i) + nm2 * (xr->getVar(i) - xc->getVar(i));
	 		xe->setVar(i, valorCalculado);	
	 	}
	 	evaluate(xe);

	  if(xe->getObj(0) < (*population)[0]->getObj(0))
	  {
      delete (*population)[dimension - 1];
	    (*population)[dimension - 1] = xe; // agregamos el nuevo
		}
		else
		{
      delete (*population)[dimension - 1];
	    (*population)[dimension - 1] = xr; // agregamos el nuevo	
		}
	}

	else if((xr->getObj(0) >= (*population)[0]->getObj(0)) && (xr->getObj(0) < (*population)[dimension-1]->getObj(0)))
	{
    delete (*population)[dimension - 1];
	  (*population)[dimension - 1] = xr; // agregamos el nuevo	
	}
	  
	else if(xr->getObj(0) >= (*population)[dimension - 1]->getObj(0))
	{
	  if(xr->getObj(0) < (*population)[dimension - 1]->getObj(0))
	 	{
		  Individual *xcf = getSampleInd()->internalClone(); // punto de expansión
		 	for (int i = 0; i < xcf->getNumberOfVar(); i++) {
		   	double valorCalculado = xc->getVar(i) + nm3 * (xr->getVar(i) - xc->getVar(i));
		  	xcf->setVar(i, valorCalculado);	
		  }
		 	evaluate(xcf);
		  if(xcf->getObj(0) < (*population)[0]->getObj(0))
		  {
        delete (*population)[dimension - 1];
		 	  (*population)[dimension - 1] = xcf; // agregamos el nuevo
			}
			else
			{
			  shrink = true;
			}
		}
		else
		{
		  Individual *xcd = getSampleInd()->internalClone(); // punto de contracción
		 	for (int i = 0; i < xcd->getNumberOfVar(); i++) {
		   	double valorCalculado=xc->getVar(i) - nm3*(xc->getVar(i)-(*population)[dimension - 1]->getVar(i));
		  	xcd->setVar(i, valorCalculado);	
		  }
		  evaluate(xcd);
			if(xcd->getObj(0) < (*population)[0]->getObj(0))
			{
        delete (*population)[dimension - 1];
		    (*population)[dimension - 1] = xcd; // agregamos el nuevo	
			}
			else
			{
			  shrink = true;
			}
	  }
	}

	if(shrink==true)
  {
    for (int j = 1; j < dimension; j++) {
		  for (int i = 0; i < (*population)[j]->getNumberOfVar(); i++) {	
		    double valorCalculado = (*population)[0]->getVar(i) + nm4 * ((*population)[j]->getVar(i)-(*population)[0]->getVar(i));
		    (*population)[j]->setVar(i, valorCalculado);	
		  }
	    evaluate((*population)[j]);
	  } 	  
  }
}

void NelderMead::sortPopulation(){
	sort(population->begin(), population->end(), sortByObj0);
}

void NelderMead::printInfo(ostream &os) const {
	os << "Nelder-Mead Method"  << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Population Size = " << getPopulationSize() << endl;
	os << "NM1 = " << nm1 << endl;
	os << "NM2 = " << nm2 << endl;
	os << "NM3 = " << nm3 << endl;
	os << "NM4 = " << nm4 << endl;
}

void NelderMead::getSolution(MOFront *p) {
	sortPopulation();
	p->insert((*population)[0]);
}

bool sortByObj0(const Individual *i1, const Individual *i2){
	return (i1->getInternalOptDirection(0) == MINIMIZE)?(i1->getObj(0) < i2->getObj(0)):(i1->getObj(0) > i2->getObj(0));
}
