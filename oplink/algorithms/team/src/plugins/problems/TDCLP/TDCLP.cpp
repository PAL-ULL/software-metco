#include "TDCLP.h"
#include <vector>
#include <stack>
#include <set>
#include <algorithm>

int TDCLP::numPat;  
int TDCLP::totalNumPat;
double TDCLP::widSheet;
double TDCLP::lenSheet;
double TDCLP::heiSheet;
double TDCLP::volSheet;
double TDCLP::weiSheet;
int* TDCLP::number; 
double* TDCLP::len;
double* TDCLP::wid;
double* TDCLP::hei;
double* TDCLP::wei;
int* TDCLP::orient;
vector<int> TDCLP::cont_items;
vector<int> TDCLP::ori1;
vector<int> TDCLP::ori2;
vector<int> TDCLP::ori3;
bool TDCLP::load = false;

// ---------------------------------------------------------------------------------
// Devuelve un entero entre 0 y maxValue-1
// Para mas informacion mirar en: man 3 rand
// --------------------------------------------------------------------------------
int TDCLP::uniformRandom(int maxValue) const {
        return (int) (rand() / ((RAND_MAX + 1.0) / maxValue));
}

// -------------------------------------------------------------------------------
// Fijar si el objetivo se debe maximizar o minimizar (indice i)
// -------------------------------------------------------------------------------
unsigned int TDCLP::getOptDirection(const int i) const{ 
        return MAXIMIZE;
}

// ---------------------------------------------------------------------------------
// Inicializacion. 
// Fija:
//   - Numero de variables: Numero de tipos diferentes de piezas
//   - Numero de objetivos: Peso y volumen
//   - Direccion de optimizacion de los objetivos: Maximizar peso y volumen
//   - Rangos de las variables (minimos y maximos)
// ---------------------------------------------------------------------------------
bool TDCLP::init (const vector<string> &params) {
cout<<"aqui"<<endl;
        if (params.size() < 1) {  //!= 1) {
                cerr << "Error: TDCLP called with error number of parameters" << endl;
                return false;
        }
        if (!readFile(params[0])) 
                return false;
        
        // Lectura de parametros
        int currentArg = 1;
        type_heuristic = atoi(params[currentArg].c_str());
        type_mutation = atoi(params[currentArg+1].c_str());

        // Numero de variables y objetivos
        setNumberOfVar(totalNumPat*3);
        setNumberOfObj(2);
        
        return true;
}

//----------------------------------------------------------------------------------
// Read a problem from file
// ---------------------------------------------------------------------------------
bool TDCLP::readFile(const string filename) {
        int or1, or2, or3;  // Tipos de orientaciones
        int total;
        double random;

        ifstream input(filename.c_str());
        if (!input.is_open()) {
                cerr << "Error: TDCLP called with error name of file" << endl;
                return false;
        }

        // Dimensiones del contenedor y tipos de piezas
        input >> lenSheet;
        input >> widSheet;
        input >> heiSheet;
        input >> weiSheet;
        input >> numPat;
        
        // Dimensiones, peso, orientacion y numero de cada tipo de pieza
        len = new double[numPat];
        wid = new double[numPat];
        hei = new double[numPat];
        wei = new double[numPat];
        orient = new int[numPat];
        number = new int[numPat];
        
        ori1.resize(numPat);
        ori2.resize(numPat);
        ori3.resize(numPat);
        
        // Vector que nos permite controlar el numero de piezas que nos van quedando
        cont_items.resize(numPat);
        
        total = 0;
        for (int i = 0; i < numPat; i++) {
                input >> len[i];
                input >> or1;
                ori1[i] = or1;
                input >> wid[i];
                input >> or2;
                ori2[i] = or2;
                input >> hei[i];
                input >> or3;
                ori3[i] = or3;
                input >> wei[i];
                input >> number[i];
                orient[i] = 2 * or1 + 2 * or2 + 2 * or3;
                total += number[i];
                // Introducimos los valores en el vector de contador de piezas de cada tipo
                cont_items[i] = number[i];  // Numero de items de ese tipo de cajas
                
                
                if (wid[i] > widSheet) {
                        cerr << " Error: width of pattern bigger than width of mother sheet "<< endl;
                        return false;
                }
                if (len[i] > lenSheet) {
                        cerr << " Error: length of pattern bigger than length of mother sheet "<< endl;
                        return false;
                }
                if (hei[i] > heiSheet) {
                        cerr << " Error: heigth of pattern bigger than heigth of mother sheet "<< endl;
                        return false;
                }
        }
        volSheet = lenSheet * widSheet * heiSheet;

        //cout<<"Volumen y Peso contenedor "<<volSheet <<" "<<weiSheet<<endl;
        input.close();
        
        totalNumPat = total;
        //cout<<"Numero total de piezas "<<totalNumPat;


        for (int i = 0; i <numPat; i++){
                cout<<"len "<<len[i]<<" wid "<<wid[i]<<" hei "<<hei[i]<<" wei "<<wei[i]<<" number "<<number[i]<<" orient "<<orient[i]<<endl;
                cout<<"ori1 "<<ori1[i]<<endl;
        }
        return true;
}


// -------------------------------------------------------------------------------
// Generation of an individual
// -------------------------------------------------------------------------------
void TDCLP::restart() {
        randomRestart();
}


// -------------------------------------------------------------------------------
// Random generation of an individual
// El tama�o del individuo equivale al numero de tipos diferentes de cajas.
// Cada gen del cromosoma representa una posible rotacion del conjunto de cajas del 
// mismo tipo. Todas las cajas de un mismo tipo tienen la misma rotacion
// -------------------------------------------------------------------------------
void TDCLP::randomRestart() {
        int value;
        int ContBoxRest = totalNumPat;    // Contador de piezas que quedan de cada tipo de caja
        int p = 0;
        int id, val_id, numberBox, typeRotation;
        sizeGen = 0;
        vector<P_Cont> copy_items;
        copy_items.resize(numPat);

        // Hacer copia del vector que contiene los id y el numero de piezas de cada tipo
        for (int i = 0; i < numPat; i++) {
                copy_items[i].id = i;
                copy_items[i].numberPieces = cont_items[i];
        }
        //copy(cont_items.begin(),cont_items.end(),copy_items.begin());
        
        //cout<<"total "<<ContBoxRest<<endl;
        while (ContBoxRest != 0) {
                
                val_id = uniformRandom(copy_items.size());     //(cont_items.size());
                //cout<<"ID1 "<<val_id<<" cajas "<<copy_items[val_id].numberPieces<<endl;
                
                id = copy_items[val_id].id;
                //cout<<"ID2 "<<id<<" cajas "<<number[id]<<endl;        
                
                if (copy_items[val_id].numberPieces != 0) {                 //cont_items[val_id].numberPieces != 0) {
                        sizeGen++;
                        numberBox = uniformRandom(copy_items[val_id].numberPieces) + 1;               //cont_items[val_id].numberPieces) + 1;
                        //cout<<"Number box "<<numberBox<<" Las que habian "<<copy_items[val_id].numberPieces<<endl;
                        
                        // Actualizar vector de contador
                        if ((copy_items[val_id].numberPieces - numberBox) != 0) {
                                copy_items[val_id].numberPieces = copy_items[val_id].numberPieces - numberBox;
                                //cout<<"Nuevo valor de items "<<copy_items[val_id].numberPieces<<endl;
                        }
                        else if ((copy_items[val_id].numberPieces - numberBox) == 0) {
                                //cout<<"Se borra"<<endl;
                                copy_items.erase(copy_items.begin() + val_id);
                        }
                        
                        typeRotation = uniformRandom(orient[id]);
                        //cout<<"Rotation "<<typeRotation<<endl;
                        
                        setVar(3*p,id);  // Id del tipo de pieza
                        setVar(3*p+1,numberBox);   // Numero de piezas que se cogen del mismo tipo 
                        setVar(3*p+2,typeRotation);   // Tipo de rotacion permitida aplicada al conjunto de piezas
                        p++;
                        
                        ContBoxRest = ContBoxRest - numberBox;
                        //cout<<"Resto de piezas "<<ContBoxRest<<endl;
                }
        
        }

        copy_items.clear();     
        sizeGen = sizeGen * 3;
        //cout<<"Tamano Gen "<<sizeGen<<endl;
        //Visualizar inicializacion cromosoma
        /*cout<<"Cromosoma";
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
        cout<<"salio random"<<endl;
        */
}

//----------------------------------------------------------------------------
// Clone
// ---------------------------------------------------------------------------
Individual* TDCLP::clone (void) const {
        TDCLP* ind = new TDCLP();
        ind->sizeGen = sizeGen;
        ind->type_heuristic = type_heuristic;
        ind->type_mutation = type_mutation;
        return ind;
}

//-------------------------------------------------------------------------------
// Print
// -----------------------------------------------------------------------------
// Operador de salida: por defecto imprime variables y objetivos
void TDCLP::print(ostream &os) const { 
        if (!load) {
                os<<"\n";
                os <<"Fichero de Carga " <<"\n";
                os << lenSheet<<" "<<widSheet<<" "<<heiSheet<<" "<<weiSheet<<"\n";
                os <<numPat<<"\n";
        
                for (int i = 0; i < numPat; i++) {
                        os <<len[i] <<" "<<ori1[i]<<" "<<wid[i]<<" "<<ori2[i]<<" " <<hei[i]<<" "<<ori3[i]<<" "<<wei[i]<<" "<<number[i]<<"\n";
                }
                load = true;
        }
        os <<"\n";
        for (unsigned int i = 0; i < getNumberOfVar(); i++)
                os << getVar(i) << " ";
        for (unsigned int i = 0; i < getNumberOfObj(); i++)
                os << getObj(i) << " ";
        
        os << endl;
}


// -----------------------------------------------------------------------------------
// Mutation:
// Se escoge un gen al azar y se cambia el valor de la rotacion, por un valor permitido
// ------------------------------------------------------------------------------------
void TDCLP::dependentMutation (const double pm){
        mutation_moves(pm);
}

// -------------------------------------------------------------------------------
// Mutacion basada en la aplicacion de los tres tipos de movimientos
// -------------------------------------------------------------------------------
void TDCLP::mutation_moves (double pm) {       

        // Mutation fuerte
        double prob = rand() / (RAND_MAX + 1.0);
        if (prob < pm) 
                mutation_add (pm);
        prob = rand() / (RAND_MAX + 1.0);
        if (prob < pm) 
                mutation_remove (pm);
        prob = rand() / (RAND_MAX + 1.0);
        if (prob < pm) 
                mutation_replace (pm);
        // Operadores de mutacion
        prob = rand() / (RAND_MAX + 1.0);
        if (prob < pm ) {
                if (type_mutation == 1) {
                        mutation_change(pm);
                }
                if (type_mutation == 2) {
                        mutation_swap(pm);
                }
                if (type_mutation == 3) {
                        mutation_compact(pm);
                }
                if (type_mutation == 4) {
                        mutation_shift(pm);
                }
        }
}               


void TDCLP::mutation_add (double pm) {

        //cout<<"mutation add"<<endl;
        vector<int> positions;
        int init_pos, rotation, numPieces, type, new_pos;
        
        int sizeCrom = sizeGen/3;

        if (sizeCrom > 1) {
                type = uniformRandom(numPat);  // Se genera un tipo de piezas
                //cout<<"type "<<type<<endl;
                create_vector(positions,type,-1);    // Generamos un vector con todas las posiciones donde se genero piezas del mismo tipo
                init_pos = uniformRandom(positions.size());
                //cout<<"Posicion dentro del vector "<<init_pos<<endl;
                init_pos = positions[init_pos] * 3;
                //cout<<"Posicione en el gen "<<init_pos<<endl;
                // Cuantas piezas eliminamos [1-numPieces]
                numPieces = getVar(init_pos+1);               // Numero de piezas del tipo type en esa posicion del cromosoma
                if (numPieces > 1) {
                        //cout<<"numero de pieces en el gen "<<numPieces<<endl;
                        numPieces = rand() % (numPieces-1) + 1; // Valor [1- (numPieces-1)]
                        //cout<<"numero piezas que se le restan "<<numPieces<<endl;
                        rotation = uniformRandom(orient[type]);  // Selecciona un valor de rotacion permitido para el tipo de piezas
                        //cout<<"rotation "<<rotation<<endl;
                        setVar(init_pos+1,getVar(init_pos+1)-numPieces);   // Adecuar el valor nuevo del gen al que se le quitan piezas
                        // Seleccionar la posicion en la que se va a aniadir
                        new_pos = uniformRandom(sizeCrom+1);  // Se genera un valor [0-(size/3)+1] -> el +1 para posicionarlo al final del gen
                        //cout<<"Posicion en la que se introduce en el gen "<<new_pos<<endl;
                        new_pos = new_pos * 3; // Posicion en el gen
                        if (new_pos > sizeGen) {  // En la ultima posicion
                                setVar(new_pos,type);
                                setVar(new_pos+1,numPieces);
                                setVar(new_pos+2,rotation);
                        }
                        else {
                                for (int i = (sizeGen-1); i >= new_pos; i--) {
                                        setVar(i+3, getVar(i));
                                }
                                setVar(new_pos,type);
                                setVar(new_pos+1,numPieces);
                                setVar(new_pos+2,rotation);
                        }
                        sizeGen = sizeGen + 3;   // Actualizar tamanio del cromosoma
                }
        }
        positions.clear();
        /*
        cout<<"CromosomaFin add"<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
        */
}


void TDCLP::mutation_remove (double pm) {
        
        //cout<<"mutation_remove"<<endl;
        vector<int> positions;
        int position, type, type_count, init_pos, new_pos, rotation;
        int sizeCrom = sizeGen/3;
        
        if (sizeCrom > 1) {    // Si el tama?o del gen es m?s de uno
                position = uniformRandom(sizeCrom);  // Genera un valor entre [0-(GenSize-1)]
                init_pos = position * 3;             // Posicion de la que se parte
                //cout<<"Valor de position "<<position<<endl;
                type = getVar(position*3);        // El tipo de cajas que vamos a eliminar
                //cout<<"el tipo "<<type<<endl;
                // Contar cuantos valores de type existen en el cromosoma
                type_count = count_type(type);
                if (type_count > 1) {
                        //cout<<"Es mayor de uno "<<type_count<<endl;
                        create_vector(positions,type,position); // Vector contiene las posiciones con igual type
                        //cout<<"valor positions "<<positions[1];
                        new_pos = uniformRandom(positions.size());
                        //cout<<"Valor vector "<<new_pos<<endl;
                        new_pos = positions[new_pos] * 3;
                        //cout<<"Posicion vector "<<new_pos<<endl;
                        //cout<<"Comprobacion "<<init_pos<<" "<<sizeGen<<endl;
                        rotation = uniformRandom(orient[type]);
                        if ((init_pos +2) == (sizeGen-1)) {
                                setVar(new_pos+1,getVar(init_pos + 1) + getVar(new_pos + 1));                   
                                setVar(new_pos+2,rotation);
                        }
                        else {
                                setVar(new_pos+1, getVar(init_pos + 1) + getVar(new_pos + 1));
                                setVar(new_pos+2,rotation);
                                for (int i = init_pos; i < sizeGen; i++) {
                                        setVar(i,getVar(i+3));
                                }
                        }
                        sizeGen = sizeGen - 3;
                }
        }       
        positions.clear();
        /*
        cout<<"CromosomaFin remove"<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
        */
}

