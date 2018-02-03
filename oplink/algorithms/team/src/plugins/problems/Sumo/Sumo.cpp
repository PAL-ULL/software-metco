/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo González
 * 
 * FECHA
 *    Noviembre 2015
 ************************************************************************************/

#include "Sumo.h"

bool Sumo::init (const vector<string> &params) {
  if (params.size() != 6) {
    cerr << "Error in init - Parameters: instanceFile execDir tlFileName resultsFileName mut_op cross_op" << endl;
    exit(-1);
  }


  instance.read(params[0]);
  command = "mkdir -p " + instance.getPath() + params[1] + "/";
  execCommandPipe(command);

  tlFile = instance.getPath() + params[1] + "/" + params[2];
  resultsFile = instance.getPath() + params[1] + "/" + params[3];
  command = "/home/edusegre/sumo-wrapper/sumo-wrapper " + params[0] + " " + params[1] + " " + tlFile + " " + resultsFile + " 0";

  mut_op = params[4];
  cross_op = params[5];

  setNumberOfVar(instance.getNumberOfTLlogics() + instance.getTotalNumberOfPhases());
	setNumberOfObj(NOBJ);

	return true;
}

// Evaluation
void Sumo::evaluate (void) {
  doubleToIntSolution();
  writeSolutionFile();
	//cout << "command: " << command << endl;
  string result = execCommandPipe(command);
	//cout << "result: " << result << endl;
  double fitness = readFitnessFile();
	//cout << "fitness: " << fitness << endl;
  setObj(0, fitness);
}

// Cloning procedure
Individual* Sumo::clone (void) const {
  Sumo* aux = new Sumo();
  aux->instance = instance;
  aux->command = command;
  aux->tlFile = tlFile;
  aux->resultsFile = resultsFile;
  aux->mut_op = mut_op;
  aux->cross_op = cross_op;
  return aux;
}

void Sumo::print(ostream &os) const {
  for (unsigned int i = 0; i < getNumberOfVar(); i++)
    os << getVar(i) << " " << flush;
  for (unsigned int i = 0; i < getNumberOfObj(); i++)
    os << getObj(i) << " " << flush;
  os << endl << flush;
}

void Sumo::dependentMutation(double pm) {
  if (mut_op == "Mutate_Uniform") {
    mutate_uniform(pm, 0, getNumberOfVar());
  }
  else if (mut_op == "Mutate_Pol") {
    mutate_Pol(pm, 0, getNumberOfVar());
  }
  repair();
}

void Sumo::dependentCrossover(Individual* ind) {
  if (cross_op == "Crossover_Uniform") {
    crossover_uniform(ind, 0, getNumberOfVar());
  }
  else if (cross_op == "Crossover_SBX") {
    crossover_SBX(ind, 0, getNumberOfVar());
  }
  ((Sumo*)ind)->repair();
  repair();
}

void Sumo::restart(void) {
  int abs_pos = 0;
  for(int j = 0; j < instance.getNumberOfTLlogics(); j++) {
    setVar(abs_pos, rand() % 120);
    abs_pos++;
    for(int k = 0; k < instance.getPhases(j).size(); k++) {
      setVar(abs_pos, ((double) rand () / (double) RAND_MAX));
      setVar(abs_pos, getVar(abs_pos) * (getMaximum(abs_pos) - getMinimum(abs_pos)) + getMinimum(abs_pos));
      abs_pos++;
    }
  }
  repair();
}

void Sumo::repair(void) {
  int abs_pos = 0;
  for(int j = 0; j < instance.getNumberOfTLlogics(); j++) {

		if (getVar(abs_pos) >= 120.0) {
      setVar(abs_pos, rand() % 120);
    }
		abs_pos++;

    vector<string> phases = instance.getPhases(j);

    for(int k = 0; k < phases.size(); k++) {
      int pos;
      // Those cases where all the TLs belonging to a phase change from "G"reen to "y"ellow
      if (areAllYellow(phases[k]))
        setVar(abs_pos, (double) 4.0 * phases[k].size());
      // Remaining cases where there exist TLs that change from "G"reen to "y"ellow
      else if (isSubString(phases[k], "y", pos))
        setVar(abs_pos, (double) 4.0);
      abs_pos++;
    }
  }
}

void Sumo::doubleToIntSolution(void) {
  for (int i = 0; i < getNumberOfVar(); i++)
//    if (((double) rand () / (double) RAND_MAX) >= 0.5)
//      setVar(i, ceil(getVar(i)));
//    else
//      setVar(i, floor(getVar(i)));
    setVar(i, round(getVar(i)));
}

bool Sumo::areAllYellow(string phase) {
  for (int i = 0; i < phase.size(); i++)
    if (phase[i] != 'y')
      return false;
  return true;
}

void Sumo::writeSolutionFile() {
  ofstream f(tlFile);
  for(int i = 0; i < getNumberOfVar(); i++)
      f << (unsigned int)getVar(i) << " ";
  f.close();
}

double Sumo::readFitnessFile() {
  double fitness;
  ifstream f(resultsFile);
  string s;
  for(int i = 0; i < 6; i++) // skip lines
    getline(f, s);
  f >> fitness;
  f.close();
  return fitness;
}

string Sumo::execCommandPipe(string command) {
  const int MAX_BUFFER_SIZE = 128;
  FILE* pipe = popen(command.c_str(), "r");
	
  // Waits until the execution ends
	if (wait(NULL) == -1){
		cerr << "Error waiting for simulator results" << endl;
		return "-1";
	}

  char buffer[MAX_BUFFER_SIZE];
  string result = "";
  while (!feof(pipe)) {
    if (fgets(buffer, MAX_BUFFER_SIZE, pipe) != NULL)
      result += buffer;
  }
  pclose(pipe);
  return result;
}
