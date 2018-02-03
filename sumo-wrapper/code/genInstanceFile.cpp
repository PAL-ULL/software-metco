#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <cstdlib>
#include "simpleXMLParser.hpp"

using namespace std;

// Convert a number to string
string to_string(unsigned long v)
{
	stringstream ss;
	ss << v;

	return ss.str();
}

int main(int argc, char **argv)
{
	ofstream fout;
	ifstream fin;
	string name, line, gPhase ="", gFile = "";
	unsigned nTL = 0, nPhase = 0, nPhaseTL = 0, nVeh = 0;
	string id;
	int pos;
	map<string, string> m;

	if(argc < 3)
	{
		cout << "Usage: " << argv[0] << " <path> <instance name> <simulation time>" << endl;
		exit(-1);
	}

	name = argv[2];
	name += ".txt";
	fout.open(name.c_str());
	fout << argv[2] << endl;
	fout << argv[1] << endl;

	name = argv[1];
	name += argv[2];
	name += ".net.xml";
	fin.open(name.c_str());
	if(!fin.good())
	{
		cout << name << " doesn't exists" << endl;
		exit(-1);
	}
	getline(fin, line);
	while(!fin.eof())
	{
		if(isSubString(line,"<tlLogic ",pos))
		{
			// get map
			getPairMap(line, m);

			id = m["id"]; 
			nTL++;
			gPhase = "";
			nPhaseTL = 0;
		}
		else if(isSubString(line,"<phase ",pos))
		{
			// get map
			getPairMap(line, m);

			gPhase += " " +m["state"]; 
			nPhase++; 
			nPhaseTL++;
		}
		else if(isSubString(line,"</tlLogic>",pos))
		{
			gFile += id + " " + to_string(nPhaseTL) + gPhase + " \n";
		}
		getline(fin, line);
	}

	fin.close();

	fout << nTL << endl << nPhase << endl << gFile;

	name = argv[1];
	name += argv[2];
	name += ".rou.xml";
	fin.open(name.c_str());
	if(!fin.good())
	{
		cout << name << " doesn't exists" << endl;
		exit(-1);
	}

	getline(fin, line);
	while(!fin.eof())
	{
		if(isSubString(line,"</vehicle>",pos))
		{
			nVeh++;
		}
		else if(isSubString(line,"<flow ",pos))
		{
			// get map
			getPairMap(line, m);

			nVeh += atoi(m["number"].c_str()); 
		}
		getline(fin, line);
	}
	fin.close();

	fout << nVeh << endl << argv[3] << endl;

	fout.close();

	return 0;
}
