 /***********************************************************************************
 * AUTORES
 *    Jesica de Armas Adrian 
 *    
 * FECHA
 *    Junio 2010
 ************************************************************************************/

#include "TDCSP.h"
#include <vector>
#include <stack>
#include <set>
#include <algorithm>

typedef int (TDCSP::*hptr)(int, int, int);

// Direcciones de optimizacion
double* TDCSP::len;
double* TDCSP::wid;
double* TDCSP::prof;
double TDCSP::widSheet;
double TDCSP::lenSheet;

int TDCSP::sizeGen;		// Tamaño del gen
int TDCSP::totalSize;
int TDCSP::numPat;
int TDCSP::totalNumPat;
hptr TDCSP::heuristic[NHS];
int TDCSP::generatedInds;

/*------------------------------------------------------------------------------------------
 * Funcion auxiliar para la ordenacion de pieces segun su l
 *-------------------------------------------------------------------------------------------*/
bool cmpLengthOrd(const P_Order &p1, const P_Order &p2) {
        return (p1.len > p2.len);
}
/*------------------------------------------------------------------------------------------
 * Funcion auxiliar para la ordenacion de pieces segun su w
 *-------------------------------------------------------------------------------------------*/
bool cmpWidthOrd(const P_Order &p1, const P_Order &p2) {
        return (p1.wid > p2.wid);
}
/*------------------------------------------------------------------------------------------
 * Funcion auxiliar para la ordenacion de pieces segun su l
 *-------------------------------------------------------------------------------------------*/
bool cmpLength(const T_Order &p1, const T_Order &p2) {
	return (p1.l > p2.l);
}
/*------------------------------------------------------------------------------------------
 * Funcion auxiliar para la ordenacion de pieces segun su w
 *-------------------------------------------------------------------------------------------*/
bool cmpWidth(const T_Order &p1, const T_Order &p2) {
	return (p1.w > p2.w);
}
/*------------------------------------------------------------------------------------------
 * Funcion auxiliar para la ordenacion de pieces segun su area
 * -------------------------------------------------------------------------------------------*/
bool cmpArea(const T_Order &p1, const T_Order &p2) {
        return (p1.l*p1.w > p2.l*p2.w);
}
/*------------------------------------------------------------------------------------------
 * Funcion auxiliar para la ordenacion de pieces segun su perimetro
 * -------------------------------------------------------------------------------------------*/
bool cmpPerim(const T_Order &p1, const T_Order &p2) {
	return ((2*p1.l + 2*p1.w) > (2*p2.l + 2*p2.w));
}
/*------------------------------------------------------------------------------------------
 * Funcion auxiliar para la ordenacion de pieces segun su profit. De + a -
 * -------------------------------------------------------------------------------------------*/
bool cmpProf(const T_Order &p1, const T_Order &p2) {
	return (p1.prof > p2.prof);
}
/*------------------------------------------------------------------------------------------
 * Funcion auxiliar para la ordenacion de pieces segun su profit. De - a +
 * -------------------------------------------------------------------------------------------*/
bool cmpProfmin(const T_Order &p1, const T_Order &p2) {
	return (p1.prof < p2.prof);
}
/*------------------------------------------------------------------------------------------
 *  Clase auxiliar para la ordenacion de huecos
 * -------------------------------------------------------------------------------------------*/
class cmpOrder {

public:
	vector<T_Hole> *h;
	double param1, param2;
	int  tO;

	cmpOrder(vector<T_Hole> holes, int typeOrder) {
		h = &holes; 
		tO = typeOrder;
	}
	bool operator ()(const int p1,const int p2) {
		switch(tO) {
                case 0:
                case 1:
                        param1 = (*h)[p1].a;
                        param2 = (*h)[p2].a;
                        return (param1 > param2);
                case 2: param1 = (*h)[p1].a;
                        param2 = (*h)[p2].a;
                        return (param1 < param2);
                case 3: param1 = (*h)[p1].x;
                        param2 = (*h)[p2].x;
                        return (param1 < param2);
                case 4: param1 = (*h)[p1].y;
                        param2 = (*h)[p2].y;
                        return (param1 > param2);
        	}

	}
};
//--------------------------------------------------------------------------------
// Devuelve un entero entre 0 y maxValue - 1
// Para más información mirar en: man 3 rand
//--------------------------------------------------------------------------------
int TDCSP::uniformRandom(int maxValue) const {
        return (int) (rand() / ((RAND_MAX + 1.0) / maxValue));
}
//-------------------------------------------------------------------------------
// Fijar si el objetivo se debe maximizar o minimizar (indice i)
//-------------------------------------------------------------------------------
unsigned int TDCSP::getOptDirection(const int i) const{ 
	if (i == 0) 
		return MAXIMIZE;
	else	
		return MINIMIZE;     
}
// ---------------------------------------------------------------------------------
// Inicialización. 
// Fija:
//   - Numero de variables
//   - Numero de objetivos
//   - Direccion de optimizacion de los objetivos
//   - Rangos de las variables (mínimos y máximos)
// ---------------------------------------------------------------------------------
bool TDCSP::init (const vector<string> &params) {
	if (params.size() != 1) {
		cerr << "Error: TDCSP called with error number of parameters" << endl;
		return false;
	}
	if (!readFile(params[0])) 
		return false;
	
	setNumberOfVar(sizeGen); // vars number 
	setNumberOfObj(NOBJ);
	
	heuristic[0] = &TDCSP::NFDH;
	heuristic[1] = &TDCSP::FFDH;
	heuristic[2] = &TDCSP::BFDH;
	
	generatedInds = 0;

	return true;
}

//----------------------------------------------------------------------------------
// Reads a problem from file
// ---------------------------------------------------------------------------------
bool TDCSP::readFile(const string filename) {
	double* length;
	double* width;
	double* profit;
	int* number;
	int total;

	ifstream input(filename.c_str());
	if (!input.is_open()) {
		cerr << "Error: TDCSP called with error name of file" << endl;
		return false;
	}
	input >> numPat;
	length = new double[numPat];
  	width = new double[numPat];
	profit = new double[numPat];
	number = new int[numPat];
	

	input >> lenSheet;
	input >> widSheet;
	total = 0;
	for (int i = 0; i < numPat; i++) {
		input >> length[i];
		input >> width[i];
		input >> profit[i];
		input >> number[i];

		total += number[i];

		if (width[i] > widSheet) {
			cerr << " Error: width of pattern bigger than width of mother sheet "<< endl;
			return false;
		}
		if (length[i] > lenSheet) {
			cerr << " Error: length of pattern bigger than length of mother sheet "<< endl;
			return false;
		}
	}
	input.close();

	totalNumPat = total;
	
	totalSize = totalNumPat * 2 - 1;
	sizeGen = totalNumPat * 3; //numPat estaba puesto antes

	len = new double[totalNumPat];
	wid = new double[totalNumPat];
	prof = new double[totalNumPat];

	total = 0;
	for (int i = 0; i < numPat; i++) {
		for (int j = total; j < (total + number[i]); j++) {
			len[j] =  length[i];
			wid[j] = width[i];
			prof[j] = profit[i];
		}
		total += number[i];		
	}

	delete(length);
	delete(width);
	delete(profit);
	delete(number);

	return true;
}

//--------------------------------------------------------------------------------
bool TDCSP::improveOne(Individual *ind, int objIndex) {
        if (getObj(objIndex) <= ind->getObj(objIndex))
                return true;
        else
                return false;
}

