/*****************************************************************************************************
 *	Authors
 *		Gara Miranda Valladares
 *		Carlos Segura Gonzalez
 *****************************************************************************************************/
 

#include "TwoDPack.h"
#include <limits.h>
#include <set>
#include <iostream>
#include <fstream>
using namespace std;

//Borra los elementos estáticos cuando el último individuo es liberado
TwoDPack::~TwoDPack(){
	nInds--;
	if (nInds == 0){
		endLocalSearch1();
		endLocalSearch2();
	}
}


//Los vecinos que se generan estan asociados a cada uno de las casillas de la grid
void TwoDPack::initLocalSearch1(){
	neighboursToExplore = new vector< pair<int, int> * >();
	for (int i = 0; i < twoDPackingProblem->getXSize(); i++){
		for (int j = 0; j < twoDPackingProblem->getYSize(); j++){
			neighboursToExplore->push_back(new pair<int,int>(i, j));
		}	
	}
}

void TwoDPack::endLocalSearch1(){
	for (unsigned int i = 0; i < neighboursToExplore->size(); i++)
		delete (*neighboursToExplore)[i];
	neighboursToExplore = NULL;
}

void TwoDPack::initLocalSearch2(){
	//partners para la busqueda local2
	partnersToExplore = new vector< pair<Position, Position> * >();
	// a) Horizontales
	for (int i = 0; i < twoDPackingProblem->getXSize() - 1; i++){
		for (int j = 0; j < twoDPackingProblem->getYSize(); j++){
			partnersToExplore->push_back(new pair<Position, Position>(Position(i, j), Position(i+1, j)));
		}
	}
	// b) Verticales
	for (int i = 0; i < twoDPackingProblem->getXSize(); i++){
		for (int j = 0; j < twoDPackingProblem->getYSize() - 1; j++){
			partnersToExplore->push_back(new pair<Position, Position>(Position(i, j), Position(i, j+1)));
		}
	}
	// c) Diagonal, con el de la izquierda por arriba
	for (int i = 0; i < twoDPackingProblem->getXSize() - 1; i++){
		for (int j = 0; j < twoDPackingProblem->getYSize() - 1; j++){
			partnersToExplore->push_back(new pair<Position, Position>(Position(i, j), Position(i+1, j+1)));
		}
	}
	// d) Diagonal, con el de la izquierda por debajo
	for (int i = 0; i < twoDPackingProblem->getXSize() - 1; i++){
		for (int j = 1; j < twoDPackingProblem->getYSize(); j++){
			partnersToExplore->push_back(new pair<Position, Position>(Position(i, j), Position(i+1, j-1)));
		}
	}
}

void TwoDPack::endLocalSearch2(){
	for (unsigned int i = 0; i < partnersToExplore->size(); i++)
		delete (*partnersToExplore)[i];
	partnersToExplore = NULL;
}

bool TwoDPack::init (const vector<string> &params){
	if (params.size() != 7){
		cout << "Parametros incorrectos (fichero_instancia tipoMut mutProbLow mutProbHigh mutProbWLow mutProbWHigh typeLocalSearch)" << endl;
		return false;
	}
	twoDPackingProblem = new TwoDPackingProblem(params[0]);
	typeMutation = atoi(params[1].c_str());
	mutProbLow = atof(params[2].c_str());
	mutProbHigh = atof(params[3].c_str());
	mutProbWLow = atof(params[4].c_str());
	mutProbWHigh = atof(params[5].c_str());
	typeLocalSearch = atoi(params[6].c_str());

	//Reservamos espacio para almacenar los valores del grid
	appearance.insert(appearance.begin(), twoDPackingProblem->getMaxNumber(), vector<int>(twoDPackingProblem->getMaxNumber(), 0) );
	pAppearance.insert(pAppearance.begin(), twoDPackingProblem->getMaxNumber(), vector<int>(twoDPackingProblem->getMaxNumber(), 0) );
	
	//Inicializaciones para las búsquedas locales
	initLocalSearch1();
	initLocalSearch2();

	setNumberOfObj(1); //un único objetivo
	setNumberOfVar(twoDPackingProblem->getXSize() * twoDPackingProblem->getYSize());

	return true;
}

void TwoDPack::evaluate (void){
	calculateFitness();
}

//Generación de aleatorio y evaluación de objetivo
void TwoDPack::restart(void){
	generateRandom();
	calculateFitness();
	localSearch();
}

void TwoDPack::deserialized() {
  evaluate();
}

// Generacion de individuo aleatorio: se asigna un valor cualquiera (prob. uniforme)
// a cada casilla del grid
void TwoDPack::generateRandom(){
	for (int i = 0; i < twoDPackingProblem->getXSize(); i++){
		for (int j = 0; j < twoDPackingProblem->getYSize(); j++){
			setGrid(i, j, (int) ((double)(twoDPackingProblem->getMaxNumber()) * rand() / (RAND_MAX + 1.0)));
		}
	}
}

// Actualizar el fitness y aparición de parejas considerando la casilla (x1, y1) - (x2, y2)
void TwoDPack::update(const int x1, const int y1, const int x2, const int y2){
	int n1 = getGrid(x1, y1);
	int n2 = getGrid(x2, y2);
	appearance[n1][n2]++;
	if (appearance[n1][n2] == 1)
		setObj(0, getObj(0) + twoDPackingProblem->getValue(n1, n2));
	if (n1 != n2)
		appearance[n2][n1]++;
}

