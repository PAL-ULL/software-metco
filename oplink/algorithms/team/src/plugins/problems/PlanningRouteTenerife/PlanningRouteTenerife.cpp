/* -----------------------------------------------------------------------------
 *
 *
 * -------------------------------------------------------------------------- */

#include "PlanningRouteTenerife.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdlib>


	// Porcentaje de actividades, coste de estas y tiempoTotal
	// double PlanningRouteTenerife::coste = 0;
	// float PlanningRouteTenerife::porcentajeActividades = 0;
	int PlanningRouteTenerife::tiempoTotal;
	
	// Actividades, costes, duracion, distancias, individuos
	vector<string>PlanningRouteTenerife::actividades; 
	vector<double>PlanningRouteTenerife::costes;
	vector<int>PlanningRouteTenerife::duracion;
	vector< vector<int> >PlanningRouteTenerife::distancias;
	// vector<int>PlanningRouteTenerife::individuos;

// Inicialización del problema
bool PlanningRouteTenerife::init(const vector<string> &params) {
 	// Check for the problem parameters
	if (params.size() != 2) {
    cout << "Error: No ha pasado el número correcto de parametros: crossoverType fileName" << endl;
   	return false;
	}
  
	/*profit = new float [nObj * nParam];
	weight = new float [nObj * nParam];
	capacity = new float [nObj];
	deleteOrder = new int [nParam];
	
	for (int i = 0; i < nParam; i++) {
		deleteOrder[i] = i;
	}*/

	type_crossover = atoi(params[0].c_str());

	// Reads a problem from file
	readFile(params[1].c_str());
	//ratios();
	
  setNumberOfVar(actividades.size());
	setNumberOfObj(2);
	
	return true;
}

// Leemos la instancia del fichero
int PlanningRouteTenerife::readFile(const string filename) {
	string cadena;
	string cadena2 = "";
	int noNumero = 0;
	int posicion = 0;
	int actividad = -1;
	int primera = 0;
	int numDias = 1;
	int numHoras;
	int numero;
  int vez = 0;
	vector <int> pru;
	
	
	ifstream f(filename.c_str());
	if (!f)
	{
	  cout << "Error al abrir el fichero" << endl;
	  return -1;
	} else {
           f >> cadena;
	   //guardamos numero de días numDias = cadena;
           f >> cadena;
	   sscanf(cadena.c_str(),"%d",&numero);
	   numHoras = numero;
	   tiempoTotal = numHoras * numDias; 
           while (!f.eof())
           {
              f >> cadena;
	      
	      for (int i = 0; i < cadena.length(); i++)
		if (isdigit(cadena[i]) != 1) {
		   noNumero = 1;
		   i = cadena.length();
		}
	      if (noNumero != 0) {
		if (primera == 0) {
		  primera = 1;
		  actividad = actividad + 1;
		}
		cadena2 = cadena2 + " " + cadena;
		posicion = 0;
	      }
	      else {
		if (posicion == 0) {
		   if (vez != 0) {
		      distancias.push_back(pru);
		      vez++;
		      pru.clear();
		   }
		   cout << "actividad numero: " << actividad << endl;
		   cout << "nombre actividad" << cadena2 << endl;
		   actividades.push_back(cadena2);
		   cadena2 = "";
		   primera = 0;
		   sscanf(cadena.c_str(),"%d",&numero); 
		   costes.push_back(numero);
		   cout << "guardamos el precio" << cadena << endl;
		}
		else if (posicion == 1) {
		   sscanf(cadena.c_str(),"%d",&numero);
		   duracion.push_back(numero);
	           cout << "guardamos la duracion" << cadena << endl;
		}  
		else {
		  sscanf(cadena.c_str(),"%d",&numero);
		  pru.push_back(numero);
		  vez = vez+1;
		  cout << "guardamos las distancias desde i hacia los demas" << cadena << endl;
		}
		posicion++;
	      }	
	      noNumero = 0;
           }
           distancias.push_back(pru);
	}
}

// Genetic Operators - Binary Mutation
void PlanningRouteTenerife::dependentMutation(double pm) {
	mutate_binary(pm);
	repair();
}

// Genetic Operators - Uniform Crossover 
void PlanningRouteTenerife::dependentCrossover(Individual* ind) {
	if (type_crossover == CROSSOVER_UNIFORM) {
		crossover_uniform(ind);
	}
	//else if (type_crossover == CROSSOVER_HUX){
	//	crossover_HUX(ind);
	//} 
	else {
		cout<<"Error en el tipo de Crossover. type_crossover = 0 -> crossover_uniforme. type_crossover = 1 -> crossover_hux"<<endl;
	}
      
	((PlanningRouteTenerife*)ind)->repair();
	repair();
}


/*void PlanningRouteTenerife::crossover_HUX (Individual *ind) {
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

}*/

// Generamos un individuo aleatoramiente
void PlanningRouteTenerife::restart(void) {
	int numero;
  for (int i = 0; i < getNumberOfVar(); i++) {
	  numero = (rand() % 2);
		setVar(i, numero);
	}
	repair();
}

// Reparamos el individuo si se pasa de la restricción de tiempo
void PlanningRouteTenerife::repair(void) {
	int numero;
	while (!checkTiempo()) {
	  numero = (rand() % getNumberOfVar());
		if (getVar(numero) != 0)
		  setVar(numero, 0);
	}
}

// Calculates the max profit/weight ratio for each item in increasing order
/*void PlanningRouteTenerife::ratios(void) {
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
void PlanningRouteTenerife::quicksort(int *deleteOrder, float *maxRatio, int begin, int end) {
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
}*/

// Chequeamos la restricción de tiempo
bool PlanningRouteTenerife::checkTiempo(void) {
  int tiempo = 0;
	for (int i = 0; i < getNumberOfVar(); i++) {
		if (getVar(i) == 1)
		  tiempo = tiempo + duracion[i];
  }
	// faltaria lo de la distancia de ese punto al resto
	if (tiempo > tiempoTotal)
		return false;
	return true;
}

// Evaluamos el individuo
void PlanningRouteTenerife::evaluate (void) {
  double coste = 0;
  int numActividades = 0;

	for (int i = 0; i < getNumberOfVar(); i++) {
		if (getVar(i) != 0) {
		  coste = coste + costes[i];
		  numActividades++;
		}
	}

	setObj(0, coste);
	setObj(1, numActividades / (float)getNumberOfVar() * 100);
}

Individual* PlanningRouteTenerife::clone(void) const {
	return new PlanningRouteTenerife();
}

/*int main(int argc, char *argv[])
{
        return 0;
}*/
