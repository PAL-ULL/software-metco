/************************************************************************************
 * AUTHORS
 *    Eduardo Manuel Segredo Gonzalez
 *    Gara Miranda Valladares
 *    Carlos Segura Gonzalez
 * 
 * DATE 
 *   December 2007
 ***********************************************************************************/
 
#include "IBEA_Adapt.h"

// Destructor
IBEA_Adapt::~IBEA_Adapt() {
  delete valuesTable;
}

// Defines an algorithm search generation
void IBEA_Adapt::runGeneration() {
	fitness();
	selection();
	matingSelection();
	variation();
	evaluation();
}


// Initializes the algorithm parameters
bool IBEA_Adapt::init(const vector<string> &params) {
	
	// Check for the proper parameters
	if (params.size() != 4) {
		cerr << "Error IBEA: Incorrect parameters (pm pc pSize sFactor)" << endl;
    return false;
	}

	// Initiate the parameters
	this->pm = atof(params[0].c_str());
	this->pc = atof(params[1].c_str());
	setPopulationSize(atoi(params[2].c_str()));
	this->sFactor = atof(params[3].c_str());
	valuesTable = new double [(2 * getPopulationSize()) * (2 * getPopulationSize())];

	for (int i = 0; i < getSampleInd()->getNumberOfObj(); i++) {
  	minObj.push_back(DBL_MAX);
  	maxObj.push_back(-DBL_MAX);
	}

	return true;

}

string IBEA_Adapt::getParameterValue(int i) const{
	stringstream s;
	switch(i){
		case 0:
			s << pm;
			return s.str();
		case 1:
			s << pc;
			return s.str();
		case 2:
			s << getPopulationSize();
			return s.str();
		case 3:
			s << sFactor;
			return s.str();
	}
	return "invalid";

}

string IBEA_Adapt::getParameterName(int i) const{
	switch(i){
		case 0:
			return "pm";
		case 1:
			return "pc";
		case 2:
			return "pSize";
		case 3:
			return "sFactor";
	}
	return "invalid";
}

int IBEA_Adapt::getNumberOfParameters() const{
	return 4;
}

// Actual solution to the Pareto Front
void IBEA_Adapt::getSolution(MOFront *p) {
	for (unsigned int i = 0;  i < population->size(); i++) {
		p->insert((*population)[i]);
	}
}


// Shows the algorithm information
void IBEA_Adapt::printInfo(ostream &os) const {
	os << "Genetic Algorithm IBEA (Adaptative)" << endl;
	os << "Number of Evaluations = " << getPerformedEvaluations() << endl;
	os << "Mutation Probability = " << pm << endl;
	os << "Crossover Probability = " << pc << endl;
	os << "Population Size = " << getPopulationSize() << endl;				
	os << "Scaling Factor (k) = " << sFactor << endl;
}

/*******************************************************************************
 * Specific methods of IBEA algorithm.
 * *****************************************************************************/

// Mutators
void IBEA_Adapt::setValue(const int &i, const int &j, const double &value) {
  valuesTable[i * (2 * getPopulationSize()) + j] = value;
}

// Inspectors
double IBEA_Adapt::getValue(const int &i, const int &j) {
  return valuesTable[i * (2 * getPopulationSize()) + j];
}

// Selection of best individuals in the population
void IBEA_Adapt::selection(void) {

	// Only after first generation
  if (population->size() ==  (2 * getPopulationSize())) {
  	
		// Selection of best individuals from the population	
  	int i = 0;
  	
		while (i < getPopulationSize()) {
		 
		  int pos = 0;
		  double minValue = DBL_MAX;

			// Looks for the individual with minimum fitness value
  		for (int j = 0; j < population->size(); j++) {
  			double actualValue = (*population)[j]->getFitnessValue();
  		  if (actualValue < minValue) {
  			  minValue = actualValue;
  				pos = j;
  		  }
  		}

			// Updates the fitness value of all individuals without the worst of them
  		updateFitness(pos);
			
			// Swapp the worst individual with the last individual in population
			Individual *aux = (*population)[pos];
			(*population)[pos] = (*population)[population->size() - 1];
			(*population)[population->size() - 1] = aux;

			// Updates the table of indicator values
		  for (int k = 0; k < population->size(); k++) {
		    setValue(k, pos, getValue(k, population->size() - 1));
	      setValue(pos, k, getValue(population->size() - 1, k));
	    }
	    setValue(pos, pos, 0);

			// Removes the individual with worst fitness value
  	  delete((*population)[population->size() - 1]);
			population->pop_back();
			
  		i++;
  	}
	}
}