// -------------------------------------------------------------------------------
// Generation of an individual
// -------------------------------------------------------------------------------
void TDCSP::restart() {
	//fixSolution();
	randomRestart();
}
// -------------------------------------------------------------------------------
// Random generation of an individual
// -------------------------------------------------------------------------------
void TDCSP::randomRestart() {
	int h, ord, num_pieces = 0;
	int used_pieces = 0;
	int i = 0;

	numHS = 0;
	size = 0;
	while (used_pieces < totalNumPat) {
		num_pieces = 1 + uniformRandom(totalNumPat - used_pieces);
		used_pieces += num_pieces;
		h = uniformRandom(NHS*NORDER);
		numHS++;
		ord = uniformRandom(NORDERP);
		setVar(i, h);
		i++;
		setVar(i, num_pieces);
		i++;
		setVar(i, ord);
		i++;
	}
	size = numHS * 3;
}
//-------------------------------------------------------------------------------
// Mutacion 
// -- Se eligen 2 elementos de la cadena, p1 y p2 (donde p1 esta mas a la izquierda)
//
//		- Si ambos son piezas u operadores, se intercambian.
//		- Si p1 es un operador y p2 pieza, se intercambian.
//		- Si p1 es pieza y p2 operador, se intercambian solo cuando se cumpla:
//					1 <= no <= np - 1
//		  Se debe de seguir cumpliendo después del intercambio.
//
// -- Se elige un operador aleatorio y se cambia basándose en la probabilidad de mutación.
//------------------------------------------------------------------------------							
void TDCSP::dependentMutation (double pm) {
		//cout << "mutac" << endl;
		 mutation_moves(pm);
		//cout << "sale mutac" << endl;
		//cleanInd(this);
}
// -------------------------------------------------------------------------------
// Mutacion basada en la aplicacion de los tres tipos de movimientos
// -------------------------------------------------------------------------------
void TDCSP::mutation_moves (double pm) {       
	// Mutation fuerte
	double prob = rand() / (RAND_MAX + 1.0);
	if (prob < pm) 
               mutation_add (pm, -1);
	prob = rand() / (RAND_MAX + 1.0);
	if (prob < pm) 
              mutation_remove (pm, -1);

	prob = rand() / (RAND_MAX + 1.0);
	if (prob < pm) 
              mutation_replace (pm, -1);
}
// -------------------------------------------------------------------------------
// Mutacion basada en la aplicacion de los tres tipos de movimientos
// La suma/resta de piezas se hace con heuristicas aleatorias (no tienen por que
// ser las contiguas a la incluida/eliminada)
// -------------------------------------------------------------------------------
void TDCSP::mutation_moves_random (double pm) {       
	double prob = rand() / (RAND_MAX + 1.0);
	if (prob < pm) 
               mutation_add_random (pm, -1);
	prob = rand() / (RAND_MAX + 1.0);
	if (prob < pm) 
               mutation_remove_random (pm, -1);
	prob = rand() / (RAND_MAX + 1.0);
	if (prob < pm) 
               mutation_replace (pm, -1);
}
// -------------------------------------------------------------------------------
// Movimiento ADD:
//  - Genera aleatoriamente una posición dentro de la codificación
//  - Elige aleatoriamente una heuristica, numero de piezas, ordenacion de piezas 
//      (dentro del intervalo de piezas que usa la siguiente heuristica)
//  - Introduce esa heuristica-piezas en el gen, desplazando el resto del gen.
// -------------------------------------------------------------------------------
void TDCSP::mutation_add (double pm, int pos) {
	if (pos == -1)
           pos = uniformRandom(numHS); 
        int init_pos = 3 * pos;
        int new_h = uniformRandom(NHS * NORDER);
	int old_p = (int) getVar(init_pos + 1);
        int new_p = 1 + uniformRandom(old_p);
        int new_ord = uniformRandom(NORDERP);
	
        if (new_p == old_p) {
                setVar (init_pos, new_h);
		setVar (init_pos + 2, new_ord);
	}
        else {
                for (int i = (numHS -1) * 3; i >= init_pos; i -=3) { 
                        setVar (i + 3, getVar(i));
                        setVar (i + 4, getVar(i + 1));
                        setVar (i + 5, getVar(i + 2));
			
		}
                setVar (init_pos, new_h);
                setVar (init_pos + 1, new_p);
		setVar (init_pos + 2, new_ord);
				
                setVar (init_pos + 4, old_p - new_p);
                numHS++;
		size = numHS*3;
        }
}
// -------------------------------------------------------------------------------
// Movimiento ADD:
//  - Genera aleatoriamente una posición dentro de la codificación
//  - Elige aleatoriamente una posicion a la que restarle las piezas que necesite 
//  - Elige aleatoriamente una heuristica, numero y ordenacion de piezas
//      (dentro del intervalo de piezas que usa la heuristica de la pos seleccionada)
//  - Introduce esa heuristica-piezas-ord en el gen, desplazando el resto del gen.
// -------------------------------------------------------------------------------
void TDCSP::mutation_add_random (double pm, int pos) {
	// Generar posicion en la que introducir nueva heuristica y a la que restar piezas
	if (pos == -1)
        	pos = uniformRandom(numHS); 
	int pos_sub = uniformRandom(numHS);
        int init_pos = 3 * pos;
        int new_h = uniformRandom(NHS * NORDER);
        int new_p = 1 + uniformRandom((int)(getVar(3 * pos_sub + 1)));
	int new_ord = uniformRandom(NORDERP);

        /*for (int i = numHS * 3 - 1; i >= init_pos; i--)
                setVar (i + 3, getVar(i));*/

	for (int i = (numHS - 1) * 3; i >= init_pos; i -= 3) {
		setVar (i + 3, getVar(i));
		setVar (i + 4, getVar(i + 1));
		setVar (i + 5, getVar(i + 2));
	}
	
	setVar (init_pos, new_h);
	setVar (init_pos + 1, new_p);
        setVar (init_pos + 2, new_ord);
	
	if (pos_sub >= pos)
		pos_sub++; 
        setVar (3 * pos_sub + 1, getVar(3 * pos_sub + 1) - new_p);
        numHS++;

	// Eliminar heuristica auxiliar si le hemos quitado todas sus piezas
        if ((int) getVar(3 * pos_sub + 1) == 0) {
                for (int i = 3 * pos_sub; i < 3 * (numHS - 1); i++)
               		setVar (i, getVar(i + 3));
                numHS--;
	}
	size = numHS*3;
}
// -------------------------------------------------------------------------------
// Movimiento REMOVE:
//  - Genera aleatoriamente una posición dentro de la codificación
//  - Elimina esa heurística-piezas-ord-orient
//  - Las piezas que elimina de esa heurística debe sumárselas a la siguiente o 
//      a la anterior en su defecto
//  - Para eliminarla compacta la codificación
// -------------------------------------------------------------------------------
void TDCSP::mutation_remove (double pm, int pos) {
	if (numHS > 1) {
		if (pos == -1)
		         pos = uniformRandom(numHS);
                int init_pos = 3 * pos;

                if (pos == numHS - 1)
                        setVar(init_pos - 2, getVar(init_pos + 1) + getVar(init_pos - 2));
                else {
                        setVar(init_pos + 4, getVar(init_pos + 1) + getVar(init_pos + 4));
                        for (int i = init_pos; i < (numHS - 1) * 3; i+=3) {
                                setVar (i, getVar(i + 3));
                                setVar (i+1, getVar(i + 4));
                                setVar (i+2, getVar(i + 5));
			}
                }
                numHS--;
		size = numHS*3;
        }
}
// -------------------------------------------------------------------------------
// Movimiento REMOVE:
//  - Genera aleatoriamente una posición dentro de la codificación
//  - Elimina esa heurística-piezas-ord
//  - Las piezas que elimina de esa heurística debe sumárselas a una posicion
//    elegida al azar  
//  - Para eliminarla compacta la codificación
// -------------------------------------------------------------------------------
void TDCSP::mutation_remove_random (double pm, int pos) {
        if (numHS > 1) {
		if (pos == -1)
			pos = uniformRandom(numHS);
                int init_pos = 3 * pos;

		int pieces = (int) (getVar(init_pos + 1));
		

                for (int i = init_pos; i < (numHS - 1) * 3; i+=3) {
			setVar (i, getVar(i + 3));
			setVar (i+1, getVar(i + 4));
			setVar (i+2, getVar(i + 5));
		}
                numHS--;
		size = numHS*3;
		int pos_add = uniformRandom(numHS);
		setVar(3 * pos_add + 1, getVar(3 * pos_add + 1) + pieces);
        }
}
// -------------------------------------------------------------------------------
// Movimiento REPLACE:
//  - Genera aleatoriamente una posición dentro de la codificación
//  - Cambia la heurística de esa heurística-piezas-orden
// -------------------------------------------------------------------------------
void TDCSP::mutation_replace (double pm, int pos) {
   if (pos == -1)
      pos = uniformRandom(numHS);
   int init_pos = 3 * pos;
   int new_h;
   do {
      new_h = uniformRandom(NHS * NORDER);
   } while (new_h == getVar(init_pos));
   setVar (init_pos, new_h);
}
// -------------------------------------------------------------------------------
// Cruce:
//  - One-point crossover
//  - Two-point crossover
// -------------------------------------------------------------------------------
void TDCSP::dependentCrossover (Individual* ind) {
	//cout << "cross " << numHS << endl;
        //crossover_onepoint_h (ind);
        //crossover_twopoint_h (ind);
        //crossover_onepoint_p (ind);
        crossover_twopoint_p (ind);

	//cleanInd(this);
	//cleanInd(ind);
	//cout << "sale cross " << numHS << endl;
}
// -------------------------------------------------------------------------------
// One-point crossover
//  - Se elige un punto del gen de menor longitud 
//  - Se intercambian las dos segundas mitades de las codificaciones
//  - Se comprueba que el número total de piezas utilizadas no exceda a las 
//      realmente disponibles 
// -------------------------------------------------------------------------------
void TDCSP::crossover_onepoint_h (Individual* ind) {
        int old_p_this = 0, new_p_this = 0, old_p_ind = 0, new_p_ind = 0;
        int old_h_this, old_h_ind;
	  
        if ((numHS > 1) && (((TDCSP*) ind)->numHS > 1)) {
                int min_numHS = (numHS <= ((TDCSP*) ind)->numHS)? numHS: ((TDCSP*) ind)->numHS;
                int pos = 1 + uniformRandom(min_numHS - 1);

                // Se calculan la longitud de las partes a intercambiar
                old_h_this = numHS - pos;
                old_h_ind = ((TDCSP*) ind)->numHS - pos;

                // Se hace copia de las partes de los individuos que se van a intercambiar
                vector<int> copy_this(old_h_this * 3);
                vector<int> copy_ind(old_h_ind * 3);
                for (int i = 0; i < old_h_this; i++) {
                        copy_this[i * 3] = (int) getVar((pos + i) * 3);
                        copy_this[i * 3 + 1] = (int) getVar((pos + i) * 3 + 1);
                        copy_this[i * 3 + 2] = (int) getVar((pos + i) * 3 + 2);
                        old_p_this += (int) getVar((pos + i) * 3 + 1);
                }
                for (int i = 0; i < old_h_ind; i++) {
                        copy_ind[i * 3] = (int) ind->getVar((pos + i) * 3);
                        copy_ind[i * 3 + 1] = (int) ind->getVar((pos + i) * 3 + 1);
                        copy_ind[i * 3 + 2] = (int) ind->getVar((pos + i) * 3 + 2);
                        old_p_ind += (int) ind->getVar((pos + i) * 3 + 1);
                }
                numHS = ((TDCSP*) ind)->numHS = pos;

                // Se intercambia desde this a ind
                for (int i = 0; i < old_h_this; i++) {
                        ind->setVar(((TDCSP*) ind)->numHS * 3, copy_this[i * 3]);
                        ind->setVar(((TDCSP*) ind)->numHS * 3 + 1, copy_this[i * 3 + 1]);
                        ind->setVar(((TDCSP*) ind)->numHS * 3 + 2, copy_this[i * 3 + 2]);
                        ((TDCSP*) ind)->numHS++;
                        new_p_ind += copy_this[i * 3 + 1];
                        if (new_p_ind == old_p_ind)
                                break;
                        if (new_p_ind > old_p_ind) {
                                ind->setVar((((TDCSP*) ind)->numHS - 1) * 3 + 1, copy_this[i * 3 + 1] - (new_p_ind - old_p_ind));
                                break;
                        }
                }
                if (new_p_ind < old_p_ind)
                   ind->setVar((((TDCSP*) ind)->numHS - 1) * 3 + 1, ind->getVar((((TDCSP*) ind)->numHS - 1) * 3 + 1) + (old_p_ind - new_p_ind));

                // Se intercambia desde ind a this
                for (int i = 0; i < old_h_ind; i++) {
                        setVar(numHS * 3, copy_ind[i * 3]);
                        setVar(numHS * 3 + 1, copy_ind[i * 3 + 1]);
                        setVar(numHS * 3 + 2, copy_ind[i * 3 + 2]);
                        numHS++;
                        new_p_this += copy_ind[i * 3 + 1];
                        if (new_p_this == old_p_this)
                                break;
                        if (new_p_this > old_p_this) {
                                setVar((numHS - 1) * 3 + 1, copy_ind[i * 3 + 1] - (new_p_this - old_p_this));
                                break;
                        }
                }
                if (new_p_this < old_p_this)
                   setVar((numHS - 1) * 3 + 1, getVar((numHS - 1) * 3 + 1) + (old_p_this - new_p_this));
	}
}
// -------------------------------------------------------------------------------
// Two-point crossover
//  - Se eligen dos puntos del gen de menor longitud 
//  - Se intercambian las dos mitades centrales de las codificaciones
//  - Se comprueba que el número total de piezas utilizadas no exceda a las 
//      realmente disponibles 
// -------------------------------------------------------------------------------
void TDCSP::crossover_twopoint_h (Individual* ind) {
        int old_p_this = 0, new_p_this = 0, old_p_ind = 0, new_p_ind = 0;
        int old_h_this, old_h_ind;

        if ((numHS >= 3) && (((TDCSP*) ind)->numHS >= 3)) {
                int min_numHS = (numHS <= ((TDCSP*) ind)->numHS)? numHS: ((TDCSP*) ind)->numHS;
                int pos1 = 1 + uniformRandom(min_numHS - 1);
                int pos2 = 1 + uniformRandom(min_numHS - 1);
                while (pos2 == pos1) {
                   pos2 = 1 + uniformRandom(min_numHS - 1);
					 }
                if (pos1 > pos2) {
                        int tmp = pos1;
                        pos1 = pos2;
                        pos2 = tmp;
                }

                // Se hace copia de las partes de los individuos que se van a intercambiar
                int size_cross = (pos2 - pos1);
                vector<int> copy_this(size_cross * 3);
                vector<int> copy_ind(size_cross * 3);
                for (int i = 0; i < size_cross; i++) {
                        copy_this[i * 3] = (int) getVar((pos1 + i) * 3);
                        copy_this[i * 3 + 1] = (int) getVar((pos1 + i) * 3 + 1);
                        copy_this[i * 3 + 2] = (int) getVar((pos1 + i) * 3 + 2);
                        old_p_this += (int) getVar((pos1 + i) * 3 + 1);
                        copy_ind[i * 3] = (int) ind->getVar((pos1 + i) * 3);
                        copy_ind[i * 3 + 1] = (int) ind->getVar((pos1 + i) * 3 + 1);
                        copy_ind[i * 3 + 2] = (int) ind->getVar((pos1 + i) * 3 + 2);
                        old_p_ind += (int) ind->getVar((pos1 + i) * 3 + 1);
                }
                old_h_this = numHS;
                old_h_ind = ((TDCSP*) ind)->numHS;
                numHS = ((TDCSP*) ind)->numHS = pos1;

                // Se intercambia la mitad central desde this a ind
                for (int i = 0; i < size_cross; i++) {
                        ind->setVar(((TDCSP*) ind)->numHS * 3, copy_this[i * 3]);
                        ind->setVar(((TDCSP*) ind)->numHS * 3 + 1, copy_this[i * 3 + 1]);
                        ind->setVar(((TDCSP*) ind)->numHS * 3 + 2, copy_this[i * 3 + 2]);
                        ((TDCSP*) ind)->numHS++;
                        new_p_ind += copy_this[i * 3 + 1];
                        if (new_p_ind == old_p_ind)
                                break;
                        if (new_p_ind > old_p_ind) {
                                ind->setVar((((TDCSP*) ind)->numHS - 1) * 3 + 1, copy_this[i * 3 + 1] - (new_p_ind - old_p_ind));
                                break;
                        }
                }
                if (new_p_ind < old_p_ind)
                   ind->setVar((((TDCSP*) ind)->numHS - 1) * 3 + 1, ind->getVar((((TDCSP*) ind)->numHS - 1) * 3 + 1) + (old_p_ind - new_p_ind));
                if (((TDCSP*) ind)->numHS < pos2) {
                        for (int i = pos2; i < old_h_ind; i++) {
                                ind->setVar(((TDCSP*) ind)->numHS * 3, ind->getVar(i * 3));
                                ind->setVar(((TDCSP*) ind)->numHS * 3 + 1, ind->getVar(i * 3 + 1));
                                ((TDCSP*) ind)->numHS++;
                        }
                } else 
						((TDCSP*) ind)->numHS = old_h_ind;

                // Se intercambia la mitad central desde ind a this
                for (int i = 0; i < size_cross; i++) {
                        setVar(numHS * 3, copy_ind[i * 3]);
                        setVar(numHS * 3 + 1, copy_ind[i * 3 + 1]);
                        setVar(numHS * 3 + 2, copy_ind[i * 3 + 2]);
                        numHS++;
                        new_p_this += copy_ind[i * 3 + 1];
                        if (new_p_this == old_p_this)
                                break;
                        if (new_p_this > old_p_this) {
                                setVar((numHS - 1) * 3 + 1, copy_ind[i * 3 + 1] - (new_p_this - old_p_this));
                                break;
                        }
                }
                if (new_p_this < old_p_this)
                   setVar((numHS - 1) * 3 + 1, getVar((numHS - 1) * 3 + 1) + (old_p_this - new_p_this));
                if (numHS < pos2) {
                        for (int i = pos2; i < old_h_this; i++) {
                                setVar(numHS * 3, getVar(i * 3));
                                setVar(numHS * 3 + 1, getVar(i * 3 + 1));
                                setVar(numHS * 3 + 2, getVar(i * 3 + 2));
                                numHS++;
                        }
                } else
			numHS = old_h_this;
	}
}
// -------------------------------------------------------------------------------
// One-point crossover
//  - Se elige un punto del gen en el que se acumulen un número determinado de piezas 
//  - Si no coincide exacto, se divide la heurística correspondiente en dos partes
//  - Se intercambian las dos segundas mitades de las codificaciones
// -------------------------------------------------------------------------------
void TDCSP::crossover_onepoint_p (Individual* ind) {
        int counted_pieces_this = 0, counted_pieces_ind = 0, index_h_this = 0, index_h_ind = 0;

        // Generar el punto de corte
	     int pieces = 1 + uniformRandom(numPat - 1);

        // Encontrar los puntos de particion
        while (counted_pieces_this < pieces) {
                counted_pieces_this += (int) getVar(index_h_this * 3 + 1);
                index_h_this++;
        }
        while (counted_pieces_ind < pieces) {
                counted_pieces_ind += (int) ind->getVar(index_h_ind * 3 + 1);
                index_h_ind++;
        }
		  
        // Hacer el intercambio de las dos mitades
        int size_tmp = numHS - index_h_this;
        if (counted_pieces_this > pieces)
                size_tmp++;
        vector<int> tmp(3 * size_tmp);
        int i = 0;

        // Se comprueba si hay que desdoblar una heurística
        if (counted_pieces_this > pieces) {
                tmp[i++] = (int) getVar(3 * (index_h_this - 1));
                tmp[i++] = counted_pieces_this - pieces;
                setVar(3 * (index_h_this - 1) + 1, getVar(3 * (index_h_this - 1) + 1) - tmp[i - 1]);
		tmp[i++] = (int) getVar(3 * (index_h_this - 1) + 2);   // no tengo claro
        }
        for (int j = index_h_this; j < numHS; j++) {
                tmp[i++] = (int) getVar (3 * j);
                tmp[i++] = (int) getVar (3 * j + 1);
                tmp[i++] = (int) getVar (3 * j + 2);
        }
        numHS = index_h_this;

        // Copiar la mitad de ind a this
        if (counted_pieces_ind > pieces) {
                setVar(3 * index_h_this, ind->getVar(3 * (index_h_ind - 1)));
                setVar(3 * index_h_this + 1, counted_pieces_ind - pieces);
                ind->setVar(3 * (index_h_ind - 1) + 1, ind->getVar(3 * (index_h_ind - 1) + 1) - (counted_pieces_ind - pieces));
		index_h_this++;
		setVar(3 * index_h_this + 2, ind->getVar(3 * (index_h_ind - 1) + 2)); // no tengo claro
        }
        for (int j = index_h_ind; j < ((TDCSP*) ind)->numHS; j++) {
                setVar(3 * index_h_this, ind->getVar(3 * j));
                setVar(3 * index_h_this + 1, ind->getVar(3 * j + 1));
                setVar(3 * index_h_this + 2, ind->getVar(3 * j + 2));
                index_h_this++;
        }
	numHS = index_h_this;

        // Copiar la mitad de this (almacenado en tmp) a ind
        for (int j = 0; j < size_tmp; j++) {
                ind->setVar(3 * index_h_ind, tmp[3 * j]);
                ind->setVar(3 * index_h_ind + 1, tmp[3 * j + 1]);
                ind->setVar(3 * index_h_ind + 2, tmp[3 * j + 2]);
                index_h_ind++;
        }
        ((TDCSP*) ind)->numHS = index_h_ind;
}
// -------------------------------------------------------------------------------
// Two-point crossover
//  - Se eligen dos puntos del gen en el que se acumulen un determinado número de 
//      piezas
//  - Si el número de piezas no coincide con las particiones de las heurísticas,
//      se desdobla el gen como corresponda 
//  - Se intercambian las dos mitades centrales de las codificaciones
// -------------------------------------------------------------------------------
void TDCSP::crossover_twopoint_p (Individual* ind) {
      	int index_h_this = 0, size_this = 0;
        int index_h_ind = 0, size_ind = 0;
        int counted_pieces_this = 0;
        int counted_pieces_ind = 0;
        int disp;

        // Generar los dos puntos de corte
        int pieces1 = 1 + uniformRandom(totalNumPat - 1);
        int pieces2 = 1 + uniformRandom(totalNumPat - pieces1 - 1);

        // Encontrar primer punto de particion dentro de this
        while (counted_pieces_this < pieces1) {
                counted_pieces_this += (int) getVar(index_h_this * 3 + 1);
                index_h_this++;
		//cout << "NUMERO PIEZAS " << pieces1 << " counted: "<< counted_pieces_this << endl;
        }
	
	// Desdoblar heuristica si es necesario
	if (counted_pieces_this > pieces1) {
                for (int i = numHS - 1; i >= (index_h_this -1); i--) {
                       setVar((i + 1) * 3, (int) getVar(i * 3));          		
                       setVar((i + 1) * 3 + 1, (int) getVar(i * 3 + 1));          		
                       setVar((i + 1) * 3 + 2, (int) getVar(i * 3 + 2));          		
		}
		
		int dif = counted_pieces_this - pieces1;
                setVar(index_h_this * 3 + 1, dif);   
                setVar((index_h_this - 1) * 3 + 1, (int) getVar((index_h_this - 1) * 3 + 1) - dif);          
		numHS++;
        }	 

        // Encontrar primer punto de particion dentro de ind
        while (counted_pieces_ind < pieces1) {
                counted_pieces_ind += (int) ind->getVar(index_h_ind * 3 + 1);
                index_h_ind++;
        }
	// Desdoblar heuristica si es necesario
	if (counted_pieces_ind > pieces1) {
                for (int i = ((TDCSP *)ind)->numHS - 1; i >= (index_h_ind - 1); i--) {
                       ind->setVar((i + 1) * 3, (int) ind->getVar(i * 3));          		
                       ind->setVar((i + 1) * 3 + 1, (int) ind->getVar(i * 3 + 1));          		
                       ind->setVar((i + 1) * 3 + 2, (int) ind->getVar(i * 3 + 2));          		
		}
                ind->setVar(index_h_ind * 3 + 1, counted_pieces_ind - pieces1);          		
                ind->setVar((index_h_ind - 1) * 3 + 1, (int) ind->getVar((index_h_ind - 1) * 3 + 1) - (counted_pieces_ind - pieces1));  
		((TDCSP *)ind)->numHS++;
        }	 

        // Encontrar segundo punto de particion dentro de this
	counted_pieces_this = 0;
        while (counted_pieces_this < pieces2) {
                counted_pieces_this += (int) getVar((index_h_this + size_this) * 3 + 1);
                size_this++;
        }
	// Desdoblar heuristica si es necesario
	if (counted_pieces_this > pieces2) {
                for (int i = numHS - 1; i >= index_h_this + size_this - 1; i--) {
                       setVar((i + 1) * 3, getVar(i * 3));          		
                       setVar((i + 1) * 3 + 1, getVar(i * 3 + 1));          		
                       setVar((i + 1) * 3 + 2, getVar(i * 3 + 2));          		
		}
                setVar((index_h_this + size_this) * 3 + 1, counted_pieces_this - pieces2);          		
                setVar((index_h_this + size_this - 1) * 3 + 1, (int) getVar((index_h_this + size_this - 1) * 3 + 1) - (counted_pieces_this - pieces2));          
		numHS++;
        }	 
        // Encontrar segundo punto de particion dentro de ind
	counted_pieces_ind = 0;
        while (counted_pieces_ind < pieces2) {
                counted_pieces_ind += (int) ind->getVar((index_h_ind + size_ind) * 3 + 1);
                size_ind++;
        }
	// Desdoblar heuristica si es necesario
	if (counted_pieces_ind > pieces2) {
                for (int i = ((TDCSP *)ind)->numHS - 1; i >= index_h_ind + size_ind - 1; i--) {
                       ind->setVar((i + 1) * 3, ind->getVar(i * 3));          		
                       ind->setVar((i + 1) * 3 + 1, ind->getVar(i * 3 + 1));          		
                       ind->setVar((i + 1) * 3 + 2, ind->getVar(i * 3 + 2));          		
		}
                ind->setVar((index_h_ind + size_ind) * 3 + 1, counted_pieces_ind - pieces2);          		
                ind->setVar((index_h_ind + size_ind - 1) * 3 + 1, ind->getVar((index_h_ind + size_ind - 1) * 3 + 1) - (counted_pieces_ind - pieces2));  
		(((TDCSP *)ind)->numHS)++;
        }	 

        // Hacer copia de la parte de this a intercambiar
        vector<int> tmp_this(3 * size_this);
        for (int i = 0; i < size_this; i++) {
                tmp_this[i * 3] = (int) getVar((index_h_this + i) * 3);
                tmp_this[i * 3 + 1] = (int) getVar((index_h_this + i) * 3 + 1);
                tmp_this[i * 3 + 2] = (int) getVar((index_h_this + i) * 3 + 2);
        }

        // Hacer copia de la parte de ind a intercambiar
        vector<int> tmp_ind(3 * size_ind);
        for (int i = 0; i < size_ind; i++) {
                tmp_ind[i * 3] = (int) ind->getVar((index_h_ind + i) * 3);
                tmp_ind[i * 3 + 1] = (int) ind->getVar((index_h_ind + i) * 3 + 1);
                tmp_ind[i * 3 + 2] = (int) ind->getVar((index_h_ind + i) * 3 + 2);
        }

        // Desplazar this lo que sea necesario
        disp = size_ind - size_this;
        if (disp > 0) {  // Desplazar a la derecha
                for (int j = numHS - 1; j >= index_h_this + size_this; j--) {
                        setVar(3 * (j + disp), getVar(3 * j));
                        setVar(3 * (j + disp) + 1, getVar(3 * j + 1));
                        setVar(3 * (j + disp) + 2, getVar(3 * j + 2));
                }
        } else if (disp < 0) {  // Desplazar a la izquierda
                for (int j = index_h_this + size_this; j < numHS; j++) {
                        setVar(3 * (j + disp), getVar(3 * j));
                        setVar(3 * (j + disp) + 1, getVar(3 * j + 1));
                        setVar(3 * (j + disp) + 2, getVar(3 * j + 2));
                }
        }

        // Calcular nuevo tamaño de this
        numHS += disp;

        // Copiar de tmp_ind a this 
        for (int i = 0; i < size_ind; i++) {
                setVar((index_h_this + i) * 3, tmp_ind[3 * i]);
                setVar((index_h_this + i) * 3 + 1, tmp_ind[3 * i + 1]);
                setVar((index_h_this + i) * 3 + 2, tmp_ind[3 * i + 2]);
	}

        // Desplazar ind lo que sea necesario
        disp = size_this - size_ind;
        if (disp > 0) {  // Desplazar a la derecha
                for (int j = ((TDCSP*) ind)->numHS - 1; j >= index_h_ind; j--) {
                        ind->setVar(3 * (j + disp), ind->getVar(3 * j));
                        ind->setVar(3 * (j + disp) + 1, ind->getVar(3 * j + 1));
                        ind->setVar(3 * (j + disp) + 2, ind->getVar(3 * j + 2));
                }
        } else if (disp < 0) {  // Desplazar a la izquierda
                for (int j = index_h_ind + size_ind; j < ((TDCSP*) ind)->numHS; j++) {
                        ind->setVar(3 * (j + disp), ind->getVar(3 * j));
                        ind->setVar(3 * (j + disp) + 1, ind->getVar(3 * j + 1));
                        ind->setVar(3 * (j + disp) + 2, ind->getVar(3 * j + 2));
                }
        }

        // Calcular nuevo tamaño de ind
        ((TDCSP*) ind)->numHS += disp;

        // Copiar de tmp_this a ind
        for (int i = 0; i < size_this; i++) {
                ind->setVar((index_h_ind + i) * 3, tmp_this[3 * i]);
                ind->setVar((index_h_ind + i) * 3 + 1, tmp_this[3 * i + 1]);
                ind->setVar((index_h_ind + i) * 3 + 2, tmp_this[3 * i + 2]);
        }
	 tmp_this.clear();
	 tmp_ind.clear();
	 
}
// -------------------------------------------------------------------------------
// Limpia la codificacion de un individuo 
//  - A partir de la ultima pareja heuristica-piezas mostrara ceros
// -------------------------------------------------------------------------------
void TDCSP::cleanInd(Individual *ind) {
        for (int i = 3 * ((TDCSP *)ind)->numHS; i < ind->getNumberOfVar(); i++) {
      		ind->setVar(i, 0);
        }
}
//-------------------------------------------------------------------------------
// Evalua una construccion guillotinable
// ------------------------------------------------------------------------------
void TDCSP::calculateProfitCuts () {
	int var, a, b, c;
	double lengthA, lengthB, widthA, widthB;
	double  *l, *w;
	int nc = 0;
	int numNewP = totalNumPat/2;      
	stack<int> st;
	double total_profit = 0;	

 	l = new double[numNewP];
   	w = new double[numNewP];
	 
	c = totalNumPat;
	
	for (int i = 0; i < itFinal; i++) {
		var = notation[i];
	
		if (var != V && var != H) {
			st.push(var);
			total_profit += prof[var];
		}
		else {
			nc++;
			a = st.top();
			st.pop();
			b = st.top();
			st.pop();

			if (a >= totalNumPat) { 		// es una pieza creada por concatenacion
				lengthA = l[a-totalNumPat];
				widthA = w[a-totalNumPat];
			}
			else {				// es una pieza original
				lengthA = len[a];
				widthA = wid[a];
			}
			if (b >= totalNumPat) {		// es una pieza creada por concatenación
				lengthB = l[b-totalNumPat];
				widthB = w[b-totalNumPat];                                                                                                  
			}
			else {                         // es una pieza original                
				lengthB = len[b];
				widthB = wid[b]; 
			}
			
			if (var == H) {
				l[c - totalNumPat] = lengthA + lengthB;
				w[c - totalNumPat] = max(widthA, widthB);
				if (widthA != widthB)
		                        nc++;
			}
			else {
				l[c - totalNumPat] = max(lengthA, lengthB);
				w[c - totalNumPat] = widthA + widthB;
				if (lengthA != lengthB)
		                        nc++;
			}
			
			st.push(c);
			c++;
		}
	}
	a = st.top();
	st.pop();
	
	if (a >= totalNumPat) {
		lengthA = l[a-totalNumPat];
	 	widthA = w[a-totalNumPat];
	}
	else {
		lengthA = len[a];
		widthA = wid[a];
	}


	setObj(0, total_profit);
	
	if (widthA != widSheet)
		nc += 1;
	if (lengthA != lenSheet)
		nc += 1;
	
	setObj(1, nc);

	delete(l);
	delete(w);
}
//-------------------------------------------------------------------------------
// Apila en vertical u horizontal las piezas sobrantes que se van a colocar de relleno
//-------------------------------------------------------------------------------
void TDCSP::pilUp(vector<P_Order> &piecesOrd, int it, int op) {
        int count = 0;
        int itfinal, total = 0;

        if (op == H) {
                itfinal = it;
                while (itfinal >= 0 && total + piecesOrd[itfinal].wid <= widSheet){
                        total += piecesOrd[itfinal].wid;
                        notation[size] = piecesOrd[itfinal].pat;
                        size++;
                        count++;
                        itfinal--;
                }
                for (int j = 1; j < count; j++) {
                        notation[size] = V;
                        size++;
                }
                notation[size] = op;
                size++;
                piecesOrd.erase(piecesOrd.begin()+itfinal+1, piecesOrd.begin()+it+1);
        }
        else {
                itfinal = it;
                while (itfinal >= 0 && total + piecesOrd[itfinal].len <= lenSheet){
                        total += piecesOrd[itfinal].len;
                        notation[size] = piecesOrd[itfinal].pat;
                        size++;
                        count++;
                        itfinal--;
                }
                for (int j = 1; j < count; j++) {
                        notation[size] = H;
                        size++;
                }
                notation[size] = op;
                size++;
                piecesOrd.erase(piecesOrd.begin() + itfinal + 1, piecesOrd.begin() + it + 1);
        }
}
// -------------------------------------------------------------------------------
//  Calcula l y w
//  Devuelve el largo (0) o el ancho (1) sobrante de la materia prima
// -------------------------------------------------------------------------------
int TDCSP::getRemainSize(int dimension) {
        int var, a, b, c;
        double lengthA, lengthB, widthA, widthB;
        double  *l, *w;
        int numNewP = size; // /2;
        stack<int> st;

  	//cout << "dentro de getRemain" << endl; 
        l = new double[numNewP];
        w = new double[numNewP];

        c = totalNumPat;

        for (int i = 0; i < size; i++) {
  		//cout << "detro de getRemain " << i << " size " << size<< endl; 
                var = notation[i];
  		//cout << "dentro2 de getRemain " << i << endl; 

                if (var != V && var != H)
                        st.push(var);
                else {
                        a = st.top();
                        st.pop();
                        b = st.top();
                        st.pop();
  			//cout << "dentro3 de getRemain " << endl; 
                        if (a >= totalNumPat) {                 // es una pieza creada por concatenacion
                                lengthA = l[a-totalNumPat];
                                widthA = w[a-totalNumPat];
                        }
                        else {                  // es una pieza original
                                lengthA = len[a];
                                widthA = wid[a];
                        }
  			//cout << "dentro4 de getRemain " << endl; 
                        if (b >= totalNumPat) {         // es una pieza creada por concatenación
                                lengthB = l[b-totalNumPat];
                                widthB = w[b-totalNumPat];
                        }
                        else {                                  // es una pieza original
                                lengthB = len[b];
                                widthB = wid[b];
                        }

  			//cout << "dentro5 de getRemain " << endl; 
                        if (var == H) {
                                l[c - totalNumPat] = lengthA + lengthB;
                                w[c - totalNumPat] = max(widthA, widthB);
                        }
                        else {
                                l[c - totalNumPat] = max(lengthA, lengthB);
                                w[c - totalNumPat] = widthA + widthB;
                        }

  			//cout << "dentro6 de getRemain " << endl; 
                        if (l[c - totalNumPat] > lenSheet) {
                                delete(l);
                                delete(w);
                                return 0;
			}
                        if (w[c - totalNumPat] > widSheet) {
                                delete(l);
                                delete(w);
                                return 0;
                        }
                        st.push(c);
                        c++;
                }
        }
        a = st.top();
        st.pop();

        if (a >= totalNumPat) {
                lengthA = l[a-totalNumPat];
                widthA = w[a-totalNumPat];
        }
        else {
                lengthA = len[a];
                widthA = wid[a];
        }

        delete(l);
        delete(w);

        if (dimension == 0)
                return (lenSheet - lengthA);
        else
                return (widSheet - widthA);
}

