#include <string>
#include <vector>
#include <iostream>

using namespace std;

#ifndef _C_INSTANCE
#define _C_INSTANCE

typedef struct{
	string id;
	vector<string> phases;
} tTL;

class cInstance
{
	public:
		cInstance();
		cInstance(string fileInput);

		~cInstance();

		bool read(string fileInput);

		string getPath() const;
		string getName() const;
		unsigned getNumberOfTLlogics() const;
		unsigned getTotalNumberOfPhases() const;
		string getTLID(unsigned position) const; // From 0 to getNumberOfTLlogics()-1
		vector<string> getPhases(unsigned position) const;
		unsigned getNumberOfVehicles() const;
		unsigned getSimulationTime() const;		

	private:
		string _path;
		string _name;
		unsigned _nTLlogics;
		unsigned _nPhases;
		vector<tTL> _phases;
		unsigned _nVehicles;
		unsigned _time;
};

ostream &operator << (ostream &os, const cInstance &i);

#endif
