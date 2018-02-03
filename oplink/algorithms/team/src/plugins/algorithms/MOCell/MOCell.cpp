/***********************************************************************************
 * AUTHORS
 *   - Antonio J. Nebro 
 * 
 * DATE
 *   December 2007
 * *********************************************************************************/


#include "MOCell.h"
#include <float.h>

// Constructor
MOCell::MOCell() {
	archive_ = new vector<Individual *> ;
}

// Destructor
MOCell::~MOCell() {
	for (unsigned int i = 0 ; i < archive_->size(); i++)
  	delete (*archive_)[i];
 	delete (archive_);	

	delete (neighboors_);
}

// Static variable for the current objective being ordered
int MOCell::objOrder = 0;


// Define una generaci� de bsqueda del algoritmo 
void MOCell::runGeneration() {
	for (int i = 0; i < getPopulationSize(); i++) {
		neighboors_[i] = neighborhood_->getEightNeighbors(population, i) ;
		neighboors_[i]->push_back((*population)[i]) ;

		// Selection using binary tournament
		int parent1Index ;
		int parent2Index ;
		Individual * parent1 ;
		Individual * parent2 ;

		parent1Index = binaryTournament(neighboors_[i]) ;
		parent1 = (*neighboors_[i])[parent1Index]->internalClone() ;

		if (archive_->size() > 0) {
			parent2Index = binaryTournament(archive_) ;
			parent2 = (*archive_)[parent2Index]->internalClone() ;
		} // if
		else {
			parent2Index = binaryTournament(neighboors_[i]) ;
			parent2 = (*neighboors_[i])[parent2Index]->internalClone() ;
		} // else
			

		// Crossover
	  if ((double)rand() / RAND_MAX <= pc)
	 	  parent1->crossover(parent2) ;

		delete parent2 ;
		// Mutation
		parent1->mutation(pm) ;
		//parent2->mutation(pm) ;
		evaluate(parent1) ;
		insertInArchive(parent1) ;
		//evaluate(parent2) ;
		int dominates = dominanceTest(parent1, (*population)[i]) ;
		if (dominates == FIRST_DOMINATES) { // parent1 dominates newIndividual
			delete (*population)[i] ;
			(*population)[i] = parent1 ;
			addToArchive(parent1->internalClone(), archive_) ;
		} // if
		else if (dominates == SECOND_DOMINATES) { // newIndividual dominates parent1
			delete parent1 ;
		}
		else { // the two individuals are non-dominated
			for (int j = 0; j < population->size(); j++) {
				(*population)[j]->setAuxData(0, j) ;
			} // for
				
			parent1->setAuxData(0, -1) ;
			neighboors_[i]->push_back(parent1) ;

			crowOrder(*neighboors_[i]) ;
				
			int worst = neighboors_[i]->size()-1 ;
			int worstLocation = (int) neighboors_[i]->at(worst)->getAuxData(0) ;
			if (worstLocation == -1) {// parent1 is the worst
				addToArchive((*population)[i]->internalClone(), archive_) ;
		  }
			else {
				delete (*population)[worstLocation] ;
				(*population)[worstLocation] = parent1->internalClone() ;
				addToArchive(parent1, archive_) ;
			}
		}
		delete neighboors_[i] ;
	} // for
}
	

// Inicializa los par�etros iniciales del algoritmo
bool MOCell::init(const vector<string> &params) {

	// Check for the algorithm parameters
	if (params.size() != 4) {
		cerr << "Error MOCell: Incorrect parameters (pm pc pSize aSize)" << endl;
		return false;
	}

	// Initiate the parameters
	this->pm = atof(params[0].c_str());
	this->pc = atof(params[1].c_str());
	setPopulationSize(atoi(params[2].c_str()));
	archiveSize_ = atoi(params[3].c_str()) ;
	getSampleInd()->setAuxDataSize(1);
	
	neighborhood_ =  new Neighborhood(getPopulationSize());
  neighboors_   =  new vector<Individual *>*[getPopulationSize()];

	return true;
}


// Rellena un frente con las soluciones actuales
void MOCell::getSolution(MOFront *p) {
	for (unsigned int i = 0;  i < archive_->size(); i++) {
		p->insert((*archive_)[i]);
	}
}


// Muestra la informaci� relativa al algoritmo
void MOCell::printInfo(ostream &os) const {
	os << "Genetic Algorithm MOCell" << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Mutation Probability = " << pm << endl;
	os << "Crossover Probability = " << pc << endl;
	os << "Population Size = " << getPopulationSize() << endl;
	os << "Archive Size = " << archiveSize_ << endl;
}


/*******************************************************************************
 * Los siguientes m�odos son espec�icos de MOCell y describen las operaciones
 * internas y el propio funcionamiento del algoritmo.
 * *****************************************************************************/