//-------------------------------------------------------------------------------
// Operador de reparacion
//-------------------------------------------------------------------------------
void TDCSP::repair(void) {
        set <int> pieces;
 	vector <P_Order> piecesOrd;
	int v1;

        for (int i = 0; i < size; i++) {
                v1 = notation[i];
                if (v1 >= 0)
                        pieces.insert(v1);
        }
        int f = 0;
        // Relleno con las piezas sobrantes
        piecesOrd.resize(totalNumPat - pieces.size());
        for (int i = 0; i < totalNumPat; i++) {
                if (pieces.find(i) == pieces.end()) {
                        piecesOrd[f].pat = i;
                        piecesOrd[f].len = len[i];
                        piecesOrd[f].wid = wid[i];
                        f++;
                }
        }

        // Las ordeno de menor a mayor largo
        sort (piecesOrd.begin(), piecesOrd.end(), cmpLengthOrd);

        int it = 0;
	
  	//cout << "que pasa" << endl; 
        while (!piecesOrd.empty()) {
                        // si cabe la pieza
  			//cout << "depurando... " << it << endl; 
                        if (getRemainSize(0) >= piecesOrd[it].len) {
  				//cout << "depurado" << endl; 
                                if (piecesOrd.size() > 1 && it < piecesOrd.size()-1) {
  					//cout << "aqui" << endl; 
                                        // si no cabe la siguiente pieza
                                        if (getRemainSize(0) < piecesOrd[it+1].len) {
                                                pilUp(piecesOrd,it,H);
                                                it = 0;
                                        }
                                        else // si cabe la siguiente pieza
                                                it++;
  					//cout << "aaca" << endl; 
                                }
                                else {

  					//cout << "aqui2" << endl; 
                                        if (piecesOrd.size() == 1) {
                                                notation[size] = piecesOrd[it].pat;
                                                notation[size + 1] = H;
                                                size += 2;
                                                piecesOrd.pop_back();
                                        }
                                        else { // si it estÃ¡ en el ultimo lugar,no hay mas piezas despues
                                                pilUp(piecesOrd,it,H);
                                                it = 0;
                                        }
  					//cout << "aca2" << endl; 
                                }
                        }
                        else
                                break;
        }
  	//cout << "que coño  pasa" << endl; 
	
	 if (!piecesOrd.empty()) {
                        sort (piecesOrd.begin(), piecesOrd.end(), cmpWidthOrd);
                        it = 0;

                        while (!piecesOrd.empty()) {
                                // si cabe la pieza
                                if (getRemainSize(1) >= piecesOrd[it].wid) {
                                        if (piecesOrd.size() > 1 && it < piecesOrd.size()-1) {
                                                // si no cabe la siguiente pieza
                                                if (getRemainSize(1) < piecesOrd[it+1].wid) {
                                                        pilUp(piecesOrd,it,V);
                                                        it = 0;
                                                }
                                                else // si cabe la siguiente pieza
                                                        it++;
                                        }
                                        else {
                                                if (piecesOrd.size() == 1) {
                                                        notation[size] = piecesOrd[it].pat;
                                                        notation[size + 1] = V;
                                                        size += 2;
                                                        piecesOrd.pop_back();
                                                }
                                                else { // si it esta en el ultimo lugar
                                                        pilUp(piecesOrd,it,V);
                                                        it = 0;
                                                }
                                        }
                                }
                                else
                                        break;
                        }
        }

        pieces.clear();
        piecesOrd.clear();

}
//-------------------------------------------------------------------------------
// Evaluacion
// ------------------------------------------------------------------------------
void TDCSP::evaluate (void) {
	int i, h1, h2, ord1, ord2, orient1, orient2, param1, param2, param3, param4, indexGen, putPieces, nonPutPieces, group, rest, dif;

//	cout << "eval" << endl;
	tree = NULL;
	typeOrder = 0;
	notation = new int[totalSize];
	tempNotation = new int[4*totalNumPat+1]; // no estoy segura

	order.clear();
	holes.clear() ;
	orderHoles.clear();

	order.resize(totalNumPat);
	for (int i = 0; i < totalNumPat; i++) {
		order[i].p = i;
		order[i].l = len[i];
		order[i].w = wid[i];
		order[i].prof = prof[i];
	}

	indexGen = numHS*3 - 1;
	put = 0;
	i = 0;
	putPieces = put;
       
	while ((i+1) < indexGen) {
		param1 = 0;
		h1 = (int) getVar(i);
		h2 = h1;
		ord1 = (int) getVar(i + 2);
		ord2 = ord1;
		group = 0;
		while (h1 == h2 && ord1 == ord2) {
			group++;
			i++;
			param1 += (int) getVar(i);

			if ((i+1) < indexGen) {
				i += 2;
				h2 = (int) getVar(i);
				ord2 = (int) getVar(i + 2);
			}
			else
 				break;
		}
		
		param2 = h1 % NORDER;
		param3 = ord1;

		putPieces = (this->*heuristic[h1 / NORDER]) (param1, param2, param3);

		//cout << "enviadas " << param1 <<  " puestas " << putPieces<< endl;
		if (putPieces != param1) {
			rest = putPieces;
			dif  = param1 - rest;

			int j;
			if (group > 1) {
				if (i % 3 == 0)
                                        j = i - 2;
                                else
                                        j = i;
			}
			while (group > 1) {
				if (dif < (int) getVar(j)) {
					rest = (int) getVar(j) - dif;
					break;
				}
				else {
					dif -= (int) getVar(j);
					j -= 3;
					group--;
					if (i % 3 == 0)
						i -= 3;     // ???
					else 
						i -= 1;	
				}
			}
			if (rest == 0) {
				if (i % 3  == 0)
					size = i - 3;
				else
					size = i - 1;
			}
			else {
				if (i % 3 != 0) 			//(i == 1)
					size = i+2;
				else
					size = i;
				put += putPieces;
				nonPutPieces = param1 - rest; 		// - putPieces;

				if (dif != 0) {
					// Si hay mas heurísticas sumarle las que no se han colocado a la siguiente heu
					if (size+1 < indexGen) {
						setVar(size-2,  rest);
						//cout << "en size-2 " << rest << " en size+1 " <<getVar(size+1) << " nonPut "<< nonPutPieces << endl;
						setVar(size+1, (int) getVar(size+1) + dif); //+ nonPutPieces);
					}
					else {
						// si no hay mas, crear una heu al final
						numHS++;
						setVar(size-2, rest);
						setVar(size, uniformRandom(NHS*NORDER));
						//setVar(size, (int) getVar(size-3));
						setVar(size+1, dif); //nonPutPieces);
						setVar(size+2, uniformRandom(NORDERP));
						//setVar(size+2, (int) getVar(size-1));
						sizeGen += 3;
					}	
				}

				/*Añadir un continue aqui, fijar sizeGen si agregue heu nueva, size no estaria fijado*/
				continue;
			}
			break;
		}
		else {
			if (i%3 != 0) //(i == 1)
				size = i+2;
			else
				size = i;
						
			put += param1;
		}

	}

	itTemp = 0;
	// obtener notacion con huecos
	postorden(tree);
	// obtener notacion final
	getNotation();

	repair();

	calculateProfitCuts();

	/*cout << endl << "notacion postfija: " ;
        for (int i = 0; i < itFinal; i++)
                cout << notation[i] << " ";
        cout << endl;

	cout << "Length: " << getObj(0) << endl;
	cout << "Number of cuts: " << getObj(1) << endl << endl;

	printInd(this);
*/
	podar(tree);
	delete notation;
	delete tempNotation;
	order.clear();
	holes.clear() ;
	orderHoles.clear();

	}

