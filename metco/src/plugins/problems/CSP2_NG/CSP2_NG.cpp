 /***********************************************************************************
 * AUTORES
 *    Jesica de Armas Adrian 
 *    
 * FECHA
 *    Noviembre 2007
 ************************************************************************************/

#include "CSP2_NG.h"
#include <vector>
#include <stack>
#include <set>
#include <algorithm>

// Direcciones de optimizacion
double* CSP2_NG::len[2];
double* CSP2_NG::wid[2];
double CSP2_NG::widSheet;
int CSP2_NG::size;
int CSP2_NG::totalSize;
int CSP2_NG::numPat;

// ---------------------------------------------------------------------------------
// Inicialización. 
// Fija:
//   - Numero de variables
//   - Numero de objetivos
//   - Direccion de optimizacion de los objetivos
//   - Rangos de las variables (mínimos y máximos)
// ---------------------------------------------------------------------------------
bool CSP2_NG::init (const vector<string> &params) {
   if (params.size() != 1) {
      cerr << "Error: CSP2_NG called with error number of parameters" << endl;
      return false;
   }
   if (!readFile(params[0])) 
      return false;
   
   setNumberOfVar(totalSize); // vars number 
   setNumberOfObj(NOBJ);

	//fixSolution();
   return true;
}

//----------------------------------------------------------------------------------
// Reads a problem from file
// ---------------------------------------------------------------------------------
bool CSP2_NG::readFile(const string filename) {
   double l, w;

   ifstream input(filename.c_str());
   if (!input.is_open()) {
      cerr << "Error: CSP2_NG called with error name of file" << endl;
      return false;
   }
   input >> numPat;
   len[0] = new double[numPat];
   len[1] = new double[numPat];
   wid[0] = new double[numPat];
   wid[1] = new double[numPat];
   size = numPat * 2 - 1;
   totalSize = size + numPat;
   input >> widSheet;
   for (int i = 0; i < numPat; i++) {
      input >>  l;
      setLength(0, i, l);
      setWidth(1, i, l);
      input >>  w;
      setWidth(0, i, w);
      setLength(1, i, w);
      if (w > widSheet) {
         cerr << " Error: width of pattern bigger than width of mother sheet "<< endl;
         return false;
      }
   }

   input.close();
   return true;
}

//----------------------------------------------------------------------------------
// Check condition 1 <= no <= np - 1 and no = np - 1, for an operator:
// no = number of operators to its left plus itself
// np = number of patterns to its left
// ---------------------------------------------------------------------------------
bool CSP2_NG::checkCondition() {
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

// -------------------------------------------------------------------------------
// Random generation of an individual
// -------------------------------------------------------------------------------
void CSP2_NG::restart() {
   int *pat, *op, j, k, numO;
   int value = (int) round ((rand()/RAND_MAX) * (numPat - 1));
   set<int> st;
   
   pat = new int[numPat];       // patrones
   op = new int[numPat - 1];   // operadores

   // se genera un vector de patrones ordenados aleatoriamente
   for (int i = 0; i < numPat; i++) {   
      while(st.find(value) != st.end()) {
         value = (int) round(((double)rand()/(double) RAND_MAX) * (numPat - 1));
      }
      pat[i] = value;
      st.insert(pat[i]);
   }
   // se genera un vector de operadores H y V 
   for (int i = 0; i < numPat - 1; i++) {      
      op[i] = (int) round((double)(rand() / (double) RAND_MAX) - 2);
   }
   // los dos primeros elementos han de ser patrones
   setVar(0, pat[0]);
   setVar(1, pat[1]);
   
   j = 2;
   k = 0;
   
   // se escoge aleatoriamente un patron o un operador (pat = 0, op = 1)
   // si es patron se agrega si aun quedan por agregar,
   // de lo contrario se agregara un operador
   // si es operador se agrega solo si cumple con la condicion 1 <= no <= np -1,
   // de lo contrario se agrega un patron
   for (int i = 2; i < size; i++) {
      value = (int) round ((double)rand()/(double) RAND_MAX);
      if (value == 0 && j < numPat) {
         setVar(i,pat[j]);
         j++;
      }
      else {
         numO = k + 1;
         if ((numO < 1) || (numO > (j - 1) )) {
             setVar(i,pat[j]);
            j++;
         }
         else {
            setVar(i,op[k]);
            k++;
         }
      }
   }
   
   // Se llenan las orientaciones aleatoriamente (horizontal = 0, vertical = 1)
   for (int i = size; i < totalSize; i++) {
      value = (int) round ((double)rand()/(double) RAND_MAX);
      setVar(i,value);
   }

	delete(pat);
	delete(op);
}

//-------------------------------------------------------------------------------
// Mutacion 
// -- Se eligen 2 elementos de la cadena, p1 y p2 (donde p1 esta mas a la izquierda)
//
//      - Si ambos son piezas u operadores, se intercambian.
//      - Si p1 es un operador y p2 pieza, se intercambian.
//      - Si p1 es pieza y p2 operador, se intercambian solo cuando se cumpla:
//               1 <= no <= np - 1
//        Se debe de seguir cumpliendo después del intercambio.
//
// -- Se elige un operador aleatorio y se cambia basándose en la probabilidad de mutación.
//------------------------------------------------------------------------------                     
void CSP2_NG::dependentMutation (double pm) {
   int var1, var2, aux, point1, point2;
   
   point1 = (int) round (((double)rand()/(double) RAND_MAX) * (size - 1));
   point2 = (int) round (((double)rand()/(double) RAND_MAX) * (size - 1));
   
   // Asegurar que son diferentes   
   while (point1 == point2)
      point2 = (int) round (((double)rand()/(double) RAND_MAX) * (size - 1));   
   
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
   }
   else {
      setVar(point1, var2);
      setVar(point2, var1);
                 
      if (!checkCondition()) {
          setVar(point1, var1);
          setVar(point2, var2);
      }
   }

   // Se elige un operador aleatorio y se cambia basándose en la probabilidad de mutación
   point1 = (int) round (((double)rand()/(double) RAND_MAX) * (size - 1));

   while (getVar(point1) >= 0)
       point1 = (int) round (((double)rand()/(double) RAND_MAX) * (size - 1));

   double vmut = ((double) rand()) / (double) (RAND_MAX);

   if (vmut < pm) {
      if (getVar(point1) == H)
         setVar(point1, V);
      else
         setVar(point1, H);
   }
   // Podría volver a hallarse un vmut?
   // Se elige una orientacion aleatoria y se cambia basandose en la probabilidad de mutacion
   point1 = (int) round (((double)rand()/(double) RAND_MAX) * (totalSize - 1 - size) + size);

   if (vmut < pm) {
      if (getVar(point1) == 0)
         setVar(point1, 1);
      else
         setVar(point1, 0);
   }
}