// Calcular el fitness y la matriz appearance
void TwoDPack::calculateFitness(){
	setObj(0, 0);
	for (int i = 0; i < twoDPackingProblem->getMaxNumber(); i++){
		for (int j = 0; j < twoDPackingProblem->getMaxNumber(); j++){
			appearance[i][j] = 0;
		}
	}
	// Con diagonal arriba derecha
	for (int i = 0; i < twoDPackingProblem->getXSize() - 1; i++)
		for (int j = 1; j < twoDPackingProblem->getYSize(); j++)
			update(i, j, i+1, j-1);	
	//Con los de la derecha
	for (int i = 0; i < twoDPackingProblem->getXSize() - 1; i++)
		for (int j = 0; j < twoDPackingProblem->getYSize(); j++)
			update(i, j, i+1, j);
	// Con diagonal abajo derecha
	for (int i = 0; i < twoDPackingProblem->getXSize() - 1; i++)
		for (int j = 0; j < twoDPackingProblem->getYSize()-1; j++)
			update(i, j, i+1, j+1);
	// Abajo
	for (int i = 0; i < twoDPackingProblem->getXSize(); i++)
		for (int j = 0; j < twoDPackingProblem->getYSize()-1; j++)
			update(i, j, i, j+1);
  
}

// Actualizar el fitness y aparición de parejas considerando la casilla (x1, y1) - (x2, y2)
void TwoDPack::updatePartial(const int x1, const int y1, const int x2, const int y2) {
	int n1 = getGrid(x1, y1);
	int n2 = getGrid(x2, y2);
	pAppearance[n1][n2]++;
	if (pAppearance[n1][n2] == 1)
		pFitness += twoDPackingProblem->getValue(n1, n2);
	if (n1 != n2)
		pAppearance[n2][n1]++;
}

// Calcular el fitness y la matriz appearance de una ventana de la grid
void TwoDPack::calculatePartialFitness(const int initX, const int endX, const int initY, const int endY) {
	pFitness = 0;
	for (int i = 0; i < twoDPackingProblem->getMaxNumber(); i++) {
		for (int j = 0; j < twoDPackingProblem->getMaxNumber(); j++) {
			pAppearance[i][j] = 0;
		}
	}
	// Con diagonal arriba derecha
	for (int i = initX; i < endX - 1; i++)
		for (int j = initY + 1; j < endY; j++)
			updatePartial(i, j, i+1, j-1);	
	//Con los de la derecha
	for (int i = initX; i < endX - 1; i++)
		for (int j = initY; j < endY; j++)
			updatePartial(i, j, i+1, j);
	// Con diagonal abajo derecha
	for (int i = initX; i < endX - 1; i++)
		for (int j = initY; j < endY - 1; j++)
			updatePartial(i, j, i+1, j+1);
	// Abajo
	for (int i = initX; i < endX; i++)
		for (int j = initY; j < endY - 1; j++)
			updatePartial(i, j, i, j+1);
}

//Imprimir una solucion en el formato aceptado por el concurso:
//maxX maxY maxNumber
//V11 V12 ...
//V21 ...
//V31 ....
//...
ostream& operator<< (ostream &os, const TwoDPack &twoDPackingIndividual){
	int maxX = twoDPackingIndividual.twoDPackingProblem->getXSize();
	int maxY = twoDPackingIndividual.twoDPackingProblem->getYSize();
	int maxNumber = twoDPackingIndividual.twoDPackingProblem->getMaxNumber();
	os << maxX << " " << maxY << " " << maxNumber << endl;
	for (int i = 0; i < maxY; i++){
		for (int j = 0; j < maxX; j++){
			int tmpValue = twoDPackingIndividual.getGrid(j, i);
			int espacios = 0;
			do{ 
				espacios++;
				tmpValue /= 10;
			} while (tmpValue > 0);
			for (int k = 0; k < 5 - espacios; k++)
				os << " ";
			os << twoDPackingIndividual.getGrid(j, i);
		}
		os << endl;
	}
	return os;
}

// Ejecutar una búsqueda local sobre el elemento actual
void TwoDPack::localSearch(){
  switch(typeLocalSearch){
		case 0:
			localSearch1();
			break;
		case 1:
			localSearch2();
			break;
	}
}

//La busqueda local tiene las siguientes caracteristicas
//- Los vecinos consisten en cambiar una única casilla, por el valor que maximice la asignacion
//- Se mueve al primer vecino que encuentre que la asignacion mejora
//- Se para cuando no se puede cambiar una único asignación consiguiendo que el valor mejore
//- Por eficiencia cuando se cambia a un vecino, no se vuelve a rellenar la lista de vecinos a los
//que se puede mover, sino que sigue con los que quedan.
void TwoDPack::localSearch1(){
	bool improved = false;
	int last = neighboursToExplore->size();
	while((improved) || (last != 0)){//mientras se mejore o queden posibilidades por explorar
		if (last == 0){
			improved = false;
			last = neighboursToExplore->size();
		}
		//randomly selection of one position
		int n =  (int) ((double)(last) * rand() / (RAND_MAX + 1.0));
		if (improvePosition((*neighboursToExplore)[n]->first, (*neighboursToExplore)[n]->second))
			improved = true;
		pair<int,int> *tmp = (*neighboursToExplore)[n]; 
		(*neighboursToExplore)[n] = (*neighboursToExplore)[last-1];
		(*neighboursToExplore)[last-1] = tmp;
		last--;
	}				
}