void TDCLP::create_vector(vector<int> &positions, int type, int pos) {

        //cout<<"llamada vectorp"<<endl;
        for (int i = 0; i < (sizeGen/3); i++){
                if (getVar(i*3) == type){
                        if (i != pos) {
                                positions.push_back(i);
                        }
                }
        }
/*      cout<<"Vector positions"<<endl;
        for (int i = 0; i < positions.size();i++) {
                cout<<"Valor "<<positions[i];
        }
        cout<<endl;
cout<<"sale"<<endl;
*/      
}

int TDCLP::count_type(int type) {
        
        int count = 0;
        for (int i = 0; i < (sizeGen/3); i++) {
                //cout<<"Gen "<<getVar(i*3)<<endl;
                if (getVar(i*3) == type) {
                        count++;
                        //cout<<"count "<<count<<endl;
                }
        }
        return count-1;
}

void TDCLP::mutation_replace(double pm) {

        //cout<<"mutation_replace"<<endl;
        int init_pos, position, id, rotation;
        int sizeCrom = sizeGen/3;
        /*cout<<"sizeCrom "<<sizeCrom<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
*/
        if (sizeCrom > 1) {
                position = uniformRandom(sizeCrom);  // Genera un valor entre [0-(GenSize-1)]
                //cout<<"position "<<position<<endl;
                init_pos = position * 3;             // Posicion de la que se parte
                id = getVar(init_pos);               // El id del conjunto de piezas
                //cout<<"init_pos id "<<init_pos<<" "<<id<<endl;
                do {
                        rotation = uniformRandom(orient[id]);
                        //cout<<"rotation "<<rotation<<endl;
                }while (rotation == getVar(init_pos + 2));
                setVar(init_pos+2,rotation);
        }
        
        /*cout<<"CromosomaFinen replace"<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
        */
}


// --------------------------------------------------------------------------
// Mutation: Operador de cambio.
// Recorremos todo el cromosoma y para cada tipo de pieza cambiar de forma
// aleatoria la orientacion. Teniendo en cuenta una probabilidad de mutacion
// --------------------------------------------------------------------------
void TDCLP::mutation_change(double pm) {
       /* cout<<"Cromosoma en change"<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;*/
        
        vector<int> orientaciones;
        double prob;
        int rotation, rot, type;
        int sizeCrom = sizeGen/3;

        if (sizeCrom > 1) {
                for (int i = 0; i < sizeCrom; i++) {
                        //cout<<"Valor de i "<<i<<" valor de la rotacion "<<getVar(i*3+2)<<endl;
                        prob = rand() / (RAND_MAX + 1.0);
                        //cout<<"Probalidad "<<prob<<endl;
                        if (prob < pm) {
                                type = getVar(i*3);
                                //cout<<"Type "<<type<<endl;
                                rot = orient[type];
                                //cout<<"Tipo de orientacion "<<rot<<endl;
                                for (int j = 0; j < rot; j++){
                                        if (j != getVar(i*3+2)) {
                                                //cout<<"Valor de la orientacion "<<j<<endl;
                                                orientaciones.push_back(j);
                                        }
                                }
                                rotation = uniformRandom(orientaciones.size());
                                rotation = orientaciones[rotation];
                                //cout<<"rotation "<<rotation<<endl;
                                setVar(i*3+2,rotation);
                        }
                        orientaciones.clear();
                }
        }
       /*cout<<"Cromosoma final en change"<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
 */
}

// -------------------------------------------------------------------------
// Mutation: Intercambiar dos genes aleatorios del cromosoma
// -------------------------------------------------------------------------
void TDCLP::mutation_swap(double pm) {

        int tmp_type, tmp_numb, tmp_or, position1, position2;
        vector<int> positions;
        int sizeCrom = sizeGen/3;
/*              cout<<"Cromosoma en swap"<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
*/ 
        if (sizeCrom > 1) {
                position1 = uniformRandom(sizeCrom);
                //cout<<"Position1 "<<position1<<endl;
                for (int j = 0; j < sizeCrom; j++){
                        if (j != position1) {
                                positions.push_back(j);
                        }
                }
                position2 = uniformRandom(positions.size());
                position2 = positions[position2];
                //cout<<"Position2 "<<position2<<endl;
                tmp_type = getVar(position1*3);
                tmp_numb = getVar(position1*3 + 1);
                tmp_or = getVar(position1*3 +2);
                setVar(position1*3, getVar(position2*3));
                setVar(position1*3+1, getVar(position2*3+1));
                setVar(position1*3+2, getVar(position2*3+2));
                setVar(position2*3, tmp_type);
                setVar(position2*3+1, tmp_numb);
                setVar(position2*3+2, tmp_or);
        }
        positions.clear();
       /*cout<<"Cromosoma final en swap"<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
 */
}

// -------------------------------------------------------------------------
// Mutation: Elegir un gen en el cromosoma y seleccionar un gen del mismo
// tipo de pieza, y compactarlos con una orientacion aleatoria
// -------------------------------------------------------------------------
void TDCLP::mutation_compact(double pm) {

        vector<int> positions;
        int type, position, gen, rotation;
        int sizeCrom = sizeGen/3;
/*        cout<<"Cromosoma en compact"<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
 */
        if (sizeCrom > 1) {
                gen = uniformRandom(sizeCrom);
                //cout<<"Gen "<<gen<<endl;
                type = getVar(gen*3);
                //cout<<"Type "<<type<<endl;
                create_vector(positions, type, gen);
                if (positions.size() >= 1) {
                        position = uniformRandom(positions.size());
                        position = positions[position];
                        rotation = uniformRandom(orient[type]);
                        //cout<<"position "<<position<<" rotation "<<rotation<<endl;
                        if ((gen*3+2) == (sizeGen -1)) {
                                setVar(position*3+1, getVar(gen*3+1) + getVar(position*3+1));
                                setVar(position*3+2,rotation);
                        }
                        else {
                                setVar(position*3+1, getVar(gen*3+1) + getVar(position*3+1));
                                setVar(position*3+2,rotation);
                                for (int i = gen*3; i < sizeGen; i++) {
                                        setVar(i, getVar(i+3));
                                }
                        }
                        sizeGen = sizeGen - 3;
                }
        }
        positions.clear();
/*        cout<<"Cromosoma final en compact"<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
*/ 
}

// -------------------------------------------------------------------------
// Mutation: Coger un gen de forma aleatoria y una posicion aleatoria. 
// Introducir el gen en la posicion seleccionada, y desplazar el resto de
// genes de cromosomas hacia la derecha
// -------------------------------------------------------------------------
void TDCLP::mutation_shift(double pm) {

        int gen, position, type, numb, orient;
        vector<int> positions;
        int sizeCrom = sizeGen/3;

/*      cout<<"Cromosoma en shift"<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
*/
        if (sizeCrom > 1) {
                gen = uniformRandom(sizeCrom);
                type = getVar(gen*3);
                numb = getVar(gen*3+1);
                orient = getVar(gen*3+2);
                //cout<<"Gen aleatorio "<<gen<<endl;
                for (int j = 0; j < sizeCrom; j++){
                        if (j != gen) {
                                positions.push_back(j);
                        }
                }
                position = uniformRandom(positions.size());
                position = positions[position];
                //cout<<"Position aleatoria "<<position<<endl;
                /*if (position > sizeGen) {
                        setVar(position*3, getVar(gen*3));
                        setVar(position*3+1, getVar(gen*3+1));
                        setVar(position*3+2, getVar(gen*3+2));
                }
                else {*/
                        for (int i = (sizeGen - 1); i >= (position*3); i--) {
                                //cout<<"Valor i "<<i<<" getVar "<<getVar(i)<<endl;
                                setVar(i+3, getVar(i));
                        }
                        setVar(position*3,type);
                        setVar(position*3+1, numb);
                        setVar(position*3+2, orient);
                //}
                sizeGen = sizeGen + 3;

                if (gen > position) {
                // Borrar el hueco donde se encontraba el gen
                        for (int i = (gen+1)*3; i < sizeGen; i++) {
                                setVar(i, getVar(i+3));
                        }
                }
                else {
                        for (int i = gen*3; i < sizeGen; i++) {
                                setVar(i, getVar(i+3));
                }
                }
                sizeGen = sizeGen - 3;
        }       
/*      cout<<"Cromosoma final en shift"<<endl;
        for (int i = 0; i < sizeGen; i++) {
                cout<<getVar(i)<<" ";
        }
        cout<<endl;
*/
        positions.clear();

}


// -----------------------------------------------------------------------------
// Se parten ambos individuos por el mismo punto de corte, generado este
// a partir de un numero aleatorio. Una vez se tiene el punto de corte
// se hace que la parte traceada mayor del individuo 1 se une con la
// con la parte troceada menor del individuo 2. Y lo mismo ocurre con
// el individuo dos, se coge su parte troceada mayor y se une con la
// parte troceada menor del individuo 1
// ------------------------------------------------------------------------------
void TDCLP::dependentCrossover(Individual *ind){
        crossover_onepoint(ind);
}

// ------------------------------------------------------------------------------
// Crossover de un punto
// Se corta cada cromosoma por un punto al azar y se intercambian las segundas partes
// ------------------------------------------------------------------------------
void TDCLP::crossover_onepoint(Individual *ind) {
        int p0, p1, type, n, i, j, k, genPos;
        vector<int> t, posDel;
        vector<int> v0, v1;
        set<int> setDel;

        t.resize(numPat);

        //printInd(this);
        //printInd(ind);
 
        p0 = uniformRandom(sizeGen - 5)/3 * 3 + 3;
        p1 = uniformRandom(((TDCLP*) ind)->sizeGen - 5)/3 * 3 + 3;
        //cout << sizeGen << " " << ((TDCLP*) ind)->sizeGen << " p0, p1 "<< p0 << "  " << p1<< endl;
        // Rellenar v0 con la parte que se va a intercambiar del cromosoma0
        for (i = p0; i < sizeGen; i++) 
                v0.push_back((int) getVar(i));  
        // Rellenar v1 con la parte que se va a intercambiar del cromosoma1
        for (i = p1; i < ((TDCLP*) ind)->sizeGen; i++) 
                v1.push_back((int) ind->getVar(i));
       

        // Hacer copia del vector que contiene el numero de piezas de cada tipo
        copy(cont_items.begin(),cont_items.end(),t.begin());
        
        // Obtener para cada tipo cuantas piezas faltan en la primera parte del cromosoma0 si se agrega la parte del cromosoma1
        for (i = 0; i < v1.size()/3; i++) {
                t[v1[i*3]] -= v1[i*3+1];
        //      cout << " tipo" << v1[i*3] << " faltan "<< t[v1[i*3]];
        }
        //cout << endl;
        // Obtener los tipos de piezas que hay que eliminar de la primera parte del cromosoma0 porque ya estan completas en la segunda parte
        for (i = 0; i < p0/3; i++) {
                type = getVar(i*3);
                n = getVar(i*3+1);
        //      cout << type << " " << n << endl;
                if (t[type] == 0) {
                        posDel.push_back(i*3);
                        setDel.insert(i*3);
        //              cout << "eliminar en posicion " << i*3 << endl;
                }
                else {
                        if (t[type] >= n) 
                                t[type] -= n;
                        else {
                                setVar(i*3+1, t[type]);
                                t[type] = 0;
                        }
                }
        }
        genPos = p0;
        //for (i = 0; i < v1.size()/3; i++) {
        //      cout << " tipo" << v1[i*3] << " faltan "<< t[v1[i*3]];
        //}
        //cout << endl;
        //cout << "compactar0 " << posDel.size() << endl;
        // Compactar
        for (k = 0; k < posDel.size(); k++) {
                //for (int f = 0; f <posDel.size(); f++) {
                //      cout << posDel[f] << " ";
                //}
                //cout << endl;
                // Buscar gen posterior para rellenar hueco
                for (j = posDel[k] + 3; j < p0; j += 3) {
                        if (setDel.find(j) == setDel.end())
                                break;
                }
                if (j < p0) {
                //      cout << "rellenar con pos " << j << endl;
                        // Rellenar hueco
                        setVar(posDel[k], getVar(j));
                        setVar(posDel[k]+1, getVar(j+1));
                        setVar(posDel[k]+2, getVar(j+2)); 
                        // Se crea nuevo hueco
                        setDel.insert(j);
                        for (i = k + 1; i < posDel.size(); i++) {
                                if (posDel[i] > j) {
                //                      cout << "insertado en medio" << endl;
                                        posDel.insert(posDel.begin()+i,j);
                                        break;
                                }
                        }
                        if (i == posDel.size()) {
                                 posDel.push_back(j);
                //               cout << "insertado al final" << endl;
                        }
                }
                else {
                        genPos = posDel[k];
                        break;
                }
                //for (int f = 0; f < posDel.size(); f++) {
                //      cout << posDel[f] << " ";
                //}
                //cout << endl;
        }
        //cout << "anadir0 " << endl;
        // A�adir piezas si faltan por poner, a partir de k
        for (i = 0; i < t.size(); i++) {
                if (t[i] > 0) {
                        setVar(genPos,i);
                        setVar(genPos+1, t[i]);
                        setVar(genPos+2, uniformRandom(orient[i]));
                        genPos+=3;
        //              cout << "anadir en " << genPos-3 << endl;
                }
        }
        // Agregar la segunda parte del cromosoma
        for (i = 0; i < v1.size(); i++) {
                setVar(genPos, v1[i]);
                genPos++;
        }
        sizeGen = genPos;

        //cout << "segundo" << endl;
        posDel.clear();
        setDel.clear();

        t.clear();      
        t.resize(numPat);
        
        // Hacer copia del vector que contiene el numero de piezas de cada tipo
        copy(cont_items.begin(),cont_items.end(),t.begin());
        
        // Obtener para cada tipo cuantas piezas faltan en la primera parte del cromosoma1 si se agrega la parte del cromosoma0
        for (i = 0; i < v0.size()/3; i++) { 
                t[v0[i*3]] -= v0[i*3+1];
        //       cout << " tipo" << v0[i*3] << " faltan "<< t[v0[i*3]];
        }
        //cout << endl;
        // Obtener los tipos de piezas que hay que eliminar de la primera parte del cromosoma1 porque ya estan completas en la segunda parte
        for (i = 0; i < p1/3; i++) {
                type = ind->getVar(i*3);
                n = ind->getVar(i*3+1);
        //      cout << type << " " << n << endl;
                if (t[type] == 0) {
                        posDel.push_back(i*3);
                        setDel.insert(i*3);
                }
                else {
                        if (t[type] >= n) 
                                t[type] -= n;
                        else {
                                ind->setVar(i*3+1, t[type]);
                                t[type] = 0;
                        }
                }
        }
        //for (i = 0; i < v0.size()/3; i++) {
        //       cout << " tipo" << v0[i*3] << " faltan "<< t[v0[i*3]];
        //}

        genPos = p1;
        //cout << "compactar1" << posDel.size() << endl;
        // Compactar
        for (k = 0; k < posDel.size(); k++) {
                //for (int f = 0; f <posDel.size(); f++) {
                //        cout << posDel[f] << " ";
                //}
                //cout << endl;
                // Buscar gen posterior para rellenar hueco
                for (j = posDel[k] + 3; j < p1; j += 3) {
                        if (setDel.find(j) == setDel.end())
                                break;
                }
                if (j < p1) {
                  //      cout << "rellenar con pos " << j << endl;
                        // Rellenar hueco
                        ind->setVar(posDel[k], ind->getVar(j));
                        ind->setVar(posDel[k]+1, ind->getVar(j+1));
                        ind->setVar(posDel[k]+2, ind->getVar(j+2));
                        // Se crea nuevo hueco
                        setDel.insert(j);
                        for (i = k + 1; i < posDel.size(); i++) {
                                if (posDel[i] > j) {
                //                      cout << "insertado en medio" << endl;
                                        posDel.insert(posDel.begin()+i,j);
                                        break;
                                }
                        }
                        if (i ==  posDel.size()) {
                                posDel.push_back(j);
                //              cout << "insertado al final" << endl;
                        }
                }
                else {
                        genPos = posDel[k];
                        break;
                }
                //for (int f = 0; f < posDel.size(); f++) {
                //        cout << posDel[f] << " ";
                //}
                //cout << endl;

        }
        //cout << "anadir1" << endl;
        // A�adir piezas si faltan por poner, a partir de k
        for (i = 0; i < t.size(); i++) {
                if (t[i] > 0) {
                        ind->setVar(genPos,i);
                        ind->setVar(genPos+1, t[i]);
                        ind->setVar(genPos+2, uniformRandom(orient[i]));
                        genPos += 3;
        //               cout << "anadir en " << genPos -3 << endl;
                }
        }
        // Agregar la segunda parte del cromosoma
        for (i = 0; i < v0.size(); i++) {
                ind->setVar(genPos, v0[i]);
                genPos++;
        }
        ((TDCLP*) ind)->sizeGen = genPos;

        posDel.clear();
        setDel.clear();
        t.clear();
        v0.clear();
        v1.clear();
        
        //cout << "fin" << endl;
        //printInd(this);
        //printInd(ind);
        
}

