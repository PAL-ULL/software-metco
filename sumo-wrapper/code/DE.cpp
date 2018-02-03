#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>
#include <sys/time.h> 
#include <sys/wait.h>
#include <cstring>
#include "cInstance.hpp"
#include "simpleXMLParser.hpp"

using namespace std;

// A solution
typedef vector<int> tSol;

// Algorithm settings
typedef struct{
  unsigned dimension;	// Size of the individual
  unsigned popsize;		// Size of the population
  int max_steps;		// budget of evaluations
  long max_time;		// budget of time (seconds)
  long verbose;			// Number of seconds between two shows
  string dir;   		// Directory with input and output files
  string inputFile; 	// input file (tl)
  string outputFile;  	// output file (result)
  double F;				// Mutation factor
  double CR;			// Crossover factor
  tSol x_min;			// minimum value for x[i]
  tSol x_max;			// maximum value for x[i]
} tSetting;				

// Current status of the algorithm
typedef struct{
  unsigned current_step;	// Current step
  vector<tSol> pop;			// Current population
  vector<double> pop_fit;	// fitness of the current popultation
  tSol best;				// Best solution so far
  double best_fit;			// Fitness of the best solution 
} tStatus;

// If all the TL is yellow is a phase for pedestrian (4*lanes)
bool areAllYellow(string phase) {
  for (int i = 0; i < phase.size(); i++)
    if (phase[i] != 'y')
      return false;
  return true;
}