//La busqueda local tiene las siguientes caracteristicas:
//- Los vecinos consisten en cambiar 2 casillas adyacentes, por el valor que maximice la asignación
//- Como las posibles asignaciones son demasiados (400 x 400 en la instancia del concurso) se ha
//diseñado un método basado en cotas superiores, para no tener que visitar todas las posibles 
//asignaciones (ver documento del gecco)
//- Se mueve al primer vecino que encuentre que la asignacion mejora
//- Se para cuando no se puede cambiar una único asignación consiguiendo que el valor mejore
//- Por eficiencia cuando se cambia a un vecino, no se vuelve a rellenar la lista de vecinos a los
//que se puede mover, sino que sigue con los que quedan.
void TwoDPack::localSearch2(){
	bool improved = false;
	int last = partnersToExplore->size();
	while((improved) || (last != 0)){// mientras se mejore o queden posibilidades por explorar
		if (last == 0){
			improved = false;
			last = partnersToExplore->size();
		}
		//randomly selection of one position
		int n =  (int) ((double)(last) * rand() / (RAND_MAX + 1.0));
		if (improvePosition((*partnersToExplore)[n]->first, (*partnersToExplore)[n]->second))
			improved = true;
		pair<Position,Position> *tmp = (*partnersToExplore)[n]; 
		(*partnersToExplore)[n] = (*partnersToExplore)[last-1];
		(*partnersToExplore)[last-1] = tmp;
		last--;
	}				
}

// Suprimir una asignación, restando el fitness y la apareición de las parejas correspondientes
int TwoDPack::removeAssignation(const int x, const int y){
	int removeFitness = 0;
	int value = getGrid(x, y);
	int initX = x>0?x-1:0;
	int initY = y>0?y-1:0;
	int finX = ((x+1)<twoDPackingProblem->getXSize())?x+1:x;
	int finY = ((y+1)<twoDPackingProblem->getYSize())?y+1:y;
	for (int i = initX; i <= finX; i++){
		for (int j = initY; j <= finY; j++){
			if ((i == x) && (y == j))
				continue;
			int value2 = getGrid(i, j);
			if (value2 == -1)
				continue;
			appearance[value][value2]--;
			if (value != value2){
				appearance[value2][value]--;
			}
			if (appearance[value][value2] == 0){
				removeFitness += twoDPackingProblem->getValue(value, value2);
			}
		}
	}
	setGrid(x, y, -1);
	setObj(0, getObj(0) - removeFitness);
	return removeFitness;
}

// Obtener el fitness asociado a realizar una asignación
void TwoDPack::getAssignationCosts(const int x, const int y, vector<int> &costValues){
	set<int> consideredValues;
	int initX = x>0?x-1:0;
	int initY = y>0?y-1:0;
	int finX = ((x+1)<twoDPackingProblem->getXSize())?x+1:x;
	int finY = ((y+1)<twoDPackingProblem->getYSize())?y+1:y;
	for (int i = initX; i <= finX; i++){
		for (int j = initY; j <= finY; j++){
			if ((i == x) && (y == j))
				continue;
			int value2 = getGrid(i, j);
			if (value2 == -1)
				continue;
			if (consideredValues.count(value2) != 0)
				continue;
			consideredValues.insert(value2);
			for (vector<int>::const_iterator it = twoDPackingProblem->getPartners(value2).begin(); it != twoDPackingProblem->getPartners(value2).end(); it++){
				int value = *it;
				if (appearance[value][value2] == 0){
					costValues[value] += twoDPackingProblem->getValue(value, value2);
				}
			}
		}
	}
}



// Obtener el fitness asociado a realizar una asignación
int TwoDPack::getAssignationCost(const int x, const int y, const int value){
	set <pair <int, int> > newPartners;
	int fitnessAssignation = 0;
	int initX = x>0?x-1:0;
	int initY = y>0?y-1:0;
	int finX = ((x+1)<twoDPackingProblem->getXSize())?x+1:x;
	int finY = ((y+1)<twoDPackingProblem->getYSize())?y+1:y;
	for (int i = initX; i <= finX; i++){
		for (int j = initY; j <= finY; j++){
			if ((i == x) && (y == j))
				continue;
			int value2 = getGrid(i, j);
			if (value2 == -1)
				continue;
			int menor, mayor;
			if (value < value2){
				menor = value;
				mayor = value2;
			} else {
				menor = value2;
				mayor = value;
			}
			if ((appearance[value][value2] == 0) && (newPartners.count(pair<int,int>(menor, mayor)) == 0)){
				newPartners.insert(pair<int, int>(menor, mayor));
				fitnessAssignation += twoDPackingProblem->getValue(value, value2);
			}
		}
	}
	return fitnessAssignation;
}

