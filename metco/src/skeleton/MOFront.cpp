/***********************************************************************************
 * AUTORES
 *   - Ofelia Gonz�lez P�rez
 *   - Gara Miranda Valladares
 *   - Carlos Segura Gonz�lez
 *
 * FECHA
 *    Noviembre 2007
 ************************************************************************************/

#include "MOFront.h"
#include <math.h>
#include "hv.h"
#include "utils.h"

// Constructor
MOFront::MOFront(const Individual *ejInd, const bool copyOnInsert,
                 const bool deleteOnDestroy) {
    this->ejInd = ejInd;
    this->copyOnInsert = copyOnInsert;
    this->deleteOnDestroy = deleteOnDestroy;
}

// Insercion de un individuo en el frente
bool MOFront::insert(Individual *ind) {
    bool iguales;
    int erased;
    return insert(ind, erased, iguales);
}

// Insercion de un nuevo individuo en el frente
// 'erased' se corresponde con el numero de individuos eliminados del
// frente en caso de que sea insertado
bool MOFront::insert(Individual *ind, int &erased) {
    bool iguales;
    return insert(ind, erased, iguales);
}

// Insercion de un individuo en el frente
// 'igual' determina si ya hay una soluci�n id�ntica en el frente
bool MOFront::insert(Individual *ind, bool &igual) {
    int erased;
    return insert(ind, erased, igual);
}

void MOFront::crowOrder1by1(const unsigned int finalSize) {
    int currentSize = getFrontSize();
    while (currentSize > finalSize) {
        crowOrder(currentSize--);
    }
}

// Lee de un fichero de entrada los puntos de un Frente de Pareto: solo
// objetivos
void MOFront::read(const string &inputFile) {
    // Comprobaci�n de ficheros de entrada
    ifstream fi(inputFile.c_str(), ios::in);
    if (!fi) {
        cout << "\n\tERROR: fichero de entrada no valido" << inputFile << endl;
        exit(-1);
    }

    int nObj = ejInd->getNumberOfObj();
    long double obj;
    bool fin = false;

    while (!fi.eof()) {
        Individual *ind = ejInd->internalClone();
        for (int i = 0; i < nObj; i++) {
            fi >> obj;
            if (fi.fail()) {
                delete (ind);
                fin = true;
                break;
            }
            ind->setObj(i, obj);
        }
        if (!fin) {
            insert(ind);
        }
    }
    fi.close();
}

// Lee de un fichero de entrada los puntos de un Frente de Pareto: variables y
// objetivos
void MOFront::readAll(const string &inputFile) {
    // Comprobaci�n de ficheros de entrada
    ifstream fi(inputFile.c_str(), ios::in);
    if (!fi) {
        cout << "\n\tERROR: Fichero de entrada no valido" << inputFile << endl;
        exit(1);
    }

    int nObj = ejInd->getNumberOfObj();
    int nVar = ejInd->getNumberOfVar();
    long double obj, var;
    bool fin = false;

    while ((!fi.eof() && (!fi.fail()))) {
        Individual *ind = ejInd->internalClone();
        for (int i = 0; i < nVar; i++) {
            fi >> var;
            if (fi.fail()) {
                delete (ind);
                fin = true;
                break;
            }
            ind->setVar(i, var);
        }
        if (fin) {
            break;
        }
        for (int i = 0; i < nObj; i++) {
            fi >> obj;
            if (fi.fail()) {
                delete (ind);
                fin = true;
                break;
            }
            ind->setObj(i, obj);
        }
        if (!fin) {
            insert(ind);
        }
    }
    fi.close();
}

// Escribe a fichero los puntos soluciones (objetivos) del frente
void MOFront::write(const string &outputFileName) {
    ofstream outputFile(outputFileName.c_str());
    for (unsigned int i = 0; i < getFrontSize(); i++) {
        Individual *ind = getInd(i);
        for (unsigned int j = 0; j < ind->getNumberOfObj(); j++) {
            outputFile << ind->getObj(j) << " ";
        }
        outputFile << endl;
    }
    outputFile.close();
}

// Escribe a fichero los puntos soluciones (variables y objetivos) del frente
void MOFront::writeAll(const string &outputFileName) {
    ofstream outputFile(outputFileName.c_str());
    if (!outputFile.is_open()) {
        cout << "File " << outputFileName << " could not be opened" << endl;
        exit(-1);
    }
    for (unsigned int i = 0; i < getFrontSize(); i++) {
        Individual *ind = getInd(i);
        for (unsigned int j = 0; j < ind->getNumberOfVar(); j++) {
            outputFile << ind->getVar(j) << " ";
        }
        for (unsigned int j = 0; j < ind->getNumberOfObj(); j++) {
            outputFile << ind->getObj(j) << " ";
        }
        outputFile << endl;
    }
    outputFile.close();
}

// Minimization is considered in all objectives
// Values between values 0 and 1
// 1 is summed to every value, so values are between 1 and 2
// The returned value is also between 0.5 and 2
// The indicator is to minimize
// If there is no point in the front the value 2 is returned
long double MOFront::getEpsilonIndicator(MOFront &p) {
    long double maxVal1 = LDBL_MIN;

    if (getFrontSize() == 0) return 2;
    for (unsigned int i = 0; i < p.getFrontSize(); i++) {
        long double minVal2 = LDBL_MAX;
        for (unsigned int j = 0; j < getFrontSize(); j++) {
            long double maxVal3 = LDBL_MIN;
            Individual *indB = p.getInd(i);
            Individual *indA = getInd(j);
            for (unsigned int k = 0; k < indA->getNumberOfObj(); k++) {
                if (((indA->getObj(k) + 1) / (indB->getObj(k) + 1)) > maxVal3)
                    maxVal3 = ((indA->getObj(k) + 1) / (indB->getObj(k) + 1));
            }
            if (maxVal3 < minVal2) minVal2 = maxVal3;
        }
        if (minVal2 > maxVal1) maxVal1 = minVal2;
    }
    return maxVal1;
}

