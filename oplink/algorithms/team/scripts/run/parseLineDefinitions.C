#include "parseLineDefinitions.h"
#include "parseDefinitions.h"

#include <sstream>
#include <iostream>
#include <stdlib.h>

using namespace std;

void set_variable(const string &key, const int &value){
	stringstream converter;
	variables.erase(key);
	converter << value;
	variables.insert(make_pair(key, converter.str()));
}

void set_variable(const string &key, const double &value){
	stringstream converter;
	variables.erase(key);
	converter << value;
	variables.insert(make_pair(key, converter.str()));
}

void set_variable(const string &key, const string &value){
	stringstream converter;
	variables.erase(key);
	converter << value;
	variables.insert(make_pair(key, converter.str()));
}

string get_variable(const string &key){
	return variables[key];
}

void line_parser(const char *line, const char *error){
	line_parsed.str("");
	struct yy_buffer_state *buffer = Line_scan_string(line);
	Lineparse();
	Line_delete_buffer(buffer);
	if (errorParsing != 0){
		cout << "Error parsing " << error << endl;
		exit(-1);
	}
}

map < string, string, LessThan_casecmp> variables;
stringstream line_parsed;
