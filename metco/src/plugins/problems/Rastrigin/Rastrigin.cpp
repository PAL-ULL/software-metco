/*
 * @Author: Alejandro Marrero 
 * @Contact: alu0100825008@ull.edu.es 
 * @Date: 2018-04-13 09:15:09 
 * @Last Modified by: mikey.zhaopeng
 * @Last Modified time: 2018-04-13 09:16:57
 */

#include "Rastrigin.h"
#include <cmath>
#include <vector>


bool Rastrigin::init(const vector<string> &params) {
  if (params.size() != 1) {
    cerr << "Error in Rastrigin init: the parameter dim must be especificated" << endl;
    exit(-1);
  }
  numVar = atoi(params[0].c_str());
  minVar.resize(numVar, -5.12);
  maxVar.resize(numVar, 5.12);
  setNumberOfVar(numVar);
  setNumberOfObj(1);
  setFitnessValue(0.00);
  return true;
}

Individual* Rastrigin::clone(void) const {
  return new Rastrigin();
}

void Rastrigin::evaluate(){
  double evaluation = 0.0;
  for(int i = 0; i < getNumberOfVar(); i++)
    evaluation += pow(getVar(i), 2) - 10 * cos(2 * M_PI * getVar(i));
  evaluation = 10 * numVar + evaluation;
  setObj(0, evaluation);
}

vector<double> Rastrigin::minVar;
vector<double> Rastrigin::maxVar;
int Rastrigin::numVar = 30;
