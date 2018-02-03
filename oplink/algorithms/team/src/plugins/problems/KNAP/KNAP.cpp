/* -----------------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------------- */

#include "KNAP.h"

// Number of items and number of knapsacks, respectively
int KNAP::nParam;
int KNAP::nObj;
	
// Profits, weights, capacities and max profit/weight ratios, respectively
float* KNAP::profit;
float* KNAP::weight;
float* KNAP::capacity;
int* KNAP::deleteOrder;

// Problem Initialization
bool KNAP::init(const vector<string> &params) {
	// Check for the problem parameters
	if (params.size() != 4) {
    cout << "Error KNAP: Incorrect parameters (number_of_objectives) type_crossover" << endl;
   	return false;
	}
  
	nParam = atoi((params[1]).c_str());
	setNumberOfVar(nParam);
	nObj = atoi(params[2].c_str());
	setNumberOfObj(nObj);
	
	type_crossover = atoi(params[3].c_str());
	
	profit = new float [nObj * nParam];
	weight = new float [nObj * nParam];
	capacity = new float [nObj];
	deleteOrder = new int [nParam];
	
	for (int i = 0; i < nParam; i++) {
		deleteOrder[i] = i;
	}

	// Reads a problem from file
	readFile(params[0]);
	ratios();
	
	return true;
}


// Reads a problem from file
void KNAP::readFile(const string filename) {
	ifstream input(filename.c_str());
	string aux;
	for (int i = 0; i < nObj; i++) {
		int knapNumber;
		input >> aux >> knapNumber >> aux;
		float capacity;
		input >> aux >> capacity;
		this->capacity[i] = capacity;
		for (int j = 0; j < nParam; j++) {
			int itemNumber;
			input >> aux >> itemNumber >> aux;
			float weight;
			input >> aux >> weight;
			float profit;
			input >> aux >> profit;
			setWeight(i, j, weight);
			setProfit(i, j, profit);
		}
		input >> aux;
	}
	input.close();
}

// Genetic Operators - Binary Mutation
void KNAP::dependentMutation(double pm) {
	mutate_binary(pm);
	repair();
}

// Genetic Operators - Uniform Crossover 
void KNAP::dependentCrossover(Individual* ind) {
	if (type_crossover == CROSSOVER_UNIFORM) {
		crossover_uniform(ind);
	}
	else if (type_crossover == CROSSOVER_HUX){
		crossover_HUX(ind);
	} else {
		cout<<"Error en el tipo de Crossover. type_crossover = 0 -> crossover_uniforme. type_crossover = 1 -> crossover_hux"<<endl;
	}
      
	((KNAP*)ind)->repair();
	repair();
}


void KNAP::crossover_HUX (Individual *ind) {
   int distanciaInd = hammingDistance(this,ind);
   vector<int> vectorIndices;
   
   //Se genera un vector de valores aleatorios para seleccionar de forma aleatoria la primera
   //mitad de los elementos
   generateRandomValues (0, distanciaInd-1, distanciaInd, vectorIndices);
 
   for (unsigned int i = 0; i < getNumberOfVar(); i++) {  
      
      //Se cruza si los elementos son diferentes
      if (getVar(i) != ind->getVar(i)) {

	 //Se comprueba si el elemento a cruzar esta en la primera mitad de la lista aleatoria
	 for (unsigned int j = 0; j < floor(vectorIndices.size()/2); j++) {
	    
	    //Si esta en la lista se realiza el cruce
	    if (i == vectorIndices[j]) {	    
	       double tmp = ind->getVar(i);
	       ind->setVar(i, getVar(i));
	       setVar(i, tmp);
	    }
	 }
      }
   }

}

// Random generation of an individual
void KNAP::restart(void) {
	for (int i = 0; i < nParam; i++) {
		setVar(i, ((double) rand () / (double) RAND_MAX));
  	setVar(i, getVar(i) * (getMaximum(i) - getMinimum(i)) + getMinimum(i));
		if (getVar(i) >= 0.5)
			setVar(i, 1);
		else
			setVar(i, 0);
	}
	repair();
}

// Repairs an individual with capacity restriction
void KNAP::repair(void) {
	int i = 0;
	while (!checkCapacity()) {
		setVar(deleteOrder[i], 0);
		i++;
	}
}

// Calculates the max profit/weight ratio for each item in increasing order
void KNAP::ratios(void) {
	float *maxRatio = new float [nParam];
	for (int j = 0; j < nParam; j++) {
		maxRatio[j] = -1.0;
		for (int i = 0; i < nObj; i++) {
			float ratio = getProfit(i, j) / getWeight(i, j);
			if (ratio >= maxRatio[j])
				maxRatio[j] = ratio;
		}
	}
	quicksort(deleteOrder, maxRatio, 0, nParam - 1);
	delete maxRatio;
}

// Quicksort algorithm to order the max profit/weight ratio vector
void KNAP::quicksort(int *deleteOrder, float *maxRatio, int begin, int end) {
	float pivote = maxRatio[end];
	int i = begin;
	int j = end;
	bool cont = true;

	if (begin >= end)
		return;

	while (cont) {
		while (maxRatio[++i] < pivote);
		while (maxRatio[--j] > pivote);
		if (i < j) {
			float temp = maxRatio[i];
			int aux = deleteOrder[i];
			maxRatio[i] = maxRatio[j];
			deleteOrder[i] = deleteOrder[j];
			maxRatio[j] = temp;
			deleteOrder[j] = aux;
		}
		else
			cont = false;
	}
               
	float temp = maxRatio[i];
	int aux = deleteOrder[i];
	maxRatio[i] = maxRatio[end];
	deleteOrder[i] = deleteOrder[end];
	maxRatio[end] = temp;
	deleteOrder[end] = aux;
                            
	quicksort(deleteOrder, maxRatio, begin, i - 1);
	quicksort(deleteOrder, maxRatio, i + 1, end);		 
}

// Checks all capacities restrictions
bool KNAP::checkCapacity(void) {
	for (int i = 0; i < nObj; i++) {
		float sum = 0;
		for (int j = 0; j < nParam; j++)
			sum += (getWeight(i, j) * getVar(j));
		if (sum > capacity[i])
			return false;
	}
	return true;
}

// Evaluation of an individual
void KNAP::evaluate (void) {
	for (int i = 0; i < nObj; i++) {
		setObj(i, 0);
		for (int j = 0; j < nParam; j++) 
			setObj(i, getObj(i) + (getProfit(i, j) * getVar(j)));
	}
}

Individual* KNAP::clone(void) const {
	return new KNAP();
}