//-------------------------------------------------------------------------------
// Evaluate
// ------------------------------------------------------------------------------
void TDCLP::evaluate (void) {
        if (type_heuristic == 1) {
                heuristic_AC();   // Ancho con cierre
        }
        if (type_heuristic == 2) {
                heuristic_AS();   // Ancho sin cierre
        }
        if (type_heuristic == 3) {
                heuristic_LC();   // Largo con cierre
        }
        if (type_heuristic == 4) {
                heuristic_LS();   // Largo sin cierre
        }
}

// ------------------------------------------------------------------------------
// Funcion auxiliar para la ordenacion de los huecos de menor a mayor volumen
// -----------------------------------------------------------------------------
bool cmpVol(const Holes &h1, const Holes &h2) {
        return (h1.getVol() < h2.getVol());     
}

// ------------------------------------------------------------------------------
// Funcion auxiliar para la ordenacion de las piezas de mayor a menor peso
// -----------------------------------------------------------------------------
bool cmpWei(const P_Order &p1, const P_Order &p2) {
        return (p1.wei > p2.wei);       
}

// ------------------------------------------------------------------------------
// Funcion auxiliar para la ordenacion de las piezas de mayor a menor peso
// -----------------------------------------------------------------------------
bool cmpWid(const P_Order &p1, const P_Order &p2) {
        return (p1.wid > p2.wid);       
}

// -----------------------------------------------------------------------------
// Establece la rotacion indicada por el gen para cada tipo de pieza.
// Los vectores len, wid, hei, wei y orient solo contienen los valores de los
// distintos tipos de caja (numPat)
// ------------------------------------------------------------------------------
void TDCLP::orientation_pieces(int id, int rot) {

        switch(rot) {
                case 0: widR = wid[id];
                        depR = len[id];
                        heiR = hei[id];
                        //cout<<"case 0"<<endl;
                        break;
                case 1: widR = len[id];
                        depR = wid[id];
                        heiR = hei[id];
                        //cout<<"case 1"<<endl;
                        break;
                case 2: widR = hei[id];
                        depR = len[id];
                        heiR = wid[id];
                        //cout<<"case 2"<<endl;
                        break;
                case 3: widR = len[id];
                        depR = hei[id];
                        heiR = wid[id];
                        //cout<<"case 3"<<endl;
                        break;
                case 4: widR = wid[id];
                        depR = hei[id];
                        heiR = len[id];
                        //cout<<"case 4"<<endl;
                        break;
                case 5: widR = hei[id];
                        depR = wid[id];
                        heiR = len[id];
                        //cout<<"case 5"<<endl;
                        break;
        } // switch
}


// --------------------------------------------------------
// Rellena todo el vector segun el numero de items de cada
// tipo
// --------------------------------------------------------
void TDCLP::fillVect() {

        //cout<<"Dentro de fillVect "<<totalNumPat<<endl;
        itemsCont.resize(totalNumPat);
        int total, cont, id, rot;
        total = cont = 0;       
        
        for (int p = 0; p < (sizeGen/3); p++) {
                id = getVar(p*3);     // Identificador de la pieza
                rot = getVar(p*3+2);  // Tipo de rotacion
                //cout<<"id rot "<<id<<" "<<rot<<endl;
                orientation_pieces(id,rot);
                cont = getVar(p*3+1); // Numero de piezas del mismo tipo
                for (int j = total; j < (total + cont); j++) {
                        itemsCont[j].wid = widR;
                        //cout<<"widR "<<widR<<endl;
                        itemsCont[j].len = depR;
                        //cout<<"depR "<<depR<<endl;
                        itemsCont[j].hei = heiR;
                        //cout<<"heiR "<<heiR<<endl;
                        itemsCont[j].wei = wei[id];
                        //cout<<"wei "<<wei[id]<<endl;
                        itemsCont[j].vol = itemsCont[j].wid * itemsCont[j].hei * itemsCont[j].len;
                        //cout<<"vol "<<itemsCont[j].vol<<endl;
                        itemsCont[j].pack = false;
                        itemsCont[j].index = j;
                }
                total += cont;
                //cout<<"total "<<total<<endl;
        }

        /*cout<<"Piezas ordenadas en fillVetc "<<endl;
        for (int i = 0; i < totalNumPat; i++) {
                cout<<" wid "<<itemsCont[i].wid<<" len "<<itemsCont[i].len<<" hei "<<itemsCont[i].hei<<" wei "<<itemsCont[i].wei<<" pack "<<itemsCont[i].pack<<"index "<<itemsCont[i].index<<endl;
        }
        */
        //cout<<"fillVect sale"<<endl;
}


