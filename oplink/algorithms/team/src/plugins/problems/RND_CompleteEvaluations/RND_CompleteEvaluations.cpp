#include "RND_CompleteEvaluations.h"
#include <libgen.h>
#include <string.h>


// Problema a resolver
ProblemRND_CompleteEvaluations *RND_CompleteEvaluations::problem = NULL;	
map< int, vector< vector <int> > > RND_CompleteEvaluations::bsRegion;
vector< vector< vector< vector <int> > > > RND_CompleteEvaluations::RegionBs;
vector< vector<int> > RND_CompleteEvaluations::posiciones;
string RND_CompleteEvaluations::instanceFile;
unsigned int RND_CompleteEvaluations::type_instance;
vector<string> RND_CompleteEvaluations::crossParamNames;
vector<string> RND_CompleteEvaluations::crossParamValues;

//Destructor
RND_CompleteEvaluations::~RND_CompleteEvaluations(){
	for (unsigned int i = 0; i < grid.size(); i++)
		grid[i].clear();
	grid.clear();
}

// Individual Initialization
bool RND_CompleteEvaluations::init(const vector<string> &params) {
	// Check for the problem parameters
	if (params.size() < 1){
		cout << "Error problem RND_CompleteEvaluations: type_fich other_parameters" << endl;
	}
		
	int currentArg = 0;
	if (atoi(params[0].c_str()) == 1){
		if (params.size() < 5) {
			cout << "Error problem RND_CompleteEvaluations: type_fich fich_dim fich_inst radio type_crossover Other_parameters" << endl;
			return false;
		}
		currentArg = 4;
	} else if (atoi(params[0].c_str()) == 2){
		if (params.size() < 3){
			cout << "Error problem RND_CompleteEvaluations: type_fich fich_inst type_crossover Other_parameters" << endl;
			return false;
		}
		currentArg = 2; 
	}

	//Check parameters
	type_crossover = atoi(params[currentArg].c_str());
	if ((type_crossover == CROSSOVER_OPC) && (params.size() != currentArg + 1)){//Crossover OPC
		cout << "Error problem RND_CompleteEvaluations with crossover OPC: type_fich fich_dim fich_inst radio type_crossover" << endl;
		return false;
	} else if ((type_crossover == CROSSOVER_GEOGRAPHIC_CIRCULAR) && (params.size() != currentArg + 2)){//Crossover geographic Circular
		cout << "Error problem RND_CompleteEvaluations with crossover Geographic Circular: type_fich fich_dim fich_inst radio type_crossover radio" << endl;
		return false;
	} else if ((type_crossover == CROSSOVER_GEOGRAPHIC_RECTANGULAR) && (params.size() != currentArg + 4)){
		cout << "Error problem RND_CompleteEvaluations with crossover Geographic Rectangular: type_fich fich_dim fich_inst radio type_crossover type_file Min_dim Max_dim" << endl;
		return false;
	} else if ((type_crossover == CROSSOVER_HUX) && (params.size() != currentArg + 1)){//Crossover HUX
		cout << "Error problem RND_CompleteEvaluations with crossover HUX: type_fich fich_dim fich_inst radio type_crossover" << endl;
		return false;
	} 
	if ((type_crossover != CROSSOVER_OPC) && (type_crossover != CROSSOVER_GEOGRAPHIC_CIRCULAR) && (type_crossover != CROSSOVER_GEOGRAPHIC_RECTANGULAR) && (type_crossover != CROSSOVER_HUX)){
		cout << "Error problem RND_CompleteEvaluations with unknown crossover id" << endl;
		return false;
	}

  if (problem == NULL){//only initialize once the problem (and the info about the parameters)
		if (type_crossover == CROSSOVER_GEOGRAPHIC_CIRCULAR) {   
			crossParamNames.push_back("Radio");
			crossParamValues.push_back(params[currentArg+1]);
		} else if (type_crossover == CROSSOVER_GEOGRAPHIC_RECTANGULAR){ 
			crossParamNames.push_back("dim_min");
			crossParamValues.push_back(params[currentArg+2]);
			crossParamNames.push_back("dim_max");
			crossParamValues.push_back(params[currentArg+3]);
		}
		if (atoi(params[0].c_str()) == 1){
			problem = new ProblemRND_CompleteEvaluations(params[1],params[2], atoi(params[3].c_str()));
	 		char *tmpBaseFile = new char[params[2].size() + 1];
			strcpy(tmpBaseFile, params[2].c_str());
 		 	instanceFile = basename(tmpBaseFile); 
			delete tmpBaseFile;
		} else if (atoi(params[0].c_str()) == 2){
			problem = new ProblemRND_CompleteEvaluations(params[1]);
	 		char *tmpBaseFile = new char[params[1].size() + 1];
			strcpy(tmpBaseFile, params[1].c_str());
 		 	instanceFile = basename(tmpBaseFile); 
			delete tmpBaseFile;
		} else {
			cout << "Error problem RND_CompleteEvaluations: incorrect type_fich" << endl;
		}
	}

	//Initialize structures for crossover if neccesary
	if (type_crossover == CROSSOVER_GEOGRAPHIC_CIRCULAR) {
		radio_region = atoi(params[currentArg+1].c_str());
		calculateRegion();
	} else if (type_crossover == CROSSOVER_GEOGRAPHIC_RECTANGULAR){
		type_instance = atoi((params[currentArg+1]).c_str());
		dim_min = atoi((params[currentArg+2]).c_str());
		dim_max = atoi((params[currentArg+3]).c_str());
		if (type_instance == 1){
			calculateRegionRect();
		}
	}

	// Inicializacion de la solucion y variables auxiliares
	coveredPositions = 0;
	grid.insert(grid.begin(), problem->getGridSizeX(), vector<int>(problem->getGridSizeY(), 0));
	//Inicializacion de las posiciones de las antenas que son usadas
	posiciones.insert(posiciones.begin(), problem->getGridSizeX(), vector<int>(problem->getGridSizeY(),-1));
	// Calculo del vector posiciones
	for (unsigned int bs = 0; bs < problem->getBsLocationsSize(); bs++){
		unsigned int x = problem->getbsLocationsX(bs);
		unsigned int y = problem->getbsLocationsY(bs);
		(posiciones[x][y]) = bs;
	}

	//Fijar el numero de variables
	setNumberOfVar(problem->getBsLocationsSize());
	//Fijar el numero de objetivos
	setNumberOfObj(2);
	return true;
}

