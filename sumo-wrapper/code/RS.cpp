#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <ctime> 
#include "cInstance.hpp"
#include "simpleXMLParser.hpp"

using namespace std;

void generateSolution(vector<unsigned> &solution, const cInstance &c)
{
	solution.clear();

	// Random Values
/*	
	for(int i = 0; i < c.getTotalNumberOfPhases(); i++)
	{
		solution.push_back(rand()%56 + 4);
	}
*/
	// "Advanced" solution
	vector<string> phases;
	int pos;
	for(int j = 0; j < c.getNumberOfTLlogics(); j++)
	{
		phases = c.getPhases(j);
    solution.push_back(rand()%120);
		for(int k = 0; k < phases.size(); k++)
		{
			if(isSubString(phases[k],"y",pos))
				solution.push_back(4);		
			else
				solution.push_back(rand()%55 + 5);
		}
	}
}

void writeSolutionFile(const vector<unsigned> &solution, const cInstance &c)
{
	ofstream f("tl.txt");
	for(int i = 0; i < solution.size(); i++)
		f <<  solution[i] << " ";
	f.close();
}

void readFitnessFile(float &fitness)
{
	ifstream f("result.txt");
	string s;
	for(int i = 0; i < 6; i++) // skip lines
		getline(f,s);
	f >> fitness;
	f.close();
}

int main(int argc, char **argv)
{
	vector<unsigned> solution, best_sol;
	float fitness, best_fit;
	unsigned steps;
	string command;
	cInstance c;

	srand(time(0));

	if(argc < 3)
	{
		cout << "Usage: " << argv[0] << " <instance> <number of iterations>" << endl;
		exit(-1);
	}

	c.read(argv[1]);

	steps = atoi(argv[2]);

	command = "./sumo-wrapper " + string(argv[1]) + " . " + "tl.txt result.txt 0";

	generateSolution(solution, c);
	writeSolutionFile(solution, c);
	system(command.c_str());
	readFitnessFile(fitness);
	best_sol = solution; best_fit = fitness;

	for(int i = 1; i < steps; i++)
	{
		generateSolution(solution, c);
		writeSolutionFile(solution, c);
		system(command.c_str());
		readFitnessFile(fitness);
	cout << fitness << endl;
		if(fitness < best_fit)
		{
			best_sol = solution; 
			best_fit = fitness;
		}
	}

	cout << "Best solution: " << endl;
	for(int i = 0; i < best_sol.size(); i++)
		cout << best_sol[i] << " ";
	cout << endl << "Fitness: " << best_fit << endl;

	return 0;
}



