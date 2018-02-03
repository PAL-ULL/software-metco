/* -----------------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------------- */

#include "KNAPII.h"

//Number of items and number of knapsacks, respectively
int KNAPII::nParam;
int KNAPII::nObj;

// Profits, weights, capacities and max profit/weight ratios, respectively
float* KNAPII::profit;
float* KNAPII::weight;
float KNAPII::capacity;
int* KNAPII::deleteOrder;
const int KNAPII::OBJFUNC[] = {MAXIMIZE, MINIMIZE};

//Problem Initialization
bool KNAPII::init(const vector<string> &params) {
  // Check for the problem parameters
	if (params.size() != 3) {
	  cout << "Error KNAPII: Incorrect parameters (number_of_objectives)" << endl;
		return false;
  }
  nParam = atoi((params[1]).c_str());
	setNumberOfVar(nParam);
	nObj = atoi(params[2].c_str());
	setNumberOfObj(nObj);

	profit = new float [nParam];
	weight = new float [nParam];
	deleteOrder = new int [nParam];

  for (int i = 0; i < nParam; i++) {
    deleteOrder[i] = i;
  }

  readFile(params[0]);
  ratios();

	return true;
}

// Reads a problem from file
void KNAPII::readFile(const string filename) {
	ifstream input(filename.c_str());
	string aux;
	int knapNumber;
	input >> aux >> knapNumber >> aux;
	float capacity;
	input >> aux >> capacity;
	this->capacity = capacity;
	for (int i = 0; i < nParam; i++) {
		int itemNumber;
		input >> aux >> itemNumber >> aux;
		float weight;
		input >> aux >> weight;
		float profit;
		input >> aux >> profit;
		this->weight[i] = weight;
		this->profit[i] = profit;
	}
	input.close();
}

// Genetic Operators - Binary Mutation
void KNAPII::dependentMutation(double pm) {
  mutate_binary(pm);
	repair();
}

// Genetic Operators - Uniform Crossover
void KNAPII::dependentCrossover(Individual* ind) {
  crossover_uniform(ind);
	((KNAPII*)ind)->repair();
	repair();
}

// Random generation of an individual
void KNAPII::restart(void) {
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
void KNAPII::repair(void) {
	int i = 0;
	while (!checkCapacity()) {
		setVar(deleteOrder[i], 0);
		i++;
	}
}

// Calculates the profit/weight ratio for each item in increasing order
void KNAPII::ratios(void) {
	float *ratio = new float [nParam];
	for (int i = 0; i < nParam; i++)
		ratio[i] = profit[i] / weight[i];
	quicksort(deleteOrder, ratio, 0, nParam - 1);
	delete [] ratio;
}

// Quicksort algorithm to order the profit/weight ratio vector
void KNAPII::quicksort(int *deleteOrder, float *ratio, int begin, int end) {
	float pivote = ratio[end];
	int i = begin;
	int j = end;
	bool cont = true;

	if (begin >= end)
		return;

	while (cont) {
		while (ratio[++i] < pivote);
		while (ratio[--j] > pivote);
		if (i < j) {
			float temp = ratio[i];
			int aux = deleteOrder[i];
			ratio[i] = ratio[j];
			deleteOrder[i] = deleteOrder[j];
			ratio[j] = temp;
			deleteOrder[j] = aux;
		}
		else
			cont = false;
	}
               
	float temp = ratio[i];
	int aux = deleteOrder[i];
	ratio[i] = ratio[end];
	deleteOrder[i] = deleteOrder[end];
	ratio[end] = temp;
	deleteOrder[end] = aux;
                            
	quicksort(deleteOrder, ratio, begin, i - 1);
	quicksort(deleteOrder, ratio, i + 1, end);		 
}

// Checks capacity restriction
bool KNAPII::checkCapacity(void) {
	float sum = 0;
	for (int i = 0; i < nParam; i++)
		sum += (weight[i] * getVar(i));
	if (sum > capacity)
		return false;
	return true;
}

// Evaluation of an individual
void KNAPII::evaluate (void) {
	setObj(0, 0);
	for (int i = 0; i < nParam; i++)
		setObj(0, getObj(0) + (profit[i] * getVar(i)));
	setObj(1, 0);
	for (int i = 0; i < nParam; i++)
		setObj(1, getObj(1) + (weight[i] * getVar(i)));
}

Individual* KNAPII::clone(void) const {
	return new KNAPII();
}
