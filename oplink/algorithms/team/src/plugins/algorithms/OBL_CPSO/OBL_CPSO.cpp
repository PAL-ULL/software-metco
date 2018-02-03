#include "OBL_CPSO.h"

//#define DEBUG

// Algorithm initialization
bool OBL_CPSO::init(const vector<string>& params) {
	// Check number of parameters
	if (params.size() != 2) {
		cout << "Parametros: gamma popSize" << endl;
		return false;
	} 

  // Only mono-objective optimization is supported
	if (getSampleInd()->getNumberOfObj() != 1) {
		cout << "Multi-Objective not supported" << endl;
		return false;
	}

  setGamma(atof(params[0].c_str()));
	setPopulationSize(atoi(params[1].c_str()));
	mean = getSampleInd()->internalClone();
	randomPop.resize(getPopulationSize(), 0);

	return true;
}

void OBL_CPSO::runGeneration() {
	if (getGeneration() == 0)
		initVelocity();
  compete();
}

void OBL_CPSO::initVelocity() {
	const int dimension = (*population)[0]->getNumberOfVar();
  velocity.resize(getPopulationSize(), vector<double>(dimension, 0.0));

	/*for (int i = 0; i < getPopulationSize(); i++) {
		for (int j = 0; j < dimension; j++) {
      double aux = ((double) rand () / (double) RAND_MAX);
      velocity[i][j] = aux * ((*population)[i]->getMaximum(j) - (*population)[i]->getMinimum(j)) + (*population)[i]->getMinimum(j);
		}
	}*/
}

void OBL_CPSO::shufflePop() {
  randomPop.clear();
  for (int i = 0; i < getPopulationSize(); i++)
    randomPop.push_back(i);
  std::random_shuffle(randomPop.begin(), randomPop.end());
}

void OBL_CPSO::calculateMean() {
	const int dim = (*population)[0]->getNumberOfVar();
	const int pop = getPopulationSize();

	for (int j = 0; j < dim; j++) {
		mean->setVar(j, 0.0);
		for (int i = 0; i < pop; i++) {
			mean->setVar(j, mean->getVar(j) + (*population)[i]->getVar(j));
		}
		mean->setVar(j, mean->getVar(j) / (double)pop);
	}
}

void OBL_CPSO::compete() {
	const int pop = getPopulationSize();
  shufflePop();
  calculateMean();

  /*for (int i = 0; i < pop; i++) {
    cout << "randomPop[" << i << "]: " << randomPop[i] << endl;
  }*/

	for (int i = 0; i < pop / 3; i++) {
		int r1 = randomPop[i];
		int r2 = randomPop[i + (int)(pop / 3)];
		int r3 = randomPop[i + 2 * (int)(pop / 3)];

		int winner = compete1(r1, r2, r3);
		int loser = compete2(r1, r2, r3);
		int neutral = r1 + r2 + r3 - winner - loser;

		update(winner, neutral, loser);
	}
}

int OBL_CPSO::compete1(int r1, int r2, int r3) {
	if ((*population)[r1]->getObj(0) < (*population)[r2]->getObj(0) && (*population)[r1]->getObj(0) < (*population)[r3]->getObj(0))
		return r1;
	if ((*population)[r2]->getObj(0) < (*population)[r1]->getObj(0) && (*population)[r2]->getObj(0) < (*population)[r3]->getObj(0))
		return r2;
	if ((*population)[r3]->getObj(0) < (*population)[r1]->getObj(0) && (*population)[r3]->getObj(0) < (*population)[r2]->getObj(0))
		return r3;
	if ((*population)[r1]->getObj(0) == (*population)[r2]->getObj(0))
		return r1;
	if ((*population)[r1]->getObj(0) == (*population)[r3]->getObj(0))
		return r1;
	if ((*population)[r2]->getObj(0) == (*population)[r3]->getObj(0))
		return r2;
}

/**
 * Determina el peor de todos, el de OBJ mayor
 */