// Obtener el coste asociado a hacer 2 asignaciones de casillas contiguas
int TwoDPack::getAssignationCost(const int x1, const int y1, const int v1, const int x2, const int y2, const int v2){
	set <pair <int, int> > newPartners;
	int fitnessAssignation = 0;
	int x, y, value;
	for (int i = 0; i < 2; i++){
		if (i == 0){
			x = x1;
			y = y1;
			value = v1;
		} else {
			x = x2;
			y = y2;
			value = v2;
		}
		int initX = x>0?x-1:0;
		int initY = y>0?y-1:0;
		int finX = ((x+1)<twoDPackingProblem->getXSize())?x+1:x;
		int finY = ((y+1)<twoDPackingProblem->getYSize())?y+1:y;
		for (int i = initX; i <= finX; i++){
			for (int j = initY; j <= finY; j++){
				if ((i == x) && (y == j))
					continue;
				int value2 = getGrid(i, j);
				if (value2 == -1)
					continue;
				int menor, mayor;
				if (value < value2){
					menor = value;
					mayor = value2;
				} else {
					menor = value2;
					mayor = value;
				}
				if ((appearance[value][value2] == 0) && (newPartners.count(pair<int,int>(menor, mayor)) == 0)){
					newPartners.insert(pair<int, int>(menor, mayor));
					fitnessAssignation += twoDPackingProblem->getValue(value, value2);
				}
			}
		}
	}
	//añadimos la pareja value1 - value2
	int menor, mayor;
	if (v1 < v2){
		menor = v1;
		mayor = v2;
	} else {
		menor = v2;
		mayor = v1;
	}
	if ((appearance[menor][mayor] == 0) && (newPartners.count(pair<int,int>(menor, mayor)) == 0)){
		fitnessAssignation += twoDPackingProblem->getValue(v1, v2);
	}
	return fitnessAssignation;
}

// Obtener el coste asociado a hacer 2 asignaciones de casillas contiguas
int TwoDPack::getAssignationCost(const int x1, const int y1, const int v1, const int x2, const int y2, const int v2, const int x3, const int y3, const int v3){
	set <pair <int, int> > newPartners;
	int fitnessAssignation = 0;
	int x, y, value;
	for (int i = 0; i < 3; i++){
		if (i == 0){
			x = x1;
			y = y1;
			value = v1;
		} else if (i == 1) {
			x = x2;
			y = y2;
			value = v2;
		} else {
			x = x3;
			y = y3;
			value = v3;
		}
		int initX = x>0?x-1:0;
		int initY = y>0?y-1:0;
		int finX = ((x+1)<twoDPackingProblem->getXSize())?x+1:x;
		int finY = ((y+1)<twoDPackingProblem->getYSize())?y+1:y;
		for (int i = initX; i <= finX; i++){
			for (int j = initY; j <= finY; j++){
				if ((i == x) && (y == j))
					continue;
				int value2 = getGrid(i, j);
				if (value2 == -1)
					continue;
				int menor, mayor;
				if (value < value2){
					menor = value;
					mayor = value2;
				} else {
					menor = value2;
					mayor = value;
				}
				if ((appearance[value][value2] == 0) && (newPartners.count(pair<int,int>(menor, mayor)) == 0)){
					newPartners.insert(pair<int, int>(menor, mayor));
					fitnessAssignation += twoDPackingProblem->getValue(value, value2);
				}
			}
		}
	}
	//añadimos la pareja value1 - value2
	int menor, mayor;
	if (v1 < v2){
		menor = v1;
		mayor = v2;
	} else {
		menor = v2;
		mayor = v1;
	}
	if ((appearance[menor][mayor] == 0) && (newPartners.count(pair<int,int>(menor, mayor)) == 0)){
		newPartners.insert(pair<int, int>(menor, mayor));
		fitnessAssignation += twoDPackingProblem->getValue(v1, v2);
	}
	//añadimos value2 - value3
	if (v2 < v3){
		menor = v2;
		mayor = v3;
	} else {
		menor = v3;
		mayor = v2;
	}
	if ((appearance[menor][mayor] == 0) && (newPartners.count(pair<int,int>(menor, mayor)) == 0)){
		newPartners.insert(pair<int, int>(menor, mayor));
		fitnessAssignation += twoDPackingProblem->getValue(v2, v3);
	}
	return fitnessAssignation;
}

// Realizar la asignación de un valor a una casilla
void TwoDPack::makeAssignation(const int x, const int y, const int value){
	int initX = x>0?x-1:0;
	int initY = y>0?y-1:0;
	int finX = ((x+1)<twoDPackingProblem->getXSize())?x+1:x;
	int finY = ((y+1)<twoDPackingProblem->getYSize())?y+1:y;
	for (int i = initX; i <= finX; i++){
		for (int j = initY; j <= finY; j++){
			if ((i == x) && (y == j))
				continue;
			int value2 = getGrid(i, j);
			if (value2 == -1)
				continue;
			appearance[value][value2]++;
			if (value != value2)
				appearance[value2][value]++;
			if (appearance[value][value2] == 1){
				setObj(0, getObj(0) + twoDPackingProblem->getValue(value, value2));
			}
		}
	}	
	setGrid(x, y, value);
}