string execCommandPipe(string command) {
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

// Initial configuration of the algorithm (It depends on the problem instance)
void initializeSetting(tSetting &s, const cInstance &c)
{
	s.dimension = c.getTotalNumberOfPhases() + c.getNumberOfTLlogics();
	s.popsize = 50;
	s.F = 0.7;
	s.CR = 0.1;
	s.x_min.clear();
	s.x_max.clear();

	vector<string> phases;
	int pos;
	for(int j = 0; j < c.getNumberOfTLlogics(); j++)
	{
		phases = c.getPhases(j);
		s.x_min.push_back(0);
		s.x_max.push_back(119);
		for(int k = 0; k < phases.size(); k++)
		{
			if(areAllYellow(phases[k])) // Pedestrian phase: 4*lanes
			{
				s.x_min.push_back(4 * phases[k].size());
				s.x_max.push_back(4 * phases[k].size());
			}
			else if(isSubString(phases[k],"y",pos))	// phases with yellow TLs are fixed to 4
			{
				s.x_min.push_back(4);
				s.x_max.push_back(4);		
			}
			else
			{
				s.x_min.push_back(5);
				s.x_max.push_back(60);		
			}
		}
	}
}


// Write a solution to a tl.txt file 
void writeSolutionFile(const tSol &solution, const tSetting &s, const cInstance &c)
{
    string outputFile = c.getPath() + s.dir + "/" + s.inputFile;
	ofstream f(outputFile.c_str());
	for(int i = 0; i < s.dimension; i++)
		f <<  solution[i] << " ";
	f.close();
}

// Read the file returned by sumo_wrapper 
void readFitnessFile(double &fitness, const tSetting &s, const cInstance &c)
{
	string inputFile = c.getPath() + s.dir + "/" + s.outputFile;
	ifstream f(inputFile.c_str());
	string str;
	for(int i = 0; i < 6; i++) // skip lines
		getline(f,str);
	f >> fitness;
	f.close();
}

// Generate a random solution
void generateSolution(tSol &solution, const tSetting &s)
{
	solution.clear();
	// Random Values
	for(int i = 0; i < s.dimension; i++)
		solution.push_back(rand()%(s.x_max[i]-s.x_min[i]+1) + s.x_min[i]);
}

// Create the initial population and all related individuals
void initializeAlgorithm(tStatus &a, const tSetting &s)
{
	a.pop.clear();
	a.current_step = 0;
	for(unsigned i = 0; i < s.popsize; i++)
	{
		tSol sol;
		generateSolution(sol, s);
		a.pop.push_back(sol);
	}
}

// Evaluate the initial pop and modify the rest of status value
// This only works for minimization problem
void evaluateInitialPopulation(tStatus &a, const tSetting &s, string command, const cInstance &c)
{
	double fitness;
	
	a.pop_fit.clear();
	writeSolutionFile(a.pop[0], s, c);
	execCommandPipe(command);
	readFitnessFile(fitness, s, c);
	a.pop_fit.push_back(fitness);
	a.best = a.pop[0];
	a.best_fit = fitness;

	for(unsigned i = 1; i < s.popsize; i++)
	{
		writeSolutionFile(a.pop[i], s, c);
		execCommandPipe(command);
		readFitnessFile(fitness, s, c);
		a.pop_fit.push_back(fitness);
		
		if(fitness < a.best_fit)
		{
			a.best = a.pop[i];
			a.best_fit = fitness;			
		}
	}
}

// Mutate -> Croosver -> Replace
void doStep(tStatus &a, const tSetting &s, string command, const cInstance &c)
{
	const unsigned siz = a.pop[0].size();
	for(unsigned i = 0; i < s.popsize; i++)
	{
		tSol temp(siz);
		unsigned r1 = rand()%(s.popsize - 1), r2 = rand()%(s.popsize - r1) + r1, RI, RJ;
		RI = rand()%siz;
		for(unsigned j = 0; j < siz; j++){
			// Mutation
			temp[j] = a.best[j] + s.F*(a.pop[r1][j] - a.pop[r2][j]);
			if(temp[j] < s.x_min[j]) temp[j] = s.x_min[j];
			else if(temp[j] > s.x_max[j]) temp[j] = s.x_max[j];
			// Crossover
			RJ = rand()*1.0/RAND_MAX;
			if(RJ > s.CR && RI != j)
				temp[j] = a.pop[i][j];
		}
		writeSolutionFile(temp, s, c);
		execCommandPipe(command);
		double fit;
		readFitnessFile(fit, s, c);
		if(fit < a.pop_fit[i]){
			a.pop[i] = temp;
			a.pop_fit[i] = fit;
			if(fit < a.best_fit)
			{
				a.best = a.pop[i];
				a.best_fit = a.pop_fit[i];			
			}			
		}
	}
}


bool stopCondition(long time, long max_time, int evals, int max_evals){
	bool finish = false;


	if(max_time > 0)	finish = finish || (time >= max_time);
	if(max_evals > 0)	finish = finish || (evals >= max_evals); 

	return finish;
}

void readParameters(int argc, char **argv, tSetting &s, string &instance){
	if(argc%2 == 0){ cout << "Number of parameters incorrect" << endl; exit(-1);}

	int p = 1;

	instance = "";
	s.verbose = -1;
	s.max_time = -1;
	s.max_steps = -1;
  	s.dir = "";
  	s.inputFile = "";
  	s.outputFile = "";

	while(p < argc){
		if(strcmp(argv[p],"-i") == 0) instance = string(argv[p+1]);
		else if(strcmp(argv[p],"-t") == 0) sscanf(argv[p+1],"%ld",&s.max_time);
		else if(strcmp(argv[p],"-s") == 0) sscanf(argv[p+1],"%d",&s.max_steps);
		else if(strcmp(argv[p],"-v") == 0) sscanf(argv[p+1],"%ld",&s.verbose);
		else if(strcmp(argv[p],"-d") == 0) s.dir = string(argv[p+1]);
		else if(strcmp(argv[p],"-in") == 0) s.inputFile = string(argv[p+1]);
		else if(strcmp(argv[p],"-out") == 0) s.outputFile = string(argv[p+1]);
		else { cout << "Option unknown: " << argv[p] << endl; exit(-1);}
		p += 2;
	}

	if(instance == ""){ cout << "No instance file" << endl; exit(-1);}
	if(s.max_time == -1 && s.max_steps == -1){ cout << "No stop condition" << endl; exit(-1);}
	if(s.dir == ""){ cout << "A directory must be specified" << endl; exit(-1);}
	if(s.inputFile == ""){ cout << "An input file must be specified" << endl; exit(-1);}
	if(s.outputFile == ""){ cout << "An output file must be specified" << endl; exit(-1);}
}

int main(int argc, char **argv)
{
	tSetting settings;
	tStatus algorithm;
	string command, instance;
	struct timeval tv;
	long initial_time, current_time, show;
	cInstance c;

	srand(time(0));

	readParameters(argc, argv, settings, instance);

	if(!c.read(instance)){
		cout << "Error reading " << instance << endl;
		exit(-1);
	}

	command = "mkdir -p " + c.getPath() + settings.dir + "/";
	execCommandPipe(command);
  
	string inputFile = c.getPath() + settings.dir + "/" + settings.inputFile;
	string outputFile = c.getPath() + settings.dir + "/" + settings.outputFile;

	command = "/home/edusegre/sumo-wrapper/sumo-wrapper " + instance + " " + settings.dir + " " + inputFile + " " + outputFile + " 0";
	//command = "./sumo-wrapper " + instance + " " + inputFile + " " + outputFile + " 0";
	
	initializeSetting(settings, c);

	initializeAlgorithm(algorithm, settings);
	gettimeofday(&tv, NULL);
	initial_time = tv.tv_sec;
	show = 0;

	evaluateInitialPopulation(algorithm, settings, command, c);
	gettimeofday(&tv, NULL);
	current_time = tv.tv_sec  - initial_time;

	if(settings.verbose > 0 && current_time - show > settings.verbose){
		cout << "iteration " << algorithm.current_step << " best: " << algorithm.best_fit << " time: " << current_time << endl;
		for(int i = 0; i < settings.dimension; i++)
			cout << algorithm.best[i] << " ";
		cout << endl;
		show = current_time;
	}

	for(algorithm.current_step = 1; !stopCondition(current_time, settings.max_time, algorithm.current_step, settings.max_steps); algorithm.current_step++)
	{
		doStep(algorithm,settings,command, c);
		gettimeofday(&tv, NULL);
		current_time = tv.tv_sec  - initial_time;

		if(settings.verbose > 0 && current_time - show > settings.verbose){
			cout << "iteration " << algorithm.current_step << " best: " << algorithm.best_fit << " time: " << current_time << endl;
			for(int i = 0; i < settings.dimension; i++)
				cout << algorithm.best[i] << " ";
			cout << endl;
			show = current_time;
		}
	}

	cout << "Best solution: " << endl;
	for(int i = 0; i < settings.dimension; i++)
		cout << algorithm.best[i] << " ";
	cout << endl << "Fitness: " << algorithm.best_fit << endl;

	return 0;
}