void RND_CompleteEvaluations::restart(){

	//Read from file /tmp/test.txt
/*	ifstream f("/tmp/test.txt");	
	for (unsigned int i = 0; i < problem->getBsLocationsSize(); i++) {
		int valor;
		f >> valor;
		setVar(i, valor);
	}
	coveredPositions = problem->calculateCoverage(this, usedBs,&grid);
	setObj(0, usedBs);
	updateCoverage();
	cout << getObj(0) << " " << getObj(1) << " " coveredPosition << endl;
	exit(-1);*/
	
	//Inicializacion del gen inicial y del vector de solucion
	for (unsigned int i = 0; i < problem->getBsLocationsSize(); i++) {
		int valor = int(rand()%2);
		setVar(i,valor);
	}

	unsigned int usedBs;
	coveredPositions = problem->calculateCoverage(this, usedBs,&grid);
	setObj(0, usedBs);
	updateCoverage();
}

void RND_CompleteEvaluations::evaluate() {
   unsigned int usedBs;
   grid.clear();
   grid.insert(grid.begin(), problem->getGridSizeX(), vector<int>(problem->getGridSizeY(), 0));
   coveredPositions = problem->calculateCoverage(this, usedBs,&grid);
   setObj(0, usedBs);
   updateCoverage();
}



// Fijar si el objetivo se debe maximizar o minimizar (indice i)
unsigned int RND_CompleteEvaluations::getOptDirection(const int i) const{ 

	if (i == 0) 
		return MINIMIZE;
	else
		return MAXIMIZE;
}

// Clonacion
Individual *RND_CompleteEvaluations::clone() const{
	
	RND_CompleteEvaluations *nuevo = new RND_CompleteEvaluations();
	nuevo->grid = grid;

	nuevo->type_crossover = type_crossover;
	nuevo->radio_region = radio_region;
	nuevo->dim_min = dim_min;
	nuevo->dim_max = dim_max;
	nuevo->coveredPositions = coveredPositions;
	return nuevo; 
}