//----------------------------------------------------------------------------
// Clonacion
// ---------------------------------------------------------------------------
Individual* TDCSP::clone (void) const {
//  	cout << "clone" << endl;
	TDCSP* ind = new TDCSP();
	ind->numHS = numHS;
	ind->size = size;
//  	cout << "sale clone" << endl;
	return ind;
}
// -------------------------------------------------------------------------------
// Imprime por pantalla un individuo
// -------------------------------------------------------------------------------
void TDCSP::printInd(Individual *ind) {
	cleanInd(ind);
	for (int i = 0; i <((TDCSP*) ind)->numHS*3; i++) {
		cout << (int) ind->getVar(i) << " ";
	}
	cout << "--- ";
	for (int i = 0; i < ind->getNumberOfObj(); i++) {
		cout << (int) ind->getObj(i) << " ";
	}
	cout << endl;
	cout << ((TDCSP *)ind)->size << endl;
}

/*-----------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------
 * Codigo manejo heuristicas
 * ----------------------------------------------------------------------------------------------------
 * ----------------------------------------------------------------------------------------------------

/*-----------------------------------------------------------------------------------------------------
 * Liberar memoria de un arbol t.
 *----------------------------------------------------------------------------------------------------*/
void TDCSP::podar(Tree &t) {
  if(t != NULL) {
        podar(t->left);
        podar(t->right);
        t->father = NULL;
        delete t;
        t = NULL;
  }
}
/*-----------------------------------------------------------------------------------------------------
 * Recorrido del arbol y obtencion de la notacion plana con huecos.
 *----------------------------------------------------------------------------------------------------*/
void TDCSP::postorden(Tree t) {
        if (t != NULL) {
                postorden(t->left);
                postorden(t->right);
                tempNotation[itTemp++] = t->id;                        	
        }
}
/*-----------------------------------------------------------------------------------------------------
 * Obtencion de la notacion plana sin huecos.
 *----------------------------------------------------------------------------------------------------*/
void TDCSP::getNotation() {
        itFinal = 0;
        for (int i = 0; i < itTemp; i++) {
                if (tempNotation[i] < totalNumPat) {
                        notation[itFinal] = tempNotation[i];
                        itFinal++;
                }
                else 
                        i++;
        }
	size = itFinal;
}
/*-------------------------------------------------------------------------------------------
* Inserta un elemento id con ancho w en topHoles
* -------------------------------------------------------------------------------------------*/
void TDCSP::insertTopHole(int id, double w) {
        bool flag = false;
        if (!topHoles.empty()) {
                for (vector<int>::iterator it = topHoles.begin(); it < topHoles.end(); it++) {
                        if (holes[*it].w > w) {
                                topHoles.insert(it, id);
                                flag = true;
                                break;
                        }
                }
                if (!flag)
                        topHoles.push_back(id);
        }
        else 
                topHoles.push_back(id);		
}
/*------------------------------------------------------------------------------------------
* Eliminar el elemento id de holes y modificar en orderHoles los huecos > id restando 1
*-------------------------------------------------------------------------------------------*/
void TDCSP::delHole(int id) {
        // Eliminar el elemento id de holes
        podar(holes[id].ptr);
        holes.erase(holes.begin()+id);
        // Modificar en orderHoles todos los elementos > id restandoles 1
        for (int i = 0; i < orderHoles.size(); i++) {
                if (orderHoles[i] > id) 
                        orderHoles[i]--;
        }
	for (int i = 0; i < topHoles.size(); i++) {
                if (topHoles[i] > id)
                        topHoles[i]--;
        }
        // Modificar los identificadores tambien en el arbol
        for (int i = id; i < holes.size(); i++) {  
                holes[i].ptr->id--;
        }
}
/*-------------------------------------------------------------------------------------------
* Elimina un elemento id de la lista orderHoles
* ------------------------------------------------------------------------------------------*/
void TDCSP::delOrderHoleId(int id) {
        for (int i = 0; i < orderHoles.size();i++) {
                if (orderHoles[i] == id) {
                        orderHoles.erase(orderHoles.begin() + i);
                        break;
                }
        }
        /*for (vector<int>::iterator it=orderHoles.begin(); it<orderHoles.end(); it++) {
                if (*it == id) {
                        orderHoles.erase(it);
                        break;
                }
        }*/
}
/*-------------------------------------------------------------------------------------------
* Inserta un elemento id con area 'param', coord x 'param', coord y 'param' 
* (segun el tipo de orden) en orderHoles
* -------------------------------------------------------------------------------------------*/
void TDCSP::insertOrderHole(int id, double param) {
        bool flag = false;

        if (!orderHoles.empty()) {
                // Si el orden de seleccion es aleatorio mantener el tipo de ordenacion que se tenia previamente
                // if (tOrder == 5)
                //	tOrder = typeOrder;

                switch(typeOrder) {
                        case 0: 
                                for (vector<int>::iterator it = orderHoles.begin(); it < orderHoles.end(); it++) 
                                        if (holes[*it].a < param) {
                                                orderHoles.insert(it, id);
                                                flag = true;
                                                break;
                                        }
                                if (!flag)
                                        orderHoles.push_back(id);
                                break;
                        case 1: 
                                for (vector<int>::iterator it = orderHoles.begin(); it < orderHoles.end(); it++) 
                                        if (holes[*it].a > param) {
                                                orderHoles.insert(it, id);
                                                flag = true;
                                                break;
                                        }
                                if (!flag)
                                        orderHoles.push_back(id);
                                break;
                        case 2: 
                                for (vector<int>::iterator it = orderHoles.begin(); it < orderHoles.end(); it++) 
                                        if (holes[*it].x > param) {
                                                orderHoles.insert(it, id);
                                                flag = true;
                                                break;
                                        }
                                if (!flag)
                                        orderHoles.push_back(id);
                                break;
                        case 3: 
                                for (vector<int>::iterator it = orderHoles.begin(); it < orderHoles.end(); it++) 
                                        if (holes[*it].y < param) {
                                                orderHoles.insert(it, id);
                                                flag = true;
                                                break;
                                        }
                                if (!flag)
                                        orderHoles.push_back(id);
                }	
        }
        else 
                orderHoles.push_back(id);		
                
}
/*------------------------------------------------------------------------------------------
 * Obtener subarbol de la particion hueco: 
 * ------------------
 * |                |
 * |     h4         |
 * |                |
 * |----------------|
 * |   p  |    h3   |
 * |      |         |
 * ------------------
--------------------------------------------------------------------------------------------*/
void TDCSP::getTreeHole2(Tree &node, int it, int p, int subtree) {
        switch(subtree) {
                case 1:
                        //  p h3 H h4 V
                        node->id = V;
                        node->father = holes[it].ptr->father;
                        holes[it].ptr->father->right = node;
                        node->right = new T_Node;
                        node->left = new T_Node;
                        node->left->id = H;
                        node->left->father = node;
                        node->left->right = new T_Node;
                        node->left->left = new T_Node;
                        node->left->right->id = it+totalNumPat;         // sustituye al hueco que llena
                        node->left->right->father = node->left;
                        node->left->right->right = NULL;
                        node->left->right->left = NULL;
                        node->left->left->id = p;
                        node->left->left->father = node->left;
                        node->left->left->right = NULL;
                        node->left->left->left = NULL;
                        node->right->id = totalNumPat+holes.size();          //nuevo
                        node->right->father = node;
                        node->right->right = NULL;
                        node->right->left = NULL;
                        break;
                case 2:
                        // p h4 V
                        node->id = V;
                        node->father = holes[it].ptr->father;
                        holes[it].ptr->father->right = node;
                        node->right = new T_Node;
                        node->left = new T_Node;
                        node->left->id = p;
                        node->left->father = node;
                        node->left->right = NULL;
                        node->left->left = NULL;
                        node->right->id = it+totalNumPat;       //sustituye al hueco que se llena
                        node->right->father = node;
                        node->right->right = NULL;
                        node->right->left = NULL;
                        break;
                case 3:
                        // p h3 H
                        node->id = H;
                        node->father = holes[it].ptr->father;
                        holes[it].ptr->father->right = node;
                        node->right = new T_Node;
                        node->left = new T_Node;
                        node->left->id = p;
                        node->left->father = node;
                        node->left->right = NULL;
                        node->left->left = NULL;
                        node->right->id = it+totalNumPat;       //sustituye al hueco que se llena
                        node->right->father = node;
                        node->right->right = NULL;
                        node->right->left = NULL;
                        break;
                default:
                        // p
                        node->id = p;
                        node->father = holes[it].ptr->father;
                        holes[it].ptr->father->right = node;
                        node->right = NULL;
                        node->left = NULL;
        }
}
/*------------------------------------------------------------------------------------------
 * Obtener subarbol de la particion hueco: 
 * --------------------|
 * |      |         ...|
 * |  h2  |         ...|
 * |      |         ...|
 * |------|         ...|
 * |   p  |    h1   ...|
 * |      |         ...|
 * --------------------|
 *  it = -1 cuando no haya ningun hueco creado (al principio)
--------------------------------------------------------------------------------------------*/
void TDCSP::getTreeHole0(Tree &node, int it, int p, int subtree) {
        switch(subtree) {
                case 1:
                        // p h2 V h1 H
                                node->id = H;
                        if (it != -1) {
                                node->father = holes[it].ptr->father;
                                holes[it].ptr->father->right = node;
                        }
                        else {
                                node->father = NULL;
                                tree = node;
                        }
                        node->right = new T_Node;
                        node->left = new T_Node;
                         node->left->id = V;	
                        node->left->father = node;
                        node->left->right = new T_Node;
                        node->left->left = new T_Node;
                        if (it != -1)
                                node->left->right->id = it+totalNumPat;         // sustituye al hueco que llena
                        else
                                node->left->right->id = totalNumPat;         // nuevo: el primer hueco creado
                        node->left->right->father = node->left;
                        node->left->right->right = NULL;
                        node->left->right->left = NULL;
                        node->left->left->id = p;
                        node->left->left->father = node->left;
                        node->left->left->right = NULL;
                        node->left->left->left = NULL;
                        if (it != -1)
                                node->right->id = totalNumPat+holes.size();      //nuevo
                        else
                                node->right->id = totalNumPat+1;
                        node->right->father = node;
                        node->right->right = NULL;
                        node->right->left = NULL;
                        break;
                case 2:
                        // p h2 V
                        node->id = V;
                        if (it == -1) {
                                node->father = NULL;
                                tree = node;
                        }
                        else {
                                node->father = holes[it].ptr->father;
                                holes[it].ptr->father->right = node;
                        }
                        node->right = new T_Node;
                        node->left = new T_Node;
                        node->left->id = p;
                        node->left->father = node;
                        node->left->right = NULL;
                        node->left->left = NULL;
                        node->right->id = it+totalNumPat; 	            //sustituye al hueco que se llena
                        node->right->father = node;
                        node->right->right = NULL;
                        node->right->left = NULL;		
                        break;
                case 3:
                        //p h1 H
                        node->id = H;
                        if (it == -1) {
                                node->father = NULL;
                                tree = node;
                        }
                        else {
                                node->father = holes[it].ptr->father;
                                holes[it].ptr->father->right = node;
                        }
                        node->right = new T_Node;
                        node->left = new T_Node;
                        node->left->id = p;
                        node->left->father = node;
                        node->left->right = NULL;
                        node->left->left = NULL;
                        if (it != -1) 
                                node->right->id = it + totalNumPat;                   //sustituye al hueco que se llena
                        else
                                node->right->id = totalNumPat;         // nuevo: el primer hueco creado
                        node->right->father = node;
                        node->right->right = NULL;
                        node->right->left = NULL;	
                        break;
                default:
                        // p
                        node->id = p;
                        if (it != -1) {
                                node->father = holes[it].ptr->father;
                                holes[it].ptr->father->right = node;
                        }
                        else {
                                node->father = NULL;
                                tree = node;
                        }
                        node->right = NULL;
                        node->left = NULL;
        }
}
/*-------------------------------------------------------------------------------------------
 * Heuristica NFDH:
 * 1.orienta W>=L (EN ESTE CASO NO HAY ORIENTACIONES POSIBLES)
 * 2.ordena mayor a menor largo
 * 3.primer layer pieza al fondo izquierda.
 * 4.resto de piezas sobre esa, o si no cabe, nuevo layer.
 * ------------------------------------------------------------------------------------------*/