// Usado por localSearch1, busca la mejor asignación posible para la casilla (x,y)
bool TwoDPack::improvePosition(const int x, const int y){
	vector<int> explored(twoDPackingProblem->getMaxNumber(), 0);
	// Valores de las casillas vecinas, dentro de la grid
	int initX = x>0?x-1:0;
	int initY = y>0?y-1:0;
	int finX = ((x+1)<twoDPackingProblem->getXSize())?x+1:x;
	int finY = ((y+1)<twoDPackingProblem->getYSize())?y+1:y;

	int originalValue = getGrid(x, y);
	int originalFitness = removeAssignation(x, y);
	int bestAssignation = originalValue;
	int bestAssignationFitness = originalFitness;
	explored[originalValue] = 1;
	for (int i = initX; i <= finX; i++){
		for (int j = initY; j <= finY; j++){
			if ((i == x) && (y == j))//con los vecinos, excluyéndose a si mismo
				continue;
			// sólo consideramos valores que pueden puntuar
			const vector<int>& partners = twoDPackingProblem->getPartners(getGrid(i, j));
			for (unsigned int k = 0; k < partners.size(); k++){
				if (explored[partners[k]] != 0)
					continue;
				int newAssignationFitness = getAssignationCost(x, y, partners[k]);
				if (newAssignationFitness > bestAssignationFitness){
					bestAssignationFitness = newAssignationFitness;
					bestAssignation = partners[k];
				}
				explored[partners[k]] = 1;
			}
		}
	}
	makeAssignation(x, y, bestAssignation);
	return (bestAssignation != originalValue);
}

bool TwoDPack::improvePosition(const Position &p1, const Position &p2, const Position &p3){
	vector<int> explored(twoDPackingProblem->getMaxNumber(), 0);
	vector<AssignationCost *> p1costs, p2costs, p3costs;
	int originalFitness = (int)getObj(0); 

	//Borramos la asignacion de p1, p2 y p3
	int bestAssignationP1 = getGrid(p1.getX(), p1.getY());
	int bestAssignationP2 = getGrid(p2.getX(), p2.getY());
	int bestAssignationP3 = getGrid(p3.getX(), p3.getY());

	int valuep1p2p3 = removeAssignation(p1.getX(), p1.getY());
	valuep1p2p3 += removeAssignation(p2.getX(), p2.getY());
	valuep1p2p3 += removeAssignation(p3.getX(), p3.getY());

	int actualValue = valuep1p2p3;
	//Obtenemos los costes de cada posible asignacion
	vector<int> p1costValues(twoDPackingProblem->getMaxNumber(), 0);
	vector<int> p2costValues(twoDPackingProblem->getMaxNumber(), 0);
	vector<int> p3costValues(twoDPackingProblem->getMaxNumber(), 0);
	getAssignationCosts(p2.getX(), p2.getY(), p2costValues);
	for (int i = 0; i < twoDPackingProblem->getMaxNumber(); i++){
		p1costs.push_back(new AssignationCost(i, p1costValues[i], 2*twoDPackingProblem->getBestValue(i)));
		p2costs.push_back(new AssignationCost(i, p2costValues[i], 2*twoDPackingProblem->getBestValue(i)));
		p3costs.push_back(new AssignationCost(i, p3costValues[i], 2*twoDPackingProblem->getBestValue(i)));
	}

	//Ordenamos p2 segun su upperBound
	sort(p2costs.begin(), p2costs.end(), cmpUpperBound);
	for (unsigned int i = 0; i < p1costs.size(); i++){
		for (unsigned int j = 0; j < p3costs.size(); j++){
			for (unsigned int k = 0; k < p2costs.size(); k++){
				int tope = actualValue -  p1costs[i]->getV() - p3costs[j]->getV();
				if (p2costs[k]->getUpperBound() < tope)
						break;
				int newValue = getAssignationCost(p1.getX(), p1.getY(), p1costs[i]->getAssignation(), p2.getX(), p2.getY(), p2costs[j]->getAssignation(), p3.getX(), p3.getY(), p3costs[i]->getAssignation());
				if (newValue > actualValue){
					bestAssignationP1 = p1costs[i]->getAssignation();
					bestAssignationP2 = p2costs[j]->getAssignation();
					bestAssignationP3 = p3costs[k]->getAssignation();
					actualValue = newValue;
				}
			}
		}
	}
	makeAssignation(p1.getX(), p1.getY(), bestAssignationP1);
	makeAssignation(p2.getX(), p2.getY(), bestAssignationP2);
	makeAssignation(p3.getX(), p3.getY(), bestAssignationP3);
	for (unsigned int i = 0; i < p1costs.size(); i++){
		delete p1costs[i];
	}
	for (unsigned int j = 0; j < p2costs.size(); j++){
		delete p2costs[j];
	}
	for (unsigned int j = 0; j < p3costs.size(); j++){
		delete p3costs[j];
	}

	if (getObj(0) != (originalFitness - valuep1p2p3 + actualValue)){
		cout << "Fail: " << getObj(0) << " " << originalFitness - valuep1p2p3 + actualValue << endl;
		exit(-1);
	}
	return (actualValue > valuep1p2p3);
}
// Busca la mejor asignación posible para las casillas p1 y p2 (a la vez)
bool TwoDPack::improvePosition(const Position &p1, const Position &p2){
	vector<int> explored(twoDPackingProblem->getMaxNumber(), 0);
	vector<AssignationCost *> p1costs, p2costs;
	int originalFitness = (int)getObj(0);

	//Borramos la asignacion de p1 y p2
	int bestAssignationP1 = getGrid(p1.getX(), p1.getY());
	int bestAssignationP2 = getGrid(p2.getX(), p2.getY());

	int valuep1p2 = removeAssignation(p1.getX(), p1.getY());
	valuep1p2 += removeAssignation(p2.getX(), p2.getY());

	int actualValue = valuep1p2;
	//Obtenemos los costes de cada posible asignacion
	vector<int> p1costValues(twoDPackingProblem->getMaxNumber(), 0);
	vector<int> p2costValues(twoDPackingProblem->getMaxNumber(), 0);
	getAssignationCosts(p1.getX(), p1.getY(), p1costValues);
	getAssignationCosts(p2.getX(), p2.getY(), p2costValues);
	for (int i = 0; i < twoDPackingProblem->getMaxNumber(); i++){
		p1costs.push_back(new AssignationCost(i, p1costValues[i], twoDPackingProblem->getBestValue(i)));
		p2costs.push_back(new AssignationCost(i, p2costValues[i], twoDPackingProblem->getBestValue(i)));
	}

	//Ordenamos p2 segun su upperBound
	sort(p2costs.begin(), p2costs.end(), cmpUpperBound);
	for (unsigned int i = 0; i < p1costs.size(); i++){
		int tope = actualValue -  p1costs[i]->getV();
		for (unsigned int j = 0; j < p2costs.size(); j++){
			if (p2costs[j]->getUpperBound() < tope)
				break;
			//if (p2costs[i]->getV() + p1costs[i]->getUpperBound() < actualValue) // este es un posible "corte", pero no parece muy efectivo
				//continue;
			int newValue = getAssignationCost(p1.getX(), p1.getY(), p1costs[i]->getAssignation(), p2.getX(), p2.getY(), p2costs[j]->getAssignation());
			if (newValue > actualValue){
				bestAssignationP1 = p1costs[i]->getAssignation();
				bestAssignationP2 = p2costs[j]->getAssignation();
				actualValue = newValue;
			}

		}
	}
	makeAssignation(p1.getX(), p1.getY(), bestAssignationP1);
	makeAssignation(p2.getX(), p2.getY(), bestAssignationP2);
	for (unsigned int i = 0; i < p1costs.size(); i++){
		delete p1costs[i];
	}
	for (unsigned int j = 0; j < p2costs.size(); j++){
		delete p2costs[j];
	}
	if (getObj(0) != (originalFitness - valuep1p2 + actualValue)){
		cout << "Fail: " << getObj(0) << " " << originalFitness - valuep1p2 + actualValue << endl;
		exit(-1);
	}
	return (actualValue > valuep1p2);
}

