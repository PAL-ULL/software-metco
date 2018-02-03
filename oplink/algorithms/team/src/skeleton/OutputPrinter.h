/***********************************************************************************
 * AUTORES
 *   - Carlos Segura González
 * 
 * FECHA
 *    Mayo 2010 
 *
 * DESCRIPCION
 **********************************************************************************/


#ifndef __OUTPUT_PRINTER_H_
#define __OUTPUT_PRINTER_H_

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdlib.h>
 
#include "EA.h"
#include "MOFront.h"

using namespace std;

class EA;

class OutputPrinter : public Plugin {
public:
	virtual void printSolution(EA *ga, bool end) = 0;
	virtual void printInit(EA *ga) = 0;
	virtual void finish() = 0;
};

// Library functions
OutputPrinter *getOutputPrinter(const string &pluginPath, const string &pluginName, const vector<string> &indArgs, bool printError);
#endif