// Empaquetar items dentro de cada nivel
// Cuando se llegue al final del vector de items se cierra
// el nivel actual, y se genera un nuevo nivel
// itemsCont -> vector de piezas ordenadas
// ----------------------------------------------------------
void TDCLP::heuristic_LC() {
        vector<P_Order> aux;
        
        // Variables para controlar las dimensiones de los huecos
        double Hwid;
        double Hlen;
        double Hhei;
        double Hvol;
        
        // Variables para ir acumulando los objetivos del problema
        double Obj1_Volumen, Obj2_Weigth;
        Obj1_Volumen = Obj2_Weigth = 0;   // Valores de los objetivos Obj1->Volumen Obj2->Peso
            
        //sortItems();  // Ordenar las cajas segun tipo de ordenacion aplicada
        fillVect();     // Genera vector con las rotaciones realizadas

        // BORRAR
        
        /*cout<<"Piezas ordenadas "<<endl;
        for (int i = 0; i < totalNumPat; i++) {
                cout<<" wid "<<itemsCont[i].wid<<" len "<<itemsCont[i].len<<" hei "<<itemsCont[i].hei<<" wei "<<itemsCont[i].wei<<" pack "<<itemsCont[i].pack<<endl;
        }
        */

        int W_Sheet_free = lenSheet;   // Contador de wid libre en el contenedor
        double LDB;              // LDB es el ancho del nivel
        int count = 0;       // Indice dentro del vector itemsCont
        int holesId = 0;         // Indice dentro de los vectores de huecos
        bool label_control, finish;

        //cout<<"item s"<<itemsCont.size()<<endl;
        // Comprobaciones:
        // 1.- Existen cajas que introducir en el contenedor
        // 2.- Se puede crear un nuevo nivel en el contenedor
        // 3.- Hay espacio en el contenedor
        while ((!itemsCont.empty()) && (W_Sheet_free >= itemsCont[itemsCont.size()-1].len) && ((lenSheet - W_Sheet_free) < lenSheet) && (Obj2_Weigth < weiSheet)) {

                //cout<<"Comprobaciones primer while "<<endl;
                //cout<<"W_Sheet_free >= itemsCont[itemsCont.size()-1].wid "<<W_Sheet_free<<" "<<itemsCont[itemsCont.size()-1].wid<<endl;
                //cout<<"widSheet - W_Sheet_free) < widSheet "<<widSheet - W_Sheet_free<<" "<<widSheet<<endl;
                
                //cout<<"Fuera del while que recorre itemsCont "<<count<<""<<Obj2_Weigth<<""<<itemsCont[count].wei<<" "<<Obj2_Weigth + itemsCont[count].wei<<endl;
                // Se recorre el vector de itemsCont en busca de una caja que pueda ser introducida en el contenedor
                //cout<<"entrar "<<itemsCont.size()<<endl;
                finish = false;
                while ( (itemsCont[count].len > W_Sheet_free) || (Obj2_Weigth + itemsCont[count].wei > weiSheet && count < itemsCont.size()) ) { 
                        count++; // Indica el indice de la primera caja que puede ser introducida en el contenedor, marcando el valor del LDB
                        //cout<<"Dentro del while que recorre itemsCont "<<count<<""<<Obj2_Weigth<<""<<itemsCont[count].wei<<" "<<Obj2_Weigth + itemsCont[count].wei<<endl;
                }
                //cout<<"salir "<<count<<endl;
                if (count < itemsCont.size()) { // Puede o no, ser el ultimo item del itemsCont

                        //cout<<"Se crean los primeros huecos "<<endl;
                        LDB = itemsCont[count].len;                          // Se establece el wid del nivel
                        W_Sheet_free = W_Sheet_free - LDB;                   // Wid libre en el contenedor para el resto de niveles que se puedan crear
                        itemsCont[count].pack = true;
                        Obj2_Weigth += itemsCont[count].wei;                 // Se acumula el peso con la nueva pieza introducida
                        
                        // Importante:
                        // Siempre que se introduce una pieza para generar un nuevo nivel,
                        // solo se producen los huecos front y above
                        
                        // Codigo nuevo. Teniendo en cuenta los huecos
                        // Calculo de los que huecos que se generan
                        if ((widSheet - itemsCont[count].wid) > 0) { // Si despues de empaquetar la pieza queda hueco en el Front
                                // Se genera el primer hueco del nivel en el vector holesF
                                //cout<<"Se genera hueco front"<<endl;
                                Hwid = widSheet - itemsCont[count].wid;
                                Hlen = LDB;
                                Hhei = itemsCont[count].hei;
                                Hvol = Hwid * Hlen * Hhei;
                                holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Se crea el hueco en el vector holesF
                        }
                        if ((heiSheet - itemsCont[count].hei) > 0 ) {  // Si despues de empaquetar la pieza queda hueco en el Above
                                Hwid = widSheet;
                                Hlen = LDB;
                                Hhei = heiSheet - itemsCont[count].hei;
                                Hvol = Hwid * Hlen * Hhei;
                                holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                        }
                        count++;
                        holesId = 0;
                        label_control = false;   // Etiqueta de control
                        

                        // Recorrer vectores de huecos
                        /*
                        cout<<"Inicio nivel HolesF"<<endl;
                        for (int i = 0; i < holesF.size(); i++){
                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                        }

                        cout<<"Inicio nivel HolesA"<<endl;
                        for (int i = 0; i < holesA.size(); i++){
                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                        }
                        
                        cout<<"Comienza el while de relleno"<<endl;
                        */
                        
                        while ((count < itemsCont.size()) && (itemsCont[count].len <= LDB) && ((Obj2_Weigth + itemsCont[count].wei) <= weiSheet)) { // Se rellena el nivel con 
                                                                                                                        // nuevas cajas, que quepan en el nivel
                                //cout<<"Relleno  "<<count<<" "<<Obj2_Weigth<<" "<<itemsCont[count].wei<<" "<<Obj2_Weigth + itemsCont[count].wei<<endl;
                                holesId = 0;
                                // Se recorre primero el vector de holesF
                                while ((holesId < holesF.size()) && (label_control != true)) {
                                        //cout<<"while de holesF holesId "<<holesId<<endl;
                                        if ((holesF[holesId].getWid() == itemsCont[count].wid) &&
                                            (holesF[holesId].getLen() == itemsCont[count].len) &&
                                            (holesF[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                                //cout<<"Cabe justo en Front: se borra hueco Front"<<endl;
                                                holesF.erase(holesF.begin() + holesId);  // Se elimina el hueco del vector
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                        }
                                        else if ((holesF[holesId].getWid() >= itemsCont[count].wid) &&
                                            (holesF[holesId].getLen() >= itemsCont[count].len) &&
                                            (holesF[holesId].getHei() >= itemsCont[count].hei)) { // si sobra espacio, se deben crear los nuevos huecos
                                                //cout<<"No cabe justo en Front"<<endl;
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                                // Calcular huecos
                                                if ((holesF[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se crea hueco Beside
                                                        //cout<<"Crear hueco beside con Front"<<endl;
                                                        Hwid = holesF[holesId].getWid() - itemsCont[count].wid;
                                                        Hlen = holesF[holesId].getLen();
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesF[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se crea hueco Above
                                                        //cout<<"Crear hueco above con Front"<<endl;
                                                        Hwid = holesF[holesId].getWid();
                                                        Hlen = holesF[holesId].getLen();
                                                        Hhei = holesF[holesId].getHei() - itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesF[holesId].getLen() - itemsCont[count].len) == 0) {  //Se crea hueco Beside
                                                        //cout<<"Elimina hueco front con Front"<<endl;
                                                        holesF.erase(holesF.begin() + holesId);
                                                }
                                                else if ((holesF[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                                        //cout<<"Modifica hueco front con Front"<<endl;
                                                        Hwid = itemsCont[count].wid;
                                                        Hlen = holesF[holesId].getLen() - itemsCont[count].len;
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        // Modificar los valores
                                                        holesF.erase(holesF.begin()+holesId);   
                                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                                }
                                        
                                        } //else if
                                        
                                        //////////////////////////////////////////
                                        // Mostrar huecos resultantes
                                        /*
                                        cout<<"HolesF huecos con Front"<<endl;
                                        for (int i = 0; i < holesF.size(); i++){
                                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                                        }

                                        cout<<"HolesA huecos con Front"<<endl;
                                        for (int i = 0; i < holesA.size(); i++){
                                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                                        }
                                        
                                        cout<<"HolesB huecos con Front"<<endl;
                                        for (int i = 0; i < holesB.size(); i++){
                                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                                        }
                                        */
                                        ////////////////////////////////////////////
                                        holesId++;
                                } //while2
                        
                                holesId = 0; // Se inicializa el contador de huecos
                                // Se recorre segundo el vector de holesA
                                while ((holesId < holesA.size()) && (label_control != true)) {
                                        //cout<<"while de holesA holesId "<<holesId<<endl;
                                        if ((holesA[holesId].getWid() == itemsCont[count].wid) &&
                                            (holesA[holesId].getLen() == itemsCont[count].len) &&
                                            (holesA[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                                //cout<<"Cabe justo en Above"<<endl;
                                                holesA.erase(holesA.begin() + holesId);  // Se elimina el hueco del vector
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 HolesA "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                        }
                                        else if ((holesA[holesId].getWid() >= itemsCont[count].wid) &&
                                            (holesA[holesId].getLen() >= itemsCont[count].len) &&
                                            (holesA[holesId].getHei() >= itemsCont[count].hei)) { // si sobra espacio, se deben crear los nuevos huecos
                                                //cout<<"No cabe justo en Above"<<endl;
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 A "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                                // Calcular huecos
                                                if ((holesA[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se crea hueco Beside
                                                        //cout<<"Crear hueco beside en above"<<endl;
                                                        Hwid = holesA[holesId].getWid() - itemsCont[count].wid;
                                                        Hlen = holesA[holesId].getLen();
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesA[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                                        //cout<<"Crear hueco front en above"<<endl;
                                                        Hwid = itemsCont[count].wid; 
                                                        Hlen = holesA[holesId].getLen() - itemsCont[count].len;
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesA[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se modifica hueco Above
                                                        //cout<<"Modifica hueco above en above"<<endl;
                                                        Hwid = holesA[holesId].getWid();
                                                        Hlen = holesA[holesId].getLen();
                                                        Hhei = holesA[holesId].getHei() - itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        // Modificar los valores
                                                        holesA.erase(holesA.begin()+holesId);   
                                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                                }
                                                else if ((holesA[holesId].getHei() - itemsCont[count].hei) == 0) {  //Se elimina hueco Above
                                                        //cout<<"Elimina hueco above en above"<<endl;
                                                        holesA.erase(holesA.begin() + holesId);
                                                }
                                        } //else if
                                        
                                        // Mostrar huecos resultantes
                                        /*
                                        cout<<"HolesF huecos en above "<<endl;
                                        for (int i = 0; i < holesF.size(); i++){
                                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                                        }

                                        cout<<"HolesA huecos en above"<<endl;
                                        for (int i = 0; i < holesA.size(); i++){
                                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                                        }
                                        
                                        cout<<"HolesB huecos en above"<<endl;
                                        for (int i = 0; i < holesB.size(); i++){
                                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                                        }
                                        */
                                        holesId++;
                                } //while2
                                
                                holesId = 0; // Se inicializa el contador de huecos
                                // Se recorre tercero el vector de holesB
                                while ((holesId < holesB.size()) && (label_control != true)) {
                                        //cout<<"while de holesB holesId "<<holesId<<endl;
                                        if ((holesB[holesId].getWid() == itemsCont[count].wid) &&
                                            (holesB[holesId].getLen() == itemsCont[count].len) &&
                                            (holesB[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                                //cout<<"Cabe justo en Beside"<<endl;
                                                holesB.erase(holesB.begin() + holesId);  // Se elimina el hueco del vector
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 B "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                        }
                                        else if ((holesB[holesId].getWid() >= itemsCont[count].wid) &&
                                            (holesB[holesId].getLen() >= itemsCont[count].len) &&
                                            (holesB[holesId].getHei() >= itemsCont[count].hei)){ // si sobra espacio, se deben crear los nuevos huecos
                                                //cout<<"No cabe justo en Beside"<<endl;
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 B "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                                // Calcular huecos
                                                if ((holesB[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se crea hueco Above
                                                        //cout<<"Crear hueco above en Beside"<<endl;
                                                        Hwid = holesB[holesId].getWid();
                                                        Hlen = holesB[holesId].getLen();
                                                        Hhei = holesB[holesId].getHei() - itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesB[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                                        //cout<<"Crear hueco front en beside"<<endl;
                                                        Hwid = itemsCont[count].wid;
                                                        Hlen = holesB[holesId].getLen() - itemsCont[count].len;
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesB[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se modifica hueco beside
                                                        //cout<<"Modifica hueco beside en beside"<<endl;
                                                        Hwid = holesB[holesId].getWid() - itemsCont[count].wid;
                                                        Hlen = holesB[holesId].getLen();
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        // Modificar los valores
                                                        holesB.erase(holesB.begin()+holesId);   
                                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                                }
                                                else if ((holesB[holesId].getWid() - itemsCont[count].wid) == 0) {  //Se elimina hueco Beside
                                                        //cout<<"Elimina hueco beside en beside"<<endl;
                                                        holesB.erase(holesB.begin() + holesId);
                                                }
                                        } //else if
                                        
                                        // Mostrar huecos resultantes
                                        /*
                                        cout<<"HolesF huecos en beside"<<endl;
                                        for (int i = 0; i < holesF.size(); i++){
                                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                                        }

                                        cout<<"HolesA huecos en beside"<<endl;
                                        for (int i = 0; i < holesA.size(); i++){
                                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                                        }
                                        
                                        cout<<"HolesB huecos en beside"<<endl;
                                        for (int i = 0; i < holesB.size(); i++){
                                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                                        }
                                        */
                                        holesId++;
                                        
                                } //while2

        
                                count++;
                                //cout<<"Valor de count "<<count<<endl;
                                // Ordenar los vectores de menor a mayor volumen
                                sort(holesF.begin(),holesF.end(),cmpVol);
                                // Mostrar huecos resultantes
                                /*cout<<"Orden HolesF huecos "<<endl;
                                for (int i = 0; i < holesF.size(); i++){
                                        cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                                }
                                */
                                sort(holesA.begin(),holesA.end(),cmpVol);
                                /*cout<<"Orden HolesA huecos "<<endl;
                                for (int i = 0; i < holesA.size(); i++){
                                        cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                                }
                                */
                                        
                                sort(holesB.begin(),holesB.end(),cmpVol);
                                /*cout<<"Orden HolesB huecos "<<endl;
                                for (int i = 0; i < holesB.size(); i++){
                                        cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                                }
                                */
                                //sort(holesA.begin(), holesA.end(),cmpVol);
                                label_control = false;  
                        }//while1 externo
                
                } // if
                else {
                        finish = true;
                }
                //cout<<"sale y comienza nuevo nivel"<<endl;
        
                count = 0;
                for (int i = 0; i < itemsCont.size(); i++) {
                        if (itemsCont[i].pack != true) {
                                aux.push_back(itemsCont[i]);
                        }
                        else {
                                Obj1_Volumen += itemsCont[i].wid * itemsCont[i].len * itemsCont[i].hei;
                                //Obj2_Weigth += itemsCont[i].wei;
                                //cout<<"Obj1 "<<Obj1_Volumen<<" Peso "<<Obj2_Weigth<<endl; 
                        }
                }
                /*cout<<"Mostrar las cajas restantes"<<endl;
                for (int i = 0; i < aux.size(); i ++) {
                        cout<<"wid len hei wei"<<" "<<aux[i].wid<<" "<<aux[i].len<<" "<<aux[i].hei<<" "<<aux[i].wei<<endl;
                }
                cout<<"Fin muestra"<<endl;
                */
                itemsCont.clear();
                itemsCont.resize(aux.size());
                copy(aux.begin(),aux.end(),itemsCont.begin());
                aux.clear();
                if (finish)
                        break;
        
        }
        Obj1_Volumen = (Obj1_Volumen * 100)/volSheet;
        setObj(0,Obj1_Volumen);
        setObj(1,Obj2_Weigth);
        holesF.clear();
        holesA.clear();
        holesB.clear();
        
}


// ----------------------------------------------------------
// Empaquetar items dentro de cada nivel
// Cuando se llegue al final del vector de items se cierra
// el nivel actual, y se genera un nuevo nivel
// itemsCont -> vector de piezas ordenadas
// ----------------------------------------------------------
void TDCLP::heuristic_AC() {
        vector<P_Order> aux;
        
        // Variables para controlar las dimensiones de los huecos
        double Hwid;
        double Hlen;
        double Hhei;
        double Hvol;
        
        // Variables para ir acumulando los objetivos del problema
        double Obj1_Volumen, Obj2_Weigth;
        Obj1_Volumen = Obj2_Weigth = 0;   // Valores de los objetivos Obj1->Volumen Obj2->Peso
            
        //sortItems();  // Ordenar las cajas segun tipo de ordenacion aplicada
        fillVect();     // Genera vector con las rotaciones realizadas

        // BORRAR
        
        /*cout<<"Piezas ordenadas "<<endl;
        for (int i = 0; i < totalNumPat; i++) {
                cout<<" wid "<<itemsCont[i].wid<<" len "<<itemsCont[i].len<<" hei "<<itemsCont[i].hei<<" wei "<<itemsCont[i].wei<<" pack "<<itemsCont[i].pack<<endl;
        }
        */

        int W_Sheet_free = widSheet;   // Contador de wid libre en el contenedor
        double LDB;              // LDB es el ancho del nivel
        int count = 0;       // Indice dentro del vector itemsCont
        int holesId = 0;         // Indice dentro de los vectores de huecos
        bool label_control, finish;

        //cout<<"item s"<<itemsCont.size()<<endl;
        // Comprobaciones:
        // 1.- Existen cajas que introducir en el contenedor
        // 2.- Se puede crear un nuevo nivel en el contenedor
        // 3.- Hay espacio en el contenedor
        while ((!itemsCont.empty()) && (W_Sheet_free >= itemsCont[itemsCont.size()-1].wid) && ((widSheet - W_Sheet_free) < widSheet) && (Obj2_Weigth < weiSheet)) {

                //cout<<"Comprobaciones primer while "<<endl;
                //cout<<"W_Sheet_free >= itemsCont[itemsCont.size()-1].wid "<<W_Sheet_free<<" "<<itemsCont[itemsCont.size()-1].wid<<endl;
                //cout<<"widSheet - W_Sheet_free) < widSheet "<<widSheet - W_Sheet_free<<" "<<widSheet<<endl;
                
                //cout<<"Fuera del while que recorre itemsCont "<<count<<""<<Obj2_Weigth<<""<<itemsCont[count].wei<<" "<<Obj2_Weigth + itemsCont[count].wei<<endl;
                // Se recorre el vector de itemsCont en busca de una caja que pueda ser introducida en el contenedor
                //cout<<"entrar "<<itemsCont.size()<<endl;
                finish = false;
                while ( (itemsCont[count].wid > W_Sheet_free) || (Obj2_Weigth + itemsCont[count].wei > weiSheet && count < itemsCont.size()) ) { 
                        count++; // Indica el indice de la primera caja que puede ser introducida en el contenedor, marcando el valor del LDB
                        //cout<<"Dentro del while que recorre itemsCont "<<count<<""<<Obj2_Weigth<<""<<itemsCont[count].wei<<" "<<Obj2_Weigth + itemsCont[count].wei<<endl;
                }
                //cout<<"salir "<<count<<endl;
                if (count < itemsCont.size()) { // Puede o no, ser el ultimo item del itemsCont

                        //cout<<"Se crean los primeros huecos "<<endl;
                        LDB = itemsCont[count].wid;                          // Se establece el wid del nivel
                        W_Sheet_free = W_Sheet_free - LDB;                   // Wid libre en el contenedor para el resto de niveles que se puedan crear
                        itemsCont[count].pack = true;
                        Obj2_Weigth += itemsCont[count].wei;                 // Se acumula el peso con la nueva pieza introducida
                        
                        // Importante:
                        // Siempre que se introduce una pieza para generar un nuevo nivel,
                        // solo se producen los huecos front y above
                        
                        // Codigo nuevo. Teniendo en cuenta los huecos
                        // Calculo de los que huecos que se generan
                        if ((lenSheet - itemsCont[count].len) > 0) { // Si despues de empaquetar la pieza queda hueco en el Front
                                // Se genera el primer hueco del nivel en el vector holesF
                                //cout<<"Se genera hueco front"<<endl;
                                Hwid = LDB;
                                Hlen = lenSheet - itemsCont[count].len;
                                Hhei = itemsCont[count].hei;
                                Hvol = Hwid * Hlen * Hhei;
                                holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Se crea el hueco en el vector holesF
                        }
                        if ((heiSheet - itemsCont[count].hei) > 0 ) {  // Si despues de empaquetar la pieza queda hueco en el Above
                                Hwid = LDB;
                                Hlen = lenSheet;
                                Hhei = heiSheet - itemsCont[count].hei;
                                Hvol = Hwid * Hlen * Hhei;
                                holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                        }
                        count++;
                        holesId = 0;
                        label_control = false;   // Etiqueta de control
                        

                        // Recorrer vectores de huecos
                        /*
                        cout<<"Inicio nivel HolesF"<<endl;
                        for (int i = 0; i < holesF.size(); i++){
                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                        }

                        cout<<"Inicio nivel HolesA"<<endl;
                        for (int i = 0; i < holesA.size(); i++){
                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                        }
                        
                        cout<<"Comienza el while de relleno"<<endl;
                        */
                        
                        while ((count < itemsCont.size()) && (itemsCont[count].wid <= LDB) && ((Obj2_Weigth + itemsCont[count].wei) <= weiSheet)) { // Se rellena el nivel con 
                                                                                                                        // nuevas cajas, que quepan en el nivel
                                //cout<<"Relleno  "<<count<<" "<<Obj2_Weigth<<" "<<itemsCont[count].wei<<" "<<Obj2_Weigth + itemsCont[count].wei<<endl;
                                holesId = 0;
                                // Se recorre primero el vector de holesF
                                while ((holesId < holesF.size()) && (label_control != true)) {
                                        //cout<<"while de holesF holesId "<<holesId<<endl;
                                        if ((holesF[holesId].getWid() == itemsCont[count].wid) &&
                                            (holesF[holesId].getLen() == itemsCont[count].len) &&
                                            (holesF[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                                //cout<<"Cabe justo en Front: se borra hueco Front"<<endl;
                                                holesF.erase(holesF.begin() + holesId);  // Se elimina el hueco del vector
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                        }
                                        else if ((holesF[holesId].getWid() >= itemsCont[count].wid) &&
                                            (holesF[holesId].getLen() >= itemsCont[count].len) &&
                                            (holesF[holesId].getHei() >= itemsCont[count].hei)) { // si sobra espacio, se deben crear los nuevos huecos
                                                //cout<<"No cabe justo en Front"<<endl;
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                                // Calcular huecos
                                                if ((holesF[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se crea hueco Beside
                                                        //cout<<"Crear hueco beside con Front"<<endl;
                                                        Hwid = holesF[holesId].getWid() - itemsCont[count].wid;
                                                        Hlen = itemsCont[count].len;
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesF[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se crea hueco Above
                                                        //cout<<"Crear hueco above con Front"<<endl;
                                                        Hwid = holesF[holesId].getWid();
                                                        Hlen = holesF[holesId].getLen();
                                                        Hhei = holesF[holesId].getHei() - itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesF[holesId].getLen() - itemsCont[count].len) == 0) {  //Se crea hueco Beside
                                                        //cout<<"Elimina hueco front con Front"<<endl;
                                                        holesF.erase(holesF.begin() + holesId);
                                                }
                                                else if ((holesF[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                                        //cout<<"Modifica hueco front con Front"<<endl;
                                                        Hwid = holesF[holesId].getWid();
                                                        Hlen = holesF[holesId].getLen() - itemsCont[count].len;
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        // Modificar los valores
                                                        holesF.erase(holesF.begin()+holesId);   
                                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                                }
                                        
                                        } //else if
                                        
                                        //////////////////////////////////////////
                                        // Mostrar huecos resultantes
                                        /*
                                        cout<<"HolesF huecos con Front"<<endl;
                                        for (int i = 0; i < holesF.size(); i++){
                                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                                        }

                                        cout<<"HolesA huecos con Front"<<endl;
                                        for (int i = 0; i < holesA.size(); i++){
                                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                                        }
                                        
                                        cout<<"HolesB huecos con Front"<<endl;
                                        for (int i = 0; i < holesB.size(); i++){
                                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                                        }
                                        */
                                        ////////////////////////////////////////////
                                        holesId++;
                                } //while2
                        
                                holesId = 0; // Se inicializa el contador de huecos
                                // Se recorre segundo el vector de holesA
                                while ((holesId < holesA.size()) && (label_control != true)) {
                                        //cout<<"while de holesA holesId "<<holesId<<endl;
                                        if ((holesA[holesId].getWid() == itemsCont[count].wid) &&
                                            (holesA[holesId].getLen() == itemsCont[count].len) &&
                                            (holesA[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                                //cout<<"Cabe justo en Above"<<endl;
                                                holesA.erase(holesA.begin() + holesId);  // Se elimina el hueco del vector
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 HolesA "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                        }
                                        else if ((holesA[holesId].getWid() >= itemsCont[count].wid) &&
                                            (holesA[holesId].getLen() >= itemsCont[count].len) &&
                                            (holesA[holesId].getHei() >= itemsCont[count].hei)) { // si sobra espacio, se deben crear los nuevos huecos
                                                //cout<<"No cabe justo en Above"<<endl;
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 A "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                                // Calcular huecos
                                                if ((holesA[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se crea hueco Beside
                                                        //cout<<"Crear hueco beside en above"<<endl;
                                                        Hwid = holesA[holesId].getWid() - itemsCont[count].wid;
                                                        Hlen = itemsCont[count].len;
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesA[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                                        //cout<<"Crear hueco front en above"<<endl;
                                                        Hwid = holesA[holesId].getWid();
                                                        Hlen = holesA[holesId].getLen() - itemsCont[count].len;
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesA[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se modifica hueco Above
                                                        //cout<<"Modifica hueco above en above"<<endl;
                                                        Hwid = holesA[holesId].getWid();
                                                        Hlen = holesA[holesId].getLen();
                                                        Hhei = holesA[holesId].getHei() - itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        // Modificar los valores
                                                        holesA.erase(holesA.begin()+holesId);   
                                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                                }
                                                else if ((holesA[holesId].getHei() - itemsCont[count].hei) == 0) {  //Se elimina hueco Above
                                                        //cout<<"Elimina hueco above en above"<<endl;
                                                        holesA.erase(holesA.begin() + holesId);
                                                }
                                        } //else if
                                        
                                        // Mostrar huecos resultantes
                                        /*
                                        cout<<"HolesF huecos en above "<<endl;
                                        for (int i = 0; i < holesF.size(); i++){
                                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                                        }

                                        cout<<"HolesA huecos en above"<<endl;
                                        for (int i = 0; i < holesA.size(); i++){
                                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                                        }
                                        
                                        cout<<"HolesB huecos en above"<<endl;
                                        for (int i = 0; i < holesB.size(); i++){
                                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                                        }
                                        */
                                        holesId++;
                                } //while2
                                
                                holesId = 0; // Se inicializa el contador de huecos
                                // Se recorre tercero el vector de holesB
                                while ((holesId < holesB.size()) && (label_control != true)) {
                                        //cout<<"while de holesB holesId "<<holesId<<endl;
                                        if ((holesB[holesId].getWid() == itemsCont[count].wid) &&
                                            (holesB[holesId].getLen() == itemsCont[count].len) &&
                                            (holesB[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                                //cout<<"Cabe justo en Beside"<<endl;
                                                holesB.erase(holesB.begin() + holesId);  // Se elimina el hueco del vector
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 B "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                        }
                                        else if ((holesB[holesId].getWid() >= itemsCont[count].wid) &&
                                            (holesB[holesId].getLen() >= itemsCont[count].len) &&
                                            (holesB[holesId].getHei() >= itemsCont[count].hei)){ // si sobra espacio, se deben crear los nuevos huecos
                                                //cout<<"No cabe justo en Beside"<<endl;
                                                itemsCont[count].pack = true;
                                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                                //cout<<"Obj2 B "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                                // Calcular huecos
                                                if ((holesB[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se crea hueco Above
                                                        //cout<<"Crear hueco above en Beside"<<endl;
                                                        Hwid = holesB[holesId].getWid();
                                                        Hlen = holesB[holesId].getLen();
                                                        Hhei = holesB[holesId].getHei() - itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesB[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                                        //cout<<"Crear hueco front en beside"<<endl;
                                                        Hwid = holesB[holesId].getWid();
                                                        Hlen = holesB[holesId].getLen() - itemsCont[count].len;
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesB[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se modifica hueco beside
                                                        //cout<<"Modifica hueco beside en beside"<<endl;
                                                        Hwid = holesB[holesId].getWid() - itemsCont[count].wid;
                                                        Hlen = itemsCont[count].len;
                                                        Hhei = itemsCont[count].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        // Modificar los valores
                                                        holesB.erase(holesB.begin()+holesId);   
                                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                                }
                                                else if ((holesB[holesId].getWid() - itemsCont[count].wid) == 0) {  //Se elimina hueco Beside
                                                        //cout<<"Elimina hueco beside en beside"<<endl;
                                                        holesB.erase(holesB.begin() + holesId);
                                                }
                                        } //else if
                                        
                                        // Mostrar huecos resultantes
                                        /*
                                        cout<<"HolesF huecos en beside"<<endl;
                                        for (int i = 0; i < holesF.size(); i++){
                                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                                        }

                                        cout<<"HolesA huecos en beside"<<endl;
                                        for (int i = 0; i < holesA.size(); i++){
                                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                                        }
                                        
                                        cout<<"HolesB huecos en beside"<<endl;
                                        for (int i = 0; i < holesB.size(); i++){
                                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                                        }
                                        */
                                        holesId++;
                                        
                                } //while2

        
                                count++;
                                //cout<<"Valor de count "<<count<<endl;
                                // Ordenar los vectores de menor a mayor volumen
                                sort(holesF.begin(),holesF.end(),cmpVol);
                                // Mostrar huecos resultantes
                                /*cout<<"Orden HolesF huecos "<<endl;
                                for (int i = 0; i < holesF.size(); i++){
                                        cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                                }
                                */
                                sort(holesA.begin(),holesA.end(),cmpVol);
                                /*cout<<"Orden HolesA huecos "<<endl;
                                for (int i = 0; i < holesA.size(); i++){
                                        cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                                }
                                */
                                        
                                sort(holesB.begin(),holesB.end(),cmpVol);
                                /*cout<<"Orden HolesB huecos "<<endl;
                                for (int i = 0; i < holesB.size(); i++){
                                        cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                                }
                                */
                                //sort(holesA.begin(), holesA.end(),cmpVol);
                                label_control = false;  
                        }//while1 externo
                
                } // if
                else {
                        finish = true;
                }
                //cout<<"sale y comienza nuevo nivel"<<endl;
        
                count = 0;
                for (int i = 0; i < itemsCont.size(); i++) {
                        if (itemsCont[i].pack != true) {
                                aux.push_back(itemsCont[i]);
                        }
                        else {
                                Obj1_Volumen += itemsCont[i].wid * itemsCont[i].len * itemsCont[i].hei;
                                //Obj2_Weigth += itemsCont[i].wei;
                                //cout<<"Obj1 "<<Obj1_Volumen<<" Peso "<<Obj2_Weigth<<endl; 
                        }
                }
                /*cout<<"Mostrar las cajas restantes"<<endl;
                for (int i = 0; i < aux.size(); i ++) {
                        cout<<"wid len hei wei"<<" "<<aux[i].wid<<" "<<aux[i].len<<" "<<aux[i].hei<<" "<<aux[i].wei<<endl;
                }
                cout<<"Fin muestra"<<endl;
                */
                itemsCont.clear();
                itemsCont.resize(aux.size());
                copy(aux.begin(),aux.end(),itemsCont.begin());
                aux.clear();
                if (finish)
                        break;
        
        }
        Obj1_Volumen = (Obj1_Volumen * 100)/volSheet;
        setObj(0,Obj1_Volumen);
        setObj(1,Obj2_Weigth);
        holesF.clear();
        holesA.clear();
        holesB.clear();
        
}


void TDCLP::heuristic_AS() {
        //cout<<"heuristica 2"<<endl;
        // Variables para controlar las dimensiones de los huecos
        double Hwid;
        double Hlen;
        double Hhei;
        double Hvol;
        
        // Variables para ir acumulando los objetivos del problema
        double Obj1_Volumen, Obj2_Weigth;
        Obj1_Volumen = Obj2_Weigth = 0;   // Valores de los objetivos Obj1->Volumen Obj2->Peso
            
        fillVect();     // Genera vector con las rotaciones realizadas

        // BORRAR
        
        /*cout<<"Piezas ordenadas "<<endl;
        for (int i = 0; i < totalNumPat; i++) {
                cout<<" wid "<<itemsCont[i].wid<<" len "<<itemsCont[i].len<<" hei "<<itemsCont[i].hei<<" wei "<<itemsCont[i].wei<<" vol "<<itemsCont[i].vol<< endl;
        }
        */

        int W_Sheet_free = widSheet;   // Contador de wid libre en el contenedor
        double LDB;              // LDB es el ancho del nivel
        int count = 0;       // Indice dentro del vector itemsCont
        int holesId = 0;         // Indice dentro de los vectores de huecos
        bool label_control;

        while ((count < itemsCont.size()) && ((Obj2_Weigth + itemsCont[count].wei) <= weiSheet) && ((Obj1_Volumen + itemsCont[count].vol) <= volSheet)){    //7200.0)){

        //while ((count < itemsCont.size()) && ((Obj1_Volumen + itemsCont[count].vol) <= volSheet)){    //7200.0)){

                
                //((Obj1_Volumen + itemsCont[count].vol) <= 24486000.0) && ((Obj2_Weigth + itemsCont[count].wid) <= 7200.0)){
                //cout<<"Entra en bucle superior con count a "<<count<<" Obj1 "<<Obj1_Volumen<<" Obj2 "<< Obj2_Weigth <<endl;
                //cout<<"Vol " <<itemsCont[count].vol<<" "<<Obj1_Volumen + itemsCont[count].vol<<endl;
                //cout<<" Peso " <<itemsCont[count].wei<<" "<<Obj2_Weigth + itemsCont[count].wei<<endl;

                // Se recorre primero el vector de holesF
                while ((holesId < holesF.size()) && (label_control != true)) {
                        //cout<<"while de holesF holesId "<<holesId<<endl;
                        if ((holesF[holesId].getWid() == itemsCont[count].wid) &&
                            (holesF[holesId].getLen() == itemsCont[count].len) &&
                            (holesF[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                //cout<<"Cabe justo en Front: se borra hueco Front"<<endl;
                                holesF.erase(holesF.begin() + holesId);  // Se elimina el hueco del vector
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;         // Se suma el volumen
                                //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2 "<<Obj2_Weigth<<endl;
                                label_control = true;
                        }
                        else if ((holesF[holesId].getWid() >= itemsCont[count].wid) &&
                            (holesF[holesId].getLen() >= itemsCont[count].len) &&
                            (holesF[holesId].getHei() >= itemsCont[count].hei)) { // si sobra espacio, se deben crear los nuevos huecos
                                //cout<<"No cabe justo en Front"<<endl;
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;         // Se suma el volumen
                                //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2 "<<Obj2_Weigth<<endl;
                                label_control = true;
                                // Calcular huecos
                                if ((holesF[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se crea hueco Beside
                                        //cout<<"Crear hueco beside con Front"<<endl;
                                        Hwid = holesF[holesId].getWid() - itemsCont[count].wid;
                                        Hlen = itemsCont[count].len;
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesF[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se crea hueco Above
                                        //cout<<"Crear hueco above con Front"<<endl;
                                        Hwid = holesF[holesId].getWid();
                                        Hlen = holesF[holesId].getLen();
                                        Hhei = holesF[holesId].getHei() - itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesF[holesId].getLen() - itemsCont[count].len) == 0) {  //Se crea hueco Beside
                                        //cout<<"Elimina hueco front con Front"<<endl;
                                        holesF.erase(holesF.begin() + holesId);
                                }
                                else if ((holesF[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                        //cout<<"Modifica hueco front con Front"<<endl;
                                        Hwid = holesF[holesId].getWid();
                                        Hlen = holesF[holesId].getLen() - itemsCont[count].len;
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        // Modificar los valores
                                        holesF.erase(holesF.begin()+holesId);   
                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                }
                        
                        } //else if
                                
                        //////////////////////////////////////////
                        // Mostrar huecos resultantes
/*              if (itemsCont[count].pack == true) {            
                        cout<<"HolesF huecos con Front"<<endl;
                        for (int i = 0; i < holesF.size(); i++){
                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                        }
        
                        cout<<"HolesA huecos con Front"<<endl;
                        for (int i = 0; i < holesA.size(); i++){
                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                        }
                                
                        cout<<"HolesB huecos con Front"<<endl;
                        for (int i = 0; i < holesB.size(); i++){
                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                        }
                        
                }*/
                        ////////////////////////////////////////////
                        holesId++;
                } //whileHolesF
        
                holesId = 0; // Se inicializa el contador de huecos
                // Se recorre segundo el vector de holesA
                while ((holesId < holesA.size()) && (label_control != true)) {
                        //cout<<"while de holesA holesId "<<holesId<<endl;
                        if ((holesA[holesId].getWid() == itemsCont[count].wid) &&
                            (holesA[holesId].getLen() == itemsCont[count].len) &&
                            (holesA[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                //cout<<"Cabe justo en Above"<<endl;
                                holesA.erase(holesA.begin() + holesId);  // Se elimina el hueco del vector
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;           // Se suma el volumen
                                //cout<<"Obj1 "<<Obj1_Volumen<< " Obj2 "<<Obj2_Weigth<<endl;
                                label_control = true;
                        }
                        else if ((holesA[holesId].getWid() >= itemsCont[count].wid) &&
                            (holesA[holesId].getLen() >= itemsCont[count].len) &&
                            (holesA[holesId].getHei() >= itemsCont[count].hei)) { // si sobra espacio, se deben crear los nuevos huecos
                                //cout<<"No cabe justo en Above"<<endl;
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;           // Se suma el volumen
                                //cout<<"Obj1 "<<Obj1_Volumen<< " Obj2 "<<Obj2_Weigth<<endl;
                                label_control = true;
                                // Calcular huecos
                                if ((holesA[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se crea hueco Beside
                                        //cout<<"Crear hueco beside en above"<<endl;
                                        Hwid = holesA[holesId].getWid() - itemsCont[count].wid;
                                        Hlen = itemsCont[count].len;
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesA[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                        //cout<<"Crear hueco front en above"<<endl;
                                        Hwid = holesA[holesId].getWid();
                                        Hlen = holesA[holesId].getLen() - itemsCont[count].len;
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesA[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se modifica hueco Above
                                        //cout<<"Modifica hueco above en above"<<endl;
                                        Hwid = holesA[holesId].getWid();
                                        Hlen = holesA[holesId].getLen();
                                        Hhei = holesA[holesId].getHei() - itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        // Modificar los valores
                                        holesA.erase(holesA.begin()+holesId);   
                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                }
                                else if ((holesA[holesId].getHei() - itemsCont[count].hei) == 0) {  //Se elimina hueco Above
                                        //cout<<"Elimina hueco above en above"<<endl;
                                        holesA.erase(holesA.begin() + holesId);
                                }
                        } //else if
                                        
                        // Mostrar huecos resultantes
        /*      if (itemsCont[count].pack == true) {    
                        cout<<"HolesF huecos en above "<<endl;
                        for (int i = 0; i < holesF.size(); i++){
                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                        }
                        cout<<"HolesA huecos en above"<<endl;
                        for (int i = 0; i < holesA.size(); i++){
                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                        }
                        cout<<"HolesB huecos en above"<<endl;
                        for (int i = 0; i < holesB.size(); i++){
                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                        }
                }*/
                        holesId++;
                } //whileAbove
                        
                
                holesId = 0; // Se inicializa el contador de huecos
                // Se recorre tercero el vector de holesB
                while ((holesId < holesB.size()) && (label_control != true)) {
                        //cout<<"while de holesB holesId "<<holesId<<endl;
                        if ((holesB[holesId].getWid() == itemsCont[count].wid) &&
                            (holesB[holesId].getLen() == itemsCont[count].len) &&
                            (holesB[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                //cout<<"Cabe justo en Beside"<<endl;
                                holesB.erase(holesB.begin() + holesId);  // Se elimina el hueco del vector
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;
                                //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2  "<<Obj2_Weigth<<endl;
                                label_control = true;
                        }
                        else if ((holesB[holesId].getWid() >= itemsCont[count].wid) &&
                            (holesB[holesId].getLen() >= itemsCont[count].len) &&
                            (holesB[holesId].getHei() >= itemsCont[count].hei)){ // si sobra espacio, se deben crear los nuevos huecos
                                //cout<<"No cabe justo en Beside"<<endl;
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;
                                //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2  "<<Obj2_Weigth<<endl;
                                label_control = true;
                                // Calcular huecos
                                if ((holesB[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se crea hueco Above
                                        //cout<<"Crear hueco above en Beside"<<endl;
                                        Hwid = holesB[holesId].getWid();
                                        Hlen = holesB[holesId].getLen();
                                        Hhei = holesB[holesId].getHei() - itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesB[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                        //cout<<"Crear hueco front en beside"<<endl;
                                        Hwid = holesB[holesId].getWid();
                                        Hlen = holesB[holesId].getLen() - itemsCont[count].len;
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesB[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se modifica hueco beside
                                        //cout<<"Modifica hueco beside en beside"<<endl;
                                        Hwid = holesB[holesId].getWid() - itemsCont[count].wid;
                                        Hlen = itemsCont[count].len;
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        // Modificar los valores
                                        holesB.erase(holesB.begin()+holesId);   
                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                }
                                else if ((holesB[holesId].getWid() - itemsCont[count].wid) == 0) {  //Se elimina hueco Beside
                                        //cout<<"Elimina hueco beside en beside"<<endl;
                                        holesB.erase(holesB.begin() + holesId);
                                }
                        } //else if
                                        
                        // Mostrar huecos resultantes
        /*      if (itemsCont[count].pack == true) {    
                        cout<<"HolesF huecos en beside"<<endl;
                        for (int i = 0; i < holesF.size(); i++){
                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                        }
                        cout<<"HolesA huecos en beside"<<endl;
                        for (int i = 0; i < holesA.size(); i++){
                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                        }
                        cout<<"HolesB huecos en beside"<<endl;
                        for (int i = 0; i < holesB.size(); i++){
                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                        }
                }*/
                        holesId++;
                                        
                } //whileBeside
                
                //cout<<"parte nueva"<<endl;
                // Parte nueva de la heuristica
                if ((itemsCont[count].wid <= W_Sheet_free) && (label_control != true)){
                        //cout<<"Se crean los primeros huecos del nivel actual"<<endl;
                        LDB = itemsCont[count].wid;                          // Se establece el wid del nivel
                        W_Sheet_free = W_Sheet_free - LDB;                   // Wid libre en el contenedor para el resto de niveles que se puedan crear
                        itemsCont[count].pack = true;
                        Obj2_Weigth += itemsCont[count].wei;                 // Se acumula el peso con la nueva pieza introducida
                        Obj1_Volumen += itemsCont[count].vol;
                
                        // Importante:
                        // Siempre que se introduce una pieza para generar un nuevo nivel,
                        // solo se producen los huecos front y above
                        
                        // Calculo de los que huecos que se generan
                        if ((lenSheet - itemsCont[count].len) > 0) { // Si despues de empaquetar la pieza queda hueco en el Front
                                // Se genera el primer hueco del nivel en el vector holesF
                                //cout<<"Se genera hueco front del nivel"<<endl;
                                Hwid = LDB;
                                Hlen = lenSheet - itemsCont[count].len;
                                Hhei = itemsCont[count].hei;
                                Hvol = Hwid * Hlen * Hhei;
                                holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Se crea el hueco en el vector holesF
                        }
                        if ((heiSheet - itemsCont[count].hei) > 0 ) {  // Si despues de empaquetar la pieza queda hueco en el Above
                                //cout<<"se genera hueco above del nivel"<<endl;
                                Hwid = LDB;
                                Hlen = lenSheet;
                                Hhei = heiSheet - itemsCont[count].hei;
                                Hvol = Hwid * Hlen * Hhei;
                                holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                        }
                // Recorrer vectores de huecos
                /*cout<<"Inicio nivel HolesF"<<endl;
                for (int i = 0; i < holesF.size(); i++){
                        cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                }
                cout<<"Inicio nivel HolesA"<<endl;
                for (int i = 0; i < holesA.size(); i++){
                        cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                }*/
        
                }
                count++;
                holesId = 0;
                label_control = false;   // Etiqueta de control
                        
        
                        
                // Ordenar los vectores de menor a mayor volumen
                sort(holesF.begin(),holesF.end(),cmpVol);
                // Mostrar huecos resultantes
                /*cout<<"Orden HolesF huecos "<<endl;
                for (int i = 0; i < holesF.size(); i++){
                        cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                }*/
                
                sort(holesA.begin(),holesA.end(),cmpVol);
                /*cout<<"Orden HolesA huecos "<<endl;
                for (int i = 0; i < holesA.size(); i++){
                        cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                }*/
                
                sort(holesB.begin(),holesB.end(),cmpVol);
                /*cout<<"Orden HolesB huecos "<<endl;
                for (int i = 0; i < holesB.size(); i++){
                        cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                }*/
        
        }
        int sum = 0;
        for (int p = 0; p < itemsCont.size(); p++){
                if (itemsCont[p].pack == true){
                        sum = sum + 1;
                }
        }
        //cout<<"Cajas totales empaquetadas "<<sum<<endl;
        //cout<<" Objetivos "<<Obj1_Volumen<< " "<<Obj2_Weigth<<endl;
        Obj1_Volumen = (Obj1_Volumen * 100)/volSheet;
        setObj(0,Obj1_Volumen);
        setObj(1,Obj2_Weigth);
        holesF.clear();
        holesA.clear();
        holesB.clear();

}


//----------------------------------------------------------------------------
// Heuristica donde se contempla la inserci�n de piezas considerando su peso
// ---------------------------------------------------------------------------
void TDCLP::heuristic3() {
        //cout<<"heuristica 4 - Pesos"<<endl;
        
        // Variables para controlar las dimensiones de los huecos
        double Hwid;
        double Hlen;
        double Hhei;
        double Hvol;
        
        // Variables para ir acumulando los objetivos del problema
        double Obj1_Volumen, Obj2_Weigth;
        Obj1_Volumen = Obj2_Weigth = 0;   // Valores de los objetivos Obj1->Volumen Obj2->Peso
            
        fillVect();     // Genera vector con las rotaciones realizadas
        
        int W_Sheet_free = widSheet;   // Contador de wid libre en el contenedor
        double LDB, weight_max;              // LDB es el ancho del nivel
        int count = 0;
        int id = 0;       // Indice dentro del vector itemsCont
        int holesId = 0;         // Indice dentro de los vectores de huecos
        bool label_control, label;
        int contador = 0;

        while (((Obj2_Weigth + itemsCont[count].wei) <= weiSheet) && ((Obj1_Volumen + itemsCont[count].vol) <= volSheet) && 
                        (contador < itemsCont.size())){    //7200.0)){
                contador++;
                if (itemsCont[count].pack != true) {
                        //cout<<"Entra en bucle superior con count a "<<count<<" Obj1 "<<Obj1_Volumen<<" Obj2 "<< Obj2_Weigth <<endl;
                        //cout<<"Vol " <<itemsCont[count].vol<<" "<<Obj1_Volumen + itemsCont[count].vol<<endl;
                        //cout<<" Peso " <<itemsCont[count].wei<<" "<<Obj2_Weigth + itemsCont[count].wei<<endl;

                        // Se recorre primero el vector de holesF
                        while ((holesId < holesF.size()) && (label_control != true)) {
                                //cout<<"while de holesF holesId "<<holesId<<endl;
                                id = itemsCont[count].index;
                                weight_max = itemsCont[count].wei;
                                for (int p = count; p < itemsCont.size(); p++) {
                                //      cout<<"Dentro del for exacto holesF "<<endl;
                                        if ((holesF[holesId].getWid() == itemsCont[p].wid) &&
                                            (holesF[holesId].getLen() == itemsCont[p].len) &&
                                            (holesF[holesId].getHei() == itemsCont[p].hei) &&
                                            (itemsCont[p].pack != true) &&
                                            (weight_max < itemsCont[p].wei) && (Obj2_Weigth + itemsCont[p].wei <= weiSheet) &&
                                            (Obj1_Volumen + itemsCont[p].vol <= volSheet)) { // si cabe justo
                                                weight_max = itemsCont[p].wei;
                                                id = itemsCont[p].index;
                                                label = true;
                                        }
                                }
                                if (label == true) {
                                
                                //cout<<"El de mayor peso dentro holesF justo wei id "<<weight_max<<" "<<id<<endl;
                                        //cout<<"Cabe justo en Front: se borra hueco Front"<<endl;
                                        holesF.erase(holesF.begin() + holesId);  // Se elimina el hueco del vector
                                        itemsCont[id].pack = true;
                                        Obj2_Weigth += itemsCont[id].wei;          // Se suma el peso
                                        Obj1_Volumen += itemsCont[id].vol;            // Se suma el volumen
                                        //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2 "<<Obj2_Weigth<<endl;
                                        label_control = true;
                                }
                                else if (label != true) {                       
                                        id = itemsCont[count].index;
                                        weight_max = itemsCont[count].wei;
                                        for (int p = count; p < itemsCont.size(); p++) {
                                                //cout<<"Dentro del for no exacto holesF "<<endl;
                                                if ((holesF[holesId].getWid() >= itemsCont[p].wid) &&
                                                    (holesF[holesId].getLen() >= itemsCont[p].len) &&
                                                    (holesF[holesId].getHei() >= itemsCont[p].hei) &&
                                                    (itemsCont[p].pack != true) &&
                                                    (weight_max < itemsCont[p].wei) && 
                                                    (Obj2_Weigth + itemsCont[p].wei <= weiSheet) && (Obj1_Volumen + itemsCont[p].vol <= volSheet)) { 
                                                        // si cabe justo
                                                        weight_max = itemsCont[p].wei;
                                                        id = itemsCont[p].index;
                                                        label = true;
                                                }
                                        }
                                        if (label == true) {
                                                //cout<<"El de mayor peso dentro holesF no justo wei id "<<weight_max<<" "<<id<<endl;
                                                //cout<<"No cabe justo en Front"<<endl;
                                                itemsCont[id].pack = true;
                                                Obj2_Weigth += itemsCont[id].wei;          // Se suma el peso
                                                Obj1_Volumen += itemsCont[id].vol;            // Se suma el volumen
                                                //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2 "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                                // Calcular huecos
                                                if ((holesF[holesId].getWid() - itemsCont[id].wid) > 0) {  //Se crea hueco Beside
                                                        //cout<<"Crear hueco beside con Front"<<endl;
                                                        Hwid = holesF[holesId].getWid() - itemsCont[id].wid;
                                                        Hlen = itemsCont[id].len;
                                                        Hhei = itemsCont[id].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesF[holesId].getHei() - itemsCont[id].hei) > 0) {  //Se crea hueco Above
                                                        //cout<<"Crear hueco above con Front"<<endl;
                                                        Hwid = holesF[holesId].getWid();
                                                        Hlen = holesF[holesId].getLen();
                                                        Hhei = holesF[holesId].getHei() - itemsCont[id].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesF[holesId].getLen() - itemsCont[id].len) == 0) {  //Se crea hueco Beside
                                                        //cout<<"Elimina hueco front con Front"<<endl;
                                                        holesF.erase(holesF.begin() + holesId);
                                                }
                                                else if ((holesF[holesId].getLen() - itemsCont[id].len) > 0) {  //Se crea hueco Front
                                                        //cout<<"Modifica hueco front con Front"<<endl;
                                                        Hwid = holesF[holesId].getWid();
                                                        Hlen = holesF[holesId].getLen() - itemsCont[id].len;
                                                        Hhei = itemsCont[id].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        // Modificar los valores
                                                        holesF.erase(holesF.begin()+holesId);   
                                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                                }
                                        }
                                } //else if
                                holesId++;
                        } //while HolesF                        

                        // Se recorre primero el vector de holesA
                        holesId = 0;
                        while ((holesId < holesA.size()) && (label_control != true)) {
                                //cout<<"while de holesA holesId "<<holesId<<endl;
                                id = itemsCont[count].index;
                                weight_max = itemsCont[count].wei;
                                for (int p = count; p < itemsCont.size(); p++) {
                                        //cout<<"Dentro del for exacto holesA "<<endl;
                                        if ((holesA[holesId].getWid() == itemsCont[p].wid) &&
                                            (holesA[holesId].getLen() == itemsCont[p].len) &&
                                            (holesA[holesId].getHei() == itemsCont[p].hei) &&
                                            (itemsCont[p].pack != true) &&
                                            (weight_max < itemsCont[p].wei) &&
                                            (Obj2_Weigth + itemsCont[p].wei <= weiSheet) && (Obj1_Volumen + itemsCont[p].vol <= volSheet)) { 
                                                // si cabe justo
                                                weight_max = itemsCont[p].wei;
                                                id = itemsCont[p].index;
                                                label = true;
                                        }
                                }
                                if (label == true) {
                                
                                //cout<<"El de mayor peso dentro holesA justo wei id "<<weight_max<<" "<<id<<endl;
                                        //cout<<"Cabe justo en Front: se borra hueco Front"<<endl;
                                        holesA.erase(holesA.begin() + holesId);  // Se elimina el hueco del vector
                                        itemsCont[id].pack = true;
                                        Obj2_Weigth += itemsCont[id].wei;          // Se suma el peso
                                        Obj1_Volumen += itemsCont[id].vol;            // Se suma el volumen
                                        //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2 "<<Obj2_Weigth<<endl;
                                        label_control = true;
                                }
                                else if (label != true) {
                                        id = itemsCont[count].index;
                                        weight_max = itemsCont[count].wei;
                                        for (int p = count; p < itemsCont.size(); p++) {
                                                //cout<<"Dentro del for no exacto holesF "<<endl;
                                                if ((holesA[holesId].getWid() >= itemsCont[p].wid) &&
                                                    (holesA[holesId].getLen() >= itemsCont[p].len) &&
                                                    (holesA[holesId].getHei() >= itemsCont[p].hei) &&
                                                    (itemsCont[p].pack != true) &&
                                                    (weight_max < itemsCont[p].wei) &&
                                                    (Obj2_Weigth + itemsCont[p].wei <= weiSheet) && (Obj1_Volumen + itemsCont[p].vol <= volSheet)) { 
                                                        // si cabe justo
                                                        weight_max = itemsCont[p].wei;
                                                        id = itemsCont[p].index;
                                                        label = true;
                                                }
                                        }
                                        if (label == true) {
                                                //cout<<"El de mayor peso dentro holesA no justo wei id "<<weight_max<<" "<<id<<endl;
                                                //cout<<"No cabe justo en Front"<<endl;
                                                itemsCont[id].pack = true;
                                                Obj2_Weigth += itemsCont[id].wei;          // Se suma el peso
                                                Obj1_Volumen += itemsCont[id].vol;            // Se suma el volumen
                                                //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2 "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                                // Calcular huecos
                                                if ((holesA[holesId].getWid() - itemsCont[id].wid) > 0) {  //Se crea hueco Beside
                                                        //cout<<"Crear hueco beside con Front"<<endl;
                                                        Hwid = holesA[holesId].getWid() - itemsCont[id].wid;
                                                        Hlen = itemsCont[id].len;
                                                        Hhei = itemsCont[id].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesA[holesId].getHei() - itemsCont[id].hei) > 0) {  //Se crea hueco Above
                                                        //cout<<"Crear hueco above con Front"<<endl;
                                                        Hwid = holesA[holesId].getWid();
                                                        Hlen = holesA[holesId].getLen();
                                                        Hhei = holesA[holesId].getHei() - itemsCont[id].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesA.erase(holesA.begin() + holesId);
                                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesA[holesId].getLen() - itemsCont[id].len) > 0) {  //Se crea hueco Beside
                                                        Hwid = holesA[holesId].getWid();
                                                        Hlen = holesA[holesId].getLen() - itemsCont[id].len;
                                                        Hhei = itemsCont[id].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        // Modificar los valores
                                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                                }
                                                else if ((holesA[holesId].getHei() - itemsCont[id].hei) == 0) {  //Se crea hueco Front
                                                        holesA.erase(holesA.begin() + holesId);
                                                        //cout<<"Modifica hueco front con Front"<<endl;
                                                }
                                        }
                                } //else if
                                holesId++;
                        } //while HolesA                        
                        // Se recorre primero el vector de holesB
                        holesId = 0;
                        while ((holesId < holesB.size()) && (label_control != true)) {
                                //cout<<"while de holesB holesId "<<holesId<<endl;
                                id = itemsCont[count].index;
                                weight_max = itemsCont[count].wei;
                                for (int p = count; p < itemsCont.size(); p++) {
                                        //cout<<"Dentro del for exacto holesF "<<endl;
                                        if ((holesB[holesId].getWid() == itemsCont[p].wid) &&
                                            (holesB[holesId].getLen() == itemsCont[p].len) &&
                                            (holesB[holesId].getHei() == itemsCont[p].hei) &&
                                            (itemsCont[p].pack != true) &&
                                            (weight_max < itemsCont[p].wei) &&
                                            (Obj2_Weigth + itemsCont[p].wei <= weiSheet) && (Obj1_Volumen + itemsCont[p].vol <= volSheet)) { 
                                                // si cabe justo
                                                weight_max = itemsCont[p].wei;
                                                id = itemsCont[p].index;
                                                label = true;
                                        }
                                }
                                if (label == true) {
                                
                                //cout<<"El de mayor peso dentro holesB justo wei id "<<weight_max<<" "<<id<<endl;
                                        holesB.erase(holesB.begin() + holesId);  // Se elimina el hueco del vector
                                        itemsCont[id].pack = true;
                                        Obj2_Weigth += itemsCont[id].wei;          // Se suma el peso
                                        Obj1_Volumen += itemsCont[id].vol;            // Se suma el volumen
                                        //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2 "<<Obj2_Weigth<<endl;
                                        label_control = true;
                                }
                                else if (label != true) {
                                        id = itemsCont[count].index;
                                        weight_max = itemsCont[count].wei;
                                        for (int p = count; p < itemsCont.size(); p++) {
                                                //cout<<"Dentro del for no exacto holesB "<<endl;
                                                if ((holesB[holesId].getWid() >= itemsCont[p].wid) &&
                                                    (holesB[holesId].getLen() >= itemsCont[p].len) &&
                                                    (holesB[holesId].getHei() >= itemsCont[p].hei) &&
                                                    (itemsCont[p].pack != true) &&
                                                    (weight_max < itemsCont[p].wei) &&
                                                    (Obj2_Weigth + itemsCont[p].wei <= weiSheet) && (Obj1_Volumen + itemsCont[p].vol <= volSheet)) { 
                                                        // si cabe justo
                                                        weight_max = itemsCont[p].wei;
                                                        id = itemsCont[p].index;
                                                        label = true;
                                                }
                                        }
                                        if (label == true) {
                                                //cout<<"El de mayor peso dentro holesB no justo wei id "<<weight_max<<" "<<id<<endl;
                                                //cout<<"No cabe justo en Front"<<endl;
                                                itemsCont[id].pack = true;
                                                Obj2_Weigth += itemsCont[id].wei;          // Se suma el peso
                                                Obj1_Volumen += itemsCont[id].vol;            // Se suma el volumen
                                                //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2 "<<Obj2_Weigth<<endl;
                                                label_control = true;
                                                // Calcular huecos
                                                if ((holesB[holesId].getWid() - itemsCont[id].wid) > 0) {  //Se crea hueco Beside
                                                        //cout<<"Crear hueco beside con Front"<<endl;
                                                        Hwid = holesB[holesId].getWid() - itemsCont[id].wid;
                                                        Hlen = itemsCont[id].len;
                                                        Hhei = itemsCont[id].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesB.erase(holesB.begin() + holesId);
                                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesB[holesId].getHei() - itemsCont[id].hei) > 0) {  //Se crea hueco Above
                                                        //cout<<"Crear hueco above con Front"<<endl;
                                                        Hwid = holesB[holesId].getWid();
                                                        Hlen = holesB[holesId].getLen();
                                                        Hhei = holesB[holesId].getHei() - itemsCont[id].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                                }
                                                if ((holesB[holesId].getLen() - itemsCont[id].len) > 0) {  //Se crea hueco Beside
                                                        Hwid = holesB[holesId].getWid();
                                                        Hlen = holesB[holesId].getLen() - itemsCont[id].len;
                                                        Hhei = itemsCont[id].hei;
                                                        Hvol = Hwid * Hlen * Hhei;
                                                        // Modificar los valores
                                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                                }
                                                else if ((holesB[holesId].getWid() - itemsCont[id].wid) == 0) {  //Se crea hueco Front
                                                        holesB.erase(holesB.begin() + holesId);
                                                        //cout<<"Modifica hueco front con Front"<<endl;
                                                }
                                        }
                                } //else if
                                holesId++;
                        } //while HolesB                        



                                        
                //cout<<"parte nueva"<<endl;
                // Parte nueva de la heuristica
                if ((itemsCont[count].wid <= W_Sheet_free) && (label_control != true)){
                        //cout<<"Se crean los primeros huecos del nivel actual"<<endl;
                        LDB = itemsCont[count].wid;                     // Se establece el wid del nivel
                        W_Sheet_free = W_Sheet_free - LDB;              // Wid libre en el contenedor para el resto de niveles que se puedan crear
                        itemsCont[count].pack = true;
                        Obj2_Weigth += itemsCont[count].wei;                 // Se acumula el peso con la nueva pieza introducida
                        Obj1_Volumen += itemsCont[count].vol;
                        label = true;           
                        // Importante:
                        // Siempre que se introduce una pieza para generar un nuevo nivel,
                        // solo se producen los huecos front y above
                        
                        // Calculo de los que huecos que se generan
                        if ((lenSheet - itemsCont[count].len) > 0) { // Si despues de empaquetar la pieza queda hueco en el Front
                                // Se genera el primer hueco del nivel en el vector holesF
                                //cout<<"Se genera hueco front del nivel"<<endl;
                                Hwid = LDB;
                                Hlen = lenSheet - itemsCont[count].len;
                                Hhei = itemsCont[count].hei;
                                Hvol = Hwid * Hlen * Hhei;
                                holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Se crea el hueco en el vector holesF
                        }
                        if ((heiSheet - itemsCont[count].hei) > 0 ) {  // Si despues de empaquetar la pieza queda hueco en el Above
                                //cout<<"se genera hueco above del nivel"<<endl;
                                Hwid = LDB;
                                Hlen = lenSheet;
                                Hhei = heiSheet - itemsCont[count].hei;
                                Hvol = Hwid * Hlen * Hhei;
                                holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                        }
                // Recorrer vectores de huecos
                /*cout<<"Inicio nivel HolesF"<<endl;
                for (int i = 0; i < holesF.size(); i++){
                        cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                }
                //cout<<"Inicio nivel HolesA"<<endl;
                for (int i = 0; i < holesA.size(); i++){
                        cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                }*/
        
                }
                
                if (itemsCont[count].pack == true) {
                        count++;
                        //cout<<"Valor de count "<<count<<endl;
                }
                if ((itemsCont[count].pack != true) && (label != true)){
                        count++;
                        //cout<<"No se pudo introducir count es "<<count<<endl;
                }
                holesId = 0;
                label_control = label = false;   // Etiqueta de control
                        
        
                        
                // Ordenar los vectores de menor a mayor volumen
                sort(holesF.begin(),holesF.end(),cmpVol);
                // Mostrar huecos resultantes
                /*cout<<"Orden HolesF huecos "<<endl;
                for (int i = 0; i < holesF.size(); i++){
                        cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                }
                */
                sort(holesA.begin(),holesA.end(),cmpVol);
                /*cout<<"Orden HolesA huecos "<<endl;
                for (int i = 0; i < holesA.size(); i++){
                        cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                }
                */
                sort(holesB.begin(),holesB.end(),cmpVol);
                /*cout<<"Orden HolesB huecos "<<endl;
                for (int i = 0; i < holesB.size(); i++){
                        cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                }
        */
        }
        }
        /*int sum = 0;
        for (int p = 0; p < itemsCont.size(); p++){
                if (itemsCont[p].pack == true){
                        sum = sum + 1;
                }
        }
        cout<<"Cajas totales empaquetadas "<<sum<<endl;
        cout<<" Objetivos "<<Obj1_Volumen<< " "<<Obj2_Weigth<<endl;
        */
        Obj1_Volumen = (Obj1_Volumen * 100)/volSheet;
        setObj(0,Obj1_Volumen);
        setObj(1,Obj2_Weigth);
        holesF.clear();
        holesA.clear();
        holesB.clear();


        
}


void TDCLP::heuristic_LS() {
        //cout<<"heuristica 2"<<endl;
        // Variables para controlar las dimensiones de los huecos
        double Hwid;
        double Hlen;
        double Hhei;
        double Hvol;
        
        // Variables para ir acumulando los objetivos del problema
        double Obj1_Volumen, Obj2_Weigth;
        Obj1_Volumen = Obj2_Weigth = 0;   // Valores de los objetivos Obj1->Volumen Obj2->Peso
            
        fillVect();     // Genera vector con las rotaciones realizadas

        // BORRAR
        
        /*cout<<"Piezas ordenadas "<<endl;
        for (int i = 0; i < totalNumPat; i++) {
                cout<<" wid "<<itemsCont[i].wid<<" len "<<itemsCont[i].len<<" hei "<<itemsCont[i].hei<<" wei "<<itemsCont[i].wei<<" vol "<<itemsCont[i].vol<< endl;
        }*/
        

        int L_Sheet_free = lenSheet;   // Contador de len libre en el contenedor
        double LDB;              // LDB es el ancho del nivel
        int count = 0;       // Indice dentro del vector itemsCont
        int holesId = 0;         // Indice dentro de los vectores de huecos
        bool label_control;

        while ((count < itemsCont.size()) && ((Obj2_Weigth + itemsCont[count].wei) <= weiSheet) && ((Obj1_Volumen + itemsCont[count].vol) <= volSheet)){    //7200.0)){

                //((Obj1_Volumen + itemsCont[count].vol) <= 24486000.0) && ((Obj2_Weigth + itemsCont[count].wid) <= 7200.0)){
                //cout<<"Entra en bucle superior con count a "<<count<<" Obj1 "<<Obj1_Volumen<<" Obj2 "<< Obj2_Weigth <<endl;
                //cout<<"Vol " <<itemsCont[count].vol<<" "<<Obj1_Volumen + itemsCont[count].vol<<endl;
                //cout<<" Peso " <<itemsCont[count].wei<<" "<<Obj2_Weigth + itemsCont[count].wei<<endl;

                // Se recorre primero el vector de holesF
                while ((holesId < holesF.size()) && (label_control != true)) {
                        //cout<<"while de holesF holesId "<<holesId<<endl;
                        if ((holesF[holesId].getWid() == itemsCont[count].wid) &&
                            (holesF[holesId].getLen() == itemsCont[count].len) &&
                            (holesF[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                //cout<<"Cabe justo en Front: se borra hueco Front"<<endl;
                                holesF.erase(holesF.begin() + holesId);  // Se elimina el hueco del vector
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;         // Se suma el volumen
                                //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2 "<<Obj2_Weigth<<endl;
                                label_control = true;
                        }
                        else if ((holesF[holesId].getWid() >= itemsCont[count].wid) &&
                            (holesF[holesId].getLen() >= itemsCont[count].len) &&
                            (holesF[holesId].getHei() >= itemsCont[count].hei)) { // si sobra espacio, se deben crear los nuevos huecos
                                //cout<<"No cabe justo en Front"<<endl;
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;         // Se suma el volumen
                                //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2 "<<Obj2_Weigth<<endl;
                                label_control = true;
                                // Calcular huecos
                                if ((holesF[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se crea hueco Beside
                                        //cout<<"Crear hueco beside con Front"<<endl;
                                        Hwid = holesF[holesId].getWid() - itemsCont[count].wid;
                                        Hlen = holesF[holesId].getLen();
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesF[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se crea hueco Above
                                        //cout<<"Crear hueco above con Front"<<endl;
                                        Hwid = holesF[holesId].getWid();
                                        Hlen = holesF[holesId].getLen();
                                        Hhei = holesF[holesId].getHei() - itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesF[holesId].getLen() - itemsCont[count].len) == 0) {  //Se crea hueco Beside
                                        //cout<<"Elimina hueco front con Front"<<endl;
                                        holesF.erase(holesF.begin() + holesId);
                                }
                                else if ((holesF[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                        //cout<<"Modifica hueco front con Front"<<endl;
                                        Hwid = itemsCont[count].wid;
                                        Hlen = holesF[holesId].getLen() - itemsCont[count].len;
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        // Modificar los valores
                                        holesF.erase(holesF.begin()+holesId);   
                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                }
                        
                        } //else if
                                
                        //////////////////////////////////////////
                        // Mostrar huecos resultantes
/*              if (itemsCont[count].pack == true) {            
                        cout<<"HolesF huecos con Front"<<endl;
                        for (int i = 0; i < holesF.size(); i++){
                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                        }
        
                        cout<<"HolesA huecos con Front"<<endl;
                        for (int i = 0; i < holesA.size(); i++){
                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                        }
                                
                        cout<<"HolesB huecos con Front"<<endl;
                        for (int i = 0; i < holesB.size(); i++){
                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                        }
                        
                }*/
                        ////////////////////////////////////////////
                        holesId++;
                } //whileHolesF
        
                holesId = 0; // Se inicializa el contador de huecos
                // Se recorre segundo el vector de holesA
                while ((holesId < holesA.size()) && (label_control != true)) {
                        //cout<<"while de holesA holesId "<<holesId<<endl;
                        if ((holesA[holesId].getWid() == itemsCont[count].wid) &&
                            (holesA[holesId].getLen() == itemsCont[count].len) &&
                            (holesA[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                //cout<<"Cabe justo en Above"<<endl;
                                holesA.erase(holesA.begin() + holesId);  // Se elimina el hueco del vector
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;           // Se suma el volumen
                                //cout<<"Obj1 "<<Obj1_Volumen<< " Obj2 "<<Obj2_Weigth<<endl;
                                label_control = true;
                        }
                        else if ((holesA[holesId].getWid() >= itemsCont[count].wid) &&
                            (holesA[holesId].getLen() >= itemsCont[count].len) &&
                            (holesA[holesId].getHei() >= itemsCont[count].hei)) { // si sobra espacio, se deben crear los nuevos huecos
                                //cout<<"No cabe justo en Above"<<endl;
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;           // Se suma el volumen
                                //cout<<"Obj1 "<<Obj1_Volumen<< " Obj2 "<<Obj2_Weigth<<endl;
                                label_control = true;
                                // Calcular huecos
                                if ((holesA[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se crea hueco Beside
                                        //cout<<"Crear hueco beside en above"<<endl;
                                        Hwid = holesA[holesId].getWid() - itemsCont[count].wid;
                                        Hlen = holesA[holesId].getLen();
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesA[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                        //cout<<"Crear hueco front en above"<<endl;
                                        Hwid = itemsCont[count].wid;
                                        Hlen = holesA[holesId].getLen() - itemsCont[count].len;
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesA[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se modifica hueco Above
                                        //cout<<"Modifica hueco above en above"<<endl;
                                        Hwid = holesA[holesId].getWid();
                                        Hlen = holesA[holesId].getLen();
                                        Hhei = holesA[holesId].getHei() - itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        // Modificar los valores
                                        holesA.erase(holesA.begin()+holesId);   
                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                }
                                else if ((holesA[holesId].getHei() - itemsCont[count].hei) == 0) {  //Se elimina hueco Above
                                        //cout<<"Elimina hueco above en above"<<endl;
                                        holesA.erase(holesA.begin() + holesId);
                                }
                        } //else if
                                        
                        // Mostrar huecos resultantes
        /*      if (itemsCont[count].pack == true) {    
                        cout<<"HolesF huecos en above "<<endl;
                        for (int i = 0; i < holesF.size(); i++){
                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                        }
                        cout<<"HolesA huecos en above"<<endl;
                        for (int i = 0; i < holesA.size(); i++){
                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                        }
                        cout<<"HolesB huecos en above"<<endl;
                        for (int i = 0; i < holesB.size(); i++){
                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                        }
                }*/
                        holesId++;
                } //whileAbove
                        
                
                holesId = 0; // Se inicializa el contador de huecos
                // Se recorre tercero el vector de holesB
                while ((holesId < holesB.size()) && (label_control != true)) {
                        //cout<<"while de holesB holesId "<<holesId<<endl;
                        if ((holesB[holesId].getWid() == itemsCont[count].wid) &&
                            (holesB[holesId].getLen() == itemsCont[count].len) &&
                            (holesB[holesId].getHei() == itemsCont[count].hei)) { // si cabe justo
                                //cout<<"Cabe justo en Beside"<<endl;
                                holesB.erase(holesB.begin() + holesId);  // Se elimina el hueco del vector
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;
                                //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2  "<<Obj2_Weigth<<endl;
                                label_control = true;
                        }
                        else if ((holesB[holesId].getWid() >= itemsCont[count].wid) &&
                            (holesB[holesId].getLen() >= itemsCont[count].len) &&
                            (holesB[holesId].getHei() >= itemsCont[count].hei)){ // si sobra espacio, se deben crear los nuevos huecos
                                //cout<<"No cabe justo en Beside"<<endl;
                                itemsCont[count].pack = true;
                                Obj2_Weigth += itemsCont[count].wei;          // Se suma el peso
                                Obj1_Volumen += itemsCont[count].vol;
                                //cout<<"Obj1 "<<Obj1_Volumen <<" Obj2  "<<Obj2_Weigth<<endl;
                                label_control = true;
                                // Calcular huecos
                                if ((holesB[holesId].getHei() - itemsCont[count].hei) > 0) {  //Se crea hueco Above
                                        //cout<<"Crear hueco above en Beside"<<endl;
                                        Hwid = holesB[holesId].getWid();
                                        Hlen = holesB[holesId].getLen();
                                        Hhei = holesB[holesId].getHei() - itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesB[holesId].getLen() - itemsCont[count].len) > 0) {  //Se crea hueco Front
                                        //cout<<"Crear hueco front en beside"<<endl;
                                        Hwid = itemsCont[count].wid;
                                        Hlen = holesB[holesId].getLen() - itemsCont[count].len;
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        holesF.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                                }
                                if ((holesB[holesId].getWid() - itemsCont[count].wid) > 0) {  //Se modifica hueco beside
                                        //cout<<"Modifica hueco beside en beside"<<endl;
                                        Hwid = holesB[holesId].getWid() - itemsCont[count].wid;
                                        Hlen = holesB[holesId].getLen();
                                        Hhei = itemsCont[count].hei;
                                        Hvol = Hwid * Hlen * Hhei;
                                        // Modificar los valores
                                        holesB.erase(holesB.begin()+holesId);   
                                        holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Modificar el hueco
                                }
                                else if ((holesB[holesId].getWid() - itemsCont[count].wid) == 0) {  //Se elimina hueco Beside
                                        //cout<<"Elimina hueco beside en beside"<<endl;
                                        holesB.erase(holesB.begin() + holesId);
                                }
                        } //else if
                                        
                        // Mostrar huecos resultantes
        /*      if (itemsCont[count].pack == true) {    
                        cout<<"HolesF huecos en beside"<<endl;
                        for (int i = 0; i < holesF.size(); i++){
                                cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                        }
                        cout<<"HolesA huecos en beside"<<endl;
                        for (int i = 0; i < holesA.size(); i++){
                                cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                        }
                        cout<<"HolesB huecos en beside"<<endl;
                        for (int i = 0; i < holesB.size(); i++){
                                cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                        }
                }*/
                        holesId++;
                                        
                } //whileBeside
                
                //cout<<"parte nueva"<<endl;
                // Parte nueva de la heuristica
                if ((itemsCont[count].len <= L_Sheet_free) && (label_control != true)){
                        //cout<<"Se crean los primeros huecos del nivel actual"<<endl;
                        LDB = itemsCont[count].len;                          // Se establece el wid del nivel
                        L_Sheet_free = L_Sheet_free - LDB;                   // Wid libre en el contenedor para el resto de niveles que se puedan crear
                        itemsCont[count].pack = true;
                        Obj2_Weigth += itemsCont[count].wei;                 // Se acumula el peso con la nueva pieza introducida
                        Obj1_Volumen += itemsCont[count].vol;
                
                        // Importante:
                        // Siempre que se introduce una pieza para generar un nuevo nivel,
                        // solo se producen los huecos beside y above
                        
                        // Calculo de los que huecos que se generan
                        if ((widSheet - itemsCont[count].wid) > 0) { // Si despues de empaquetar la pieza queda hueco en el Front
                                // Se genera el primer hueco del nivel en el vector holesB
                                //cout<<"Se genera hueco beside del nivel"<<endl;
                                Hwid = widSheet - itemsCont[count].wid;
                                Hlen = LDB;
                                Hhei = itemsCont[count].hei;
                                Hvol = Hwid * Hlen * Hhei;
                                holesB.push_back(Holes(Hwid,Hlen,Hhei,Hvol)); // Se crea el hueco en el vector holesF
                        }
                        if ((heiSheet - itemsCont[count].hei) > 0 ) {  // Si despues de empaquetar la pieza queda hueco en el Above
                                //cout<<"se genera hueco above del nivel"<<endl;
                                Hwid = widSheet;
                                Hlen = LDB;
                                Hhei = heiSheet - itemsCont[count].hei;
                                Hvol = Hwid * Hlen * Hhei;
                                holesA.push_back(Holes(Hwid,Hlen,Hhei,Hvol));
                        }
                // Recorrer vectores de huecos
                /*cout<<"Inicio nivel HolesF"<<endl;
                for (int i = 0; i < holesF.size(); i++){
                        cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                }
                cout<<"Inicio nivel HolesA"<<endl;
                for (int i = 0; i < holesA.size(); i++){
                        cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                }*/
        
                }
                count++;
                holesId = 0;
                label_control = false;   // Etiqueta de control
                        
        
                        
                // Ordenar los vectores de menor a mayor volumen
                sort(holesF.begin(),holesF.end(),cmpVol);
                // Mostrar huecos resultantes
                /*cout<<"Orden HolesF huecos "<<endl;
                for (int i = 0; i < holesF.size(); i++){
                        cout<<"wid len hei vol "<< holesF[i].getWid()<<" "<<holesF[i].getLen()<<" "<<holesF[i].getHei()<<" "<<holesF[i].getVol()<<endl;
                }*/
                
                sort(holesA.begin(),holesA.end(),cmpVol);
                /*cout<<"Orden HolesA huecos "<<endl;
                for (int i = 0; i < holesA.size(); i++){
                        cout<<"wid len hei vol "<<holesA[i].getWid()<<" "<<holesA[i].getLen()<<" "<<holesA[i].getHei()<<" "<<holesA[i].getVol()<<endl;
                }*/
                
                sort(holesB.begin(),holesB.end(),cmpVol);
                /*cout<<"Orden HolesB huecos "<<endl;
                for (int i = 0; i < holesB.size(); i++){
                        cout<<"wid len hei vol "<<holesB[i].getWid()<<" "<<holesB[i].getLen()<<" "<<holesB[i].getHei()<<" "<<holesB[i].getVol()<<endl;
                }*/
        
        }
        int sum = 0;
        for (int p = 0; p < itemsCont.size(); p++){
                if (itemsCont[p].pack == true){
                        sum = sum + 1;
                }
        }
        //cout<<"Cajas totales empaquetadas "<<sum<<endl;
        //cout<<" Objetivos "<<Obj1_Volumen<< " "<<Obj2_Weigth<<endl;
        //Se calcula el %
        Obj1_Volumen = (Obj1_Volumen * 100)/volSheet;   
        setObj(0,Obj1_Volumen);
        setObj(1,Obj2_Weigth);
        holesF.clear();
        holesA.clear();
        holesB.clear();

}






void TDCLP::printInd(Individual *ind) {
        
        for (int i = 0; i < ind->getNumberOfVar(); i++) {
                cout << (int) ind->getVar(i) << " ";
        }
        cout << "--- ";
        for (int i = 0; i < ind->getNumberOfObj(); i++) {
                cout << (int) ind->getObj(i) << " ";
        }
        cout << endl;
        cout << ((TDCLP *)ind)->sizeGen << endl;
}


//****************************************
// Prueba de fichero
// **************************************
void TDCLP::fixsolutions() {
        
        setVar(0,0);
        setVar(1,0);
        setVar(2,0);
        setVar(3,0);
        evaluate();
        exit(1);
}


                                                   
// *************************************************************************************
// Clase Holes. 
// Controla los huecos que se van formando dentro del contenedor
// *************************************************************************************
Holes::Holes() {
        H_wid = 0.0;
        H_len = 0.0;
        H_hei = 0.0;
        H_vol = 0.0;
}

Holes::Holes(double H_wid, double H_len, double H_hei, double H_vol) {

        this->H_wid = H_wid;
        this->H_len = H_len;
        this->H_hei = H_hei;
        this->H_vol = H_vol;
}