//-------------------------------------------------------------------------------
// Crossover
//-------------------------------------------------------------------------------
void CSP2_NG::dependentCrossover (Individual* ind) {
   crossoverPMX(ind);   
}

//-------------------------------------------------------------------------------
// Crossover CX: Toma algunos valores de un padre y selecciona los restantes del otro.
// 1. Encontrar un ciclo que se define mediante las posiciones correspondientes
// de los valores entre los padres.
// 2. Copiar los valores de P1 que sean parte del ciclo.
// 3. Borrar de P2 los valores que esten en el ciclo.
// 4. Rellenar el hijo con los valores restantes de P2 (sustituyendo de izquierda a
// derecha.
//-------------------------------------------------------------------------------
void CSP2_NG::crossoverCX (Individual* ind){
   int value, *p1, *p2, *h1, *h2, j, k, l, v1, v2, pos;

   p2 = new int[numPat];
   p1 = new int[numPat];
   h1 = new int[numPat];
   h2 = new int[numPat];
   j = 0;
   k = 0;

   // Extracción de los patrones 
   for (int i = 0; i < size; i++) {
      v1 = (int) getVar(i);
      v2 = (int) ind->getVar(i);
      if (v1 >= 0)
         p1[j++] = v1;
         if (v2 >= 0)
            p2[k++] = v2;
   }
   
   // Se selecciona una posicion al azar 
   pos = (int) round (((double)rand()/(double) RAND_MAX) * (numPat - 1));
   
   // Se escoge el valor del padre 1 en esa posicion para seguir un ciclo
   // y se copian los valores en los hijos siguiendo el ciclo 
   value = p1[pos];
   h1[pos] = p1[pos];
   h2[pos] = p2[pos];
   p1[pos] = -1;
   while(h2[pos] != value) {
      pos = findPos(p2[pos], p1);
      h1[pos] = p1[pos];
      h2[pos] = p2[pos];
      p1[pos] = -1;
   }
   // El resto de valores se intercambian
   for (int i = 0; i < numPat; i++) {
      if (p1[i] != -1) {         
         h1[i] = p2[i];
         h2[i] = p1[i];
      }
   }
   j = 0;
   k = 0;
   for (int i = 0; i < size; i++) {
      if (getVar(i) > -1)
         setVar(i, h1[j++]);
         if (ind->getVar(i) > -1)
            ind->setVar(i,h2[k++]);
   }
}

//-------------------------------------------------------------------------------
// Find position of element elem in the array p.
// Used by crossoverCX
//-------------------------------------------------------------------------------
int CSP2_NG::findPos(int elem, int *p) {
   for(int i = 0; i < numPat; i++){
      if(p[i] == elem)
         return i;
   }
}

