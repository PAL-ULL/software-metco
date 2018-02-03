 /***********************************************************************************
 * AUTORES
 *    Jesica de Armas Adrian 
 *    
 * FECHA
 *    Abril 2010
 ************************************************************************************/

#include "TDCSP.h"
#include <vector>
#include <stack>
#include <set>
#include <algorithm>

// Direcciones de optimizacion
double* TDCSP::len;
double* TDCSP::wid;
double* TDCSP::prof;
double TDCSP::widSheet;
double TDCSP::lenSheet;

int TDCSP::totalSize;
int TDCSP::numPat;
int TDCSP::totalNumPat;
bool TDCSP::firstin;

//-------------------------------------------------------------------------------
// Funcion auxiliar para la ordenacion de pieces segun su largo
//------------------------------------------------------------------------------
bool cmpLength(const P_Order &p1, const P_Order &p2) {
	        return (p1.len < p2.len);
}
//-------------------------------------------------------------------------------
// Funcion auxiliar para la ordenacion de pieces segun su ancho
//------------------------------------------------------------------------------
bool cmpWidth(const P_Order &p1, const P_Order &p2) {
	        return (p1.wid < p2.wid);
}
//-------------------------------------------------------------------------------
// Funcion auxiliar para la ordenacion de pieces segun su relacion prof/area
//------------------------------------------------------------------------------
bool cmpProfArea(const ProfArea &p1, const ProfArea &p2) {
	        return (p1.val > p2.val);
}
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
// ------------------------------------------------------------------------------
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
	
	setNumberOfVar(totalSize); // vars number 
	setNumberOfObj(NOBJ);
	
	firstin = true;
	
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

//----------------------------------------------------------------------------------
// Check condition 1 <= no <= np - 1 and no = np - 1, for an operator:
// no = number of operators to its left plus itself
// np = number of patterns to its left
// ---------------------------------------------------------------------------------
bool TDCSP::checkCondition() {
	int variable;
	int numP = 0;
	int numO = 0;

	for (int i = 0; i < size; i++) {
		variable = (int) getVar(i);
		if (variable != H && variable != V)
			numP++;
		else {
			numO++;
			if ((numO < 1) || (numO > (numP - 1)))
				return false;
		}
	}
	if (numO != numP - 1)
		return false;
	
	return true;
}
//--------------------------------------------------------------------------------
bool TDCSP::improveOne(Individual *ind, int objIndex) {
	if (objIndex == 0) {
		if (getObj(objIndex) > ind->getObj(objIndex))
			return true;
		else
			return false;
				
	}
	else { 
        	if (getObj(objIndex) < ind->getObj(objIndex))
                	return true;
        	else
                	return false;
	}
}