// Funcion que recorre todas las antenas y va recalculando la grid
void RND_CompleteEvaluations::deserialized() {

	coveredPositions = 0;
	for (unsigned int bs = 0; bs < problem->getBsLocationsSize(); bs++){
		if ((int)getVar(bs) == 1) { // La antena se esta usando->recalcular la grid
			for (unsigned int i = 0; i < problem->getCoveredPoints(bs); i++) {
				Position pos = problem->getCoveredPoint(bs,i);
				(grid[pos.getX()][pos.getY()])++;
				if (grid[pos.getX()][pos.getY()] == 1){
					coveredPositions++;
				}
			}
		}
	}
}


void RND_CompleteEvaluations::calculateRegion() {
	if (bsRegion.count(radio_region) != 0){
		return;
	}
	//cout << "Va a calcular la region: " << radio_region << endl;
	// Inicializar la matriz de puntos cubiertos por cada antena
	bsRegion[radio_region].resize(problem->getBsLocationsSize());
	unsigned int distance;

	// Calcular el numero de puntos que cubre cada antena
	for (unsigned int i = 0; i < problem->getBsLocationsSize(); i++) {
		for (unsigned int j = i; j < problem->getBsLocationsSize(); j++){
			//Se mira si la distancia entre ambas es menor que el radio
			unsigned int x1 = problem->getbsLocationsX(i);
			unsigned int y1 = problem->getbsLocationsY(i);
			unsigned int x2 = problem->getbsLocationsX(j);
			unsigned int y2 = problem->getbsLocationsY(j);
			distance = (int)sqrt( ((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)) );
			if (distance < radio_region){
				bsRegion[radio_region][i].push_back(j);
				bsRegion[radio_region][j].push_back(i);
			}
		}
	}
}


/****************** Calculo de las regiones con forma rectangular *******/
void RND_CompleteEvaluations::calculateRegionRect(){
	cout << "Warning: codigo incompatible con la version paralela" << endl;
	//Comprobaciones
	for (unsigned int a = dim_min; a <= dim_max; a++){
		RegionBs.insert(RegionBs.begin(), dim_max + 1, vector< vector< vector<int> > >());
		RegionBs.push_back(vector< vector< vector<int> > >());
		double alt = a/2.0;
		for (unsigned int b = dim_min; b <= dim_max ; b++){
			RegionBs[a].insert(RegionBs[a].begin(), dim_max, vector< vector<int > >());
			RegionBs[a].push_back(vector< vector<int> >());
			double anch = b/2.0;
			for (unsigned int i = 0; i < problem->getBsLocationsSize(); i++){
				RegionBs[a][b].insert(RegionBs[a][b].begin(), problem->getBsLocationsSize(), vector<int>());
				RegionBs[a][b].push_back(vector<int>());
				for (unsigned int j = i; j < problem->getBsLocationsSize(); j++){
					RegionBs[a][b].insert(RegionBs[a][b].begin(), problem->getBsLocationsSize(), vector<int>());
					RegionBs[a][b].push_back(vector<int>());
					//Se mira si la distancia entre ambas es menor que el radio
					unsigned int x1 = problem->getbsLocationsX(i);
					unsigned int y1 = problem->getbsLocationsY(i);
					unsigned int x2 = problem->getbsLocationsX(j);
					unsigned int y2 = problem->getbsLocationsY(j);
					double alt = 1/2.0; 
					if (((x2 <= x1+anch) && (x2 >= x1-anch)) && ((y2 <= y1+alt) && (y2 >= y1-alt))){
						RegionBs[a][b][i].push_back(j);
						RegionBs[a][b][j].push_back(i);
					}
				}
			}
		}
	}
}



/*********************** Busqueda local (Vecindad 1) ***************************/

// Función que devuelve el numero maximo de vecinos. En el caso de
// vecindad uno, hay tantos vecinos como localizaciones.
unsigned int RND_CompleteEvaluations::getNeighbourhoodSize() const {

	// Numero maximo de vecinos = numero total de ubicaciones
	return(problem->getBsLocationsSize());
}