//-------------------------------------------------------------------------------
// Crossover OX: este cruce consiste en elegir para cada descendiente un tramo de
// uno de los progenitores y a la vez preservar el orden relativo de todos los 
// rectángulos del otro.
//-------------------------------------------------------------------------------
void CSP2_NG::crossoverOX (Individual* ind){
   int aux, *p1, *p2, *h1, *h2, j, k, l, v1, v2, point1, point2;

   p2 = new int[numPat];
   p1 = new int[numPat];
   h1 = new int[numPat];
   h2 = new int[numPat];
   j = 0;
   k = 0;

   // Extracción de los patrones 
   for (int i = 0; i < size; i++) {
      v1 = (int) getVar(i);
      v2 = (int) ind->getVar(i);
      if (v1 >= 0)
         p1[j++] = v1;
         if (v2 >= 0)
            p2[k++] = v2;
   }

   // Selección de puntos de cruce
   point1 =(int) round (((double)rand()/(double) RAND_MAX) * (numPat - 1));
   point2 =(int) round (((double)rand()/(double) RAND_MAX) * (numPat - 1));

   // El primer punto ha de estar más a la izquierda
   if (point1 > point2) {
      aux = point1;
      point1 = point2;
      point2 = aux;
   }
   // Copiar el segmento intercambiado
   for (int i = point1; i <= point2; i++) {
      h1[i] = p2[i];
           h2[i] = p1[i];
   }
   // Rellenar conservando el orden relativo y omitiendo los que ya esten presentes
   if (point2 < numPat -1)
      j = point2 + 1;
   else 
      j = 0;
   k = j;
   l = j;
   while (j < point1 || j > point2) {
      while (included(p1[k], h1, point1, point2)) {   
         if (k != numPat - 1)
            k++;
         else 
            k = 0;
      }
      h1[j] = p1[k];
      while (included(p2[l], h2, point1, point2)) {    
         if (l != numPat - 1)
            l++;
         else 
            l = 0;
      }
      h2[j] = p2[l];
      if (k != numPat - 1)
         k++;
      else 
         k = 0;
      if (l != numPat - 1)
         l++;
      else 
         l = 0;   

      if (j != numPat - 1)
         j++;
      else 
         j = 0;
   }
   j = 0;
   k = 0;
   for (int i = 0; i < size; i++) {
      if (getVar(i) > -1)
         setVar(i, h1[j++]);
      if (ind->getVar(i) > -1)
         ind->setVar(i,h2[k++]);
   }
}

//-------------------------------------------------------------------------------
// Check if a pattern already belong to the swap sector.
// Used by crossoverOX
//-------------------------------------------------------------------------------
bool CSP2_NG::included(int elem, int *p, int point1, int point2) {
   for (int i = point1; i <= point2; i++) {
      if (p[i] == elem) 
         return true;
   }
   return false;
}