// Mating selection
void IBEA_Adapt::matingSelection(void) {
	int ind1, ind2;
	
	// Reset the mating pool
	mating.clear();

	// Fill the mating pool
	for (int i = 0; i < getPopulationSize(); i++) {
		// 2 individuals are chosen randomly to do the tournament: from 0 to pSize -1
		ind1 = (int)((double)(population->size())*rand() / (RAND_MAX + 1.0));
		ind2 = (int)((double)(population->size())*rand() / (RAND_MAX + 1.0));
		if ((*population)[ind1]->getFitnessValue() > (*population)[ind2]->getFitnessValue())
			mating.push_back((*population)[ind1]);
		else
			mating.push_back((*population)[ind2]);
	}
}

// Evaluation of the individuals in the population
void IBEA_Adapt::evaluation (void) {
	// Evaluate the population. Only evaluates the individuals added from the mating pool 
	for (int i = (population->size() - getPopulationSize()); i < population->size(); i++) {
	  evaluate((*population)[i]);
	  insertInArchive((*population)[i]);
	}
}


// Variation from the maiting pool to the new population
void IBEA_Adapt::variation (void) {
	
  // Crossover and mutation
	for (int i = 0; i < getPopulationSize(); i = i + 2) {
		population->push_back(mating[i]->internalClone());
		if (i != getPopulationSize() - 1)
		  population->push_back(mating[i + 1]->internalClone());
		// Do crossover
		double vcross = rand() / (RAND_MAX + 1.0);
		if (vcross < pc) {
			if (i != getPopulationSize() - 1) {
				(*population)[population->size() - 2]->crossover((*population)[population->size() - 1]);
			} 
			else {
				Individual *tmp = mating[0]->internalClone();
				(*population)[population->size() - 1]->crossover(tmp);
				delete(tmp);
		  }
		}
		// Do mutation
		(*population)[population->size() - 1]->mutation(pm);
		if (i != getPopulationSize() - 1)
			(*population)[population->size() - 2]->mutation(pm);
	}
}

// Calculates the fitness value of all individuals
void IBEA_Adapt::fitness(void) {

	//Se recalculan los limites porque x ej. debido a la migracion podrian aparecer aqui individuos
	//diferentes a los que se dejaron la ultima vez
	for (int i = (population->size() - getPopulationSize()); i < population->size(); i++) {
		for (int j = 0; j < (*population)[i]->getNumberOfObj(); j++) {
			double value = (*population)[i]->getObj(j);
		  if (value > maxObj[j])
		    maxObj[j] = value;
			if (value < minObj[j])
			  minObj[j] = value;
		}
	}
	// Calculate indicator values and determine the maximum absolute indicator value
	maxIndValue = DBL_MIN;
	for (int i = 0; i < population->size(); i++) {
    for (int j = 0; j < population->size(); j++) {
		  if (i != j) {
				double value = getEpsilonIndicator((*population)[j], (*population)[i]);
				setValue(i, j, value);//i,j grande si i es mejor que j
				if (fabs(value) > maxIndValue)
			    maxIndValue = fabs(value);
			} else
			  setValue(i, j, 0);
    }
	}

	for (int i = 0; i < population->size(); i++) {
		double totalValue = 0;
	  for (int j = 0; j < population->size(); j++) {
		  if (i != j) {
			  totalValue -= exp(-(getValue(i, j) / (maxIndValue * sFactor)));
			}
	  }
		(*population)[i]->setFitnessValue(totalValue);
  }

}

// Updates the fitness value of all individuals
void IBEA_Adapt::updateFitness(const int &pos) {
  for (int i = 0; i < population->size(); i++) {
	  if (i != pos) {
		  double value = exp(-(getValue(i, pos) / (maxIndValue * sFactor)));
      (*population)[i]->setFitnessValue((*population)[i]->getFitnessValue() + value);
	  }
  }
}

// Minimization is considered in all objectives
// Values between values 0 and 1
// 1 is summed to every value, so values are between 1 and 2
// The returned value is between 0.5 and 2
// The indicator is to minimize
// Si B es peor ==> Devuelve valores menores
double IBEA_Adapt::getEpsilonIndicator(Individual *indA, Individual *indB) {
  double maxVal3 = DBL_MIN;

	for (int i = 0; i < indA->getNumberOfObj(); i++) {
	
		double objA = (indA->getObj(i) - minObj[i]) / (maxObj[i] - minObj[i]);
		double objB = (indB->getObj(i) - minObj[i]) / (maxObj[i] - minObj[i]);

		if (indA->getInternalOptDirection(i) == MINIMIZE) {
		  double value = (objA + 1) / (objB + 1);
	    if (value > maxVal3)
        maxVal3 = value;
		} else {
		  double value = (objB + 1) / (objA + 1);
	    if (value > maxVal3)
        maxVal3 = value;
		}
	}
	
	return maxVal3;
}