// Funcion que devuelve al vecino i. Si la antena se encuentra a
// uno, esta sera modificada al nuevo valor cero, y lo mismo, si
// la antena esta a cero, entonces se modificara a valor uno
Individual *RND_CompleteEvaluations::getNeighbour(const int i) const {

	// Crear un nuevo individuo para trabajar sobre el
	RND_CompleteEvaluations *newind = (RND_CompleteEvaluations*)internalClone();
	if ((unsigned int)i < problem->getBsLocationsSize()) {
		if ((double)newind->getVar(i) == 0)
			newind->addBs(i);
		else
			newind->removeBs(i);
	}
	newind->updateCoverage();
	return(newind);
}


// Funcion que calcula los objetivos del vecino i que se le pasa. Y 
// devuelve un vector con el valor de los objetivos de ese vecino
// El vector objectives está inicializado a el numero de objetivo
void RND_CompleteEvaluations::getNeighbourObjectives(const int i, vector<double> &objectives) const {
	
	double obj1;
	int valorgrid;
	
	if ((int)getVar(i) == 1){ //Si la antena esta a uno
		objectives[0] = getObj(0) - 1;  // Numero de antenas
		// Se recalcula el cubrimiento
		double obj = getObj(1);  // Valor del objectivo 1
		for (unsigned int j = 0; j < problem->getCoveredPoints(i); j++) {
			Position pos = problem->getCoveredPoint(i, j);
			valorgrid = (grid[pos.getX()][pos.getY()]) - 1;
		        
			if (valorgrid == 0){
				obj1 = obj - 1;
				obj = obj1;
			}
		}	
		objectives[1] = obj;  // Cobertura
	}
	else {
		objectives[0] = getObj(0) + 1;  // Numero de antenas
		// Se recalcula el cubrimiento
		double obj = getObj(1);
		for (unsigned int j = 0; j < problem->getCoveredPoints(i); j++) {
			Position pos = problem->getCoveredPoint(i, j);
			valorgrid = (grid[pos.getX()][pos.getY()]) + 1;
			if (valorgrid == 1){
				obj1 = obj + 1;
				obj = obj1;
			}
		}	
		objectives[1] = obj;  // Cobertura
	}
	/*for (int i = 0; i < objectives.size();i++){
		cout<<"RND "<<objectives[i]<<endl;
	}*/
}

/****************************************************************************/


/****************** Elimininar/Incluir antenas *******************/

// Elimina de la solucion la antena con identificador bsId
void RND_CompleteEvaluations::removeBs(const unsigned int bsId) {

	// Se elimina la antena bsId de la solucion
	setVar(bsId, 0);
	setObj(0, getObj(0)-1);
	// Se recalcula el cubrimiento
	for (unsigned int i = 0; i < problem->getCoveredPoints(bsId); i++) {
		Position pos = problem->getCoveredPoint(bsId, i);
		(grid[pos.getX()][pos.getY()])--;
		if (grid[pos.getX()][pos.getY()] == 0){
			coveredPositions--;
		}
	}
}


// Incluye en la solucion la antena con identificador bsId
void RND_CompleteEvaluations::addBs(const unsigned int bsId) {
	// Se incluye en la solucion la antena bsId 
	setVar(bsId, 1);
	setObj(0, getObj(0)+1);
	// Se recalcula el cubrimiento
	for (unsigned int i = 0; i < problem->getCoveredPoints(bsId); i++) {
		Position pos = problem->getCoveredPoint(bsId, i);
		(grid[pos.getX()][pos.getY()])++;
		if (grid[pos.getX()][pos.getY()] == 1){
			coveredPositions++;
		}
	}
}


/**************** Variaciones ****************/

// Genetic Operators - Binary Mutation
void RND_CompleteEvaluations::dependentMutation(double pm) {

	int nvars = getNumberOfVar();
 	for (unsigned int i = 0; i < nvars; i++) {
		double vmut = ((double) rand()) / (double) (RAND_MAX);
		if (vmut < pm) {
			if ((int)getVar(i) == 0){ 
				addBs(i);// Incluir una antena mas a la solucion final, e incrementar la grid
			}
			else { 
				removeBs(i);// Eliminar una antena de la solucion final, y decrementar la grid
			}
		}
	}

	updateCoverage();
}


