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

typedef vector<int> Solution;

typedef struct{
	string command;
	string dir;
	string inputFile;
	string outputFile;
	int max_steps;
	long max_time;
	long verbose;
} tParameters;

bool areAllYellow(string phase) {
  for (int i = 0; i < phase.size(); i++)
    if (phase[i] != 'y')
      return false;
  return true;
}


string execCommandPipe(const tParameters &p) {
  const int MAX_BUFFER_SIZE = 128;
  FILE* pipe = popen(p.command.c_str(), "r");
  
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


void generateSolution(Solution &solution, const cInstance &c){
	solution.clear();

	vector<string> phases;
	int pos;
	for(int j = 0; j < c.getNumberOfTLlogics(); j++)
	{
		phases = c.getPhases(j);
		solution.push_back(rand()%120);
		for(int k = 0; k < phases.size(); k++)
		{
			if(areAllYellow(phases[k]))
				solution.push_back(4 * phases[k].size());
			else if(isSubString(phases[k],"y",pos))
				solution.push_back(4);		
			else
				solution.push_back(rand()%55 + 5);
		}
	}
}

// Write a solution to a tl.txt file 
void writeSolutionFile(const Solution &solution, const tParameters &p, const cInstance &c)
{
    string outputFile = c.getPath() + p.dir + "/" + p.inputFile;
	ofstream f(outputFile.c_str());
	for(int i = 0; i < solution.size(); i++)
		f <<  solution[i] << " ";
	f.close();
}


// Read the file returned by sumo_wrapper 
void readFitnessFile(double &fitness, const tParameters &p, const cInstance &c)
{
	string inputFile = c.getPath() + p.dir + "/" + p.outputFile;
	ifstream f(inputFile.c_str());
	string str;
	for(int i = 0; i < 6; i++) // skip lines
		getline(f,str);
	f >> fitness;
	f.close();
}

double evaluate(const Solution &solution, const cInstance &c, const tParameters &p){
	double fitness;
	writeSolutionFile(solution, p, c);
	execCommandPipe(p);
	readFitnessFile(fitness, p, c);
	return fitness;
}

void generateNeighbor(Solution &neigh, int size, const cInstance &c){
	bool changed;

	int val = rand()%(size*2) - size;

	do{
		vector<string> phases;
		int pos = rand()%neigh.size(), p;	
		int counter = 0;
		changed = false;
		for(int j = 0; j < c.getNumberOfTLlogics(); j++)
		{
			phases = c.getPhases(j);
			if(counter + phases.size() + 1 > pos) break;
			counter += phases.size()+1;
		}
		if(pos-counter == 0){
			neigh[pos] = neigh[pos] + val;
			if(neigh[pos] < 0) neigh[pos] = 0;
			else if(neigh[pos] > 119) neigh[pos] = 119;
			changed = true;
		}
		else if(!areAllYellow(phases[pos-counter-1]) && !isSubString(phases[pos-counter-1],"y",p)){
			neigh[pos] = neigh[pos] + val;
			if(neigh[pos] < 5) neigh[pos] = 5;
			else if(neigh[pos] > 60) neigh[pos] = 60;
			changed = true;
		}
	}while(!changed);
}

bool stopCondition(long time, long max_time, int evals, int max_evals){
	bool finish = false;


	if(max_time > 0)	finish = finish || (time >= max_time);
	if(max_evals > 0)	finish = finish || (evals >= max_evals); 

	return finish;
}

void readParameters(int argc, char **argv, tParameters &s, string &instance){
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

int main(int argc, char **argv){
	tParameters params;
	string instance;
	srand(time(0));
	cInstance c;

	readParameters(argc, argv, params, instance);

	if(!c.read(instance)){
		cout << "Error reading " << instance << endl;
		exit(-1);
	}

	params.command = "mkdir -p " + c.getPath() + params.dir + "/";
	execCommandPipe(params);
	string inputFile = c.getPath() + params.dir + "/" + params.inputFile;
	string outputFile = c.getPath() + params.dir + "/" + params.outputFile;

	params.command = "/home/edusegre/sumo-wrapper/sumo-wrapper " + instance + " " + params.dir + " " + inputFile + " " + outputFile + " 0";
	//params.command = "./sumo-wrapper " + instance + " " + inputFile + " " + outputFile + " 0";

	// Algorithm 
	Solution current_sol, best_sol;
	double fitness, best_fit;
	struct timeval tv;
	long initial_time, current_time, show;
	unsigned current_step = 0, neighborhood = 5, convergence;

	gettimeofday(&tv, NULL);
	initial_time = tv.tv_sec;
	show = 0;

	generateSolution(current_sol, c);
	fitness = evaluate(current_sol, c, params);
	best_sol = current_sol; best_fit = fitness;

	gettimeofday(&tv, NULL);
	current_time = tv.tv_sec  - initial_time;

	if(params.verbose > 0 && current_time - show > params.verbose){
		cout << "iteration " << current_step << " best: " << best_fit << " time: " << current_time << endl;
		for(int i = 0; i < best_sol.size(); i++)
			cout << best_sol[i] << " ";
		cout << endl;
		show = current_time;
	}
	convergence = current_sol.size();

	for(current_step = 1; !stopCondition(current_time, params.max_time, current_step, params.max_steps); current_step++)
	{
		current_sol = best_sol;		
		generateNeighbor(current_sol, neighborhood,c);
		fitness = evaluate(current_sol, c, params);
		if(fitness < best_fit)
		{
			best_sol = current_sol; 
			best_fit = fitness;
			neighborhood = 5;
			convergence = current_sol.size();
		} else {
			convergence--;
			if(convergence == 0){
				neighborhood += 2;
				convergence = current_sol.size();
				if(neighborhood == 60) neighborhood = 5;
			}
		}
		gettimeofday(&tv, NULL);
		current_time = tv.tv_sec  - initial_time;

		if(params.verbose > 0 && current_time - show > params.verbose){
			cout << "iteration " << current_step << " best: " << best_fit << " time: " << current_time << endl;
			for(int i = 0; i < best_sol.size(); i++)
				cout << best_sol[i] << " ";
			cout << endl;
			show = current_time;
		}
	}


	cout << "Best solution: " << endl;
	for(int i = 0; i <  best_sol.size(); i++)
		cout <<  best_sol[i] << " ";
	cout << endl << "Fitness: " << best_fit << endl;

	return 0;
}