//-------------------------------------------------------------------------------
// Crossover PMX: PMX Partially Mapped Crossover
//
// 1. Elegir aleatoriamente dos puntos de cruce
// 2. Intercambiar estos 2 segmentos en los hijos que se generan
// 3. El resto de las cadenas se obtienen haciendo mapeos entre los 2 padres:
//    Si un valor no está contenido en el segmento intercambiado, permanece igual.
//    Si está contenido en el segmento intercambiado, entonces se sustituye por 
//    el valor que tenga dicho segmento en el otro.
//    
// ------------------------------------------------------------------------------
void CSP2_NG::crossoverPMX (Individual* ind) {
   int aux, *p1, *p2, *h1, *h2, j, k, v1, v2, point1, point2;
   
   p2 = new int[numPat];
   p1 = new int[numPat];   
   h1 = new int[numPat];
   h2 = new int[numPat];
   j = 0;
   k = 0;
   
   // Extracción de los patrones 
   for (int i = 0; i < size; i++) {
      v1 = (int) getVar(i);
      v2 = (int) ind->getVar(i);
      if (v1 >= 0)
         p1[j++] = v1;
      if (v2 >= 0)
         p2[k++] = v2;
   }
   
   // Selección de puntos de cruce
   point1 =(int) round (((double)rand()/(double) RAND_MAX) * (numPat - 1));
   point2 =(int) round (((double)rand()/(double) RAND_MAX) * (numPat - 1));
   
   // El primer punto ha de estar más a la izquierda
   if (point1 > point2) {
      aux = point1;
      point1 = point2;
      point2 = aux;
   }

   for (int i = 0; i < numPat; i++) {
      // Si no pertenece al segmento de intercambio
       if (i < point1 || i > point2) {
         // se comprueba si el numero de patron se encuentra entre los
         // ya intercambiados
         v1 = findpat(p1[i], p1, p2, point1, point2);
         // si pertenece se cambia por el valor que corresponde
         if (v1 != -1)
            h1[i] = v1;
         // si no simplemente se copia
         else
            h1[i] = p1[i];
         
         v2 = findpat(p2[i], p2, p1, point1, point2);
         if (v2 != -1)
            h2[i] = v2;
         else
            h2[i] = p2[i];
                                        
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
      if (ind->getVar(i) > -1)
         ind->setVar(i,h2[k++]); 
   }

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
int CSP2_NG::findpat(int elem, int *p1, int *p2, int point1, int point2) {
   int newelem;
   for(int i = point1; i <= point2; i++){
      if (p2[i] == elem) {
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
void CSP2_NG::evaluate (void) {
   int var, a, b, c;
  	double lenSheet, lengthA, lengthB, widthA, widthB;
   double *l, *w;
   int nc;
   int numNewP = size/2;
   stack<int> st1, st2;
   TBuild builds[numNewP]; 
   TCoordinate coord[numPat], coords[numNewP]; 

   l = new double[numNewP];
   w = new double[numNewP];
   c = numPat;
   
   for (int i = 0; i < size; i++) {
      var = (int) getVar(i);
      if (var != V && var != H) 
         st1.push(var);   
      else {
         a = st1.top();
         st1.pop();
         b = st1.top();
         st1.pop();
         
         if (a >= numPat) {  // Si es una pieza creada por concatenacion
            lengthA = l[a-numPat];
            widthA = w[a-numPat];
         } else {              // Si es una pieza original
            lengthA = len[(int) getVar(size + a)][a];
            widthA = wid[(int) getVar(size + a)][a];
         }
         if (b >= numPat) {  // Si es una pieza creada por concatenación
            lengthB = l[b-numPat];
            widthB = w[b-numPat];                                                   
			} else {              // Si es una pieza original                
            lengthB = len[(int) getVar(size + b)][b];
            widthB = wid[(int) getVar(size + b)][b]; 
         }
         
         if (var == H) {
            l[c - numPat] = lengthA + lengthB;
            w[c - numPat] = max(widthA, widthB);
         } else {
            l[c - numPat] = max(lengthA, lengthB);
            w[c - numPat] = widthA + widthB;
            if (w[c - numPat] > widSheet) {  // si se supera el ancho del material
               setVar(i, H);                 // cambiar V por H
               i--;                          // deshacer los cambios y retroceder
               st1.push(a);
               st1.push(b);
               continue;                     // volver a evaluar el operador
            }
         }
         
         builds[c-numPat].elem1 = b;      
         builds[c-numPat].elem2 = a;      
         builds[c-numPat].op = var;      
         st1.push(c);
         st2.push(c);
         c++;
      }
   }
  
  	// Calcular las dimensiones del patron final
   a = st1.top();
   st1.pop();
   if (a >= numPat) {
      coords[a-numPat].x1 = 0;
      coords[a-numPat].y1 = 0;
      lenSheet = coords[a-numPat].x2 = l[a-numPat];
      coords[a-numPat].y2 = w[a-numPat];
   }
   else {
      coord[a].x1 = 0;
      coord[a].y1 = 0;
      lenSheet = coord[a].x2 = len[(int) getVar(size+a)][a];
      coord[a].y2 = wid[(int) getVar(size+a)][a];
   }

	// Fijar objetivo 1 - longitud de la motherSheet
   setObj(0, lenSheet);
  
   // Calcular las coordenadas de cada pieza dentro del patron final	
   double e1_x1, e1_y1, e1_x2, e1_y2;   
   double e2_x1, e2_y1, e2_x2, e2_y2;   
   while (!st2.empty()) {
      a = st2.top();
      st2.pop();
      int e1 = builds[a-numPat].elem1;
      int e2 = builds[a-numPat].elem2;
      double length_e1, width_e1;
      //cout << " a = " << a << "   e1 = " << e1 << "   e2 = " << e2 << "   op = " << builds[a-numPat].op << endl;
      if (builds[a-numPat].op == H) {
         if (e1 >= numPat) {
            e1_x1 = coords[e1-numPat].x1 = coords[a-numPat].x1;
            e1_y1 = coords[e1-numPat].y1 = coords[a-numPat].y1;
            e1_x2 = coords[e1-numPat].x2 = coords[a-numPat].x1 + l[e1-numPat];
            e1_y2 = coords[e1-numPat].y2 = coords[a-numPat].y1 + w[e1-numPat];
            length_e1 = l[e1-numPat];
         } else {
             e1_x1 = coord[e1].x1 = coords[a-numPat].x1;
             e1_y1 = coord[e1].y1 = coords[a-numPat].y1;
             e1_x2 = coord[e1].x2 = coords[a-numPat].x1 + len[(int) getVar(size+e1)][e1];
             e1_y2 = coord[e1].y2 = coords[a-numPat].y1 + wid[(int) getVar(size+e1)][e1];
            length_e1 = len[(int) getVar(size+e1)][e1];
         }
         if (e2 >= numPat) {
             e2_x1 = coords[e2-numPat].x1 = coords[a-numPat].x1 + length_e1;
             e2_y1 = coords[e2-numPat].y1 = coords[a-numPat].y1; 
             e2_x2 = coords[e2-numPat].x2 = coords[a-numPat].x2;
             e2_y2 = coords[e2-numPat].y2 = coords[a-numPat].y1 + w[e2-numPat];
          } else {
            e2_x1 = coord[e2].x1 = coords[a-numPat].x1 + length_e1; 
            e2_y1 = coord[e2].y1 = coords[a-numPat].y1; 
            e2_x2 = coord[e2].x2 = coords[a-numPat].x2;
            e2_y2 = coord[e2].y2 = coords[a-numPat].y1 + wid[(int) getVar(size+e2)][e2];
         }
      } else {
         if (e1 >= numPat) {
            e1_x1 = coords[e1-numPat].x1 = coords[a-numPat].x1;
            e1_y1 = coords[e1-numPat].y1 = coords[a-numPat].y1;
            e1_x2 = coords[e1-numPat].x2 = coords[a-numPat].x1 + l[e1-numPat];
            e1_y2 = coords[e1-numPat].y2 = coords[a-numPat].y1 + w[e1-numPat];
            width_e1 = w[e1-numPat];
         } else {
            e1_x1 = coord[e1].x1 = coords[a-numPat].x1;
            e1_y1 = coord[e1].y1 = coords[a-numPat].y1;
            e1_x2 = coord[e1].x2 = coords[a-numPat].x1 + len[(int) getVar(size+e1)][e1];
            e1_y2 = coord[e1].y2 = coords[a-numPat].y1 + wid[(int) getVar(size+e1)][e1];
            width_e1 = wid[(int) getVar(size+e1)][e1];
         }
         if (e2 >= numPat) {
            e2_x1 = coords[e2-numPat].x1 = coords[a-numPat].x1;
            e2_y1 = coords[e2-numPat].y1 = coords[a-numPat].y1 + width_e1;
            e2_x2 = coords[e2-numPat].x2 = coords[a-numPat].x1 + l[e2-numPat];
            e2_y2 = coords[e2-numPat].y2 = coords[a-numPat].y2;
         } else {
            e2_x1 = coord[e2].x1 = coords[a-numPat].x1;
            e2_y1 = coord[e2].y1 = coords[a-numPat].y1 + width_e1;
            e2_x2 = coord[e2].x2 = coords[a-numPat].x1 + len[(int) getVar(size+e2)][e2];
            e2_y2 = coord[e2].y2 = coords[a-numPat].y2;
         }
      }
   }
  
   /*	
   for (int i = 0; i < size; i++) {
      cout << (int) getVar(i) << " ";
   }
   cout << endl;
   for (int i = size; i < size + numPat; i++) {
      cout << (int) getVar(i) << " ";
   }
   cout << endl;

   for (int i = 0; i < size; i++) {
      if ( (int) getVar(i) != H && (int) getVar(i) != V) {
         cout << (int) getVar(i) << endl;
         cout << "x1 = " << coord[(int)getVar(i)].x1 << "   y1 = " << coord[(int)getVar(i)].y1 << endl;
         cout << "x2 = " << coord[(int)getVar(i)].x2 << "   y2 = " << coord[(int)getVar(i)].y2 << endl;
   
      }
   }
	*/

	// Calcular el numero de cortes
	nc = NEDGES * numPat;
	//cout << "Numero de cortes iniciales: " << nc << endl;
	
	int edges[numPat][NEDGES];
	for (int i = 0; i < numPat; i++)
		for (int j = 0; j < NEDGES; j++)
			edges[i][j] = 0;

	// Recorremos cada rectangulo y comprobamos los ejes que coinciden con los bordes del material
	for (int i = 0; i < numPat; i++) {
		if (coord[i].y1 == 0) {
			edges[i][BOT] = 1;
			nc--;
		}
		if (coord[i].x1 == 0) {
			edges[i][LEF] = 1;
			nc--;
		}
		if (coord[i].y2 == widSheet) {
			edges[i][TOP] = 1;
			nc--;
		}
	}
	//cout << "Despues de comprobar esquinas: " << nc << endl;

	bool pieceInside;
	// Comprobamos las alineaciones entre los distintos rectangulos
	for (int i = 0; i < numPat; i++) {
		for (int j = i + 1; j < numPat; j++) {
			//cout << "Comprobando piezas " << i << " - " << j << endl;
			
			// Comprobar si existe una pareja de ejes horizontales alineados que no coincidan con los bordes
			// 1) Las piezas coinciden en BOT - BOT
			if ((coord[i].y1 == coord[j].y1) && (coord[i].y1 != 0)) {
				// Comprobar que los rectangulos no se tocan 
				if ((coord[i].x1 > coord[j].x2) || (coord[j].x1 > coord[i].x2)) {
					pieceInside = false;
					for (int k = 0; k < numPat; k++) {
						// Comprobar que hay una pieza que queda horizontalmente entre las piezas i-j
						if (((coord[i].x2 < coord[j].x1) && ((coord[k].x1 >= coord[i].x2) && (coord[k].x2 <= coord[j].x1))) || 
							 ((coord[j].x2 < coord[i].x1) && ((coord[k].x1 >= coord[j].x2) && (coord[k].x2 <= coord[i].x1)))) {
							// Comprobar que la pieza k tambien esta verticalmente entre las piezas i-j
							if ((coord[k].y1 < coord[i].y1) && (coord[k].y2 > coord[i].y1)) {
								pieceInside = true;
								break;
							}		 
						}
					}
			 		if ((pieceInside == false)	&& ((edges[i][BOT] != 1) || (edges[j][BOT] != 1))) { 
						nc--;
						edges[i][BOT] = edges[j][BOT] = 1;
					}
				// Si los rectangulos se tocan se puede aprovechar el corte
				} else if ((edges[i][BOT] != 1) || (edges[j][BOT] != 1)) { 
					nc--;	
					edges[i][BOT] = edges[j][BOT] = 1;
				}
			}
			// 2) Las piezas coinciden en BOT - TOP
			if (coord[i].y1 == coord[j].y2) { 
				// Comprobar que los rectangulos no se tocan 
				if ((coord[i].x1 > coord[j].x2) || (coord[j].x1 > coord[i].x2)) {
					pieceInside = false;
					for (int k = 0; k < numPat; k++) {
						// Comprobar que hay una pieza que queda horizontalmente entre las piezas i-j
						if (((coord[i].x2 < coord[j].x1) && ((coord[k].x1 >= coord[i].x2) && (coord[k].x2 <= coord[j].x1))) || 
							 ((coord[j].x2 < coord[i].x1) && ((coord[k].x1 >= coord[j].x2) && (coord[k].x2 <= coord[i].x1)))) {
							// Comprobar que la pieza k tambien esta verticalmente entre las piezas i-j
							if ((coord[k].y1 < coord[i].y1) && (coord[k].y2 > coord[i].y1)) {
								pieceInside = true;
								break;
							}
						}
					}	  
					if ((pieceInside == false) && ((edges[i][BOT] != 1) || (edges[j][TOP] != 1))) { 
						nc--;
						edges[i][BOT] = edges[j][TOP] = 1;
					}
				// Si los rectangulos se tocan se puede aprovechar el corte
				} else if ((edges[i][BOT] != 1) || (edges[j][TOP] != 1)) { 
					nc--;	
					edges[i][BOT] = edges[j][TOP] = 1;
				}
			}
			// 3) Las piezas coinciden en TOP - BOT
			if (coord[i].y2 == coord[j].y1) { 
				// Comprobar que los rectangulos no se tocan 
				if ((coord[i].x1 > coord[j].x2) || (coord[j].x1 > coord[i].x2)) {
					pieceInside = false;
					for (int k = 0; k < numPat; k++) {
						// Comprobar que hay una pieza que queda horizontalmente entre las piezas i-j
						if (((coord[i].x2 < coord[j].x1) && ((coord[k].x1 >= coord[i].x2) && (coord[k].x2 <= coord[j].x1))) || 
							 ((coord[j].x2 < coord[i].x1) && ((coord[k].x1 >= coord[j].x2) && (coord[k].x2 <= coord[i].x1)))) {
							// Comprobar que la pieza k tambien esta verticalmente entre las piezas i-j
							if ((coord[k].y1 < coord[i].y2) && (coord[k].y2 > coord[i].y2)) {
								pieceInside = true;
								break;
							}
						}
					}	  
					if ((pieceInside == false) && ((edges[i][TOP] != 1) || (edges[j][BOT] != 1))) { 
						nc--;
						edges[i][TOP] = edges[j][BOT] = 1;
					}
				// Si los rectangulos se tocan se puede aprovechar el corte
				} else if ((edges[i][TOP] != 1) || (edges[j][BOT] != 1)) { 
					nc--;	
					edges[i][TOP] = edges[j][BOT] = 1;
				}
			}
			// 4) Las piezas coinciden en TOP - TOP
			if ((coord[i].y2 == coord[j].y2) && (coord[i].y2 != widSheet)) { 
				// Comprobar que los rectangulos no se tocan 
				if ((coord[i].x1 > coord[j].x2) || (coord[j].x1 > coord[i].x2)) {
					pieceInside = false;
					for (int k = 0; k < numPat; k++) {
						// Comprobar que hay una pieza que queda horizontalmente entre las piezas i-j
						if (((coord[i].x2 < coord[j].x1) && ((coord[k].x1 >= coord[i].x2) && (coord[k].x2 <= coord[j].x1))) || 
							 ((coord[j].x2 < coord[i].x1) && ((coord[k].x1 >= coord[j].x2) && (coord[k].x2 <= coord[i].x1)))) {
							// Comprobar que la pieza k tambien esta verticalmente entre las piezas i-j
							if ((coord[k].y1 < coord[i].y2) && (coord[k].y2 > coord[i].y2)) {
								pieceInside = true;
								break;
							}
						}
					}	  
					if ((pieceInside == false) && ((edges[i][TOP] != 1) || (edges[j][TOP] != 1))) { 
						nc--;
						edges[i][TOP] = edges[j][TOP] = 1;
					}
				// Si los rectangulos se tocan se puede aprovechar el corte
				} else if ((edges[i][TOP] != 1) || (edges[j][TOP] != 1)) { 
					nc--;	
					edges[i][TOP] = edges[j][TOP] = 1;
				}
			}

			// Comprobar si existe una pareja de ejes verticales alineados que no coincidan con los bordes
			// 1) Las piezas coinciden en LEF - LEF
			if ((coord[i].x1 == coord[j].x1) && (coord[i].x1 != 0)) { 
				// Comprobar que los rectangulos no se tocan 
				if ((coord[i].y1 > coord[j].y2) || (coord[j].y1 > coord[i].y2)) {
					pieceInside = false;
					for (int k = 0; k < numPat; k++) {
						// Comprobar que hay una pieza que queda verticalmente entre las piezas i-j
						if (((coord[i].y2 < coord[j].y1) && ((coord[k].y1 >= coord[i].y2) && (coord[k].y2 <= coord[j].y1))) || 
							 ((coord[j].y2 < coord[i].y1) && ((coord[k].y1 >= coord[j].y2) && (coord[k].y2 <= coord[i].y1)))) {
							// Comprobar que la pieza k tambien esta horizontalmente entre las piezas i-j
							if ((coord[k].x1 < coord[i].x1) && (coord[k].x2 > coord[i].x1)) {
								pieceInside = true;
							  	break;
							}	
						}
					}	  
					if ((pieceInside == false) && ((edges[i][LEF] != 1) || (edges[j][LEF] != 1))) { 
						nc--;
						edges[i][LEF] = edges[j][LEF] = 1;
					}
				// Si los rectangulos se tocan se puede aprovechar el corte
				} else if ((edges[i][LEF] != 1) || (edges[j][LEF] != 1)) { 
					nc--;	
					edges[i][LEF] = edges[j][LEF] = 1;
				}
			}
			// 2) Las piezas coinciden en LEF - RIG
			if (coord[i].x1 == coord[j].x2) {
				// Comprobar que los rectangulos no se tocan 
				if ((coord[i].y1 > coord[j].y2) || (coord[j].y1 > coord[i].y2)) {
					pieceInside = false;
					for (int k = 0; k < numPat; k++) {
						// Comprobar que hay una pieza que queda verticalmente entre las piezas i-j
						if (((coord[i].y2 < coord[j].y1) && ((coord[k].y1 >= coord[i].y2) && (coord[k].y2 <= coord[j].y1))) || 
							 ((coord[j].y2 < coord[i].y1) && ((coord[k].y1 >= coord[j].y2) && (coord[k].y2 <= coord[i].y1)))) {
							// Comprobar que la pieza k tambien esta horizontalmente entre las piezas i-j
							if ((coord[k].x1 < coord[i].x1) && (coord[k].x2 > coord[i].x1)) { 
								pieceInside = true;
								break;
							}
						}
					}	  
					if ((pieceInside == false) && ((edges[i][LEF] != 1) || (edges[j][RIG] != 1))) { 
						nc--;
						edges[i][LEF] = edges[j][RIG] = 1;
					}
				// Si los rectangulos se tocan se puede aprovechar el corte
				} else if ((edges[i][LEF] != 1) || (edges[j][RIG] != 1)) { 
					nc--;	
					edges[i][LEF] = edges[j][RIG] = 1;
				}
			}
			// 3) Las piezas coinciden en RIG - LEF
			if (coord[i].x2 == coord[j].x1) { 
				// Comprobar que los rectangulos no se tocan 
				if ((coord[i].y1 > coord[j].y2) || (coord[j].y1 > coord[i].y2)) {
					pieceInside = false;
					for (int k = 0; k < numPat; k++) {
						// Comprobar que hay una pieza que queda verticalmente entre las piezas i-j
						if (((coord[i].y2 < coord[j].y1) && ((coord[k].y1 >= coord[i].y2) && (coord[k].y2 <= coord[j].y1))) || 
							 ((coord[j].y2 < coord[i].y1) && ((coord[k].y1 >= coord[j].y2) && (coord[k].y2 <= coord[i].y1)))) {
							// Comprobar que la pieza k tambien esta horizontalmente entre las piezas i-j
							if ((coord[k].x1 < coord[i].x2) && (coord[k].x2 > coord[i].x2)) {
								pieceInside = true;
							  	break;
							}	
						}
					}	  
					if ((pieceInside == false) && ((edges[i][RIG] != 1) || (edges[j][LEF] != 1))) { 
						nc--;
						edges[i][RIG] = edges[j][LEF] = 1;
					}
				// Si los rectangulos se tocan se puede aprovechar el corte
				} else if ((edges[i][RIG] != 1) || (edges[j][LEF] != 1)) { 
					nc--;	
					edges[i][RIG] = edges[j][LEF] = 1;
				}
			}
			// 4) Las piezas coinciden en RIG - RIG 
			if (coord[i].x2 == coord[j].x2) { //&& (coord[i].x2 != lenSheet)) { 
				// Comprobar que los rectangulos no se tocan 
				if ((coord[i].y1 > coord[j].y2) || (coord[j].y1 > coord[i].y2)) {
					pieceInside = false;
					for (int k = 0; k < numPat; k++) {
						// Comprobar que hay una pieza que queda verticalmente entre las piezas i-j
						if (((coord[i].y2 < coord[j].y1) && ((coord[k].y1 >= coord[i].y2) && (coord[k].y2 <= coord[j].y1))) || 
							 ((coord[j].y2 < coord[i].y1) && ((coord[k].y1 >= coord[j].y2) && (coord[k].y2 <= coord[i].y1)))) {
							// Comprobar que la pieza k tambien esta horizontalmente entre las piezas i-j
							if ((coord[k].x1 < coord[i].x2) && (coord[k].x2 > coord[i].x2)) {
								pieceInside = true;
								break;
							}
						}
					}	  
					if ((pieceInside == false) && ((edges[i][RIG] != 1) || (edges[j][RIG] != 1))) { 
						nc--;
						edges[i][RIG] = edges[j][RIG] = 1;
					}
				// Si los rectangulos se tocan se puede aprovechar el corte
				} else if ((edges[i][RIG] != 1) || (edges[j][RIG] != 1)) { 
					nc--;	
					edges[i][RIG] = edges[j][RIG] = 1;
				}
			}
		
			//cout << "Despues de comprobar piezas nc " << nc << endl;	
		}
	}
  
   // Fijar el objetivo 2 - numero de cortes	
   setObj(1, nc);
	//cout << endl << endl;
	//cout << "nc = " << nc << endl << endl;
   //exit (-1);
	//print();
	
	delete(l);
	delete(w);
}

//----------------------------------------------------------------------------
// Clonacion
// ---------------------------------------------------------------------------
Individual* CSP2_NG::clone (void) const {
   //cout << "clone" << endl;
   return new CSP2_NG();
}

//------------------------------------------------------------------------------
// Imprimir por pantalla un gen o individuo
//-----------------------------------------------------------------------------
void CSP2_NG::print(void) {
	unsigned int i;
		  
	cout << "Gene: ";
	for (i = 0; i < (getNumberOfVar() - numPat - 1); i++) {
		if (getVar(i) == -1)
			cout << "H, ";
		else if (getVar(i) == -2)
			cout << "V, ";
		else
			cout << (getVar(i) + 1) << ", ";
	}
	if (getVar(i) == -1)
		cout << "H" << endl;
	else if (getVar(i) == -2)
		cout << "V" << endl;
	else
		cout << (getVar(i) + 1) << endl;

	cout << "Orientation: ";
	for (i = (getNumberOfVar() - numPat); i < (getNumberOfVar() - 1); i++)
		cout << getVar(i) << ", ";
	cout << getVar(i) << endl;
			 
  	cout << "Length: " << getObj(0) << endl;	
  	cout << "Number of cuts: " << getObj(1) << endl << endl;	
	
}

//------------------------------------------------------------------------------
// Codigo auxiliar para iniciar una determinada solucion
//------------------------------------------------------------------------------
void CSP2_NG::fixSolution(void) {
		  
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

