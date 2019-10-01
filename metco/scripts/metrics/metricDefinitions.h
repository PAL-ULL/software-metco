#ifndef METRIC_DEFINITIONS_H
#define METRIC_DEFINITIONS_H

#include <string>
#include <vector>

using namespace std;

extern string stringValueMetric;
extern int intValueMetric;
extern vector<string> experimentFiles;
extern vector< pair< vector< pair<string, string> > , string > > contributions;
extern vector< pair< pair <string,string>, pair <string,string> > > coverages;
extern vector< pair < vector<pair<string,string> >, pair<string,int> > > unions;
extern FILE *metricFilein;
extern int errorParsingMetric;
extern string generalFrontName;
int metricFileparse();

#endif