//Reorder mutation: se elige una columna o una fila. Lo que queda a la izquierda pasa a la derecha y lo de la derecha
//a la izquierda del todo (lo mismo con arriba-abajo)
//En realidad es como un rotation operator: se hace rotacion de filas o columnas.
void TwoDPack::mutationReorder(){
	int	copy1_fila_origen_init, copy1_fila_origen_end, copy1_fila_destino_init, copy1_columna_origen_init, copy1_columna_origen_end, copy1_columna_destino_init;
	int	copy2_fila_origen_init, copy2_fila_origen_end, copy2_fila_destino_init, copy2_columna_origen_init, copy2_columna_origen_end, copy2_columna_destino_init;

	//Hacemos una copia temporal de la grid
	vector< vector < int > > tmpGrid (twoDPackingProblem->getXSize(), vector< int > (twoDPackingProblem->getYSize(), 0));

	for (int i = 0; i < twoDPackingProblem->getXSize(); i++){
		for (int j = 0; j < twoDPackingProblem->getYSize(); j++){
			tmpGrid[i][j] = getGrid(i,j);
		}
	}

	
	if (rand() > (RAND_MAX / 2)){//intercambiamos filas 
		int fila = (int)((twoDPackingProblem->getYSize()) * ((double)(rand()/(RAND_MAX + 1.0))));
		//Primera copia
		copy1_fila_origen_init = 0;
		copy1_fila_origen_end = fila;
		copy1_fila_destino_init =  twoDPackingProblem->getYSize() - fila - 1;
		copy1_columna_origen_init = 0;
		copy1_columna_origen_end = twoDPackingProblem->getXSize() - 1;
		copy1_columna_destino_init = 0;
			
		//Segunda copia
		copy2_fila_origen_init = fila + 1;
		copy2_fila_origen_end = twoDPackingProblem->getYSize() - 1;
		copy2_fila_destino_init =  0;
		copy2_columna_origen_init = 0;
		copy2_columna_origen_end = twoDPackingProblem->getXSize() - 1;
		copy2_columna_destino_init = 0;

	} else {//intercambiamos columnas
		int columna = (int)((twoDPackingProblem->getXSize()) * ((double)(rand()/(RAND_MAX + 1.0))));

		//Primera copia
		copy1_columna_origen_init = 0;
		copy1_columna_origen_end = columna;
		copy1_columna_destino_init =  twoDPackingProblem->getYSize() - columna - 1;
		copy1_fila_origen_init = 0;
		copy1_fila_origen_end = twoDPackingProblem->getXSize() - 1;
		copy1_fila_destino_init = 0;
			
		//Segunda copia
		copy2_columna_origen_init = columna + 1;
		copy2_columna_origen_end = twoDPackingProblem->getXSize() - 1;
		copy2_columna_destino_init =  0;
		copy2_fila_origen_init = 0;
		copy2_fila_origen_end = twoDPackingProblem->getYSize() - 1;
		copy2_fila_destino_init = 0;
	}

	//Primera copia
	for (int i = copy1_columna_origen_init; i <= copy1_columna_origen_end; i++){
		for (int j = copy1_fila_origen_init; j <= copy1_fila_origen_end; j++){
			setGrid(copy1_columna_destino_init + i - copy1_columna_origen_init, copy1_fila_destino_init + j - copy1_fila_origen_init, tmpGrid[i][j]);
		}
	}

	//Segunda copia
	for (int i = copy2_columna_origen_init; i <= copy2_columna_origen_end; i++){
		for (int j = copy2_fila_origen_init; j <= copy2_fila_origen_end; j++){
			setGrid(copy2_columna_destino_init + i - copy2_columna_origen_init, copy2_fila_destino_init + j - copy2_fila_origen_init, tmpGrid[i][j]);
		}
	}
}