// -------------------------------------------------------------------------------
// 1. Calcular l y w
// -------------------------------------------------------------------------------
bool TDCSP::repair(void) {
	int var, a, b, c, numOp, numPie, finalSize;
	double lengthA, lengthB, widthA, widthB;
	double  *l, *w;
	int numNewP = size/2;
	stack<int> st;
	bool returnValue = false;

//	if (!fit()) {
		// 1) Detectar cúando se pasa y cortar
/*
 		l = new double[numNewP];
   		w = new double[numNewP];
	 
		c = totalNumPat;
		
		finalSize = 1;
		numOp = numPie = 0;

		for (int i = 0; i < size; i++) {
			var = (int) getVar(i);

			if (var != V && var != H)  {
				st.push(var);
				numPie++;
			}	
			else {
				numOp++;
				a = st.top();
				st.pop();
				b = st.top();
				st.pop();
				
				if (a >= totalNumPat) { 		// es una pieza creada por concatenacion
					lengthA = l[a-totalNumPat];
					widthA = w[a-totalNumPat];
				}
				else {			// es una pieza original
					lengthA = len[a];
					widthA = wid[a];
				}
				if (b >= totalNumPat) {		// es una pieza creada por concatenación
					lengthB = l[b-totalNumPat];
					widthB = w[b-totalNumPat];                                                                                                  
				}
				else {                         		// es una pieza original                
					lengthB = len[b];
					widthB = wid[b]; 
				}
			
				if (var == H) {
					l[c - totalNumPat] = lengthA + lengthB;
					w[c - totalNumPat] = max(widthA, widthB);
						
				}
				else {
					l[c - totalNumPat] = max(lengthA, lengthB);
					w[c - totalNumPat] = widthA + widthB;
				}
				if (l[c - totalNumPat] > lenSheet) {
					size = finalSize;
					break;
				}
				if (w[c - totalNumPat] > widSheet) {
					size = finalSize;
					break;
				}
				if (numOp == numPie -1) 
					finalSize = 2 * numPie  - 1;
				st.push(c);
				c++;
			}
		}
		a = st.top();
		st.pop();
		delete(l);
		delete(w);
		
		return true;

*/	
		// 4) Cambiar una pieza por otra, un operador por otro, etc y evaluar de nuevo quedandome con el mejor, LS

		int v1, variable,rep, new_variable;
		set <int> pieces;

		//Cortar
		if (!fit()) {
                        numNewP = size/2;

                        l = new double[numNewP];
                        w = new double[numNewP];

                        c = totalNumPat;

                        finalSize = 1;
                        numOp = numPie = 0;

                        for (int i = 0; i < size; i++) {
                                var = (int) getVar(i);

                                if (var != V && var != H)  {
                                        st.push(var);
                                        numPie++;
                                }
                                else {
                                        numOp++;
                                        a = st.top();
                                        st.pop();
                                        b = st.top();
                                        st.pop();

                                        if (a >= totalNumPat) {                 // es una pieza creada por concatenacion
                                                lengthA = l[a-totalNumPat];
                                                widthA = w[a-totalNumPat];
                                        }
                                        else {                  // es una pieza original
                                                lengthA = len[a];
                                                widthA = wid[a];
                                        }
                                        if (b >= totalNumPat) {         // es una pieza creada por concatenación
                                                lengthB = l[b-totalNumPat];
                                                widthB = w[b-totalNumPat];
                                        }
                                        else {                                  // es una pieza original
                                                lengthB = len[b];
                                                widthB = wid[b];
                                        }

                                        if (var == H) {
                                                l[c - totalNumPat] = lengthA + lengthB;
                                                w[c - totalNumPat] = max(widthA, widthB);

                                        }
                                        else {
                                                l[c - totalNumPat] = max(lengthA, lengthB);
                                                w[c - totalNumPat] = widthA + widthB;
                                        }
                                        if (l[c - totalNumPat] > lenSheet) {
                                                size = finalSize;
                                                break;
                                        }
                                        if (w[c - totalNumPat] > widSheet) {
                                                size = finalSize;
                                                break;
                                        }
                                        if (numOp == numPie -1)
                                                finalSize = 2 * numPie  - 1;
                                        st.push(c);
                                        c++;
                                }
                        }
                        a = st.top();
                        st.pop();
                        delete(l);
                        delete(w);
                }


		double improve = rand() / (RAND_MAX + 1.0);
		if (improve < 0.25) {
		
		evaluate();
	
		pieces.clear();

        	TDCSP *new_ind = (TDCSP *) internalClone();
        	TDCSP *copy_this = (TDCSP *) internalClone();
		
		double one0 = rand() / (RAND_MAX + 1.0);
		double one1 = 1 - one0;
		
		int numPieces = (size + 1)/ 2;

        	// Extracción de los patrones
        	for (int i = 0; i < size; i++) {
                	v1 = (int) getVar(i);
                	if (v1 >= 0)
                        	pieces.insert(v1);
        	}
 		int i = 0;
		while (i < size && size != totalSize) {
			variable = (int) copy_this->getVar(i); // lo cambie
			new_variable = variable;
			if ((variable != H) && (variable != V)) {
				rep = 0;
				while ((!new_ind->fit()) && (rep < totalNumPat/5)) {
					new_variable = uniformRandom(totalNumPat);
					while (pieces.find(new_variable) != pieces.end()) 	
						new_variable = uniformRandom(totalNumPat);

					new_ind->setVar(i, new_variable);	
					rep++;
				}
			}
			else {
				if (variable == H)
					new_ind->setVar(i, V);
				else
					new_ind->setVar(i, H);
			}

			if (new_ind->fit()) {
				new_ind->evaluate();
                        	if (one0  < 0.5) {
					if (new_ind->improveOne(copy_this, 0)) {
						//delete (copy_this);
                                        	//copy_this = (TDCSP *) new_ind->internalClone();
                                        	//delete (new_ind);
                                        	//new_ind = (TDCSP *) copy_this->internalClone();

						for (int j = 0; j < ((TDCSP *)new_ind)->size; j++)
							copy_this->setVar(j, new_ind->getVar(j));
						copy_this->setObj(0, new_ind->getObj(0));
						copy_this->setObj(1, new_ind->getObj(1));
        					((TDCSP *)copy_this)->size = ((TDCSP *)new_ind)->size;
										
						
						if ((variable != H) && (variable != V)) {
                                        		pieces.erase(variable);
                                       			pieces.insert(new_variable);
						}
						returnValue = true;
					}
					else {
						//delete (new_ind);
						//new_ind = (TDCSP *) copy_this->internalClone();
						for (int j = 0; j < ((TDCSP *)copy_this)->size; j++)
                                                        new_ind->setVar(j, copy_this->getVar(j));
                                                new_ind->setObj(0, copy_this->getObj(0));
                                                new_ind->setObj(1, copy_this->getObj(1));
        					((TDCSP *)new_ind)->size = ((TDCSP *)copy_this)->size;

					}

				}
				else {
					if (new_ind->improveOne(copy_this, 1)) {
                                                //delete (copy_this);
                                                //copy_this = (TDCSP *) new_ind->internalClone();
                                                //delete (new_ind);
                                                //new_ind = (TDCSP *) copy_this->internalClone();
						
						for (int j = 0; j < ((TDCSP *)new_ind)->size; j++)
                                                        copy_this->setVar(j, new_ind->getVar(j));
                                                copy_this->setObj(0, new_ind->getObj(0));
                                                copy_this->setObj(1, new_ind->getObj(1));
                                                ((TDCSP *)copy_this)->size = ((TDCSP *)new_ind)->size;

						if ((variable != H) && (variable != V)) {
                                                	pieces.erase(variable);
                                                	pieces.insert(new_variable);
						}
						returnValue = true;
                                        }
                                        else {
						//delete (new_ind);
                                                //new_ind = (TDCSP *) copy_this->internalClone();
                                        	for (int j = 0; j < ((TDCSP *)copy_this)->size; j++)
                                                        new_ind->setVar(j, copy_this->getVar(j));
                                                new_ind->setObj(0, copy_this->getObj(0));
                                                new_ind->setObj(1, copy_this->getObj(1));
                                                ((TDCSP *)new_ind)->size = ((TDCSP *)copy_this)->size;
					}
				}
			}
                        else {                                  
                        	//delete (new_ind);
                        	//new_ind = (TDCSP *) copy_this->internalClone();
				for (int j = 0; j < ((TDCSP *)copy_this)->size; j++)
                                	new_ind->setVar(j, copy_this->getVar(j));
                                new_ind->setObj(0, copy_this->getObj(0));
                                new_ind->setObj(1, copy_this->getObj(1));
                                ((TDCSP *)new_ind)->size = ((TDCSP *)copy_this)->size;
			}

			i++;
		}	

		if (!copy_this->fit()) {
			//cortar
			numNewP = ((TDCSP *)copy_this)->size/2;

			l = new double[numNewP];
   			w = new double[numNewP];
	 	
			c = totalNumPat;
		
			finalSize = 1;
			numOp = numPie = 0;

			for (int i = 0; i < ((TDCSP *)copy_this)->size; i++) {
				var = (int) copy_this->getVar(i);
	
				if (var != V && var != H)  {
					st.push(var);
					numPie++;
				}	
				else {
					numOp++;
					a = st.top();
					st.pop();
					b = st.top();
					st.pop();
				
					if (a >= totalNumPat) { 		// es una pieza creada por concatenacion
						lengthA = l[a-totalNumPat];
						widthA = w[a-totalNumPat];
					}
					else {			// es una pieza original
						lengthA = len[a];
						widthA = wid[a];
					}
					if (b >= totalNumPat) {		// es una pieza creada por concatenación
						lengthB = l[b-totalNumPat];
						widthB = w[b-totalNumPat];                                                                                                  
					}
					else {                         		// es una pieza original                
						lengthB = len[b];
						widthB = wid[b]; 
					}
			
					if (var == H) {
						l[c - totalNumPat] = lengthA + lengthB;
						w[c - totalNumPat] = max(widthA, widthB);
							
					}
					else {
						l[c - totalNumPat] = max(lengthA, lengthB);
						w[c - totalNumPat] = widthA + widthB;
					}
					if (l[c - totalNumPat] > lenSheet) {
						((TDCSP *)copy_this)->size = finalSize;
						break;
					}
					if (w[c - totalNumPat] > widSheet) {
						((TDCSP *)copy_this)->size = finalSize;
						break;
					}
					if (numOp == numPie -1) 
						finalSize = 2 * numPie  - 1;
					st.push(c);
					c++;
				}
			}
			a = st.top();
			st.pop();
			delete(l);
			delete(w);
			
			returnValue = true;
		}
		

		//for (int j = 0; j < copy_this->getNumberOfVar(); j++)
		for (int j = 0; j < ((TDCSP *)copy_this)->size; j++)
 	               setVar(j, copy_this->getVar(j));
        	setObj(0, copy_this->getObj(0));
        	setObj(1, copy_this->getObj(1));

        	size = ((TDCSP *)copy_this)->size;
		
		delete(copy_this);
		delete(new_ind);
				
//-----------------------------------------------
//quite
		pieces.clear();
		vector <P_Order> piecesOrd;
		
		for (int i = 0; i < size; i++) {
                        v1 = (int) getVar(i);
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
		sort (piecesOrd.begin(), piecesOrd.end(), cmpLength);
	
		int it = 0;
		
		while (!piecesOrd.empty()) {
			// si cabe la pieza 
			if (getRemainSize(0) >= piecesOrd[it].len) {
				if (piecesOrd.size() > 1 && it < piecesOrd.size()-1) {
					// si no cabe la siguiente pieza 
					if (getRemainSize(0) < piecesOrd[it+1].len) {
						pilUp(piecesOrd,it,H);
						returnValue = true;
						it = 0;
					}
					else // si cabe la siguiente pieza
						it++;	
				}
				else {
					if (piecesOrd.size() == 1) {
						setVar(size, piecesOrd[it].pat);
						setVar(size + 1, H);
						size += 2;
						piecesOrd.pop_back();
						returnValue = true;
					}
					else { // si it estÃ¡ en el ultimo lugar,no hay mas piezas despues
						pilUp(piecesOrd,it,H);	
						returnValue = true;
                                                it = 0;
					}
				}
			}
			else 
				break;
		}
		
		if (!piecesOrd.empty()) {
			sort (piecesOrd.begin(), piecesOrd.end(), cmpWidth);
			it = 0;
		
			while (!piecesOrd.empty()) {
				// si cabe la pieza 
				if (getRemainSize(1) >= piecesOrd[it].wid) {
					if (piecesOrd.size() > 1 && it < piecesOrd.size()-1) {
						// si no cabe la siguiente pieza
						if (getRemainSize(1) < piecesOrd[it+1].wid) {
							pilUp(piecesOrd,it,V);
							returnValue = true;
							it = 0;
						}
						else // si cabe la siguiente pieza
							it++;
					}
					else {
						if (piecesOrd.size() == 1) {
							setVar(size, piecesOrd[it].pat);
							setVar(size + 1, V);
							size += 2;
							piecesOrd.pop_back();
							returnValue = true;
						}
						else { // si it estÃ¡ en el ultimo lugar
                                                	pilUp(piecesOrd,it,V);
							returnValue = true;
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
	//quite
		}	
/*------------------------------------------		
		// intentar agregar una pieza con un operador aqui y si cabe cortar dos puestos mÃs adelante. Hacerlo una serie de veces!!!!!!
		// Extraccion de los patrones
        	for (int i = 0; i < size; i++) {
                	v1 = (int) getVar(i);
                	if (v1 >= 0)
                        	pieces.insert(v1);
        	}
		int numRep = 0;
 		int newOperator;
		int newPiece;
		bool prevfit = true;
		while (prevfit) {
			prevfit = false;
			numRep = 0;
			while(numRep < totalNumPat/5 && size < totalNumPat*2 -1) {
				newOperator = uniformRandom(2) - 2;
				newPiece = uniformRandom(totalNumPat);	// una pieza que no este ya puesta
				while (pieces.find(newPiece) != pieces.end()) 
					newPiece = uniformRandom(totalNumPat);
				
				setVar(size, newPiece);
				setVar(size + 1, newOperator);
				size += 2;
				if (fit()) {
					evaluate();
					prevfit = true;
					pieces.insert(newPiece);
					returnValue = true;
					break;
				}
				else
					size -= 2;
	
				numRep++;
				
			}
		}
		pieces.clear(); 
*/
//----------------------------------------


		return(returnValue);
/*

		// 3) Cambiar H por V y viceversa  y evaluar de nuevo
		//    Si una vez hecho un cambio sigue habiendo problemas en el mismo punto, 
		//    cortar por donde sea una construcción valida
		
		bool change, changed;

		l = new double[numNewP];
   		w = new double[numNewP];
	 
		c = totalNumPat;
		
		changed = false;
		change = false;
		finalSize = 1;
		numOp = numPie = 0;

		for (int i = 0; i < size; i++) {
			var = (int) getVar(i);

			if (var != V && var != H)  {
				st.push(var);
				numPie++;
			}	
			else {
				numOp++;
				a = st.top();
				st.pop();
				b = st.top();
				st.pop();
				if (a >= totalNumPat) { 		// es una pieza creada por concatenacion
					lengthA = l[a-totalNumPat];
					widthA = w[a-totalNumPat];
				}
				else {			// es una pieza original
					lengthA = len[a];
					widthA = wid[a];
				}
				if (b >= totalNumPat) {		// es una pieza creada por concatenación
					lengthB = l[b-totalNumPat];
					widthB = w[b-totalNumPat];                                                                                                  
				}
				else {                         		// es una pieza original                
					lengthB = len[b];
					widthB = wid[b]; 
				}
			
				if (var == H) {
					l[c - totalNumPat] = lengthA + lengthB;
					w[c - totalNumPat] = max(widthA, widthB);

					if (l[c - totalNumPat] > lenSheet) {	// si se supera el largo del material
						if (change) {
							size = finalSize;
							break;	
						}
						else {
							setVar(i, V);                   // cambiar H por V
							i--;
							st.push(a);
							st.push(b);
							change = true;
							changed = true;
							numOp--;
							continue;
						}
					}
					change = false;	
				}
				else {
					l[c - totalNumPat] = max(lengthA, lengthB);
					w[c - totalNumPat] = widthA + widthB;
					
					if (w[c - totalNumPat] > widSheet) {	// si se supera el ancho del material
						if (change) {
							size = finalSize;
							break;	
						}
						else {
							setVar(i, H);                   // cambiar V por H
							i--;
							st.push(a);
							st.push(b);
							change = true;
							changed = true;
							numOp--;
							continue;
						}
					}
					change = false;
				}
				
				if (numOp == numPie - 1) 
					finalSize = 2 * numPie  - 1;		

				st.push(c);
				c++;
			}
		}
		a = st.top();
		st.pop();
		delete(l);
		delete(w);

//----------------------------------
		// intentar agregar una pieza con un operador aqui y si cabe cortar dos puestos mÃs adelante. Hacerlo una serie de veces!!!!!!
 		set<int> pieces;
		int v1;
		// Extraccion de los patrones
        	for (int i = 0; i < size; i++) {
                	v1 = (int) getVar(i);
                	if (v1 >= 0)
                        	pieces.insert(v1);
        	}
		int numRep = 0;
 		int newOperator;
		int newPiece;
		bool prevfit = true;
                while (prevfit) {
                        prevfit = false;
                        numRep = 0;
                        while(numRep < totalNumPat/5 && size < totalNumPat*2 -1) {
                                newOperator = uniformRandom(2) - 2;
                                newPiece = uniformRandom(totalNumPat);  // una pieza que no este ya puesta
                                while (pieces.find(newPiece) != pieces.end())
                                        newPiece = uniformRandom(totalNumPat);

                                setVar(size, newPiece);
                                setVar(size + 1, newOperator);
                                size += 2;
                                if (fit()) {
                                        evaluate();
                                        prevfit = true;
                                        pieces.insert(newPiece);
                                        changed = true;
                                        break;
                                }
                                else
                                        size -= 2;

                                numRep++;

                        }
                }
                pieces.clear();
//---------------------------------- 		
	if (changed)
		return true;

	else
		return false;
*/
	
//	}
//	else {
//		return false;
//	}
}

//-------------------------------------------------------------------------------
// Apila en vertical u horizontal las piezas sobrantes que se van a colocar de relleno
//-------------------------------------------------------------------------------
void TDCSP::pilUp(vector<P_Order> &piecesOrd, int it, int op) {
	int k = 0;
	int count = 0;

	while (k <= it) { // se deberia de salir antes de que se cumpla la condicion
		count = 0;		
		for (int j = it; j >= k; j--) {
			setVar(size, piecesOrd[j].pat);
			size++;
			count++;
		}
		for (int j = 1; j < count; j++) {
			if (op == H)
				setVar(size, V);
			else
				setVar(size, H);
			size++;
		}
		setVar(size, op);
			
		size++;
		if (fit()) {
			piecesOrd.erase(piecesOrd.begin()+k, piecesOrd.begin()+it+1);
			break;
		}
		else {
			size -= 2*count;
			k++;
		}
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
	int numNewP = size/2;
	stack<int> st;
	
 	l = new double[numNewP];
   	w = new double[numNewP];
	 
	c = totalNumPat;
	
	for (int i = 0; i < size; i++) {
		var = (int) getVar(i);

		if (var != V && var != H) 
			st.push(var);	
		else {
			a = st.top();
			st.pop();
			b = st.top();
			st.pop();
			if (a >= totalNumPat) { 		// es una pieza creada por concatenacion
				lengthA = l[a-totalNumPat];
				widthA = w[a-totalNumPat];
			}
			else {			// es una pieza original
				lengthA = len[a];
				widthA = wid[a];
			}
			if (b >= totalNumPat) {		// es una pieza creada por concatenación
				lengthB = l[b-totalNumPat];
				widthB = w[b-totalNumPat];                                                                                                  
			}
			else {                         		// es una pieza original                
				lengthB = len[b];
				widthB = wid[b]; 
			}
			
			if (var == H) {
				l[c - totalNumPat] = lengthA + lengthB;
				w[c - totalNumPat] = max(widthA, widthB);
			}
			else {
				l[c - totalNumPat] = max(lengthA, lengthB);
				w[c - totalNumPat] = widthA + widthB;
			}

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

// -------------------------------------------------------------------------------
//  Calcula l y w 
// devuelve true cuando las dimensiones son correctas
// devuelve false cuando se sobrepasan las dimensiones
// -------------------------------------------------------------------------------
bool TDCSP::fit(void) {
	int var, a, b, c;
	double lengthA, lengthB, widthA, widthB;
	double  *l, *w;
	int numNewP = size/2;
	stack<int> st;
	
 	l = new double[numNewP];
   	w = new double[numNewP];
	 
	c = totalNumPat;
	
	for (int i = 0; i < size; i++) {
		var = (int) getVar(i);

		if (var != V && var != H) 
			st.push(var);	
		else {
			a = st.top();
			st.pop();
			b = st.top();
			st.pop();
			if (a >= totalNumPat) { 		// es una pieza creada por concatenacion
				lengthA = l[a-totalNumPat];
				widthA = w[a-totalNumPat];
			}
			else {			// es una pieza original
				lengthA = len[a];
				widthA = wid[a];
			}
			if (b >= totalNumPat) {		// es una pieza creada por concatenación
				lengthB = l[b-totalNumPat];
				widthB = w[b-totalNumPat];                                                                                                  
			}
			else {                         		// es una pieza original                
				lengthB = len[b];
				widthB = wid[b]; 
			}
			
			if (var == H) {
				l[c - totalNumPat] = lengthA + lengthB;
				w[c - totalNumPat] = max(widthA, widthB);
			}
			else {
				l[c - totalNumPat] = max(lengthA, lengthB);
				w[c - totalNumPat] = widthA + widthB;
			}

			if (l[c - totalNumPat] > lenSheet) {
				delete(l);
				delete(w);
				return false;
			}
			if (w[c - totalNumPat] > widSheet) {
				delete(l);
				delete(w);
				return false;
			}
			st.push(c);
			c++;
		}
	}
	a = st.top();
	st.pop();
/*	
	if (a >= totalNumPat) {
		lengthA = l[a-totalNumPat];
	 	widthA = w[a-totalNumPat];
	}
	else {
		lengthA = len[a];
		widthA = wid[a];
	}

	if (lengthA > lenSheet)
		return false;
	if (widthA > widSheet)
		return false;
*/
	delete(l);
	delete(w);

	return true;
}
// -------------------------------------------------------------------------------
// Generation of an individual
// -------------------------------------------------------------------------------
void TDCSP::restart() {
	randomRestart();
}
// -------------------------------------------------------------------------------
// Random generation of an individual
// -------------------------------------------------------------------------------
void TDCSP::randomRestart() {
	int *pat, *op, j, k, numO;
	int value = uniformRandom(totalNumPat);
	set<int> st;

	pat = new int[totalNumPat]; 		// piezas
	op = new int[totalNumPat - 1];		// operadores

	profarea.resize(totalNumPat);	
	// Se genera un vector de patrones ordenados aleatoriamente
	for (int i = 0; i < totalNumPat; i++) {	
		while(st.find(value) != st.end()) {
			value = uniformRandom(totalNumPat);
		}
		pat[i] = value;
		profarea[i].pat = value;
	 	profarea[i].val = prof[value] * (prof[value]/(len[value] * wid[value]));
		st.insert(value);
	}
	
	/*if (firstin) {
		// Introducir solo un individuo con profit/area ordenado de mayor a menor	
		sort (profarea.begin(), profarea.end(), cmpProfArea);
		for (int i = 0; i < totalNumPat; i++) {
			pat[i] = profarea[i].pat;
		}
		firstin = false;
	
	}*/
	// Se genera un vector de operadores H y V 
	for (int i = 0; i < totalNumPat - 1; i++) {		
		op[i] = uniformRandom(2) - 2;  			
	}
	// los dos primeros elementos han de ser patrones
	setVar(0, pat[0]);
	setVar(1, pat[1]);
	
	j = 2;
	k = 0;
	
	size = 2;


	// 1) Se escoge aleatoriamente un patron o un operador (pat = 0, op = 1)
	// si es patron se agrega si aun quedan por agregar,
	// de lo contrario se agregara un operador
	// si es operador se agrega solo si cumple con la condicion 1 <= no <= np -1,
	// de lo contrario se agrega un patron
	// Si repara, terminar
/*	
	for (int i = 2; i < totalSize; i++) {
		value = uniformRandom(2); 		
		if (value == 0 && j < totalNumPat) {
			setVar(i,pat[j]);
			j++;
			size++;
		}
		else {
			numO = k + 1;
			if ((numO < 1) || (numO > (j - 1))) {
		 		setVar(i,pat[j]);
				j++;
				size++;
			}
			else {
				setVar(i,op[k]);
				k++;
				size++;
				if (numO == j - 1) {
					if (!fit()) {		// Si tuvo que reparar no seguir 
						repair();
						break;
					}
				}
			}
		}
		
	}
*/
	// 2) Cuando no quepa, cambiar la ultima pieza por una posterior una serie de veces.
	// Si no cabe, cambiar operador. Si no cabe, repair.
/*
	int rep, index_gen, aux = 0;
	bool fit_bool, entro;
	entro = false;
	
	for (int i = 2; i < totalSize; i++) {
		value = uniformRandom(2); 		
		if (value == 0 && j < totalNumPat) {
			setVar(i,pat[j]);
			j++;
			size++;
		}
		else {
			numO = k + 1;
			if ((numO < 1) || (numO > (j - 1))) {
		 		setVar(i,pat[j]);
				j++;
				size++;
			}
			else {
				setVar(i,op[k]);
				k++;
				size++;
				if (numO == j - 1) {
					index_gen = i;
					// restar hasta llegar a pieza
					if (!fit()) {
						while ((int) getVar(index_gen) < 0) {
							index_gen--;
						}
					}
					rep = 0;
					//intercambiar con una pieza posterior y reemplazar en gen
					fit_bool = fit();
					while ((!fit_bool) && rep < 50) {	
						value = j + uniformRandom(totalNumPat - j);
						aux = pat[j-1];
						pat[j-1] = pat[value];
						pat[value] = aux;
						setVar(index_gen, pat[j-1]);
						fit_bool = fit();
						rep++;
					}
					if (!fit_bool) {
						if (!entro) {
							entro = true;
							k--;
							size--;
							if (op[k] == H) {
								op[k] = V;
								i--;
								continue;
							}
							else {
								op[k] = H;
								i--;
								continue;
							}	
						}
						else {
							entro = false;  //?????
							repair();
							break;
						}
					}
					else {

					entro = false;
					}
				}
			}
		}
	}
	
*/	
	
	// 3) Crear, cuando no quepa, cambiar operador.
	// Si sigue sin caber, intercambiar la ultima pieza por posterior. Si no cabe, cambiar operador. Si no cabe, dejar como inicio. Repetir
	// Si sigue sin caber, igual con la pieza anterior.
	// No llama a repair().
	// Aplica relleno

	st.clear();
	int aux_size, index_gen1, index_gen2, aux, first_value, sec_value, rep = 0;	
	aux_size = 1;
	//int num_repair = 0;
	
 	for (int i = 2; i < totalSize; i++) {
                value = uniformRandom(2);
                if (value == 0 && j < totalNumPat) {
                        setVar(i,pat[j]);
                        j++;
                        size++;
                }
                else {
                        numO = k + 1;
                        if ((numO < 1) || (numO > (j - 1))) {
                                setVar(i,pat[j]);
                                j++;
                                size++;
                        }
                        else {
                                setVar(i,op[k]);
                                size++;
                                if (numO == j - 1) {
					// Cambiar operador y probar
                                        if (!fit()) {
						if (op[k] == H)
							setVar(i, V);
						else
							setVar(i, H);

//quito
						index_gen1 = i;
                                       	
						// restar hasta llegar a pieza
                                        	if (!fit()) {
                                                	while ((int) getVar(index_gen1) < 0) 
                                                        	index_gen1--;
							 index_gen2 = index_gen1 - 1;
							 while ((int) getVar(index_gen2) < 0)
                                                                index_gen2--;
                                        	}
						
						if ( j < totalNumPat - 1) {
                                        		//intercambiar con una pieza posterior y reemplazar en gen
                                        		rep = 0;
							while ((!fit()) && (rep < totalNumPat/5)) {
                                                		value = j + uniformRandom(totalNumPat - j);
                                               			aux = pat[j-1];
                                                		pat[j-1] = pat[value];
                                                		pat[value] = aux;
                                                		setVar(index_gen1, pat[j-1]);
							
								if (!fit()) {
	                                        	                if (op[k] == H)
        	                                                	        setVar(i, V);
                	                                        	else
                        	                                        	setVar(i, H);
	
        	                                        	}
        	                                        	rep++;
							
								if (!fit()) {
									if (op[k] == H)
                                                                                setVar(i, V);
                                                                        else
                                                                                setVar(i, H);

									aux = pat[j-1];
                                                                	pat[j-1] = pat[value];
                                                                	pat[value] = aux;
                                                                	setVar(index_gen1, pat[j-1]);
								}							
                	                        	}
							
							rep = 0;
                                                	while ((!fit()) && (rep < totalNumPat/5)) {
                                                        	value = j  + uniformRandom(totalNumPat - j);
                                                        	aux = pat[j-2];
                                                        	pat[j-2] = pat[value];
                                                      		pat[value] = aux;
                                                        	setVar(index_gen2, pat[j-2]);

                                                       		if (!fit()) {
                                                                	if (op[k] == H)
                                                                        	setVar(i, V);
                                                                	else
                                                                        	setVar(i, H);

                                                        	}
                                                        	rep++;
								if (!fit()) {
                                                                        if (op[k] == H)
                                                                                setVar(i, V);                                                                    
                                                                        else                                                                     
                                                                                setVar(i, H);                                                                                     
                                                                
                                                                        aux = pat[j-2];
                                                                        pat[j-2] = pat[value];
                                                                        pat[value] = aux;
                                                                        setVar(index_gen2, pat[j-2]);
                                                                }
                                                	}
						} //quito
						if (!fit()) {
							size = aux_size;
						//quite
							double improve = rand() / (RAND_MAX + 1.0);
							if (improve < 0.25) {
							//-------------------------
							set <int> pieces;
							int v1;

							vector <P_Order> piecesOrd;
		
							for (int i = 0; i < size; i++) {
                        					v1 = (int) getVar(i);
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
							sort (piecesOrd.begin(), piecesOrd.end(), cmpLength);
	
							int it = 0;
		
							while (!piecesOrd.empty()) {
								// si cabe la pieza 
								if (getRemainSize(0) >= piecesOrd[it].len) {
									if (piecesOrd.size() > 1 && it < piecesOrd.size()-1) {
										// si no cabe la siguiente pieza 
										if (getRemainSize(0) < piecesOrd[it+1].len) {
											pilUp(piecesOrd,it,H);
											it = 0;
										}
										else // si cabe la siguiente pieza
											it++;	
									}
									else {
										if (piecesOrd.size() == 1) {
											setVar(size, piecesOrd[it].pat);
											setVar(size + 1, H);
											size += 2;
											piecesOrd.pop_back();
										}
										else { // si it estÃ¡ en el ultimo lugar,no hay mas piezas despues
											pilUp(piecesOrd,it,H);	
                                                					it = 0;
										}
									}
								}
								else 
									break;
							}
		
							if (!piecesOrd.empty()) {
								sort (piecesOrd.begin(), piecesOrd.end(), cmpWidth);
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
												setVar(size, piecesOrd[it].pat);
												setVar(size + 1, V);
												size += 2;
												piecesOrd.pop_back();
											}
											else { // si it estÃ¡ en el ultimo lugar
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
							//quite	
							/*----------------------------------------------------------------------------
                					// Intentar agregar una pieza con un operador aqui y 
							// si cabe cortar dos puestos mas adelante. X veces.
							// Continuar añadiendo mientras se pueda.
							
							set <int> pieces;
							int v1;

                					// Extraccion de los patrones
                					for (int i = 0; i < size; i++) {
                        					v1 = (int) getVar(i);
                        					if (v1 >= 0)
                                					pieces.insert(v1);
                					}
                					int numRep = 0;
                					int newOperator;
                					int newPiece;
							bool prevfit = true;
							while (prevfit) {
								prevfit = false;
								numRep = 0;
                						while(numRep < totalNumPat/5 && size < totalNumPat*2 -1) {
                        						newOperator = uniformRandom(2) - 2;
                        						newPiece = uniformRandom(totalNumPat);  // una pieza que no este ya puesta
                        						while (pieces.find(newPiece) != pieces.end())
                                						newPiece = uniformRandom(totalNumPat);

                        						setVar(size, newPiece);
                        						setVar(size + 1, newOperator);
                        						size += 2;
                        						if (fit()) {
                                						evaluate();
										prevfit = true;
										pieces.insert(newPiece);		
                                						break;
                        						}
                        						else {
                                						size -= 2;
									}
                       						 	numRep++;
                						}
							}
                					pieces.clear();
							//----------------------------
							*/
							break;
			
						}
						else {
							aux_size = size; 
						}

					}
					else
						aux_size = size;
                                }
				//if (aux_size != size) {
				//	k = (size -1)/ 2;
				//	j = (size +1)/ 2;
				//}
				//else
                                k++;
                        }
                }
	
        }


	delete(pat);
	delete(op);
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
	int var1, var2, aux, point1, point2;

	if (size != 1) {
		point1 = uniformRandom(size); 		
		point2 = uniformRandom(size);
	
		// Asegurar que son diferentes	
		while (point1 == point2)
			point2 = uniformRandom(size);
	
		// El primer punto ha de estar más a la izquierda
		if (point1 > point2) {
			aux = point1;
			point1 = point2;
			point2 = aux;
		}                                                                 
	
		var1 = (int) getVar(point1);
		var2 = (int) getVar(point2);
	
		if ( (var1 < 0 && var2 < 0) || (var1 >= 0 && var2 >= 0) || (var1 < 0 && var2 >= 0)) {
			setVar(point1, var2);
			setVar(point2, var1);
			repair();
		}
		else {
			setVar(point1, var2);
			setVar(point2, var1);
					  
			if (!checkCondition()) {
				setVar(point1, var1);
			 	setVar(point2, var2);
			}
			else {
				repair();	
			}
		}

		if (size != 1) {
			// Se elige un operador aleatorio y se cambia basándose en la probabilidad de mutación
			point1 = uniformRandom(size);

			while (getVar(point1) >= 0) {
				point1 = uniformRandom(size);
			}

			double vmut =  rand() / (RAND_MAX + 1.0);

			if (vmut < pm) {
				if (getVar(point1) == H) 
					setVar(point1, V);
				else 
					setVar(point1, H);
				repair();	
			}
		}
	}
}
//-------------------------------------------------------------------------------
// Crossover
//-------------------------------------------------------------------------------
void TDCSP::dependentCrossover (Individual* ind) {
	crossoverPMX(ind);	

}
//-------------------------------------------------------------------------------
// Crossover PMX: PMX Partially Mapped Crossover
//
// 1. Elegir aleatoriamente dos puntos de cruce
// 2. Intercambiar estos 2 segmentos en los hijos que se generan (solo piezas)
// 3. El resto de las cadenas se obtienen haciendo mapeos entre los 2 padres:
// 	Si un valor no está contenido en el segmento intercambiado, permanece igual.
// 	Si está contenido en el segmento intercambiado, entonces se sustituye por 
// 	el valor que tenga dicho segmento en el otro.
// 	
// ------------------------------------------------------------------------------
void TDCSP::crossoverPMX (Individual* ind) {

	int *p1, *p2, *h1, *h2;
	int numPat1, numPat2, greater, aux, j, k, v1, v2, point1, point2, max_size_cross;
	
	numPat1 = (size +1) / 2;
	numPat2 = (((TDCSP*) ind)->size +1) / 2;

	p1 = new int[numPat1];	
	h1 = new int[numPat1];
	p2 = new int[numPat2];
	h2 = new int[numPat2];
	j = 0;
	k = 0;

	// Extracción de los patrones del primer padre
	for (int i = 0; i < size; i++) {
		v1 = (int) getVar(i);
		if (v1 >= 0)
			p1[j++] = v1;	
	}
	// Extracción de los patrones del segundo padre
	for (int i = 0; i < ((TDCSP*) ind)->size; i++) {
		v2 = (int) ind->getVar(i);
		if (v2 >= 0)
			p2[k++] = v2;		
	}

	// Seleccionar el punto maximo para el cruce y el mayor tamaño de los dos genes
	if (numPat1 > numPat2)  {		// == if (j > k)
		max_size_cross = numPat2;
		greater = numPat1;
	}
	else {
		max_size_cross = numPat1;
		greater = numPat2;
	}
	// Selección de puntos de cruce
	point1 = uniformRandom(max_size_cross); 	
	point2 = uniformRandom(max_size_cross);
	
	// El primer punto ha de estar más a la izquierda
	if (point1 > point2) {
		aux = point1;
		point1 = point2;
		point2 = aux;
	}

	for (int i = 0; i < greater; i++) {
		// Si no pertenece al segmento de intercambio
 		if (i < point1 || i > point2) {
			if (i < numPat1) {	// Si el gen 1 es de tamaño inferior al índice
				// se comprueba si el numero de patron se encuentra entre los ya intercambiados
				v1 = findpat(p1[i], p1, p2, point1, point2);
				// si pertenece se cambia por el valor que corresponde
				if (v1 != -1)
					h1[i] = v1;
				// si no simplemente se copia
				else
					h1[i] = p1[i];
			}
			
			if (i < numPat2) {	// Si el gen 2 es de tamaño inferior al índice
				v2 = findpat(p2[i], p2, p1, point1, point2);
				if (v2 != -1)
					h2[i] = v2;
				else	
					h2[i] = p2[i];
			}											 
		}
		// Si pertenece al segmento de intercambio se intercambian
		else {
			h1[i] = p2[i];
			h2[i] = p1[i];
		}
	}

	// reasignación a los genes correspondientes
	j = 0;
	k = 0;
	for (int i = 0; i < size; i++) {
		if (getVar(i) > -1)
			setVar(i, h1[j++]);	
	}
	
	for (int i = 0; i < ((TDCSP*) ind)->size; i++) {
		if (ind->getVar(i) > -1)
			ind->setVar(i,h2[k++]); 
	}
								             
	repair();
	((TDCSP*) ind)->repair();

	delete(p1);
	delete(p2);
	delete(h1);
	delete(h2);
}

//------------------------------------------------------------------------------
// Búsqueda de un patron en el segmento de intercambio, devolviendo el nuevo valor
// si ya se encuentra en el segmento intercambiado.
// Es recursiva ya que puede ocurrir que el nuevo valor también se encuentre en
// el segmento de intercambio.
// Used by crossoverPMX.
//------------------------------------------------------------------------------
int TDCSP::findpat(int elem, int *p1, int *p2, int point1, int point2) {
        int newelem;
        for(int i = point1; i <= point2; i++){
                if(p2[i] == elem) {
                        elem = p1[i];
                        newelem = findpat(elem, p1, p2, point1, point2);
                        if (newelem != -1)
                                elem = newelem;
                        return elem;
                }
        }
        return -1;
}

//-------------------------------------------------------------------------------
// Evaluacion
// ------------------------------------------------------------------------------
void TDCSP::evaluate (void) {
	int var, a, b, c;
	double lengthA, lengthB, widthA, widthB;
	double  *l, *w;
	int nc = 0;
	int numNewP = size/2;      //????
	stack<int> st;
	double total_profit = 0;	

 	l = new double[numNewP];
   	w = new double[numNewP];
	 
	c = totalNumPat;
	
	for (int i = 0; i < size; i++) {
		var = (int) getVar(i);
	
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

	//print();
	delete(l);
	delete(w);
}

//----------------------------------------------------------------------------
// Clonacion
// ---------------------------------------------------------------------------
Individual* TDCSP::clone (void) const {
  	TDCSP* ind = new TDCSP();
	ind->size = size;
	return ind;
}

//------------------------------------------------------------------------------
// Imprimir por pantalla un gen o individuo
//-----------------------------------------------------------------------------
void TDCSP::print(void) {
	unsigned int i;
		  
	cout << "Gene: ";
	for (i = 0; i < size; i++) {
		if (getVar(i) == -1)
			cout << "H ";
		else if (getVar(i) == -2)
			cout << "V ";
		else
			cout << getVar(i) << " ";
	}
			 
  	cout << endl << "Profit: " << getObj(0) << endl;	
  	cout << "Number of cuts: " << getObj(1) << endl << endl;
	
}
/*
//------------------------------------------------------------------------------
// Codigo auxiliar para iniciar una determinada solucion
//------------------------------------------------------------------------------
void TDCSP::fixSolution(void) {
	// Fijar el gen y su correspondiente orientacion
	
	// Test Problem 4 - Solution 1
	//string gene("7 8 10 3 H H H 2 4 9 H 5 H 1 6 H H H V");
	//string orientation("0 0 1 1 0 1 0 1 0 0");
	// Tests Problem 4 - Solution 2
	//string gene("5 7 2 H 9 3 H H H 1 6 H 8 4 H H 10 H V");
	//string orientation("0 0 0 1 0 0 1 1 1 1");	
	
	// Tests Problem 5 - Solution 1
	//string gene("5 13 2 H 15 9 1 H H 8 H 18 16 H H 3 H H H 12 7 17 14 19 H 4 20 H H H H H 11 10 H H 6 H V");
	//string orientation("0 0 1 1 0 1 0 1 0 0 0 0 0 0 0 0 0 1 0 0");	
	// Tests Problem 5 - Solution 2
	//string gene("18 8 5 H H 11 19 H 6 20 H 7 4 H H H 17 10 H H H 13 2 16 H 3 1 H 9 H 15 14 H 12 H H H H V");
	//string orientation("0 0 1 1 0 1 1 1 0 0 0 1 1 0 1 0 0 1 0 1");	
	// Tests Problem 5 - Solution 3
	//string gene("13 6 9 H 20 17 H 16 15 H 8 H H H 3 H H 10 19 2 H H 18 H 1 H 12 H 5 H 7 H 11 4 H H 14 H V");
	//string orientation("0 1 1 1 0 1 1 1 1 0 0 1 0 0 0 0 0 1 0");	
	// Tests Problem 5 - Solution 4
	//string gene("5 12 19 10 17 9 H H 2 7 16 H H 15 H H H H H 3 1 18 6 H 13 H 11 20 H H H H 14 H 4 8 H H V");
	//string orientation("0 0 1 0 0 1 1 1 0 1 0 1 1 1 0 0 1 1 0 0");	
	// Tests Problem 5 - Solution 5
	//string gene("12 9 H 4 13 2 20 17 H 3 H H H H 14 19 H H H 11 7 H 5 H 18 H 6 H 16 15 8 H 1 H H 10 H H V");
	//string orientation("0 0 0 1 1 1 1 1 1 0 0 0 0 1 1 0 1 1 0 1");	

	//string gene("7 8 9 H H 20 2 5 V H V 1 19 14 H 4 V 10 V 3 V H 17 12 18 H V 13 V H 15 11 H 6 16 H V H H");
	//string orientation("0 1 0 0 1 0 0 1 1 1 0 1 1 1 0 0 1 1 0 1");
	
	string gene("17 14 5 19 9 H V 13 V H 3 18 20 H V 6 7 V 1 H 11 H 2 15 H V 4 10 H V H 12 16 V H H H H");
	string orientation("0 0 0 0 1 0 1 1 1 1 1 0 0 0 1 0 0 1 1 0");
	stringstream geneStream(gene); 
	stringstream orientationStream(orientation); 
	string elem; 
	unsigned int i = 0;

	// Almacenar el gen
	while (geneStream >> elem) {
		if (elem == "H") 
			setVar(i, -1); 
		else if (elem == "V") 
			setVar(i, -2); 
		else 
			setVar(i, (atoi(elem.c_str())) - 1); 
		i++;
	}
	
	while (orientationStream >> elem) {
		setVar(i, atoi(elem.c_str())); 
		i++;
	}

	// Evalua y muestra la solucion
	evaluate();
	print();
	exit(-1);
}

*/
