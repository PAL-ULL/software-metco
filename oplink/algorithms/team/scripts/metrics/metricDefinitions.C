#include "metricDefinitions.h"

string stringValueMetric;
int intValueMetric;
vector<string> experimentFiles;
vector< pair< vector< pair<string, string> > , string > > contributions;
vector< pair< pair <string,string>, pair <string,string> > > coverages;
vector< pair < vector<pair<string,string> >, pair<string,int> > > unions;
int errorParsingMetric = 0;
string generalFrontName;