bool MOCell::addToArchive(Individual * newIndividual,
                          vector<Individual *> *archive) {
	Individual *aux ;
	//vector<Individual *> * tmp = new vector<Individual *> ;
	int i = 0 ;
	while (i < archive->size()) {
		int dominates = dominanceTest(newIndividual, (*archive)[i]) ;
		if (dominates == SECOND_DOMINATES) { // aux dominates newIndividual
			delete newIndividual ;
			return false ;
		} // if
		else if (dominates == FIRST_DOMINATES) { // newIndividual dominates aux
			delete (*archive)[i] ;
			archive->erase(archive->begin() + i) ;
		}
		else {
			i++ ;
		}
	} // while

	// Insert the solution into the archive
	archive->push_back(newIndividual) ;
        
    if (archive->size() > archiveSize_) { // The archive is full
	// calculate crowding distance and order
			crowOrder(*archive) ;      
      // remove the last
      archive->erase(archive->end() - 1) ;
    }        
    return true;
} // addToArchive

/////////////////////////
// Destructor

Neighborhood::~Neighborhood() {
  for (int i = 0; i < neighborhoodSize_; i++) {
    for (int radio = 0; radio < MAXRADIO; radio ++) 
      delete[] structure_[i][radio] ;
    delete [] structure_[i] ;
  }
  delete [] structure_ ;
}

// Constructor
Neighborhood::Neighborhood(int neighborhoodSize) {
  neighborhoodSize_ = neighborhoodSize ;

  //Create the structure_ for store the neighborhood
  structure_ = new int**[neighborhoodSize_];
  for (int i = 0; i < neighborhoodSize_; i++)
    structure_[i] = new int*[MAXRADIO] ;
       

  for (int ind = 0; ind < neighborhoodSize_; ind ++) {
    for (int radio = 0; radio < MAXRADIO; radio ++) {
      if (radio == 0) {//neighboors whit rate 1
        structure_[ind][radio] = new int[8];
      } else if (radio == 1) { //neighboors whit rate 2
        structure_[ind][radio] = new int[24];
      } // if
    } // for
  } // for

    //Calculate the size of a row
  rowSize_ = (int)sqrt((double)neighborhoodSize_);

  //Calculates the neighbors of a individual
  for (int ind = 0; ind <neighborhoodSize_; ind++){
      //rate 1
      //North neighbors
      if (ind > rowSize_ - 1){
        structure_[ind][0][N_] = ind - rowSize_;
      } else {
        structure_[ind][0][N_] = 
       (ind - rowSize_ + neighborhoodSize_) % neighborhoodSize_;                         
      }
            
      //East neighbors
      if  ((ind + 1) % rowSize_ == 0)
        structure_[ind][0][E_] = (ind - (rowSize_ - 1));
      else
        structure_[ind][0][E_] = (ind + 1) % neighborhoodSize_;

      //Western neigbors
      if (ind % rowSize_ == 0) {
        structure_[ind][0][W_] = (ind + (rowSize_ - 1)) % neighborhoodSize_;
      } else {
        structure_[ind][0][W_] = (ind - 1);
      }

      //South neigbors
      structure_[ind][0][S_] = (ind + rowSize_) % neighborhoodSize_ ;                        
    }                
        
    for (int ind = 0; ind < neighborhoodSize_; ind++){
      structure_[ind][0][NE_] = 
        structure_[structure_[ind][0][N_]][0][E_];
      structure_[ind][0][NW_] = 
        structure_[structure_[ind][0][N_]][0][W_];
      structure_[ind][0][SE_] = 
        structure_[structure_[ind][0][S_]][0][E_];
      structure_[ind][0][SW_] = 
        structure_[structure_[ind][0][S_]][0][W_];
    }
} // Constructor


vector<Individual *>* Neighborhood::getEightNeighbors(vector<Individual *>* pop, int individual){
    // Population that contains the neighbors (to return)
    vector<Individual *>* neighbors;

    neighbors = new vector<Individual *>;

    //Gets the neighboords N, S, E, W
    int index;
        
    //N
    index = structure_[individual][0][N_];        
    neighbors->push_back(pop->at(index));

    //S
    index = structure_[individual][0][S_];
    neighbors->push_back(pop->at(index));

    //E
    index = structure_[individual][0][E_];
    neighbors->push_back(pop->at(index));

    //W
    index = structure_[individual][0][W_];
    neighbors->push_back(pop->at(index));

    //NE
    index = structure_[individual][0][NE_];
    neighbors->push_back(pop->at(index));

    //NW
    index = structure_[individual][0][NW_];
    neighbors->push_back(pop->at(index));

    //SE
    index = structure_[individual][0][SE_];
    neighbors->push_back(pop->at(index));

    //SW
    index = structure_[individual][0][SW_];
    neighbors->push_back(pop->at(index));


    //Return the list of non-dominated individuals
    return neighbors;        

} // getEightNeighbors

