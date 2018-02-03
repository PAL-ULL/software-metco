/***********************************************************************************
 * AUTORES
 *   - Ofelia González Pérez
 *   - Gara Miranda Valladares
 *   - Carlos Segura González
 * 
 * FECHA
 *    Noviembre 2007
 ************************************************************************************/

#include "MANETS_ULL.h"
#include <sstream>
#include <stdio.h>
#include <sys/wait.h>

// Valores minimos de cada variable
const double MANETS_ULL::MINIMUM[] = {0.0, 0.0, 0.0, 0.0, 0.0};
// Valores maximos de cada variable
const double MANETS_ULL::MAXIMUM[] = {100.0, 1.0, 100.0, 10.0, 10.0};
// Direcciones de optimizacion
const int MANETS_ULL::OBJFUNC[]  = {MINIMIZE, MAXIMIZE, MINIMIZE};
// Script de ejecucion
string MANETS_ULL::executionScript;

// Inicialización. 
// Fija:
//   - Numero de variables
//   - Numero de objetivos
//   - Direccion de optimizacion de los objetivos
//   - Rangos de las variables (mínimos y máximos)
//   - Ruta del script de ejecucion
bool MANETS_ULL::init(const vector<string> &params){
	if (params.size() != 1){
		cerr << "Error in Individual params. Params: executionScript." << endl;
		return false;
	}
	setNumberOfVar(NPARAM);
	setNumberOfObj(NOBJ);
	this->executionScript = params[0];
	return true;
}

// Evaluacion
void MANETS_ULL::evaluate (void){
	double mayor = (getVar(3) > getVar(4))?getVar(3):getVar(4);
	double menor = (getVar(3) > getVar(4))?getVar(4):getVar(3);
	execSimulator((int)(getVar(0)), getVar(1), (int)(getVar(2)), menor, mayor);
}

// Ejecucion del simulador y obtencion de los valores objetivos
void MANETS_ULL::execSimulator(int proD, double minGain, int safeDensity, double lowerBoundRad, double upperBoundRad){
	const int MAX_OUTPUT = 256;
	// Creamos la cadena de ejecucion
	stringstream cmd;
	cmd << executionScript << " " << proD << " " << minGain << " " << safeDensity << " " << lowerBoundRad << " " << upperBoundRad;
	// Ejecutamos simulador
	FILE *results = popen(cmd.str().c_str(), "r");

	// Esperamos a que finalice
	if (wait(NULL) == -1){
		cerr << "Error waiting for simulator results" << endl;
		return;
	}
	// Obtenemos los valores objetivos de la salida
	int index = 0;
	int readCh;
	char buffer[MAX_OUTPUT];
	char *from;
	while((readCh = fread(&buffer[index], sizeof(char), MAX_OUTPUT - index - 1, results)) > 0)
		index += readCh;
	buffer[index] = '\0';
	from = buffer;
	for (int i = 0; i < NOBJ; i++)
	{
		char *newObjStr;
		setObj(i, strtod (from, &newObjStr));
		if (from == newObjStr)
		{
			cerr << "Error reading objectives" << endl;
			break;
		}
		from = newObjStr;
	}
	// Cerramos la conexión con la salida del programa
	pclose (results);
}

// Operador de impresion
// Es necesario debido a que los valores contenidos en los genes no son directamente los
// parámetros del simulador MADHOC que es lo que queremos imprimir
void MANETS_ULL::print(ostream &os) const {
	double mayor = (getVar(3) > getVar(4))?getVar(3):getVar(4);
	double menor = (getVar(3) > getVar(4))?getVar(4):getVar(3);
	os << (int)(getVar(0)) << " " << getVar(1) << " " << (int)getVar(2) << " " << menor << " " << mayor << " ";
	for (unsigned int i = 0; i < getNumberOfObj(); i++)
		os << getObj(i) << " ";
	os << endl;
}

Individual* MANETS_ULL::clone (void) const {
	return new MANETS_ULL();
}