// Mutación uniforme: se elige un valor de probabilidad de mutacion mutProb entre mutProbLow y mutProbHigh
// cada elemento es mutado con una probabilidad mutProb
void TwoDPack::mutationUniform(){
	double mutProb = mutProbLow + (mutProbHigh - mutProbLow) * ((double)(rand()) / RAND_MAX);
	for (int i = 0; i < twoDPackingProblem->getXSize(); i++){
		for (int j = 0; j < twoDPackingProblem->getYSize(); j++){
			if((1.0*rand()/(RAND_MAX+1.0)) < mutProb){
				removeAssignation(i, j);
				makeAssignation(i, j, (int) ((double)(twoDPackingProblem->getMaxNumber()) * rand() / (RAND_MAX + 1.0)));
			}
		}
	}
}

// Mutación uniforme: se elige un valor de probabilidad de mutacion mutProb entre mutProbLow y mutProbHigh
// cada elemento es mutado con una probabilidad mutProb
void TwoDPack::mutationUniformValid(){
	double mutProb = mutProbLow + (mutProbHigh - mutProbLow) * ((double)(rand()) / RAND_MAX);
	for (int i = 0; i < twoDPackingProblem->getXSize(); i++){
		for (int j = 0; j < twoDPackingProblem->getYSize(); j++){
			if((1.0*rand()/(RAND_MAX+1.0)) < mutProb){
				removeAssignation(i, j);
				//Elegimos uno de los "vecinos" al azar
				vector<Position> possibleNeighbours;
				possibleNeighbours.clear();
				for (int k = -1; k <= 1; k++){
					int neighX = i + k;
					if ((neighX >= 0) && (neighX < twoDPackingProblem->getXSize())){
						for (int l = -1; l <= 1; l++){
							int neighY = j + l;
							if ((neighY >= 0) && ( neighY < twoDPackingProblem->getYSize())){
								if ( (getGrid(neighX, neighY) != -1) && ( ( k != 0) || (l != 0) ) ){
									possibleNeighbours.push_back(Position(neighX, neighY));
								}
							}
						}
					}
				}
				int selectedNeigh =(int) (possibleNeighbours.size()*rand()/(RAND_MAX+1.0));
				makeAssignation(i, j, twoDPackingProblem->getRandomPartner(getGrid(possibleNeighbours[selectedNeigh].getX(), possibleNeighbours[selectedNeigh].getY())));
			}
		}
	}
}

// Mutación basada en ventanas: se divide la grid en ventanas y 
// se muta con mayor probabilidad aquellos elementos que estén dentro de la peor ventana
void TwoDPack::mutationWindow() {
	int windowSize = 3 + ((unsigned int) (4.0 * rand() / (RAND_MAX + 1.0)));
	int lowFitness = INT_MAX;
	int worstWindowX = -1, worstWindowY = -1;

	int init_i = (unsigned int) (1.0 * windowSize * rand() / (RAND_MAX + 1.0));
	int init_j = (unsigned int) (1.0 * windowSize * rand() / (RAND_MAX + 1.0));

	//busqueda de la peor ventana
	for (int i = init_i; i <= twoDPackingProblem->getXSize() - windowSize; i += windowSize) {
		for (int j = init_j; j <= twoDPackingProblem->getYSize() - windowSize; j += windowSize) {
			calculatePartialFitness (i, i + windowSize, j, j + windowSize);
			if (pFitness < lowFitness) {
				lowFitness = pFitness;
				worstWindowX = i; 
				worstWindowY = j;
			}
		}
	}
	//mutacion uniforme en toda la grid
	mutationUniformValid();
	//intensificamos en la peor ventana
	double mutProbW = mutProbWLow + (mutProbWHigh - mutProbWLow) * ((double)(rand()) / RAND_MAX);
	for (int i = worstWindowX; i < worstWindowX + windowSize; i++) {
		for (int j = worstWindowY; j < worstWindowY + windowSize; j++) {
			if ((1.0*rand()/(RAND_MAX+1.0)) < mutProbW){
				removeAssignation(i, j);
				makeAssignation(i, j, (int) ((double)(twoDPackingProblem->getMaxNumber()) * rand() / (RAND_MAX + 1.0)));
			}
		}
	}	
}