// Genetic Operators - Crossover
// Se parten ambos individuos por el mismo punto de corte, generado �ste
// a partir de un numero aleatorio. Una vez se tiene el punto de corte
// se hace que la parte traceada mayor del individuo 1 se une con la
// con la parte troceada menor del individuo 2. Y lo mismo ocurre con 
// el individuo dos, se coge su parte troceada mayor y se une con la 
// parte troceada menor del individuo 1 
void RND_CompleteEvaluations::crossover_opc(Individual* ind) {
	int nvars = getNumberOfVar();
	int vcross = int((double)(rand())/(RAND_MAX + 1.0)*nvars); 

	if (vcross >= (nvars/2)) {
  		// Union del primer trozo del individuo 1 con el segundo trozo 
		// individuo 2
		for (unsigned int i = (vcross+1); i < nvars; i++) {
			double tmp1 = ind->getVar(i); 
 			double tmp2 = getVar(i);
			if ((tmp2 == 1) && (tmp1 == 0)){
				((RND_CompleteEvaluations*)(ind))->addBs(i);
				removeBs(i);
			} else if ((tmp2 == 0) && (tmp1 == 1)){
				((RND_CompleteEvaluations*)(ind))->removeBs(i);
				addBs(i);
			}
		}
	}
 	else {
	  	// Union del segundo trozo del individuo 1 con el primer trozo
      		// individuo 2
      		for (unsigned int i = 0; i < vcross; i++) {
      			double tmp1 = ind->getVar(i); 
      			double tmp2 = getVar(i);
			if ((tmp2 == 1) && (tmp1 == 0)){
				((RND_CompleteEvaluations*)(ind))->addBs(i);
				removeBs(i);
			} else if ((tmp2 == 0) && (tmp1 == 1)){
				((RND_CompleteEvaluations*)(ind))->removeBs(i);
				addBs(i);
			}
      		}
	}
}


//
void RND_CompleteEvaluations::crossover_geographic_circular(Individual* ind) {
	int nvars = getNumberOfVar();
	//cout << "Hace cross con radio: " << radio_region << endl;
	//Obtener una region del terreno de forma aleatoria
	int bs_geographic = (int) ((double)(nvars)*rand()/(RAND_MAX+1.0));
	for (unsigned int i = 0; i < bsRegion[radio_region][bs_geographic].size(); i++) {
		double tmp1 = ind->getVar(bsRegion[radio_region][bs_geographic][i]);
		double tmp2 = getVar(bsRegion[radio_region][bs_geographic][i]);
		if ((tmp2 == 1) && (tmp1 == 0)){
			((RND_CompleteEvaluations*)(ind))->addBs(bsRegion[radio_region][bs_geographic][i]);
			removeBs(bsRegion[radio_region][bs_geographic][i]);
		} else if ((tmp2 == 0) && (tmp1 == 1)){
			((RND_CompleteEvaluations*)(ind))->removeBs(bsRegion[radio_region][bs_geographic][i]);
			addBs(bsRegion[radio_region][bs_geographic][i]);
		}
	}
}