int OBL_CPSO::compete2(int r1, int r2, int r3) {
	if ((*population)[r1]->getObj(0) > (*population)[r2]->getObj(0) && (*population)[r1]->getObj(0) > (*population)[r3]->getObj(0))
		return r1;
	if ((*population)[r2]->getObj(0) > (*population)[r1]->getObj(0) && (*population)[r2]->getObj(0) > (*population)[r3]->getObj(0))
		return r2;
	if ((*population)[r3]->getObj(0) > (*population)[r1]->getObj(0) && (*population)[r3]->getObj(0) > (*population)[r2]->getObj(0))
		return r3;
	if ((*population)[r1]->getObj(0) == (*population)[r2]->getObj(0))
		return r2;
	if ((*population)[r1]->getObj(0) == (*population)[r3]->getObj(0))
		return r3;
	if ((*population)[r2]->getObj(0) == (*population)[r3]->getObj(0))
		return r3;
}

void OBL_CPSO::update(int winner, int neutral, int loser) {

  //cout << "w: " << winner << " n: " << neutral << " l: " << loser << endl;

	//random_device rd;
	//mt19937 mt(rd());
	//uniform_real_distribution<double> dist(0.0, std::nextafter(1.0, DBL_MAX));

	//setGamma((0.7 - 0.2) * (getEvaluations() - getPerformedEvaluations()) / getEvaluations() + 0.2);
  //cout << "gamma: " << getGamma() << endl;

  const int dimension = (*population)[0]->getNumberOfVar();

	for (int j = 0; j <  dimension; j++) {
		double coeff1 = ((double) rand () / (double) RAND_MAX);
		double coeff2 = ((double) rand () / (double) RAND_MAX);
		double coeff3 = ((double) rand () / (double) RAND_MAX);
		double coeff4 = ((double) rand () / (double) RAND_MAX);

    /*cout << "coef1: " << coeff1 << endl;
    cout << "coef2: " << coeff2 << endl;
    cout << "coef3: " << coeff3 << endl;
    cout << "coef4: " << coeff4 << endl;*/

    double upper = (*population)[winner]->getMaximum(j);
    double lower = (*population)[winner]->getMinimum(j);

    /*cout << "upper: " << upper << endl;
    cout << "lower: " << lower << endl;*/

		velocity[loser][j] = coeff1 * velocity[loser][j] + coeff2 * ((*population)[winner]->getVar(j) - (*population)[loser]->getVar(j))
		                   + coeff3 * getGamma() * (mean->getVar(j) - (*population)[loser]->getVar(j));

		(*population)[loser]->setVar(j, (*population)[loser]->getVar(j) + velocity[loser][j]);

		if (((*population)[loser]->getVar(j) > (*population)[loser]->getMaximum(j)) || ((*population)[loser]->getVar(j) < (*population)[loser]->getMinimum(j))) {
			double aux = ((double) rand () / (double) RAND_MAX);
			(*population)[loser]->setVar(j, aux * ((*population)[loser]->getMaximum(j) - (*population)[loser]->getMinimum(j)) + (*population)[loser]->getMinimum(j));
    }
		
    (*population)[neutral]->setVar(j, upper + lower - (*population)[neutral]->getVar(j) + coeff4 * (*population)[neutral]->getVar(j));
		
    if (((*population)[neutral]->getVar(j) > (*population)[neutral]->getMaximum(j)) || ((*population)[neutral]->getVar(j) < (*population)[neutral]->getMinimum(j))) {
			double aux = ((double) rand () / (double) RAND_MAX);
			(*population)[neutral]->setVar(j, aux * ((*population)[neutral]->getMaximum(j) - (*population)[neutral]->getMinimum(j)) + (*population)[neutral]->getMinimum(j));
    }
	}

	evaluate((*population)[loser]);
	evaluate((*population)[neutral]);
}

void OBL_CPSO::sortPopulation() {
	sort(population->begin(), population->end(), sortByObj0);
}

void OBL_CPSO::getSolution(MOFront* p) {
	sortPopulation();
	p->insert((*population)[0]);
}

void OBL_CPSO::printInfo(ostream& os) const {
	os << "Opposition-based Learning Competitive Particle Swarm Optimisation (OBL_CPSO)"  << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Population Size = " << getPopulationSize() << endl;
  os << "Gamma = " << gamma << endl;
}

bool sortByObj0(const Individual* i1, const Individual* i2) {
	return (i1->getInternalOptDirection(0) == MINIMIZE) ?
	       (i1->getObj(0) < i2->getObj(0)) : (i1->getObj(0) > i2->getObj(0));
}
