#include <map>
#include <string>

using namespace std;

#ifndef _SIMPLEXMLPARSER_
#define _SIMPLEXMLPARSER_

// Given a XML entity (string with several pair params-value (params="value")), 
// it returns a map <param, value> with attributes
void getPairMap(string s, map<string, string> &m);

// Exist a substring?
bool isSubString(string o, string s, int &pos);
#endif
