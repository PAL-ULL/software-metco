#include "cInstance.hpp"
#include <fstream>

using namespace std;

cInstance::cInstance()
{}

cInstance::cInstance(string fileInput)
{
	this->read(fileInput);
}

cInstance::~cInstance()
{}

string cInstance::getPath() const
{
	return _path;
}

string cInstance::getName() const
{
	return _name;
}

unsigned cInstance::getNumberOfTLlogics() const
{
	return _nTLlogics; // This number should be the same to _phases.size()
}

unsigned cInstance::getTotalNumberOfPhases() const
{
	return _nPhases;
}
string cInstance::getTLID(unsigned position) const // From 0 to getNumberOfTLlogics()-1
{
	if(position < _phases.size())
		return _phases[position].id;
	else
		return "";
}
vector<string> cInstance::getPhases(unsigned position) const
{
	vector<string> v;
	if(position < _phases.size())
		return _phases[position].phases;
	else
		return v;
}

unsigned cInstance::getNumberOfVehicles() const
{
	return _nVehicles;
}
unsigned cInstance::getSimulationTime() const
{
	return _time;
}

bool cInstance::read(string fileInput)
{
	ifstream fin(fileInput.c_str());
	unsigned phases_per_tl;
	string phase;
	tTL tl;
	bool ok = false;

	if(fin.good())
	{
		getline(fin, _name);
		getline(fin, _path);
		fin >> _nTLlogics;
		fin >> _nPhases;

		for(unsigned i = 0; i < _nTLlogics; i++)
		{
			fin >> tl.id;
			fin >> phases_per_tl;
			tl.phases.clear();
			for(unsigned j = 0; j < phases_per_tl; j++)
			{
				fin >> phase;
				tl.phases.push_back(phase);
			}	
			_phases.push_back(tl);
		}
		
		fin >> _nVehicles;
		fin >> _time;
		ok = true;

		fin.close();
	}

	return ok;
}


ostream &operator << (ostream &os,const cInstance &i)
{
	vector<string> phases;
	os << "Instance name: " << i.getName() << endl;
	os << "Path: " << i.getPath() << endl;
	os << "Number of TL logics: " << i.getNumberOfTLlogics() << endl;
	os << "Total Number of Phases: " << i.getTotalNumberOfPhases() << endl;
	os << "TL Phases: " << endl;
	for(int j = 0; j < i.getNumberOfTLlogics(); j++)
	{
		cout << "\t ID: " << i.getTLID(j) << " - ";
		phases = i.getPhases(j);
		for(int k = 0; k < phases.size(); k++)
			cout << phases[k] << " ";
		cout << endl;
	}
	os << "Number of Vehicles: " << i.getNumberOfVehicles() << endl;
	os << "Simulation time: " << i.getSimulationTime() << endl;

	return os;
}
