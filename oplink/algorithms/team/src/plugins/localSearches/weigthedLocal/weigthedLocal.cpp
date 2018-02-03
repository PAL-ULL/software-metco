#include "weigthedLocal.h"
#include <string.h>


/********************************************************************************** 
  Funcion que se encarga de realizar una busqueda local sobre la poblacion 
  resultante al aplicar previamente un algoritmo evolutivo.
  El algoritmo consiste en:
  1) Calcular la funcion mono-objetivo de la suma de pesos, para los individuos 
     de la poblacion.
  2) Obtener todos los vecinos para cada uno de los individuos de la poblacion.
  3) Calcular la funcion mono-objetivo de cada uno de los vecinos.
  4) Mientras existan vecinos que explorar:
    4.1) Si la funcion mono-objetivo de algun vecino es mejor que la del individuo
         de la poblacion -> Guardar el vecino.
  5) Si se ha encontrado un vecino que optimice mejor la funcion mono-objetivo.
    5.1) Eliminar el individuo de la poblacion.
    5.2) Aniadir el vecino a la poblacion.
*************************************************************************************/  
void weigthedLocal::localSearch(vector <Individual*> *poblacion, MOFront *currentSolution){

	// Probabilidad de que se realice busqueda local. 
	double rLS = (double(rand()))/(RAND_MAX + 1.0);
	if (pLS <= rLS)
		return;   //No se realiza la busqueda local
	
	initialization(poblacion);
	//Vector que indica si los objetivos son de minimizar o maximizar
	vector <int> Objs;
	for (unsigned int j = 0; j < (*poblacion)[0]->getNumberOfObj(); j++){
		Objs.push_back((*poblacion)[0]->getInternalOptDirection(j));  // 1 -> El objetivo es a maximizar
	}

	// Dibujar el grafico
	ofstream salida("Posiciones.dat",ios::out);
	for (unsigned int i = 0; i < poblacion->size(); i++) {
		for (unsigned int j = 0; j < (*poblacion)[i]->getNumberOfObj(); j++) {
			salida<<(*poblacion)[i]->getObj(j)<<" ";
		}
		salida<<endl;
	}
	
	impresion = true;
	cout<<"------------------------"<<endl;
	for (unsigned int i = 0; i < poblacion->size(); i++) {
		double fitnessp = calculate_F((*poblacion)[i], Objs);
	}
	impresion = false;


		
	vector<double> objectives((*poblacion)[0]->getNumberOfObj(),0); 	
	for (unsigned int ind = 0; ind < poblacion->size(); ind ++){	
		bool changed = true;
		int iterations = 0;
		int bestNeighbour = 0;
	
		while ((changed) && (iterations <= maxIterations)) {
			changed = false;
			iterations++;
			//Calcular la funcion de fitnes del individuo de la poblacion
			double currentFitness = calculate_F((*poblacion)[ind],Objs);
			// Obtener los vecinos del individuo
			for (unsigned int i = 0; i < (*poblacion)[ind]->getNeighbourhoodSize(); i++){
				(*poblacion)[ind]->getNeighbourObjectives(i,objectives);
				double tmpFitness = calculate_F(objectives,Objs);
				if (tmpFitness < currentFitness){  //Minimizacion de la funcion de fitness
					currentFitness = tmpFitness;
					bestNeighbour = i;
					changed = true;
				}
			}
			if (changed){
				Individual *tmp = (*poblacion)[ind]->getNeighbour(bestNeighbour);
				if (currentSolution != NULL){
					currentSolution->insert(tmp);
				}
				delete (*poblacion)[ind];
				(*poblacion)[ind] = tmp;
			}
		}
	}
}


/*******************************************************************************
  Procedimiento de inicializacion de las constantes de los valores maximo y
  minimo de los objetivos del problema.
  f0_max y f0_mim valores maximo y minimo del objetivo 0 -> Bs usadas
  f1_max y f1_mim valores maximo y minimo del objetivo 1 -> Coverage
******************************************************************************/
void weigthedLocal::initialization(vector <Individual*> *poblacion){

	maximos.clear();
	minimos.clear();
	maximos.insert(maximos.begin(),(*poblacion)[0]->getNumberOfObj(),0);
	minimos.insert(minimos.begin(),(*poblacion)[0]->getNumberOfObj(),0);
	for (unsigned int i = 0; i < (*poblacion)[0]->getNumberOfObj(); i++){
		//Calculo de los valores maximo y minimo de los objetivos
		calculate_fmax(poblacion,i);
		calculate_fmim(poblacion,i);
	}
}


