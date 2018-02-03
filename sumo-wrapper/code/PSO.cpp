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
typedef vector<unsigned> tSol;

// Algorithm settings
typedef struct{
  unsigned dimension;	// Size of the particleoint/individual
  unsigned swarmsize;	// Size of the swarm
  int max_steps;	// budget of evaluations
  long max_time;		// budget of time (seconds)
  long verbose;		// Number of seconds between two shows
	string dir;    // Directory with input and output files
	string inputFile; // input file (tl)
	string outputFile;  // output file (result)
  double w_min;		// minimum inertia weight
  double w_max;		// maximum inertia weight
  double local_coef;	// local coefficient
  double global_coef;	// global coefficient
  tSol x_min;		// minimum value for x[i]
  tSol x_max;		// maximum value for x[i]
} tSetting;				

// Current status of the algorithm
typedef struct{
  unsigned current_step;		// Current step
  vector<tSol> swarm;			// Current swarm
  vector<vector<double> > speed; 	// velocity of the current particle
  vector<double> swarm_fit;		// fitness of the current particless
  vector<tSol> best_pos;		// best solution found by the particle i so far
  vector<double> best_pos_fit;		// fitness of the previous best particles
  tSol leader;				// Best solution in the current swarm
  double leader_fit;			// Fitness of the leader
  tSol best;				// Best solution so far
  double best_fit;			// Fitness of the best solution 
  double current_w;			// Current inertia weight
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
	s.swarmsize = 50;
	s.w_min = 0.1;
	s.w_max = 0.5;
	s.local_coef = 2.05;
	s.global_coef = 2.05;
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

//	s.dimension = s.x_min.size();
}


// Write a solution to a tl.txt file 
void writeSolutionFile(const vector<unsigned> &solution, const tSetting &s, const cInstance &c)
{
  //string outputFile = s.dir + "tl_" + to_string(s.num_exec) + ".txt";
  string outputFile = c.getPath() + s.dir + "/" + s.inputFile;
	ofstream f(outputFile.c_str());
	for(int i = 0; i < s.dimension; i++)
		f <<  solution[i] << " ";
	f.close();
}

// Read the file returned by sumo_wrapper 
void readFitnessFile(double &fitness, const tSetting &s, const cInstance &c)
{
  //string inputFile = s.dir + "result_" + to_string(s.num_exec) + ".txt";
  string inputFile = c.getPath() + s.dir + "/" + s.outputFile;
	ifstream f(inputFile.c_str());
	string str;
	for(int i = 0; i < 6; i++) // skip lines
		getline(f,str);
	f >> fitness;
	f.close();
}

// Generate a random particle
void generateSolution(tSol &solution, vector<double> & speed, const tSetting &s)
{
	solution.clear();
	speed.clear();
	// Random Values
	for(int i = 0; i < s.dimension; i++)
	{
		solution.push_back(rand()%(s.x_max[i]-s.x_min[i]+1) + s.x_min[i]);
		if(s.x_max[i] == 4) speed.push_back(0);
		else{	    
			speed.push_back(rand()%20 - 10);
		}
	}
}

// Create the initial swarm and all related particles
void initializeAlgorithm(tStatus &a, const tSetting &s)
{
	a.swarm.clear();
	a.speed.clear();
	a.best_pos.clear();
	for(unsigned i = 0; i < s.swarmsize; i++)
	{
		tSol sol;
		vector<double> vel;
		generateSolution(sol, vel, s);
		a.swarm.push_back(sol);
		a.best_pos.push_back(sol);
		a.speed.push_back(vel);
	}
}

// Evaluate the initial swarm and modify the rest of status value
// This only works for minimization problem
void evaluateInitialSwarm(tStatus &a, const tSetting &s, string command, const cInstance &c)
{
	double fitness;
	
	a.swarm_fit.clear();
	a.best_pos_fit.clear();
	writeSolutionFile(a.swarm[0], s, c);
	//system(command.c_str());
	execCommandPipe(command);
	readFitnessFile(fitness, s, c);
	a.swarm_fit.push_back(fitness);
	a.best_pos_fit.push_back(fitness);
	a.best = a.leader = a.swarm[0];
	a.best_fit = a.leader_fit = fitness;

	for(unsigned i = 1; i < s.swarmsize; i++)
	{
		writeSolutionFile(a.swarm[i], s, c);
		//system(command.c_str());
	  execCommandPipe(command);
		readFitnessFile(fitness, s, c);
		a.swarm_fit.push_back(fitness);
		a.best_pos_fit.push_back(fitness);
		
		if(fitness < a.best_fit)
		{
			a.best = a.leader = a.swarm[i];
			a.best_fit = a.leader_fit = fitness;			
		}
	}
}