// Construye el frente de pareto de los frentes de paretos que se
// encuentran representados en los ficheros de entrada
void MOFront::merge(const char *namefile1, const char *namefile2,
                    const int numObj) {
    ifstream f1(namefile1, ios::in);
    ifstream f2(namefile2, ios::in);

    if ((!f1) || (!f2)) {
        cout << " Error: No se pudo abrir alguno de los ficheros " << endl;
        cout << namefile1 << " " << namefile2 << endl;
        return;
    }

    long double obj;
    int front1Size = 0;
    int front2Size = 0;
    int count1 = 0;
    int count2 = 0;
    int erased = 0;

    while (!f1.eof()) {
        Individual *ind = ejInd->internalClone();
        for (int i = 0; i < numObj; i++) {
            f1 >> obj;
            if (f1.eof()) break;
            ind->setObj(i, obj);
        }
        if (!f1.eof())
            if (!insert(ind)) delete (ind);
    }
    count1 = front1Size = getFrontSize();

    while (!f2.eof()) {
        Individual *ind = ejInd->internalClone();
        for (int i = 0; i < numObj; i++) {
            f2 >> obj;
            if (f2.eof()) break;
            ind->setObj(i, obj);
        }
        if (!f2.eof()) {
            bool inserted = insert(ind, erased);
            if (!inserted)
                delete (ind);
            else if (!f2.eof()) {
                count2++;
                count1 -= erased;
            }
            if (!f2.eof()) front2Size++;
        }
    }

#ifdef DEBUG
    cout << " -----------------------------------------------" << endl;
    cout << " MERGE entre los resultados: " << endl;
    cout << "     (1) " << namefile1 << endl;
    cout << "     (2) " << namefile2 << endl;
    cout << " Tama#o frente 1: " << front1Size << endl;
    cout << " Tama#o frente 2: " << front2Size << endl;
    cout << " Tama#o frente merge: " << front.size() << endl;
    cout << "     (1) Count: " << count1 << " ("
         << ((double)count1 / front.size()) * 100 << "%)" << endl;
    cout << "         Permanecen: " << ((double)count1 / front1Size) * 100
         << "%" << endl;
    cout << "     (2) Count: " << count2 << " ("
         << ((double)count2 / front.size()) * 100 << "%)" << endl;
    cout << "         Permanecen: " << ((double)count2 / front2Size) * 100
         << "%" << endl;
    cout << " -----------------------------------------------" << endl;
#endif
    f1.close();
    f2.close();
}

// Calculo de hipervolumen (sin escalado)
// Los objetivos que son de maximizar se les invierte el signo
double MOFront::getHV(vector<long double> &refPoint) {
    // FIXME: convertir el codigo de calculo de hipervolumen para que conozca
    // como almacenamos los puntos y evitar tener que hacer transformaci�n
    if (getFrontSize() == 0) return 0;
    if (refPoint.size() != getInd(0)->getNumberOfObj()) {
        cerr << "Incorrect parameters to getHV" << endl << flush;
        exit(-1);
    }
    double *data = new double[getFrontSize() * getInd(0)->getNumberOfObj()];
    int datacnt = 0;
    int individuals = 0;
    for (int i = 0; i < getFrontSize(); i++) {
        // Si alguno de los objetivos es mayor que el punto de referencia hay
        // que quitarlo El c�digo que calcula el hipervolumen no contempla esta
        // opci�n y da valores incorrectos!!
        int initVal = datacnt;
        for (int j = 0; j < getInd(i)->getNumberOfObj(); j++) {
            if (getInd(i)->getInternalOptDirection(j) == MINIMIZE) {
                data[datacnt] = getInd(i)->getObj(j);
            } else {
                data[datacnt] = -getInd(i)->getObj(j);
            }
            if (data[datacnt] > refPoint[j]) {
                datacnt = initVal;
                individuals--;
                break;
            } else {  // OJO REVISAR ESTO PARA PROBLEMAS DE MAXIMIZACION
                data[datacnt] /= refPoint[j];
            }
            datacnt++;
        }
        individuals++;
    }
    if (individuals == 0) {
        delete data;
        return 0;
    }
    double *ref = new double[refPoint.size()];
    for (int i = 0; i < refPoint.size(); i++)
        // ref[i] = refPoint[i];
        ref[i] = 1;
    double hyperVolume =
        hv(data, getInd(0)->getNumberOfObj(), individuals, ref);
    delete data;
    delete ref;
    return hyperVolume;
}

ostream &operator<<(ostream &os, MOFront *p) {
    os << "Front Size = " << p->getFrontSize() << endl;
    vector<Individual *> allIndividuals;
    p->getAllIndividuals(allIndividuals);
    for (unsigned int i = 0; i < allIndividuals.size(); i++) {
        allIndividuals[i]->print(os);
    }
    return (os << endl << flush);
}

// Calculo de hipervolumen (con escalado)
double MOFront::getHVNorm(vector<long double> &refPoint,
                          vector<long double> &minValues,
                          vector<long double> &maxValues) {
    cout << "HVNorm no implementado" << endl;
    return 0;
}
