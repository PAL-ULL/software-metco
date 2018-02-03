#ifndef COVER_EXPERIMENTS_H
#define COVER_EXPERIMENTS_H

#include <string>
#include <vector>

using namespace std;

#define PARALLEL_ANALYSIS 0
#define SEQUENTIAL_ANALYSIS 1
#define UNION_ANALYSIS 1

typedef void (*analyzerFunc)(int repetitions, int type);
typedef void (*resultAnalyzerFunc)(const string &baseFile, const vector<int> &evaluations, const int repetitions, const int type);
void analyzeExperiment(const string &expFile, analyzerFunc analyzer);
void analyzeExperimentResults(const string &expFile, const string &filter, resultAnalyzerFunc analyzer, bool generateFronts);

#endif