// Mutación + búsqueda local
void TwoDPack::dependentMutation( double pm ) {
	switch(typeMutation){
		case 0:
			mutationUniformValid();
			break;
		case 1:
			mutationWindow();
			break;
		case 2:
			mutationUniform();
			break;
		case 3:
			mutationUniformValid();
			mutationReorder();
			break;
		case 4:
			mutationWindow();
			mutationReorder();
			break;
		case 5:
			mutationUniform();
			mutationReorder();
			break;
	}
	calculateFitness();
	localSearch();
}

//Elige una posicion del cromosoma.
//Elige hacer crossover vertical u horizontal
//Para el vertical, todo lo que esta por encima es del hijo1
//y todo lo que esta por debajo es del hijo2. Lo que esta en su
//misma fila, lo de la izq. es del hijo1 y lo de la derecha del hijo2.
//Para el horizontal, es lo mismo, pero considerando filas en vez de 
//columnas

void TwoDPack::subStringCrossover(TwoDPack *ind){
	int columna = (int)((twoDPackingProblem->getXSize()) * ((double)(rand()/(RAND_MAX + 1.0))));
	int fila = (int)((twoDPackingProblem->getXSize()) * ((double)(rand()/(RAND_MAX + 1.0))));
	if (rand() < RAND_MAX/2){ //vertical
		for (int i = 0; i < twoDPackingProblem->getXSize(); i++){
			for (int j = 0; j < twoDPackingProblem->getYSize(); j++){
				if ((j < fila) || ((j == fila) && (i < columna))){
					int tmp = getGrid(i, j);
					setGrid(i, j, ind->getGrid(i, j));	
					ind->setGrid(i, j, tmp); 
				} 
			}
		}
	} else { //horizontal
		for (int i = 0; i < twoDPackingProblem->getXSize(); i++){
			for (int j = 0; j < twoDPackingProblem->getYSize(); j++){
				if ((i < columna) || ((i == columna) && (j < fila))){
					int tmp = getGrid(i, j);
					setGrid(i, j, ind->getGrid(i, j));	
					ind->setGrid(i, j, tmp); 
				} 
			}
		}
	}
}

void TwoDPack::dependentCrossover (Individual *ind){
	subStringCrossover((TwoDPack *) ind);	

	calculateFitness();
	localSearch();

}

Individual* TwoDPack::clone() const{
	TwoDPack *newInd = new TwoDPack();
	newInd->appearance = appearance;
	newInd->pAppearance = pAppearance;
	newInd->mutProbLow = mutProbLow;
	newInd->mutProbHigh = mutProbHigh;
	newInd->mutProbWLow = mutProbWLow;
	newInd->mutProbWHigh = mutProbWHigh; 
	newInd->typeMutation = typeMutation;
	newInd->typeLocalSearch = typeLocalSearch;
	return newInd;
}

const TwoDPackingProblem* TwoDPack::twoDPackingProblem = NULL;
vector< pair<int, int> * >* TwoDPack::neighboursToExplore = NULL;
vector< pair<Position, Position> * >* TwoDPack::partnersToExplore = NULL;
int TwoDPack::nInds = 0;

//Comparacion por el valor V
bool cmpV(const AssignationCost *c1, const AssignationCost *c2){
	return (c1->getV() > c2->getV());
}

//Comparacion por la suma de V y VUpper (esto es una upperBound)
bool cmpUpperBound(const AssignationCost *c1, const AssignationCost *c2){
	return (c1->getUpperBound() > c2->getUpperBound());
}

// Comparacion por el valor VUpper
bool cmpBestPartner(const AssignationCost *c1, const AssignationCost *c2){
	return (c1->getVUpper() > c2->getVUpper());
}

Position::Position(const int x, const int y){
	this->x = x;
	this->y = y;
}

TwoDPackingProblem::TwoDPackingProblem(const string &fileName){
	ifstream file(fileName.c_str());
	if (!file.is_open()){
		cerr << "File " << fileName << " could not be opened" << endl;
		exit(-1);
	}
	//tamanios de la grid, y cantidad de numeros a usar
	file >> xSize >> ySize >> maxNumber;

	// las parejas que no aparecen tienen valor 0
	partnersValues.insert(partnersValues.begin(), maxNumber, vector<int>(maxNumber, 0) );
	bestValues.insert(bestValues.begin(), maxNumber, 0);
	partners.insert(partners.begin(), maxNumber, vector<int>());

	//leemos cada pareja y su valor
	while(true){
		int x, y, value;
		file >> x >> y >> value;
		if ( x != -1 ){
			if (x != y){
				if (partnersValues[x][y] == 0){//pueden aparecer (a,b) y (b,a), pero solo queremos incluir en partners(a) 1 vez a b
					partners[x].push_back(y);
					partners[y].push_back(x);
				}
				partnersValues[x][y] += value;
				partnersValues[y][x] += value;
			} else {
				partners[x].push_back(y);
				partnersValues[x][y] += value;
			}
			if (partnersValues[x][y] > bestValues[x]){
				bestValues[x] = partnersValues[x][y];
			}
			if (partnersValues[x][y] > bestValues[y]){
				bestValues[y] = partnersValues[x][y];
			}
		} else {
			break;
		}
	}
}

void TwoDPack::maintainOnlyObj() {
	maintainOnlyObjInternal();
	vector < vector < int > > appearanceEmpty; 
	vector < vector < int > > pAppearanceEmpty; 
	appearance.swap(appearanceEmpty);
	pAppearance.swap(pAppearanceEmpty);
}