int TDCSP::NFDH(int n, int tOrder, int tOrderP) {
        int j, it, itOrderHoles, piece, nPieces, orientPiece, idHole, itHoles1, itHoles2, itHoles4, returnPieces;
        double param, xh1, xh2, xh3, xh4, yh1, yh2, yh3, yh4, wh1, wh2, wh3, wh4, lh1, lh2, lh3, lh4, xHole, yHole, widthHole, lengthHole;
        Tree node;
        bool flag;
        T_Hole h1, h2, h3, h4;

	//cout << "llamada NFDH" << endl;

        // Reordenar la lista de huecos ordenados segun el criterio de seleccion tOrder
        if (tOrder != typeOrder) {
                typeOrder = tOrder;
		sort (orderHoles.begin(), orderHoles.end(), cmpOrder(holes, typeOrder));
        }
        
	for (int i = 0; i < totalNumPat-put; i++) { 
		order[i].l = len[order[i].p];
		order[i].w = wid[order[i].p];
 	}

	switch(tOrderP) {
		case 0:
        		// Ordenar las piezas en orden decreciente según l
			sort (order.begin(), order.end(), cmpLength);
			break;
		case 1:
			// Ordenar las piezas en orden decreciente según w
			sort (order.begin(), order.end(), cmpWidth);
			break;
		case 2:
			// Ordenar las piezas en orden decreciente según a
			sort (order.begin(), order.end(), cmpArea);
			break;
		case 3:
			 // Ordenar las piezas en orden decreciente según perímetro
			sort (order.begin(), order.end(), cmpPerim);
			break;
		 case 4:
                        // Ordenar las piezas en orden decreciente según su beneficio
                        sort (order.begin(), order.end(), cmpProf);
                        break;
		 case 5:
			// Ordenar las piezas en orden decreciente segï¿½n su beneficio
                        sort (order.begin(), order.end(), cmpProfmin);
                        break;

	}
        
        itOrderHoles = 0;
        it  = 0; 
        nPieces = 0;
       
	returnPieces = 0;

        while (nPieces < n) { // hasta que se coloquen las n piezas !!!!!!!!!!!!!!!!!!!!!!!!!!

	
		// Buscar hueco de mayor area, menor area, menor x o mayor y
		if (!orderHoles.empty()) {
                	itOrderHoles = it;
                        idHole = orderHoles[itOrderHoles];
                        xHole = holes[idHole].x;
                        yHole = holes[idHole].y;
                        widthHole = holes[idHole].w;
                        lengthHole = holes[idHole].l;
                        it++;
		}
		else {
			idHole = -1;				// Material completo
                        xHole = 0;
                        yHole = 0;
                        widthHole = widSheet;
                        lengthHole = lenSheet;
                }
                              

		piece = order[nPieces].p;
		
		if ((!orderHoles.empty() && it == orderHoles.size()) && ((widthHole < wid[piece]) || lengthHole < (len[piece]))) {
			returnPieces = nPieces;
			break;            	
		}

                if ((widthHole < wid[piece]) || lengthHole < (len[piece])) 
                        continue; 	// La pieza no cabe en ese hueco.
                

                nPieces++; 		// Se coloca la pieza
                // Largos, anchos y coordenadas inf izq de los huecos generados al colocar pieza	
                
                lh1 = lengthHole - len[piece];
                wh1 = widthHole;
                xh1 = xHole + len[piece];
                yh1 = yHole;

                lh2 = len[piece];
                wh2 = widthHole - wid[piece];
                xh2 = xHole;
                yh2 = yHole + wid[piece];
        
                // Si ninguno de los huecos generados es 0
                if (lh1 != 0 && wh2 != 0) {
                        //  p h2 V h1 H		
                        node = new T_Node;
                        getTreeHole0(node, idHole, piece, 1);

                        // Crear los 2 nuevos huecos, introducirlos en holes y eliminar el previo
                        h1.l = lh1;
                        h1.w = wh1;
                        h1.a = lh1 * wh1;
                        h1.x = xh1;
                        h1.y = yh1;

                        h1.ptr = node->right;
                        h2.l = lh2;
                        h2.w = wh2;
                        h2.a = lh2 * wh2;
                        h2.x = xh2;
                        h2.y = yh2;
                        h2.ptr = node->left->right;

                        if (idHole == -1) { 
                                holes.push_back(h2);
                                itHoles2 = holes.size()-1;
                        }	
                        else { 
                                podar(holes[idHole].ptr);
                                holes[idHole] = h2;
                                itHoles2 = idHole;
                        }	
                        holes.push_back(h1);
                        itHoles1 = holes.size()-1;

                        // Eliminar el hueco de orderHoles e introducir en orden los dos nuevos	
                        switch(typeOrder) {
                                case 0:
                                case 1: param = h2.a;
                                        break;
                                case 2: param = h2.x;
                                        break;
                                case 3: param = h2.y;
                               
                        }

                        if (idHole != -1) { 
                                orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                insertOrderHole(idHole, param);
                        }
                        else 
                                insertOrderHole(0, param);
                        
                        switch(typeOrder) {
                                case 0:
                                case 1: param = h1.a;
                                        break;
                                case 2: param = h1.x;
                                        break;
                                case 3: param = h1.y;
                        }	
                        insertOrderHole(itHoles1, param);
                        
                }
                else {
                        // Si el 1º hueco es vacio
                        if (lh1 == 0 && wh2 != 0) {
                                // p h2 V
                                node = new T_Node;
                                getTreeHole0(node, idHole, piece, 2);
                                
                                // Crear el hueco nuevo sustituyendo al previo
                                holes[idHole].l = lh2;
                                holes[idHole].w = wh2;
                                holes[idHole].a = lh2 * wh2;
                                holes[idHole].x = xh2;
                                holes[idHole].y = yh2;
                                podar(holes[idHole].ptr);
                                holes[idHole].ptr = node->right;
                                
                                // Eliminar el hueco de orderHoles e introducir el nuevo
                                orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                switch(typeOrder) {
                                        case 0:
                                        case 1: param = holes[idHole].a;
                                                break;
                                        case 2: param = holes[idHole].x;
                                                break;
                                        case 3: param = holes[idHole].y;
                                }
                                insertOrderHole(idHole, param);
                                
                                itHoles1 = -1;
                                itHoles2 = idHole;	
                        }
                        else {
                                // Si el 2º hueco es vacio
                                if (lh1 != 0 && wh2 == 0) {
                                        // p h1 H
                                        node = new T_Node;
                                        getTreeHole0(node, idHole, piece, 3);
                                        
                                        // Crear el hueco nuevo sustituyendo al previo
                                        h1.l = lh1;
                                        h1.w = wh1;
                                        h1.a = lh1 * wh1;
                                        h1.x = xh1;
                                        h1.y = yh1;
                                        h1.ptr = node->right;

                                        if (idHole != -1) {
                                                podar(holes[idHole].ptr);
                                                holes[idHole] = h1;
                                                itHoles1 = idHole;
                                        }
                                        else {
                                                holes.push_back(h1);
                                                itHoles1 = holes.size() - 1;
                                        }
                                        // Eliminar el hueco de orderHoles e introducir el nuevo
                                        switch(typeOrder) {
                                                case 0:
                                                case 1: param = h1.a;
                                                        break;
                                                case 2: param = h1.x;
                                                        break;
                                                case 3: param = h1.y;
                                        }	
                                        if (idHole != -1) {
                                                orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                                insertOrderHole(idHole, param);
                                        }
                                        else
                                                insertOrderHole(0, param);
                                        itHoles2 = -1;	
                                }
                                else {
                                        // Si los huecos que se generan son vacios los dos
                                        // p
                                        node = new T_Node;
                                        getTreeHole0(node, idHole, piece ,4);

                                        // Eliminar el hueco que se ha rellenado
                                        orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                        delHole(idHole);
                                        itHoles1 = -1;
                                        itHoles2 = -1;
                                }
                        
                        }					
                }

		it  = 0;
		itOrderHoles = 0;
				
                while (nPieces < n) {	//!!!!!!!!!!!!!!!!!!!!!!!
                        piece = order[nPieces].p;
			
                        // Primero llenar hueco h2
                        while (nPieces < n && itHoles2 != -1 && holes[itHoles2].l >= len[piece] && holes[itHoles2].w >= wid[piece]) { //!!!!!!!!!!!!
                                widthHole = holes[itHoles2].w;
                                lengthHole = holes[itHoles2].l;
                                xHole = holes[itHoles2].x;
                                yHole = holes[itHoles2].y;

                                // Largos, anchos y coordenadas de los huecos generados al colocar pieza
                                lh3 = lengthHole - len[piece];
                                wh3 = wid[piece];
                                xh3 = xHole - len[piece];
                                yh3 = yHole;
                                lh4 = lengthHole;
                                wh4 = widthHole - wid[piece];
                                xh4 = xHole;
                                yh4 = yHole + wid[piece];

                                // Si ninguno de los huecos generados es 0
                                if (lh3 != 0 && wh4 != 0) {
                                        // p h3 H h4 V
                                        node = new T_Node;
                                        getTreeHole2(node, itHoles2, piece, 1);
                                        
                                        // Crear los 2 nuevos huecos, introducirlos en holes y eliminar el previo		
                                        h3.l = lh3;
                                        h3.w = wh3;
                                        h3.a = lh3 * wh3;
                                        h3.x = xh3;
                                        h3.y = yh3;
                                        h3.ptr = node->left->right;
                                        h4.l = lh4;
                                        h4.w = wh4;
                                        h4.a = lh4 * wh4;
                                        h4.x = xh4;
                                        h4.y = yh4;
                                        h4.ptr = node->right;
                                        podar(holes[itHoles2].ptr);
                                        holes[itHoles2] = h3;
                                        holes.push_back(h4);

                                        // Eliminar el hueco de orderHoles e introducir en orden los dos nuevos
                                        delOrderHoleId(itHoles2);
                                        switch(typeOrder) {
                                                case 0:
                                                case 1: param = h3.a;
                                                        break;
                                                case 2: param = h3.x;
                                                        break;
                                                case 3: param = h3.y;
                                        }	
                                        insertOrderHole(itHoles2, param);
                                        switch(typeOrder) {
                                                case 0:
                                                case 1: param = h4.a;
                                                        break;
                                                case 2: param = h4.x;
                                                        break;
                                                case 3: param = h4.y;
                                        }	
                                        insertOrderHole(holes.size()-1, param);
                                        itHoles2= holes.size()-1;
                                }
                                else {
                                        // Si el 1º hueco es vacio
                                        if (lh3 == 0 && wh4 != 0) {
                                                // p h4 V
                                                node = new T_Node;
                                                getTreeHole2(node, itHoles2,piece, 2);

                                                // Crear el hueco nuevo sustituyendo al previo
                                                holes[itHoles2].l = lh4;
                                                holes[itHoles2].w = wh4;
                                                holes[itHoles2].a = lh4 * wh4;
                                                holes[itHoles2].x = xh4;
                                                holes[itHoles2].y = yh4;
                                                podar(holes[itHoles2].ptr);
                                                holes[itHoles2].ptr = node->right;
                                                
                                                // Eliminar el hueco de orderHoles e introducir el nuevo
                                                delOrderHoleId(itHoles2);
                                                switch(typeOrder) {
                                                        case 0:
                                                        case 1: param = holes[itHoles2].a;
                                                                break;
                                                        case 2: param = holes[itHoles2].x;
                                                                break;
                                                        case 3: param = holes[itHoles2].y;
                                                }	
                                                insertOrderHole(itHoles2, param);
                                        }
                                        else {
                                                // Si el 2º hueco es vacio
                                                if (lh3 != 0 && wh4 == 0) {
                                                        //p h3 H
                                                        node = new T_Node;
                                                        getTreeHole2(node, itHoles2, piece, 3);
                                
                                                        // Crear el hueco nuevo sustituyendo al previo
                                                        h3.l = lh3;
                                                        h3.w = wh3;
                                                        h3.a = lh3 * wh3;
                                                        h3.x = xh3;
                                                        h3.y = yh3;
                                                        h3.ptr = node->right;

                                                        podar(holes[itHoles2].ptr);
                                                        holes[itHoles2] = h3;
                                                        
                                                        // Eliminar el hueco de orderHoles e introducir el nuevo
                                                        delOrderHoleId(itHoles2);
                                                        switch(typeOrder) {
                                                                case 0:
                                                                case 1: param = h3.a;
                                                                        break;
                                                                case 2: param = h3.x;
                                                                        break;
                                                                case 3: param = h3.y;
                                                        }
                                                        insertOrderHole(itHoles2, param);
                                                        itHoles2 = -1;
                                                }
                                                else {
                                                        // Si los huecos que se generan son vacios los dos
                                                               // p
                                                        node = new T_Node;
                                                        getTreeHole2(node, itHoles2, piece, 4);					

                                                        // Eliminar el hueco que se ha rellenado
                                                        delOrderHoleId(itHoles2);
                                                        delHole(itHoles2);
                                                        if (itHoles2 < itHoles1)
                                                                itHoles1 --;
                                                        itHoles2 = -1;
                                                }
                                        }

                                }
                                nPieces++;	
                                if (nPieces < n) {
                                        piece = order[nPieces].p;
				}

			} //while3
        
			if (nPieces < n && itHoles1 != -1 && holes[itHoles1].l >= len[piece] && holes[itHoles1].w >= wid[piece]) {
                                // Largos y anchos de los huecos generados al colocar pieza
                                widthHole = holes[itHoles1].w;
                                lengthHole = holes[itHoles1].l;
                                xHole = holes[itHoles1].x;
                                yHole = holes[itHoles1].y;
               
                               
                                lh3 = lengthHole - len[piece];
                                wh3 = widthHole;
                                xh3 = xHole + len[piece];
                                yh3 = yHole;
                                lh4 = len[piece];
                                wh4 = widthHole - wid[piece];
                                xh4 = xHole;
                                yh4 = yHole + wid[piece];

                                // Si ninguno de los huecos generados es 0
                                if (lh3 != 0 && wh4 != 0) {
                                        // p h4 V h3 H
                                        node = new T_Node;
                                        getTreeHole0(node, itHoles1, piece, 1);

                                        // Crear los 2 nuevos huecos, introducirlos en holes y eliminar el previo
                                        h3.l = lh3;
                                        h3.w = wh3;
                                        h3.a = lh3 * wh3;
                                        h3.x = xh3;
                                        h3.y = yh3;
                                        h3.ptr = node->right;
                                        h4.l = lh4;
                                        h4.w = wh4;
                                        h4.a = lh4 * wh4;
                                        h4.x = xh4;
                                        h4.y = yh4;
                                        h4.ptr = node->left->right;
                                        podar(holes[itHoles1].ptr);
                                        holes[itHoles1] = h4;
                                
                                        holes.push_back(h3);

                                        // Eliminar el hueco de orderHoles e introducir en orden los dos nuevos
                                        delOrderHoleId(itHoles1);
                                        switch(typeOrder) {
                                                case 0:
                                                case 1: param = h4.a;
                                                        break;
                                                case 2: param = h4.x;
                                                        break;
                                                case 3: param = h4.y;
                                        }	
                                        insertOrderHole(itHoles1, param);
                                        switch(typeOrder) {
                                                case 0:
                                                case 1: param = h3.a;
                                                        break;
                                                case 2: param = h3.x;
                                                        break;
                                                case 3: param = h3.y;
                                        }	
                                        insertOrderHole(holes.size()-1, param);
                                        itHoles2 = itHoles1;
                                        itHoles1 = holes.size()-1;
                                }
                                else {
                                        // Si el 1º hueco es vacio
                                        if (lh3 == 0 && wh4 != 0) {
                                                // p h4 V
                                                node = new T_Node;
                                                getTreeHole0(node, itHoles1, piece, 2);

                                                // Crear el hueco nuevo sustituyendo al previo
                                                holes[itHoles1].l = lh4;
                                                holes[itHoles1].w = wh4;
                                                holes[itHoles1].a = lh4 * wh4;
                                                holes[itHoles1].x = xh4;
                                                holes[itHoles1].y = yh4;
                                                podar(holes[itHoles1].ptr);
                                                holes[itHoles1].ptr = node->right;

                                                // Eliminar el hueco de orderHoles e introducir el nuevo
                                                delOrderHoleId(itHoles1);
                                                switch(typeOrder) {
                                                        case 0:
                                                        case 1: param = holes[itHoles1].a;
                                                                break;
                                                        case 2: param = holes[itHoles1].x;
                                                                break;
                                                        case 3: param = holes[itHoles1].y;
                                                }	
                                                insertOrderHole(itHoles1, param);

                                                itHoles2 = itHoles1;
                                                itHoles1 = -1;
                                        }
                                        else {
                                                // Si el 2º hueco es vacio
                                                if (lh3 != 0 && wh4 == 0) {
                                                        //p h3 H
                                                        node = new T_Node;
                                                        getTreeHole0(node, itHoles1, piece, 3);

                                                        // Crear el hueco nuevo sustituyendo al previo
                                                        h3.l = lh3;
                                                        h3.w = wh3;
                                                        h3.a = lh3 * wh3;
                                                        h3.x = xh3;
                                                        h3.y = yh3;
                                                        h3.ptr = node->right;

                                                        podar(holes[itHoles1].ptr);
                                                        holes[itHoles1] = h3;
                                                        // Eliminar el hueco de orderHoles e introducir el nuevo
                                                        delOrderHoleId(itHoles1);
                                                        switch(typeOrder) {
                                                                case 0:
                                                                case 1: param = h3.a;
                                                                        break;
                                                                case 2: param = h3.x;
                                                                        break;
                                                                case 3: param = h3.y;
                                                        }	
                                                        insertOrderHole(itHoles1, param);
                                                        itHoles2 = -1;
                                                }
                                                else {
                                                        // Si los huecos que se generan son vacios los dos
                                                        // p
                                                        node = new T_Node;
                                                        getTreeHole0(node, itHoles1, piece, 4);

                                                        // Eliminar el hueco que se ha rellenado
                                                        delOrderHoleId(itHoles1);
                                                        delHole(itHoles1);
                                                        itHoles1 = -1;
                                                        itHoles2 = -1;
                                                }

                                        }
                                }
                                nPieces++;
                                if (nPieces < n) {
                                        piece = order[nPieces].p;
				}
			}
                	else {
				returnPieces = nPieces;
				break;	
                	}
		} // while2		
        
        } // while1
        
        order.erase(order.begin(),order.begin()+nPieces);     //!!!!!!!!!!!!!!!!!
	returnPieces = nPieces;	
	return returnPieces;
}
/*-------------------------------------------------------------------------------------------------------
 * Heuristica FFDH:
 * Igual a NFDH pero cada pieza se encaja en el 1º layer que quepa (no necesariamente se tiene que crear 
 * un nuevo layer si no cabe en el actual)
 * ---------------------------------------------------------------------------------------------------------*/