void RND_CompleteEvaluations::crossover_geographic_rectangular(Individual* ind){

	int nvars = getNumberOfVar();
	//Obtener el punto de la region de forma aleatoria
	int bs_geographic = (int) ((double) (nvars)*rand()/(RAND_MAX+1.0));
	//Obtener las dimensiones del rectangulo (a -> altura b -> base)
	int a = dim_min + (int)((double)((dim_max-dim_min)+1.0)*rand()/(RAND_MAX+1.0));
	int b = dim_min + (int)((double)((dim_max-dim_min)+1.0)*rand()/(RAND_MAX+1.0));

	if (type_instance == 0) {//Tabla no calculada
		antenas = vector<int>();
		// Punto central
		unsigned int x1 = problem->getbsLocationsX(bs_geographic);
		unsigned int y1 = problem->getbsLocationsY(bs_geographic);
		// Calculo de las dimensiones del rectangulo
		int alt = a/2;
		int anch = b/2;
		//Calculo del numero de puntos que caen dentro del area cubierta 
		//por el rectangulo para la estacion base
		int dimension = alt*anch;
		
		// Opcion1
		// Si dimension es menor que el numero de antenas totales
		if (dimension < nvars){
			//Limites superior e inferior del rectangulo
			int inita = x1 - anch;
			int enda = x1 + anch;
			int initb = y1 - alt;
			int endb = y1 + alt;
			// Comprobaciones, para que no se salgan fuera del tamano de la grid
			if (inita < 0) {
				inita = 0;
			}
			if (initb < 0) {
				initb = 0;
			}
			if (enda > (problem->getGridSizeX()-1)) {  
				enda = problem->getGridSizeX() - 1;
			}
			if (endb > (problem->getGridSizeY()-1) ) { 
				endb = problem->getGridSizeY() - 1;
			}
			for (int dim_a = inita; dim_a <= enda; dim_a++){
				for (int dim_b = initb; dim_b <= endb; dim_b++){
					if (posiciones[dim_a][dim_b] != -1){
						antenas.push_back(posiciones[dim_a][dim_b]);
					}
				}
			}
		} else {
			// Opcion2
			int cont = 0;
			// Si la dimension es mayor que el numero de antenas totales. 	
			for (unsigned int p = 0; p < problem->getBsLocationsSize(); p++){
				unsigned int x2 = problem->getbsLocationsX(p);
				unsigned int y2 = problem->getbsLocationsY(p);
				if (((x2 <= x1+anch) && (x2 + anch >= x1)) && ((y2 <= y1+alt) && (y2 + alt >= y1))){
					antenas.push_back(p);
				}
			}
		}
	}
	
	// Crossover
	for (unsigned int i = 0; i < antenas.size(); i++){
		double tmp1 = ind->getVar(antenas[i]);
		double tmp2 = getVar(antenas[i]);
		if ((tmp2 == 1) && (tmp1 == 0)){
			((RND_CompleteEvaluations*)(ind))->addBs(antenas[i]);
			removeBs(antenas[i]);
		} else if ((tmp2 == 0) && (tmp1 == 1)){
			((RND_CompleteEvaluations*)(ind))->removeBs(antenas[i]);
			addBs(antenas[i]);
		}
	}
	
}


void RND_CompleteEvaluations::crossover_hux(Individual* ind) {
   int distanciaInd = hammingDistance(this, ind);
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
	       double tmp1 = ind->getVar(i);
	       double tmp2 = getVar(i);
	       if ((tmp2 == 1) && (tmp1 == 0)){
		  ((RND_CompleteEvaluations*)(ind))->addBs(i);
		  removeBs(i);
	       } else if ((tmp2 == 0) && (tmp1 == 1)){
		  ((RND_CompleteEvaluations*)(ind))->removeBs(i);
		  addBs(i);
	       }
	    }
	 }
      }
   }
   
}

void RND_CompleteEvaluations::updateCoverage(){
	setObj(1, (double)(coveredPositions)/problem->getGridSizeX() / problem->getGridSizeY() * 100);
}

void RND_CompleteEvaluations::dependentCrossover(Individual* ind) {
	
	if (type_crossover == CROSSOVER_OPC) {
		crossover_opc(ind);
	}
	else if (type_crossover == CROSSOVER_GEOGRAPHIC_CIRCULAR){
		crossover_geographic_circular(ind);
	}
	else if (type_crossover == CROSSOVER_GEOGRAPHIC_RECTANGULAR){
		crossover_geographic_rectangular(ind);
	} 
	else if (type_crossover == CROSSOVER_HUX){
		crossover_hux(ind);
	} else {
		cout<<"Error en el tipo de Crossover. type_crossover = 0 -> crossover_opc. type_crossover = 1 -> crossover_geographic"<<endl;
	}
	((RND_CompleteEvaluations*)ind)->updateCoverage();
	updateCoverage();
}



/******************************************************************************
 *
 *                            ProblemRND
 * 
 * ***************************************************************************/
// Clase para representar las coordenadas de las posibles ubicaciones de las BS
// La grid es una matriz donde las "x" son las columnas y las "y" son las filas
Position::Position() {
        x = 0;
        y = 0;
}

Position::Position(unsigned int x, unsigned int y) {
        this->x = x;
        this->y = y;
}