/******************************************************************************
  Procedimiento que se encarga de calcular el valor maximo de cada una de las 
  funciones objetivos del problema.
  Se recorren todos los inviduos de la poblacion. Observando para cada uno de 
  ellos el valor de las funciones objetivos, buscando el valor mas alto de las
  mismas.
******************************************************************************/
void weigthedLocal::calculate_fmax(vector <Individual*> *poblacion, unsigned int i){

	// f_max = valor maximo objetivo i 
	double f_max =  (*poblacion)[0]->getObj(i); //Obtener valor objetivo i
	for (unsigned int ind = 1; ind < (*poblacion).size(); ind++){
		double f = (*poblacion)[ind]->getObj(i);
		//Comprobaciones para obtener el mejor valor
		if (f_max < f){
			f_max = f;
		}
	}
	maximos[i] = f_max;
}



/******************************************************************************
  Procedimiento que se encarga de calcular el valor minimo de cada una de las 
  funciones objetivos del problema.
  Se recorren todos los inviduos de la poblacion. Observando para cada uno de 
  ellos el valor de las funciones objetivos, buscando el valor mas bajo de las
  mismas.
******************************************************************************/
void weigthedLocal::calculate_fmim(vector <Individual*> *poblacion, unsigned int i){

	// f_mim = valor minimo objetivo i
	double f_mim =  (*poblacion)[0]->getObj(i); //Obtener valor objetivo 0 del individuo0
	for (unsigned int ind = 1; ind < (*poblacion).size(); ind++){
		double f = (*poblacion)[ind]->getObj(i);
		//Comprobaciones para obtener el mejor valor
		if (f_mim > f){
			f_mim = f;
		}
	}
	minimos[i] = f_mim;
}



/******************************************************************************
  Procedimiento que se encarga de realizar el calculo del peso para la
  funcion objetivo de minimizar.
 
  Si el objetivo va a hacer minimizado:
 
                   (f_max_j - f_j(x)) / (f_max_j - f_min_j)
       w(x)_j = ---------------------------------------------
                 E ((f_max_k - f_k(x)) / (f_max_k - f_min_k))
 
  donde E va desde k= 1..M
 
******************************************************************************/ 
double weigthedLocal::calculate_weigthedMim(Individual* ind, unsigned int i){

	// Obtener el valor maximo del objetivo
	double max = maximos[i];
	//Obtener el valor minimo del objetivo
	double mim = minimos[i];

	// Calculo de la formula
	//Contemplar el caso en el que max = mim
	double div;
	if (max == mim){
		div = 1.0;
	}else{
		div = max - mim;
	}
		
	double numerator = ((max - ind->getObj(i)) / div);
	double denominator = 0.0;
	// Calculo del denominador
	for (unsigned int j = 0; j < ind->getNumberOfObj(); j++) {
		if (maximos[j] == minimos[j]){
			div = 1.0;
		}else{
			div = maximos[j] - minimos[j];
		}
		double sum = ((maximos[j] - ind->getObj(j)) / div);
		denominator += sum;
	}
	// Comprobaciones de NaN e INF
	if (denominator == 0){
		denominator = 1;
	}
	return (numerator / denominator);
}



double weigthedLocal::calculate_weigthedMim(unsigned int i, vector<double> &objectives){

	// Obtener el valor maximo del objetivo
	double max = maximos[i];
	//Obtener el valor minimo del objetivo
	double mim = minimos[i];

	// Calculo de la formula
	//Contemplar el caso en el que max = mim
	double div;
	if (max == mim){
		div = 1.0;
	}else{
		div = max - mim;
	}
		
	double numerator = ((max - objectives[i]) / div);
	double denominator = 0.0;
	// Calculo del denominador
	for (unsigned int j = 0; j < objectives.size(); j++) {
		if (maximos[j] == minimos[j]){
			div = 1.0;
		}else{
			div = maximos[j] - minimos[j];
		}
		double sum = ((maximos[j] - objectives[j]) / div);
		denominator += sum;
	}
	if (denominator == 0)
		denominator = 1;
	return (numerator / denominator);
}