int TDCSP::FFDH(int n, int tOrder, int tOrderP) {
        int j, itOrderHoles, piece, orientPiece, nPieces, idHole, it, itHoles, itHoles1, itHoles2, itHoles4, returnPieces;
        double param, xh1, xh2, xh3, xh4, yh1, yh2, yh3, yh4, wh1, wh2, wh3, wh4, lh1, lh2, lh3, lh4, xHole, yHole, widthHole, lengthHole;
        Tree node;
        bool flag;
        T_Hole h1, h2, h3, h4;

        topHoles.clear();

        // Reordenar la lista de huecos ordenados segun el criterio de seleccion tOrder
        if (tOrder != typeOrder) {
		typeOrder = tOrder;
		sort (orderHoles.begin(), orderHoles.end(), cmpOrder(holes, typeOrder));
        }

        for (int i = 0; i < totalNumPat-put; i++) {
                order[i].l = len[order[i].p];
                order[i].w = wid[order[i].p];
        }

        switch(tOrderP) {
                case 0:
                        // Ordenar las piezas en orden decreciente según l
                        sort (order.begin(), order.end(), cmpLength);
                        break;
                case 1:
                        // Ordenar las piezas en orden decreciente según w
                        sort (order.begin(), order.end(), cmpWidth);
                        break;
                case 2:
                        // Ordenar las piezas en orden decreciente según a
                        sort (order.begin(), order.end(), cmpArea);
                        break;
                case 3:
                         // Ordenar las piezas en orden decreciente según perímetro
                        sort (order.begin(), order.end(), cmpPerim);
                        break;
		 case 4:
                        // Ordenar las piezas en orden decreciente según su beneficio
                        sort (order.begin(), order.end(), cmpProf);
                        break;
		 case 5:
                        // Ordenar las piezas en orden decreciente segï¿½n su beneficio
                        sort (order.begin(), order.end(), cmpProfmin);
                        break;
        }
        itOrderHoles = 0;
        nPieces = 0;
        it = 0;
	returnPieces = 0;
        while (nPieces < n) { // hasta que se coloquen las n piezas //!!!!!!!!!!!!!!!
                
		// Buscar hueco de mayor area, menor area, menor x o mayor y
		if (!orderHoles.empty()) {
			itOrderHoles = it;
                        idHole = orderHoles[itOrderHoles];
                        xHole = holes[idHole].x;
                        yHole = holes[idHole].y;
                        widthHole = holes[idHole].w;
                        lengthHole = holes[idHole].l;
                        it++;
                }
                else {
                        idHole = -1;				// Material completo
                        xHole = 0;
                        yHole = 0;
                        widthHole = widSheet;
                        lengthHole = lenSheet;
                }
    	
        
                piece = order[nPieces].p;

		if ((!orderHoles.empty() && it == orderHoles.size()) && ((widthHole < wid[piece]) || lengthHole < (len[piece]))) {
			returnPieces = nPieces;
			break;            
		
		}
		

                if ((widthHole < wid[piece]) || (lengthHole < len[piece])) 
                        continue; 	// La pieza no cabe en ese hueco.
                

                nPieces++; 		// Se coloca la pieza
                        
                // Largos y anchos de los huecos generados al colocar pieza	
            
                lh1 = lengthHole - len[piece];
                wh1 = widthHole;
                xh1 = xHole + len[piece];
                yh1 = yHole;
                lh2 = len[piece];
                wh2 = widthHole - wid[piece];
                xh2 = xHole;
                yh2 = yHole + wid[piece];
                
                // Si ninguno de los huecos generados es 0
                if (lh1 != 0 && wh2 != 0) {
                        //  p h2 V h1 H		
                        node = new T_Node;
                        getTreeHole0(node, idHole, piece, 1);

                        // Crear los 2 nuevos huecos, introducirlos en holes y eliminar el previo
                        h1.l = lh1;
                        h1.w = wh1;
                        h1.a = lh1 * wh1;
                        h1.x = xh1;
                        h1.y = yh1;
                        h1.ptr = node->right;
                        h2.l = lh2;
                        h2.w = wh2;
                        h2.a = lh2 * wh2;
                        h2.x = xh2;
                        h2.y = yh2;
                        h2.ptr = node->left->right;
                        if (idHole == -1) { 
                                holes.push_back(h2);
                                itHoles2 = holes.size()-1;
                        }	
                        else { 
                                podar(holes[idHole].ptr);
                                holes[idHole] = h2;
                                itHoles2 = idHole;
                        }	
                        holes.push_back(h1);
                        itHoles1 = holes.size()-1;

                        // Eliminar el hueco de orderHoles e introducir en orden los dos nuevos
                        switch(typeOrder) {
                                case 0:
                                case 1: param = h2.a;
                                        break;
                                case 2: param = h2.x;
                                        break;
                                case 3: param = h2.y;
                        }
                        if (idHole != -1) { 
                                orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                insertOrderHole(idHole, param);
                        }
                        else 
                                insertOrderHole(0, param);
                        switch(typeOrder) {
                                case 0:
                                case 1: param = h1.a;
                                        break;
                                case 2: param = h1.x;
                                        break;
                                case 3: param = h1.y;
                        }
                        insertOrderHole(holes.size()-1, param);
                        
                        // Insertar el hueco 2 en la lista de huecos superiores
                        topHoles.push_back(itHoles2);
                }
                else {
                        // Si el 1º hueco es vacio
                        if (lh1 == 0 && wh2 != 0) {
                                // p h2 V
                                node = new T_Node;
                                getTreeHole0(node, idHole, piece ,2);
                                
                                // Crear el hueco nuevo sustituyendo al previo
                                holes[idHole].l = lh2;
                                holes[idHole].w = wh2;
                                holes[idHole].a = lh2 * wh2;
                                holes[idHole].x = xh2;
                                holes[idHole].y = yh2;
                                podar(holes[idHole].ptr);
                                holes[idHole].ptr = node->right;
                                
                                // Eliminar el hueco de orderHoles e introducir el nuevo
                                orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                switch(typeOrder) {
                                        case 0:
                                        case 1: param = holes[idHole].a;
                                                break;
                                        case 2: param = holes[idHole].x;
                                                break;
                                        case 3: param = holes[idHole].y;
                                }
                                insertOrderHole(idHole, param);
                                
                                itHoles1 = -1;
                                itHoles2 = idHole;	
                                
                                // Insertar el hueco 2 en la lista de huecos superiores
                                topHoles.push_back(itHoles2);
                        }
                        else {
                                // Si el 2º hueco es vacio
                                if (lh1 != 0 && wh2 == 0) {
                                        // p h1 H
                                        node = new T_Node;
                                        getTreeHole0(node, idHole, piece ,3);
                                        
                                        // Crear el hueco nuevo sustituyendo al previo
                                        h1.l = lh1;
                                        h1.w = wh1;
                                        h1.a = lh1 * wh1;
                                        h1.x = xh1;
                                        h1.y = yh1;
                                        h1.ptr = node->right;

                                        if (idHole != -1) {
                                                podar(holes[idHole].ptr);
                                                holes[idHole] = h1;
                                                itHoles1 = idHole;
                                        }
                                        else {
                                                holes.push_back(h1);
                                                itHoles1 = holes.size() - 1;
                                        }
                                        // Eliminar el hueco de orderHoles e introducir el nuevo
                                        switch(typeOrder) {
                                                case 0:
                                                case 1: param = h1.a;
                                                        break;
                                                case 2: param = h1.x;
                                                        break;
                                                case 3: param = h1.y;
                                        }
                                        if (idHole != -1) {
                                                orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                                insertOrderHole(idHole, param);
                                        }
                                        else
                                                insertOrderHole(0, param);
                                        itHoles2 = -1;	
                                }
                                else {
                                        // Si los huecos que se generan son vacios los dos
                                        // p
                                        node = new T_Node;
                                        getTreeHole0(node, idHole, piece ,4);

                                        // Eliminar el hueco que se ha rellenado
                                        orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                        delHole(idHole);
                                        itHoles1 = -1;
                                        itHoles2 = -1;
                                }
                        
                        }					
                }
            
		it  = 0;
		itOrderHoles = 0;
				
                while (nPieces < n) {    //!!!!!!!!!!!!!!!!!!!!
                        piece = order[nPieces].p;
                        int i = 0;
                        bool fit = false;
                        // Si hay huecos en la lista de huecos superiores, buscar uno en el que quepa la pieza y se coloca
                        if (!topHoles.empty()) {
                                for (i = 0; i < topHoles.size();i++) 
                                        if (holes[topHoles[i]].l >= len[piece] && holes[topHoles[i]].w >= wid[piece]) 
                                                break;  //!!!!!!!!!!!!!!!!!! 

                                // Si cabe en un hueco colocarla
                                if (i < topHoles.size()) {
                                        fit = true;

                                        itHoles = topHoles[i];

                                        widthHole = holes[itHoles].w;
                                        lengthHole = holes[itHoles].l;
                                        xHole = holes[itHoles].x;
                                        yHole = holes[itHoles].y;

                                        // Largos, anchos y coord de los huecos generados al colocar pieza
                                        lh3 = lengthHole - len[piece];
                                        wh3 = wid[piece];
                                        xh3 = xHole + len[piece];
                                        yh3 = yHole;
                                        lh4 = lengthHole;
                                        wh4 = widthHole - wid[piece];
                                        xh4 = xHole;
                                        yh4 = yHole + wid[piece];

                                        // Si ninguno de los huecos generados es 0
                                        if (lh3 != 0 && wh4 != 0) {
                                                // p h3 H h4 V
                                                node = new T_Node;
                                                getTreeHole2(node, itHoles, piece,1);
                                        
                                                // Crear los 2 nuevos huecos, introducirlos en holes y eliminar el previo		
                                                h3.l = lh3;
                                                h3.w = wh3;
                                                h3.a = lh3 * wh3;
                                                h3.x = xh3;
                                                h3.y = yh3;
                                                h3.ptr = node->left->right;
                                                h4.l = lh4;
                                                h4.w = wh4;
                                                h4.a = lh4 * wh4;
                                                h4.x = xh4;
                                                h4.y = yh4;
                                                h4.ptr = node->right;
                                                podar(holes[itHoles].ptr);
                                                holes[itHoles] = h3;
                                                holes.push_back(h4);

                                                // Eliminar el hueco de orderHoles e introducir en orden los dos nuevos
                                                delOrderHoleId(itHoles);
                                                switch(typeOrder) {
                                                        case 0:
                                                        case 1: param = h3.a;
                                                                break;
                                                        case 2: param = h3.x;
                                                                break;
                                                        case 3: param = h3.y;
                                                }
                                                insertOrderHole(itHoles, param);
                                                switch(typeOrder) {
                                                        case 0:
                                                        case 1: param = h4.a;
                                                                break;
                                                        case 2: param = h4.x;
                                                                break;
                                                        case 3: param = h4.y;
                                                }
                                                insertOrderHole(holes.size()-1, param);

                                                // Sustituir hueco en topHoles
                                                topHoles[i] = holes.size()-1;
                                        }
                                        else {
                                                // Si el 1º hueco es vacio
                                                if (lh3 == 0 && wh4 != 0) {
                                                        // p h4 V
                                                        node = new T_Node;
                                                        getTreeHole2(node, itHoles,piece,2);
        
                                                        // Crear el hueco nuevo sustituyendo al previo
                                                        holes[itHoles].l = lh4;
                                                        holes[itHoles].w = wh4;
                                                        holes[itHoles].a = lh4 * wh4;
                                                        holes[itHoles].x = xh4;
                                                        holes[itHoles].y = yh4;
                                                        podar(holes[itHoles].ptr);
                                                        holes[itHoles].ptr = node->right;

                                                        // Eliminar el hueco de orderHoles e introducir el nuevo
                                                        delOrderHoleId(itHoles);
                                                        switch(typeOrder) {
                                                                case 0:
                                                                case 1: param = holes[itHoles].a;
                                                                        break;
                                                                case 2: param = holes[itHoles].x;
                                                                        break;
                                                                case 3: param = holes[itHoles].y;
                                                        }
                                                        insertOrderHole(itHoles, param);
                                                        
                                                        // Sustituir hueco en topHoles
                                                        // Sigue siendo el mismo identificador de hueco
                                                }	
                                                else {
                                                        // Si el 2º hueco es vacio
                                                        if (lh3 != 0 && wh4 == 0) {
                                                                //p h3 H
                                                                node = new T_Node;
                                                                getTreeHole2(node, itHoles, piece, 3);

                                                                // Crear el hueco nuevo sustituyendo al previo
                                                                h3.l = lh3;
                                                                h3.w = wh3;
                                                                h3.a = lh3 * wh3;
                                                                h3.x = xh3;
                                                                h3.y = yh3;
                                                                h3.ptr = node->right;

                                                                podar(holes[itHoles].ptr);
                                                                holes[itHoles] = h3;
                                                        
                                                                // Eliminar el hueco de orderHoles e introducir el nuevo
                                                                delOrderHoleId(itHoles);
                                                                switch(typeOrder) {
                                                                        case 0:
                                                                        case 1: param = h3.a;
                                                                                break;
                                                                        case 2: param = h3.x;
                                                                                break;
                                                                        case 3: param = h3.y;
                                                                }
                                                                insertOrderHole(itHoles, param);

                                                                // Eliminar hueco en topHoles
                                                                topHoles.erase(topHoles.begin()+i);
                                                        }
                                                        else {
                                                                // Si los huecos que se generan son vacios los dos
                                                                       // p
                                                                node = new T_Node;
                                                                getTreeHole2(node, itHoles, piece, 4);					

                                                                // Eliminar el hueco que se ha rellenado
                                                                delOrderHoleId(itHoles);
                                                                delHole(itHoles);
                                                                if (itHoles < itHoles1)
                                                                        itHoles1--;
                                        
                                                                // Eliminar hueco en topHoles
                                                                topHoles.erase(topHoles.begin()+i);
                                                        }
                                                }

                                        }
                                        
                                }
                        }
                        // Si no se ha podido encajar la pieza en ningun hueco superior	
                        if (!fit) {
                                // Si cabe en el hueco h1
                                if (itHoles1 != -1 && holes[itHoles1].l >= len[piece] && holes[itHoles1].w >= wid[piece]) {
                                        // Largos y anchos de los huecos generados al colocar pieza
                                        widthHole = holes[itHoles1].w;
                                        lengthHole = holes[itHoles1].l;
                                        xHole = holes[itHoles1].x;
                                        yHole = holes[itHoles1].y;
               
                                         
                                        lh3 = lengthHole - len[piece];
                                        wh3 = widthHole;
                                        xh3 = xHole + len[piece];
                                        yh3 = yHole;
                                        lh4 = len[piece];
                                        wh4 = widthHole - wid[piece];
                                        xh4 = xHole;
                                        yh4 = yHole + wid[piece];

                                        // Si ninguno de los huecos generados es 0
                                        if (lh3 != 0 && wh4 != 0){
                                                // p h4 V h3 H
                                                node = new T_Node;
                                                getTreeHole0(node, itHoles1, piece, 1);

                                                // Crear los 2 nuevos huecos, introducirlos en holes y eliminar el previo
                                                h3.l = lh3;
                                                h3.w = wh3;
                                                h3.a = lh3 * wh3;
                                                h3.x = xh3;
                                                h3.y = yh3;
                                                h3.ptr = node->right;
                                                h4.l = lh4;
                                                h4.w = wh4;
                                                h4.a = lh4 * wh4;
                                                h4.x = xh4;
                                                h4.y = yh4;
                                                h4.ptr = node->left->right;
                                                podar(holes[itHoles1].ptr);
                                                holes[itHoles1] = h4;
                                
                                                holes.push_back(h3);

                                                // Eliminar el hueco de orderHoles e introducir en orden los dos nuevos
                                                delOrderHoleId(itHoles1);
                                                switch(typeOrder) {
                                                        case 0:
                                                        case 1: param = h4.a;
                                                                break;
                                                        case 2: param = h4.x;
                                                                break;
                                                        case 3: param = h4.y;
                                                }
                                                insertOrderHole(itHoles1, param);
                                                switch(typeOrder) {
                                                        case 0:
                                                        case 1: param = h3.a;
                                                                break;
                                                        case 2: param = h3.x;
                                                                break;
                                                        case 3: param = h3.y;
                                                }
                                                insertOrderHole(holes.size()-1, param);

                                                // Añadir hueco h4 a topHoles
                                                topHoles.push_back(itHoles1);

                                                itHoles1 = holes.size()-1;
                                        }
                                        else {
                                                // Si el 1º hueco es vacio
                                                if (lh3 == 0 && wh4 != 0) {
                                                        // p h4 V
                                                        node = new T_Node;
                                                        getTreeHole0(node, itHoles1, piece, 2);

                                                        // Crear el hueco nuevo sustituyendo al previo
                                                        holes[itHoles1].l = lh4;
                                                        holes[itHoles1].w = wh4;
                                                        holes[itHoles1].a = lh4 * wh4;
                                                        holes[itHoles1].x = xh4;
                                                        holes[itHoles1].y = yh4;
                                                        podar(holes[itHoles1].ptr);
                                                        holes[itHoles1].ptr = node->right;

                                                        // Eliminar el hueco de orderHoles e introducir el nuevo
                                                        delOrderHoleId(itHoles1);
                                                        switch(typeOrder) {
                                                                case 0:
                                                                case 1: param = holes[itHoles1].a;
                                                                        break;
                                                                case 2: param = holes[itHoles1].x;
                                                                        break;
                                                                case 3: param = holes[itHoles1].y;
                                                        }
                                                        insertOrderHole(itHoles1, param);
                                                        
                                                        // Añadir hueco h4 a topHoles
                                                        topHoles.push_back(itHoles1);

                                                        itHoles1 = -1;
                                                }
                                                else {
                                                        // Si el 2º hueco es vacio
                                                        if (lh3 != 0 && wh4 == 0) {
                                                                //p h3 H
                                                                node = new T_Node;
                                                                getTreeHole0(node, itHoles1, piece, 3);

                                                                // Crear el hueco nuevo sustituyendo al previo
                                                                h3.l = lh3;
                                                                h3.w = wh3;
                                                                h3.a = lh3 * wh3;
                                                                h3.x = xh3;
                                                                h3.y = yh3;
                                                                h3.ptr = node->right;

                                                                podar(holes[itHoles1].ptr);
                                                                holes[itHoles1] = h3;
                                                                // Eliminar el hueco de orderHoles e introducir el nuevo
                                                                delOrderHoleId(itHoles1);
                                                                switch(typeOrder) {
                                                                        case 0:
                                                                        case 1: param = h3.a;
                                                                                break;
                                                                        case 2: param = h3.x;
                                                                                break;
                                                                        case 3: param = h3.y;
                                                                }
                                                                insertOrderHole(itHoles1, param);
                                                                
                                                        }
                                                        else {
                                                                // Si los huecos que se generan son vacios los dos
                                                                // p
                                                                node = new T_Node;
                                                                getTreeHole0(node, itHoles1, piece,4);

                                                                // Eliminar el hueco que se ha rellenado
                                                                delOrderHoleId(itHoles1);
                                                                delHole(itHoles1);
                                                                itHoles1 = -1;
                                                        }

                                                }
                                        }	

                                }
                                else {
                                        // Si el hueco h1 no existe o no cabe la pieza
                                        //topHoles.clear(); //  ?????? cambio a otro hueco, borro los top de este?
					returnPieces = nPieces;
                                        break;
                                }
                        }

                        nPieces++;	
                        if (nPieces < n) {
                                piece = order[nPieces].p;	
			}
                        
                } //while2

        } // while1

	topHoles.clear();
        order.erase(order.begin(),order.begin()+nPieces);
	returnPieces = nPieces;	
	return returnPieces;	
}
/*-------------------------------------------------------------------------------------------------------
 * Heuristica BFDH:
 * Igual a FFDH pero cada pieza se encaja en el layer que quepa (no necesariamente se tiene que crear 
 * un nuevo layer si no cabe en el actual) con menor espacio restante.
 * ---------------------------------------------------------------------------------------------------------*/