// Problem Initialization
ProblemRND_CompleteEvaluations::ProblemRND_CompleteEvaluations(const string &fich_dim, const string &fich_inst, const int radio) {
	readDimFile(fich_dim);
	readOldInstanceFile(fich_inst);
	this->bsCoverageRadius = radio;
	//Inicializar la matriz de puntos cubiertos por cada una de las Bs
  calculateBsCoveredPoints();
}

ProblemRND_CompleteEvaluations::ProblemRND_CompleteEvaluations(const string &fich_inst) {
	readInstanceFile(fich_inst);
	//Inicializar la matriz de puntos cubiertos por cada una de las Bs
  calculateBsCoveredPoints();
}

void ProblemRND_CompleteEvaluations::readDimFile(const string &fich_dim){
	// Dimensiones de la grid
	ifstream gridSizeFile(fich_dim.c_str());
	if (!gridSizeFile.is_open()) {
		cerr << "ERROR: No se pudo abrir el fichero de dimensiones - " << fich_dim << endl;
		exit(1);
	}
	// Primero se lee el numero de filas y luego el numero de columnas
	gridSizeFile >> gridSizeY >> gridSizeX;
	gridSizeFile.close();
}

//Formato:
//Radio
//Tam_X Tam_Y
//NSites
//x_1 y_1
//x_2 y_2
//...
//x_NSites Y_NSites
void ProblemRND_CompleteEvaluations::readInstanceFile(const string &fich_inst){
	ifstream instanceFile(fich_inst.c_str());
	if (!instanceFile.is_open()) {
		cerr << "ERROR: No se pudo abrir el fichero de instancia - " << fich_inst << endl;
		exit(1);
	}
	instanceFile >> bsCoverageRadius;
	instanceFile >> gridSizeX >> gridSizeY;
	instanceFile >> bsLocationsSize;
	unsigned int posX, posY;
	for (int i = 0; i < bsLocationsSize; i++){
		instanceFile >> posX >> posY;	
		bsLocations.push_back(Position(posX, posY));
	}
	instanceFile.close();	
}

// Reads a problem from file
void ProblemRND_CompleteEvaluations::readOldInstanceFile(const string &fich_inst){
	// Posibles ubicaciones de las estaciones base
	ifstream bsLocationsFile(fich_inst.c_str());
	if (!bsLocationsFile.is_open()) {
		cerr << "ERROR: No se pudo abrir el fichero de estaciones base - " << fich_inst << endl;
		exit(1);
	}
	// El formato del fichero es:
	// x1,y1,x2,y2,...,xn,yn
	char coma;
	unsigned int posX, posY;
	while(!(bsLocationsFile.eof())) {
		bsLocationsFile >> posX >> coma >> posY;
		bsLocations.push_back(Position(posX, posY));  
		bsLocationsFile >> coma;
	}
	bsLocationsSize = bsLocations.size();
	bsLocationsFile.close();
}


/**************** Calculo de los puntos de cubrimiento de una Bs ***********************************/

// Inicializa la matriz de puntos que cubre cada antena
// Dada una determinada estacion base, devuelve una lista con las coordenadas de los
// puntos de la grid que son cubiertos por dicha BS.
void ProblemRND_CompleteEvaluations::calculateBsCoveredPoints() {

        // Inicializar la matriz de puntos cubiertos por cada antena
        bsCoveredPoints.resize(bsLocationsSize);

        // Inicializar la estructura de grid to BS que cubren el punto
        //posToBs.insert(posToBs.begin(), gridSizeX, (vector < vector <unsigned int> >(gridSizeY, vector <unsigned int>())));

        // Calcular el numero de puntos que cubre cada antena
        for (unsigned int bsId = 0; bsId < bsLocationsSize; bsId++) {
                unsigned int x = bsLocations[bsId].getX();
                unsigned int y = bsLocations[bsId].getY();
                unsigned int startX = (x > bsCoverageRadius)?  (x - bsCoverageRadius): 0;
                unsigned int endX = ((x + bsCoverageRadius) < gridSizeX)? (x + bsCoverageRadius): (gridSizeX - 1);
                for (unsigned int x1 = startX; x1 <= endX; x1++) {
                        unsigned int startY = (y > bsCoverageRadius)? (y - bsCoverageRadius): 0;
                        unsigned int endY = ((y + bsCoverageRadius) < gridSizeY)? (y + bsCoverageRadius): (gridSizeY - 1);
                        unsigned int xDistance = (unsigned int) abs((int)(x1) - (int)(x));
                        unsigned int maxYDistance = (unsigned int) (sqrt((double)(bsCoverageRadius*bsCoverageRadius - xDistance*xDistance)));
                        if ((y - startY) > maxYDistance)
                                startY = y - maxYDistance;
                        if ((endY - y) > maxYDistance)
                                endY = y + maxYDistance;
                        for (unsigned int y1 = startY; y1 <= endY; y1++) {
                                bsCoveredPoints[bsId].push_back(Position(x1, y1));
                                //posToBs[x1][y1].push_back(bsId);
                        }
                }
        }
}