/******************************************************************************
  Procedimiento que se encarga de realizar el calculo del peso para la 
  funcion objetivo de maximizar.
 
  Si el objetivo va a hacer minimizado:
 
                   (f_j(x) - f_min_j) / (f_max_j - f_min_j)
       w(x)_j = ---------------------------------------------
                 E ((f_max_k - f_k(x)) / (f_max_k - f_min_k))
 
  donde E va desde k= 1..M
 
******************************************************************************/ 
double weigthedLocal::calculate_weigthedMax(Individual* ind, unsigned int i){

	
	// Obtener el valor maximo del objetivo 	
	double max = maximos[i];	
	//Obtener el valor minimo del objetivo
	double mim = minimos[i];

	// Calculo de la formula
	//Contemplar el caso en el que max = mim
	double div;
	if (max == mim){
		div = 1.0;
	}else{
		div = max - mim;
	}
	double numerator = ((ind->getObj(i) - mim) / div);
	// Calculo del denominador
	double denominator = 0.0;
	for (unsigned int j = 0; j < ind->getNumberOfObj(); j++){
		if (maximos[j] == minimos[j]){
			div = 1.0;
		}else{
			div = maximos[j] - minimos[j];
		}
	
		double sum = ((maximos[j] - ind->getObj(j)) / div);
		denominator += sum;
	}
	if (denominator == 0)
		denominator = 1;
	return (numerator / denominator);
}



double weigthedLocal::calculate_weigthedMax(unsigned int i, vector<double> &objectives){

	
	// Obtener el valor maximo del objetivo 	
	double max = maximos[i];
	//Obtener el valor minimo del objetivo
	double mim = minimos[i];

	// Calculo de la formula
	//Contemplar el caso en el que max = mim
	double div;
	if (max == mim){
		div = 1.0;
	}else{
		div = max - mim;
	}
	double numerator = ((objectives[i] - mim) / div);
	// Calculo del denominador
	double denominator = 0.0;
	for (unsigned int j = 0; j < objectives.size(); j++){
		if (maximos[j] == minimos[j]){
			div = 1.0;
		}else{
			div = maximos[j] - minimos[j];
		}
	
		double sum = ((maximos[j] - objectives[j]) / div);
		denominator += sum;
	}
	if (denominator == 0)
		denominator = 1;
	return (numerator / denominator);
}



/***********************************************************
  Procedimiento que calcula la funcion F(x) ha optimizar
 
      F(x) = E w(x)_j * f_j(x)
 
   donde E va desde j = 1..M
   Maximo * w
   Minimo * -w
**********************************************************/
double weigthedLocal::calculate_F(Individual* ind,vector<int> &Objs){

	vector <double> weights;  // Vector de pesos
	double fitness;

	for (int i = 0; i < Objs.size(); i ++) {
		if (Objs[i] == MINIMIZE) {   //Minimizar
			double weigth_ind = calculate_weigthedMim(ind,i);
			weights.push_back(-weigth_ind);
               	} else {
			// Si la funcion objetivo es de maximizar
               		double weigth_ind = calculate_weigthedMax(ind,i);
               		weights.push_back(weigth_ind);
           	}
	}
	
	for (unsigned int j = 0; j < ind->getNumberOfObj(); j++){
		fitness += weights[j] * ind->getObj(j);
	}
	
	// Impresion de las flechas
	if (impresion){
		//Impresion
		double raiz = sqrt((weights[0]*weights[0]) + (weights[1]*weights[1]));
		double wc0 = (weights[0] / raiz) * 3;
		double wc1 = (weights[1] / raiz) * 3;

		cout<<"set arrow from "<<ind->getObj(0)<<","<<ind->getObj(1)<<" to "<<ind->getObj(0) + wc0<<","<<ind->getObj(1) + wc1<<endl;
	}

	return fitness;
}

double weigthedLocal::calculate_F(vector<double> &objectives, vector<int> &Objs){

	vector <double> weights;  // Vector de pesos
	double fitness;

	for (int i = 0; i < Objs.size(); i ++) {
		if (Objs[i] == 0) {   //Minimizar
			double weigth_ind = calculate_weigthedMim(i,objectives);
			weights.push_back(-weigth_ind);
               	} else {
			// Si la funcion objetivo es de maximizar
               		double weigth_ind = calculate_weigthedMax(i,objectives);
               		weights.push_back(weigth_ind);
           	}
	}
	for (unsigned int j = 0; j < objectives.size(); j++){
		fitness += weights[j] * objectives[j];
	}
	return fitness;
}

bool weigthedLocal::init(const vector<string> &params){

	if (params.size() != 2){
		cout << "Error, WeigthedLocal parameters --> MaxIterations pLS" << endl;
		return false;                           
	}
	maxIterations = atoi(params[0].c_str());
	if (maxIterations < 1){
		cout << "Error, maxIterations < 1" << endl;
		return false;
	}
	pLS = atof(params[1].c_str());
	if ((pLS < 0) || (pLS > 1)){
		cout << "Error, pLS must be in [0, 1]" << endl;
	}
	return true;
}
