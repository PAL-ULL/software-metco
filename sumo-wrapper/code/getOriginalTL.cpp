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
	ifstream fin;
	string name, line;
	int pos;
	map<string, string> m;

	if(argc < 2)
	{
		cout << "Usage: " << argv[0] << " <network file>" << endl;
		exit(-1);
	}


	name = argv[1];
	name += ".net.xml";
	fin.open(name.c_str());
	if(!fin.good())
	{
		cout << name << " doesn't exists" << endl;
		exit(-1);
	}
	getline(fin, line);
//	while(!fin.eof())
//	{

 	while(!fin.eof())
	{
				if(isSubString(line,"offset", pos))
				{
					getPairMap(line,m);
					cout << m["offset"] << " ";
				}

		if(isSubString(line,"<phase ",pos))
		{
			// get map
			getPairMap(line, m);

			cout << m["duration"] << " ";
		}
		getline(fin, line);
	}

	fin.close();

	return 0;
}
