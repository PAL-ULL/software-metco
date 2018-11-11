#include "MOEA_D.h"
#include <float.h>
#include <random>
#include <math.h>
#include <limits>

const int MOEA_D::PARAMS = 4;

MOEA_D::~MOEA_D() {
    //
}

// Inicializa los parámetros iniciales del algoritmo
bool MOEA_D::init(const vector<string> &params) {
    if(params.size() != MOEA_D::PARAMS){
        cerr << "Error\nParams for MOEAD_D are: <population_size> <neighborhood_size> <mutationProb> <crossoverProb>" << endl;
        return false;
    } else {
        setPopulationSize(stof(params[0].c_str()));
        neighSize = stoi(params[1].c_str());
        mutationProb = stof(params[2].c_str());
        crossoverProb = stof(params[3].c_str());
        neighborhood.resize(getPopulationSize(), std::vector<int>());
        lambdaV.resize(getPopulationSize(), std::vector<double>());
        zVector = unique_ptr<Individual>(getSampleInd()->internalClone());
        return true;
    }
}

// Define una generación de búsqueda del algoritmo
void MOEA_D::runGeneration() {
    if(getGeneration() == 0){
        initWeights();
        initNeighborhood();
        initZVector();
    }
    const int size = getPopulationSize();
    random_device rd;
    mt19937 range(rd());
    uniform_int_distribution<int> uni(0, neighSize - 1);
    for(int i = 0; i < size; i++) {
        // Update
        int idxL = 0;
        int idxK = 0;
        while(idxL == idxK) {
            idxL = uni(range);
            idxK = uni(range);
        }
        unique_ptr<Individual> indL = unique_ptr<Individual>((*population)
                                [neighborhood[i][idxL]]->internalClone());
        unique_ptr<Individual> indK = unique_ptr<Individual>((*population)
                                [neighborhood[i][idxK]]->internalClone());
        indL->crossover(indK.get());
        indL->mutation(mutationProb);
        improvement(indL); // improvement
        vector<Individual*>* child = new vector<Individual*>();
        child->push_back(indL.get());
        updateReferencePoint(child);
        // Update Neighbors
        for(int j = 0; j < neighSize; j++) {
            double childEvaluation = evaluateWithG(indL.get(), lambdaV.at(j));
            double neighborEvaluation = evaluateWithG((*population)[neighborhood[i][j]], lambdaV.at(j));
            if (childEvaluation < neighborEvaluation){
                // Nuevo mejor vecino
                (*population)[neighborhood[i][j]] = indL->internalClone();
            }
        }
        indL.reset();
        indK.reset();
        child->clear();
        delete(child);
    }
}

void MOEA_D::initWeights() {
    const int objs = (*population)[0]->getNumberOfObj();
    const int size = getPopulationSize();
    default_random_engine generator;
    uniform_real_distribution<double> distribution(0.01, 1.0);
    for(int i = 0; i < size; i++) {
        lambdaV[i].resize(objs);
        for(int j = 0; j < objs; j++){
            lambdaV[i][j] = distribution(generator);
        }
    }
}

void MOEA_D::initNeighborhood() {
    const int size = getPopulationSize();
    vector<double> distance(size);
    vector<int> idx(size);
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            distance[j] = getEuclideanDistanceFromCoeff(lambdaV[i], lambdaV[j]);
            idx[j] = j;
        }
        minFastSort(distance, idx);
        neighborhood[i].resize(neighSize);
        for(int k = 0; k < neighSize; k++){
            neighborhood[i][k] = idx[k];
        }
    }
}

double MOEA_D::getEuclideanDistanceFromCoeff(const vector<double>& coeff1,
                                            const vector<double>& coeff2)
                                            const {
	double dist = 0;
    const int size = coeff1.size();
	for (int i = 0; i < size; i++){
		dist += (coeff1[i] - coeff2[i]) * (coeff1[i] - coeff2[i]);
	}
	return sqrt(dist);
}

void MOEA_D::minFastSort(vector<double>& distances, vector<int>& idx) {
    const int size = getPopulationSize();
    for (int i = 0; i < neighSize; i++) {
        for (int j = i + 1; j < size; j++) {
            if (distances[i] > distances[j]) {
                double temp = distances[i];
                distances[i] = distances[j];
                distances[j] = temp;
                int id = idx[i];
                idx[i] = idx[j];
                idx[j] = id;
            }
        }
    }
}

void MOEA_D::initZVector() {
    const int objs = (*population)[0]->getNumberOfObj();
    for(int i = 0; i < objs; i++) {
        zVector->setVar(i, 1.0e+30);
    }
    updateReferencePoint(population);
}

void MOEA_D::updateReferencePoint(vector<Individual*>* ind) {
    const int objs = (*population)[0]->getNumberOfObj();
    const int size = ind->size();
    for(int i = 0; i < objs; i++) {
        for(int j = 0; j < size; j++) {
            if((*ind)[j]->getInternalOptDirection(i) == MINIMIZE) {
                if(isless((*ind)[j]->getObj(i), zVector->getObj(i))) {
                    zVector->setObj(i, (*ind)[j]->getObj(i));
                }
            } else {
                if(isgreater((*ind)[j]->getObj(i), zVector->getObj(i))) {
                    zVector->setObj(i, (*ind)[j]->getObj(i));
                }
            }
        }
    }
}

double MOEA_D::evaluateWithG(Individual* ind, vector<double>& lambda) {
    const int objs = ind->getNumberOfObj();
    double objective = numeric_limits<double>::max();
    evaluate(ind);
    for(int i = 0; i < objs; i++) {
        double abs = fabs(ind->getObj(i) - zVector->getObj(i));
        if (lambda[i] == 0) {
            abs += 0.0001;
        } else {
            abs = abs * lambda[i];
        }
        if (isless(abs, objective)) {
            objective = abs;
        }
    }
    return objective;
}

// Mantenemos las variables dentro de los límites
void MOEA_D::improvement(unique_ptr<Individual>& ind) {
    const int vars = ind->getNumberOfVar();
    default_random_engine generator;
    for(int i = 0; i < vars; i++) {
        if(ind->getVar(i) > ind->getMaximum(i)
        || ind->getVar(i) < ind->getMinimum(i)){
            uniform_real_distribution<double> distribution(ind->getMinimum(i), ind->getMaximum(i));
            ind->setVar(i, distribution(generator));
        }
    }
}

// Rellena un frente con las soluciones actuales
void MOEA_D::getSolution(MOFront *p) {
   for (Individual* ind : (*population)) {
      p->insert(ind);
   }
}

// Muestra la información relativa al algoritmo
void MOEA_D::printInfo(ostream &os) const {
    os << "MOEA/D: Multiobjective Evolutionary Algorithm Based on Decomposition" << endl;
    os << "Number of Evaluations = " << getEvaluations() << endl;
    os << "Mutation Probability = " << mutationProb << endl;
    os << "Population Size = " << getPopulationSize() << endl;
    os << "Neighborhood size = " << neighSize << endl;
}