int TDCSP::BFDH(int n, int tOrder, int tOrderP) {
        int j, itOrderHoles, piece, nPieces, orientPiece, idHole, it, itHoles, itHoles1, itHoles2, itHoles4, returnPieces;
        double param, xh1, xh2, xh3, xh4, yh1, yh2, yh3, yh4, wh1, wh2, wh3, wh4, lh1, lh2, lh3, lh4, xHole, yHole, widthHole, lengthHole;
        Tree node;
        bool flag;
        T_Hole h1, h2, h3, h4;

        topHoles.clear();

        // Reordenar la lista de huecos ordenados segun el criterio de seleccion tOrder
        if (tOrder != typeOrder) {
		typeOrder = tOrder;
		sort (orderHoles.begin(), orderHoles.end(), cmpOrder(holes, typeOrder));
        }
	
        for (int i = 0; i < totalNumPat-put; i++) {
                order[i].l = len[order[i].p];
                order[i].w = wid[order[i].p];
        }

        switch(tOrderP) {
                case 0:
                        // Ordenar las piezas en orden decreciente según l
                        sort (order.begin(), order.end(), cmpLength);
                        break;
                case 1:
                        // Ordenar las piezas en orden decreciente según w
                        sort (order.begin(), order.end(), cmpWidth);
                        break;
                case 2:
                        // Ordenar las piezas en orden decreciente según a
                        sort (order.begin(), order.end(), cmpArea);
                        break;
                case 3:
                         // Ordenar las piezas en orden decreciente según perímetro
                        sort (order.begin(), order.end(), cmpPerim);
                        break;
		case 4:
                        // Ordenar las piezas en orden decreciente según su beneficio
                        sort (order.begin(), order.end(), cmpProf);
                        break;
		 case 5:
                        // Ordenar las piezas en orden decreciente segï¿½n su beneficio
                        sort (order.begin(), order.end(), cmpProfmin);
                        break;
        }

        itOrderHoles = 0;
        it = 0;
        nPieces = 0;
	returnPieces = 0;
        
	while (nPieces < n) { // hasta que se coloquen las n piezas
                
        	// Buscar hueco de mayor area, menor area, menor x o mayor y
                if (!orderHoles.empty()) {
                	itOrderHoles = it;
                        idHole = orderHoles[itOrderHoles];
                        xHole = holes[idHole].x;
                        yHole = holes[idHole].y;
                        widthHole = holes[idHole].w;
                        lengthHole = holes[idHole].l;
                        it++;
                }
                else {
                        idHole = -1;				// Material completo
                        xHole = 0;
                        yHole = 0;
                        widthHole = widSheet;
                        lengthHole = lenSheet;
                }
                        	
        
                piece = order[nPieces].p;

		if ((!orderHoles.empty() && it == orderHoles.size()) && ((widthHole < wid[piece]) || lengthHole < (len[piece]))) {
			returnPieces = nPieces;
			break;            
		}


                if ((widthHole < wid[piece]) || (lengthHole < len[piece])) 
                        continue; 	// La pieza no cabe en ese hueco.
                

                nPieces++; 		// Se coloca la pieza
                        
                // Largos, anchos y coord de los huecos generados al colocar pieza	
             
                lh1 = lengthHole - len[piece];
                wh1 = widthHole;
                xh1 = xHole + len[piece];
                yh1 = yHole;
                lh2 = len[piece];
                wh2 = widthHole - wid[piece];
                xh2 = xHole;
                yh2 = yHole + wid[piece];
                
                // Si ninguno de los huecos generados es 0
                if (lh1 != 0 && wh2 != 0) {
                        //  p h2 V h1 H		
                        node = new T_Node;
                        getTreeHole0(node, idHole, piece, 1);

                        // Crear los 2 nuevos huecos, introducirlos en holes y eliminar el previo
                        h1.l = lh1;
                        h1.w = wh1;
                        h1.a = lh1 * wh1;
                        h1.x = xh1;
                        h1.y = yh1;
                        h1.ptr = node->right;
                        h2.l = lh2;
                        h2.w = wh2;
                        h2.a = lh2 * wh2;
                        h2.x = xh2;
                        h2.y = yh2;
                        h2.ptr = node->left->right;
                        if (idHole == -1) { 
                                holes.push_back(h2);
                                itHoles2 = holes.size()-1;
                        }	
                        else { 
                                podar(holes[idHole].ptr);
                                holes[idHole] = h2;
                                itHoles2 = idHole;
                        }	
                        holes.push_back(h1);
                        itHoles1 = holes.size()-1;

                        // Eliminar el hueco de orderHoles e introducir en orden los dos nuevos
                        switch(typeOrder) {
                                case 0:
                                case 1: param = h2.a;
                                        break;
                                case 2: param = h2.x;
                                        break;
                                case 3: param = h2.y;
                        }
                        if (idHole != -1) { 
                                orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                insertOrderHole(idHole, param);
                        }
                        else
                                insertOrderHole(0, param);
                        switch(typeOrder) {
                                case 0:
                                case 1: param = h1.a;
                                        break;
                                case 2: param = h1.x;
                                        break;
                                case 3: param = h1.y;
                        }
                        insertOrderHole(holes.size()-1, param);
                        
                        // Insertar el hueco 2 en la lista de huecos superiores
                        insertTopHole(itHoles2, holes[itHoles2].w);
                }
                else {
                        // Si el 1º hueco es vacio
                        if (lh1 == 0 && wh2 != 0) {
                                // p h2 V
                                node = new T_Node;
                                getTreeHole0(node, idHole, piece ,2);
                                
                                // Crear el hueco nuevo sustituyendo al previo
                                holes[idHole].l = lh2;
                                holes[idHole].w = wh2;
                                holes[idHole].a = lh2 * wh2;
                                holes[idHole].x = xh2;
                                holes[idHole].y = yh2;
                                podar(holes[idHole].ptr);
                                holes[idHole].ptr = node->right;
                                
                                // Eliminar el hueco de orderHoles e introducir el nuevo
                                orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                switch(typeOrder) {
                                        case 0:
                                        case 1: param = holes[idHole].a;
                                        	break;
                                        case 2: param = holes[idHole].x;
                                                break;
                                        case 3: param = holes[idHole].y;
                                }
                                insertOrderHole(idHole, param);
                                
                                itHoles1 = -1;
                                itHoles2 = idHole;	
                                
                                // Insertar el hueco 2 en la lista de huecos superiores
                                insertTopHole(itHoles2, holes[itHoles2].w);
                        }
                        else {
                                // Si el 2º hueco es vacio
                                if (lh1 != 0 && wh2 == 0) {
                                        // p h1 H
                                        node = new T_Node;
                                        getTreeHole0(node, idHole, piece ,3);
                                        
                                        // Crear el hueco nuevo sustituyendo al previo
                                        h1.l = lh1;
                                        h1.w = wh1;
                                        h1.a = lh1 * wh1;
                                        h1.x = xh1;
                                        h1.y = yh1;
                                        h1.ptr = node->right;

                                        if (idHole != -1) {
                                                podar(holes[idHole].ptr);
                                                holes[idHole] = h1;
                                                itHoles1 = idHole;
                                        }
                                        else {
                                                holes.push_back(h1);
                                                itHoles1 = holes.size() - 1;
                                        }
                                        // Eliminar el hueco de orderHoles e introducir el nuevo
                                        switch(typeOrder) {
                                                case 0:
                                                case 1: param = h1.a;
                                                        break;
                                                case 2: param = h1.x;
                                                        break;
                                                case 3: param = h1.y;
                                        }
                                        if (idHole != -1) {
                                                orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                                insertOrderHole(idHole, param);
                                        }
                                        else
                                                insertOrderHole(0, param);
                                        itHoles2 = -1;	
                                }
                                else {
                                        // Si los huecos que se generan son vacios los dos
                                        // p
                                        node = new T_Node;
                                        getTreeHole0(node, idHole, piece ,4);

                                        // Eliminar el hueco que se ha rellenado
                                        orderHoles.erase(orderHoles.begin() + itOrderHoles);
                                        delHole(idHole);
                                        itHoles1 = -1;
                                        itHoles2 = -1;
                                }
                        
                        }					
                }
              
		it  = 0;
                itOrderHoles = 0;

                while (nPieces < n) {  //!!!!!!!!!!!!!!!
                        piece = order[nPieces].p;

                        int i = 0;
                        bool fit = false;
                        // Si hay huecos en la lista de huecos superiores, buscar uno en el que quepa la pieza y se coloca
                        if (!topHoles.empty()) {
                                for (i = 0; i < topHoles.size();i++) 
                                        if (holes[topHoles[i]].l >= len[piece] && holes[topHoles[i]].w >= wid[piece]) 
                                                break;

                                // Si cabe en un hueco colocarla
                                if (i < topHoles.size()) {
                                        fit = true;

                                        itHoles = topHoles[i];

                                        widthHole = holes[itHoles].w;
                                        lengthHole = holes[itHoles].l;
                                        xHole = holes[itHoles].x;
                                        yHole = holes[itHoles].y;

                                        // Largos y anchos de los huecos generados al colocar pieza
                                        lh3 = lengthHole - len[piece];
                                        wh3 = wid[piece];
                                        xh3 = xHole + len[piece];
                                        yh3 = yHole;
                                        lh4 = lengthHole;
                                        wh4 = widthHole - wid[piece];
                                        xh4 = xHole;
                                        yh4 = yHole + wid[piece];

                                        // Si ninguno de los huecos generados es 0
                                        if (lh3 != 0 && wh4 != 0) {
                                                // p h3 H h4 V
                                                node = new T_Node;
                                                getTreeHole2(node, itHoles, piece,1);
                                        
                                                // Crear los 2 nuevos huecos, introducirlos en holes y eliminar el previo		
                                                h3.l = lh3;
                                                h3.w = wh3;
                                                h3.a = lh3 * wh3;
                                                h3.x = xh3;
                                                h3.y = yh3;
                                                h3.ptr = node->left->right;
                                                h4.l = lh4;
                                                h4.w = wh4;
                                                h4.a = lh4 * wh4;
                                                h4.x = xh4;
                                                h4.y = yh4;
                                                h4.ptr = node->right;
                                                podar(holes[itHoles].ptr);
                                                holes[itHoles] = h3;
                                                holes.push_back(h4);

                                                // Eliminar el hueco de orderHoles e introducir en orden los dos nuevos
                                                delOrderHoleId(itHoles);
                                                switch(typeOrder) {
                                                        case 0:
                                                        case 1: param = h3.a;
                                                                break;
                                                        case 2: param = h3.x;
                                                                break;
                                                        case 3: param = h3.y;
                                                }
                                                insertOrderHole(itHoles, param);
                                                switch(typeOrder) {
                                                        case 0:
                                                        case 1: param = h4.a;
                                                                break;
                                                        case 2: param = h4.x;
                                                                break;
                                                        case 3: param = h4.y;
                                                }
                                                insertOrderHole(holes.size()-1, param);

                                                // Eliminar hueco en topHoles y agreagar el nuevo
                                                topHoles.erase(topHoles.begin()+i);
                                                insertTopHole(holes.size()-1, h4.w);
                                        }
                                        else {
                                                // Si el 1º hueco es vacio
                                                if (lh3 == 0 && wh4 != 0) {
                                                        // p h4 V
                                                        node = new T_Node;
                                                        getTreeHole2(node, itHoles,piece,2);
        
                                                        // Crear el hueco nuevo sustituyendo al previo
                                                        holes[itHoles].l = lh4;
                                                        holes[itHoles].w = wh4;
                                                        holes[itHoles].a = lh4 * wh4;
                                                        holes[itHoles].x = xh4;
                                                        holes[itHoles].y = yh4;
                                                        podar(holes[itHoles].ptr);
                                                        holes[itHoles].ptr = node->right;

                                                        // Eliminar el hueco de orderHoles e introducir el nuevo
                                                        delOrderHoleId(itHoles);
                                                        switch(typeOrder) {
                                                                case 0:
                                                                case 1: param = holes[itHoles].a;
                                                                        break;
                                                                case 2: param = holes[itHoles].x;
                                                                        break;
                                                                case 3: param = holes[itHoles].y;
                                                        }
                                                        insertOrderHole(itHoles, param);

                                                        // Eliminar hueco en topHoles y agreagar el nuevo
                                                        topHoles.erase(topHoles.begin()+i);
                                                        insertTopHole(itHoles, wh4);
                                                        
                                                }	
                                                else {
                                                        // Si el 2º hueco es vacio
                                                        if (lh3 != 0 && wh4 == 0) {
                                                                //p h3 H
                                                                node = new T_Node;
                                                                getTreeHole2(node, itHoles, piece, 3);

                                                                // Crear el hueco nuevo sustituyendo al previo
                                                                h3.l = lh3;
                                                                h3.w = wh3;
                                                                h3.a = lh3 * wh3;
                                                                h3.x = xh3;
                                                                h3.y = yh3;
                                                                h3.ptr = node->right;

                                                                podar(holes[itHoles].ptr);
                                                                holes[itHoles] = h3;
                                                        
                                                                // Eliminar el hueco de orderHoles e introducir el nuevo
                                                                delOrderHoleId(itHoles);
                                                                switch(typeOrder) {
                                                                        case 0:
                                                                        case 1: param = h3.a;
                                                                                break;
                                                                        case 2: param = h3.x;
                                                                                break;
                                                                        case 3: param = h3.y;
                                                                }
                                                                insertOrderHole(itHoles, param);

                                                                // Eliminar hueco en topHoles
                                                                topHoles.erase(topHoles.begin()+i);
                                                        }
                                                        else {
                                                                // Si los huecos que se generan son vacios los dos
                                                                       // p
                                                                node = new T_Node;
                                                                getTreeHole2(node, itHoles, piece, 4);					

                                                                // Eliminar el hueco que se ha rellenado
                                                                delOrderHoleId(itHoles);
                                                                delHole(itHoles);
                                                                if (itHoles < itHoles1)
                                                                      itHoles1--;
                                                                
                                                                // Eliminar hueco en topHoles
                                                                topHoles.erase(topHoles.begin()+i);
                                                        }
                                                }

                                        }
                                        
                                }
                        }
                        // Si no se ha podido encajar la pieza en ningun hueco superior	
                        if (!fit) {
                                // Si cabe en el hueco h1
                                if (itHoles1 != -1 && holes[itHoles1].l >= len[piece] && holes[itHoles1].w >= wid[piece]) {
                                        // Largos y anchos de los huecos generados al colocar pieza
                                        widthHole = holes[itHoles1].w;
                                        lengthHole = holes[itHoles1].l;
                                        xHole =  holes[itHoles1].x;
                                        yHole =  holes[itHoles1].y;
               
                                        
                                        lh3 = lengthHole - len[piece];
                                        wh3 = widthHole;
                                        xh3 = xHole + len[piece];
                                        yh3 = yHole;
                                        lh4 = len[piece];
                                        wh4 = widthHole - wid[piece];
                                        xh4 = xHole;
                                        yh4 = yHole + wid[piece];

                                        // Si ninguno de los huecos generados es 0
                                        if (lh3 != 0 && wh4 != 0){
                                                // p h4 V h3 H
                                                node = new T_Node;
                                                getTreeHole0(node, itHoles1, piece, 1);

                                                // Crear los 2 nuevos huecos, introducirlos en holes y eliminar el previo
                                                h3.l = lh3;
                                                h3.w = wh3;
                                                h3.a = lh3 * wh3;
                                                h3.x = xh3;
                                                h3.y = yh3;
                                                h3.ptr = node->right;
                                                h4.l = lh4;
                                                h4.w = wh4;
                                                h4.a = lh4 * wh4;
                                                h4.x = xh4;
                                                h4.y = yh4;
                                                h4.ptr = node->left->right;
                                                podar(holes[itHoles1].ptr);
                                                holes[itHoles1] = h4;
                                
                                                holes.push_back(h3);

                                                // Eliminar el hueco de orderHoles e introducir en orden los dos nuevos
                                                delOrderHoleId(itHoles1);
                                                switch(typeOrder) {
                                                        case 0:
                                                        case 1: param = h4.a;
                                                                break;
                                                        case 2: param = h4.x;
                                                                break;
                                                        case 3: param = h4.y;
                                                }
                                                insertOrderHole(itHoles1, param);
                                                switch(typeOrder) {
                                                        case 0:
                                                        case 1: param = h3.a;
                                                                break;
                                                        case 2: param = h3.x;
                                                                break;
                                                        case 3: param = h3.y;
                                                }
                                                insertOrderHole(holes.size()-1, param);

                                                // Añadir hueco h4 a topHoles
                                                insertTopHole(itHoles1, holes[itHoles1].w);

                                                itHoles1 = holes.size()-1;
                                        }
                                        else {
                                                // Si el 1º hueco es vacio
                                                if (lh3 == 0 && wh4 != 0) {
                                                        // p h4 V
                                                        node = new T_Node;
                                                        getTreeHole0(node, itHoles1, piece, 2);

                                                        // Crear el hueco nuevo sustituyendo al previo
                                                        holes[itHoles1].l = lh4;
                                                        holes[itHoles1].w = wh4;
                                                        holes[itHoles1].a = lh4 * wh4;
                                                        holes[itHoles1].x = xh4;
                                                        holes[itHoles1].y = yh4;
                                                        podar(holes[itHoles1].ptr);
                                                        holes[itHoles1].ptr = node->right;

                                                        // Eliminar el hueco de orderHoles e introducir el nuevo
                                                        delOrderHoleId(itHoles1);
                                                        switch(typeOrder) {
                                                                case 0:
                                                                case 1: param = holes[itHoles1].a;
                                                                        break;
                                                                case 2: param = holes[itHoles1].x;
                                                                        break;
                                                                case 3: param = holes[itHoles1].y;
                                                        }
                                                        insertOrderHole(itHoles1, param);
                                                        
                                                        // Añadir hueco h4 a topHoles
                                                        insertTopHole(itHoles1, holes[itHoles1].w);

                                                        itHoles1 = -1;
                                                }
                                                else {
                                                        // Si el 2º hueco es vacio
                                                        if (lh3 != 0 && wh4 == 0) {
                                                                //p h3 H
                                                                node = new T_Node;
                                                                getTreeHole0(node, itHoles1, piece, 3);

                                                                // Crear el hueco nuevo sustituyendo al previo
                                                                h3.l = lh3;
                                                                h3.w = wh3;
                                                                h3.a = lh3 * wh3;
                                                                h3.x = xh3;	
                                                                h3.y = yh3;
                                                                h3.ptr = node->right;

                                                                podar(holes[itHoles1].ptr);
                                                                holes[itHoles1] = h3;
                                                                // Eliminar el hueco de orderHoles e introducir el nuevo
                                                                delOrderHoleId(itHoles1);
                                                                switch(typeOrder) {
                                                                        case 0:
                                                                        case 1: param = h3.a;
                                                                                break;
                                                                        case 2: param = h3.x;
                                                                                break;
                                                                        case 3: param = h3.y;
                                                                }
                                                                insertOrderHole(itHoles1, param);
                                                                
                                                        }
                                                        else {
                                                                // Si los huecos que se generan son vacios los dos
                                                                // p
                                                                node = new T_Node;
                                                                getTreeHole0(node, itHoles1, piece,4);

                                                                // Eliminar el hueco que se ha rellenado
                                                                delOrderHoleId(itHoles1);
                                                                delHole(itHoles1);
                                                                itHoles1 = -1;
                                                        }

                                                }
                                        }	

                                }
                                else {
                                        // Si el hueco h1 no existe o no cabe la pieza
                                        //topHoles.clear(); //  ?????? cambio a otro hueco, borro los top de este?
					returnPieces = nPieces;
                                        break;
                                }
                        }

                        nPieces++;	
                        if (nPieces < n) { //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                                piece = order[nPieces].p;			
	}
                        
                } //while2

        } // while1

	topHoles.clear();
        order.erase(order.begin(),order.begin()+nPieces);
	returnPieces = nPieces;	
	return returnPieces;	
}

//------------------------------------------------------------------------------
// Codigo auxiliar para iniciar una determinada solucion
//------------------------------------------------------------------------------
void TDCSP::fixSolution(void) {
        /*string gene("72 50 H 51 52 H 59 H H 12 65 20 H 66 H H 48 18 H 49 H 67 68 H V 4 5 H H V V");
        stringstream geneStream(gene); 
        string elem; 
        unsigned int i = 0;

	notation = new int[totalSize];

        // Almacenar el gen
        while (geneStream >> elem) {
                if (elem == "H") 
                        notation[i] = -1; 
                else if (elem == "V") 
                        notation[i] = -2; 
                else 
                        notation[i] = atoi(elem.c_str()); 
                i++;
        }
        
	itFinal = i;
        // Evalua y muestra la solucion
	calculateProfitCuts();*/

	numHS = 1;
	setVar(0,8);
	setVar(1,totalNumPat);
	setVar(2,2);

	cout << "evaluar" << endl;
	evaluate();

        cout << "Profit: " << (int) getObj(0) << endl; 
        cout << "Cortes: " << (int) getObj(1) << endl; 
	exit(-1);                         
}

