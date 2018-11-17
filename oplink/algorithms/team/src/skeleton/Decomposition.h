/***********************************************************************************
 * AUTHOR
 *   - Alejandro Marrero
 *
 * DATE
 *   November 2018
 *
 * DESCRIPTION
 *
 * Abstract class to represent a decomposition function for MOEA
 *
 * **********************************************************************************/
#ifndef __DECOMPOSITION_H__
#define __DECOMPOSITION_H__

#include "Individual.h"
#include "Plugin.h"
#include <vector>

class Individual;

class Decomposition : public Plugin {
public:
  virtual bool init(const vector<string> &params);
  virtual double evaluate(Individual*, Individual*, std::vector<double>&) = 0;
  int direction;
};

Decomposition* getDecomposition(const string& pluginPath, const string& pluginName, const vector<string>& args, bool printError);

#endif