// Evaluate the current swarm and modify the rest of status value
// This only works for minimization problem
void evaluateSwarm(tStatus &a, const tSetting &s, string command, const cInstance &c)
{
	for(unsigned i = 0; i < s.swarmsize; i++)
	{
		writeSolutionFile(a.swarm[i], s, c);
		//system(command.c_str());
	  execCommandPipe(command);
		readFitnessFile(a.swarm_fit[i], s, c);
		if(a.swarm_fit[i] < a.best_pos_fit[i])
		{
			a.best_pos[i] = a.swarm[i];
			a.best_pos_fit[i] = a.swarm_fit[i];			
		}
		if(a.swarm_fit[i] < a.leader_fit)
		{
			a.leader = a.swarm[i];
			a.leader_fit = a.swarm_fit[i];			
		}
		if(a.swarm_fit[i] < a.best_fit)
		{
			a.best = a.swarm[i];
			a.best_fit = a.swarm_fit[i];			
		}		
	}
}

// Update velocity and position of the particles
void updateParticles(tStatus &a, const tSetting s)
{
	for(unsigned i = 0; i < s.swarmsize; i++)
	{
		for(unsigned j = 0; j < s.dimension; j++)
		{
			// Update velocity
			a.speed[i][j] = a.current_w*a.speed[i][j] +
							(rand()*1.0/RAND_MAX)*s.local_coef*(int(a.best_pos[i][j]) - int(a.swarm[i][j])) +
							(rand()*1.0/RAND_MAX)*s.global_coef*(int(a.leader[j]) - int(a.swarm[i][j]));
			if(rand()%2 == 0) // ceiling
				a.speed[i][j] = ceil(a.speed[i][j]);
			else	// flooring
				a.speed[i][j] = floor(a.speed[i][j]);

			//cout << a.speed[i][j] << " ";
								
			// Update position
			if(a.speed[i][j] > 0 && a.swarm[i][j] + a.speed[i][j] > s.x_max[j])
				a.swarm[i][j] = s.x_max[j];
			else if(a.speed[i][j] < 0 && (a.swarm[i][j] - s.x_min[j]) < abs(a.speed[i][j]))
				a.swarm[i][j] = s.x_min[j];
			else
				a.swarm[i][j] = a.swarm[i][j] + a.speed[i][j];
		}
		// cout << endl;
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
	
	initializeSetting(settings, c);

	initializeAlgorithm(algorithm, settings);
	gettimeofday(&tv, NULL);
//	initial_time = 1000000*tv.tv_sec + tv.tv_usec;
	initial_time = tv.tv_sec;
	show = 0;

	evaluateInitialSwarm(algorithm, settings, command, c);
	gettimeofday(&tv, NULL);
	//current_time = 1000000*tv.tv_sec + tv.tv_usec - initial_time;
	current_time = tv.tv_sec  - initial_time;

	if(settings.verbose > 0 && current_time - show > settings.verbose){
		cout << "iteration " << algorithm.current_step << " best: " << algorithm.best_fit << " leader: " << algorithm.leader_fit << " time: " << current_time << endl;
		for(int i = 0; i < settings.dimension; i++)
			cout << algorithm.best[i] << " ";
		cout << endl;
		show = current_time;
	}

	for(algorithm.current_step = 1; !stopCondition(current_time, settings.max_time, algorithm.current_step, settings.max_steps); algorithm.current_step++)
	{
		algorithm.current_w = settings.w_max - ((settings.w_max-settings.w_min)*algorithm.current_step)/settings.max_steps;
		updateParticles(algorithm,settings);
		evaluateSwarm(algorithm,settings,command, c);
		gettimeofday(&tv, NULL);
//		current_time = 1000000*tv.tv_sec + tv.tv_usec - initial_time;
		current_time = tv.tv_sec  - initial_time;

		if(settings.verbose > 0 && current_time - show > settings.verbose){
			cout << "iteration " << algorithm.current_step << " best: " << algorithm.best_fit << " leader: " << algorithm.leader_fit << " time: " << current_time << endl;
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
