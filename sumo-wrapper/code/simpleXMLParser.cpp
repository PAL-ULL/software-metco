#include "simpleXMLParser.hpp"

void getPairMap(string s, map<string, string> &m)
{

	int pos = s.find_first_of("<");
	string param, aux;
	double value;

	m.clear();

	if(pos != string::npos)
	{
		// Remove entity name
		pos = s.find_first_of(" ", pos+1);
		s.erase(0,pos+1);
	
		pos = s.find_first_of("=");
		while(pos != string::npos)
		{
			param = s.substr(0,pos);
		
			s.erase(0,pos+2);
			pos = s.find_first_of("\"");

			aux = s.substr(0,pos);

			m.insert(pair<string,string>(param,aux));

			s.erase(0,pos+2);
			pos = s.find_first_of("=");
		}
	}
}


bool isSubString(string o, string s, int &pos)
{
	pos = o.find(s);
	return (pos != string::npos);
}