// De momento se supone que las antenas son siempre omnidireccionales y que la cobertura es circular
// Si se introdujeran nuevos tipos de antenas habria que incluir nuevas formas de calculo del cubrimiento
//
// Dado un vector de 1/0 que indican las BS a utilizar (configuracion/solucion), 
// se calcula el numero de BS que se utilizan y el cubrimiento que se obtiene con las mismas
unsigned int ProblemRND_CompleteEvaluations::calculateCoverage(RND_CompleteEvaluations *rndInd, unsigned int &usedBs, vector< vector<int> > *grid) {
	
	// Inicializacion a cero del cubrimiento   
	usedBs = 0;
	unsigned int coverage = 0;


	// Determinar los puntos cubiertos por cada una de las estaciones base
	for (unsigned int i = 0; i < bsLocationsSize; i++) {
		if ((int)rndInd->getVar(i) == 1) {  // Si en la posicion disponible i se posiciona una BS
			usedBs++;
			for (unsigned int j = 0; j < bsCoveredPoints[i].size(); j++) {
				unsigned int x = bsCoveredPoints[i][j].getX();
				unsigned int y = bsCoveredPoints[i][j].getY();
				(*grid)[x][y]++;
				if ((*grid)[x][y] == 1) 
					coverage++;
			}
		}
	}

	return (coverage);
}


/****************** Calculo del valor del objetivo de cubrimiento **************************/

// Calcular los ratios maximos de cubrimiento
void ProblemRND_CompleteEvaluations::calculateMaxCoverages() {

        // Calcular el maximo ratio de cubrimiento (considerando que todas las BS se utilizan)
        //unsigned int 
	maxCoverageRate = 0;
        vector< vector<int> > grid;
        grid.insert(grid.begin(), gridSizeX, vector<int>(gridSizeY, 0));
        for (unsigned int i = 0; i < bsLocationsSize; i++) {
                for (unsigned int j = 0; j < bsCoveredPoints[i].size(); j++) {
                        unsigned int x = bsCoveredPoints[i][j].getX();
                        unsigned int y = bsCoveredPoints[i][j].getY();
                        grid[x][y]++;
			if (grid[x][y] == 1)
				maxCoverageRate++;
                }
        }


        maxCoverageRate = (maxCoverageRate * 100) / (gridSizeY * gridSizeX); 
}

//Info de la mutacion
string RND_CompleteEvaluations::getMutationName() const{
	return "BitFlipMutation";
}

//Info del crossover
string RND_CompleteEvaluations::getCrossoverName() const{
	if (type_crossover == 0){
		return "One-Point-Crossover";
	} else if (type_crossover == 1){
		return "Geographic-Circular";
	} else if (type_crossover == 2){
		return "Geographic-Rectangular";
	} else if (type_crossover == 3){
		return "Half Uniform Crossover";
	}
}

int RND_CompleteEvaluations::getCrossoverNumberOfParameters() const{
	return crossParamNames.size();
}

string RND_CompleteEvaluations::getCrossoverParameterName(int i) const{
	return crossParamNames[i];
}

string RND_CompleteEvaluations::getCrossoverParameterValue(int i) const{
	return crossParamValues[i];
}

//Info de los objetivos
string RND_CompleteEvaluations::getObjName(int i) const{
	if (i == 0)
		return "numberBS";
	if (i == 1)
		return "coverRate";
}
		
void RND_CompleteEvaluations::maintainOnlyObj(){
	maintainOnlyObjInternal();
	grid.clear();
}
