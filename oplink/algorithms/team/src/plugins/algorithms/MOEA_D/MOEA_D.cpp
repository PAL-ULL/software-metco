#include "MOEA_D.h"
#include <float.h>


// Define una generación de búsqueda del algoritmo 
void MOEA_D::runGeneration() {

}

// Inicializa los parámetros iniciales del algoritmo
bool MOEA_D::init(const vector<string> &params) {
    return true;
}

// Rellena un frente con las soluciones actuales
void MOEA_D::getSolution(MOFront *p) {
   for (unsigned int i = 0;  i < population->size(); i++) {
      p->insert((*population)[i]);
   }
}

// Muestra la información relativa al algoritmo
void MOEA_D::printInfo(ostream &os) const {
   os << "TODO" << endl;
}