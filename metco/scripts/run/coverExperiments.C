#include "coverExperiments.h"
#include "parseParDefinitions.h"
#include "parseSeqDefinitions.h"
#include "parseLineDefinitions.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sys/wait.h>
#include <fstream>
#include <stdlib.h>

using namespace std;

string resultFilter;
resultAnalyzerFunc resultAnalyzer;
bool generateFronts;

void analyzeParExperiment(analyzerFunc parAnalyzer){
	unsigned int i, j, l, m, n, p, q, r, s, t;
	set_variable("sendResultsPerGeneration", sendResultsPerGenerationBool?"yes":"no");
	set_variable("conversion", conversion);
	set_variable("conversionParams", conversionParams);
	set_variable("Migration_Topology", migrationTopology);
	set_variable("Migration_TopologyParams", migrationTopologyParams);
	set_variable("initPopIslandLoader", initPopIslandLoader);
	set_variable("initPopIslandLoaderParams", initPopIslandLoaderParams);
	set_variable("pluginPath", pluginPath);
	for (i = 0; i < configurations.size(); i++){
		set_variable("algFile", configurations[i]);
		for (s = 0; s < scores.size(); s++){
			set_variable("score", scores[s].moduleName);
			stringstream paramsScore;
			paramsScore.clear();
			for (int s1 = 0; s1 < scores[s].params.size(); s1++){
				paramsScore << scores[s].params[s1] << " ";
			}
			set_variable("scoreParams", paramsScore.str());
			set_variable("scoreId", scores[s].id);
			for (j = 0; j < execution_model.size(); j++){
				set_variable("execModelName", execution_model_names[j]);
				set_variable("execModelNumber", (int)j);
				for (l = 0; l < init_percent_of_individuals.size(); l++){
					set_variable("initPerc", init_percent_of_individuals[l]);
					for (m = 0; m < migration_probability.size(); m++){
						set_variable("migProb", migration_probability[m]);
						for (n = 0; n < number_of_individuals_to_migrate.size(); n++){
							set_variable("numIndMig", number_of_individuals_to_migrate[n]);
							for (p = 0; p < max_global_front_size.size(); p++){
								set_variable("maxGlobalFrontSize", max_global_front_size[p]);
								for (q = 0; q < max_final_solution_size.size(); q++){
									set_variable("maxFinalSolutionSize", max_final_solution_size[q]);
									for (r = 0; r < number_of_processors.size(); r++){
										set_variable("np", number_of_processors[r]);
										for (t = 0; t < number_parallel_executions.size(); t++){
											set_variable("numberParallelExecutions", number_parallel_executions[t]);
											(*parAnalyzer)(repetitions, PARALLEL_ANALYSIS);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	} 
}

void analyzeSeqExperiment(analyzerFunc parAnalyzer){
	//cerr << "Llama a analyzeSeqExperiment" << endl;
	vector<int> divisores;
	divisores.push_back(1);
	for (map<string, vector<string> >::iterator iter = seqParams.begin(); iter != seqParams.end(); iter++){
		divisores.push_back(divisores[divisores.size()-1] * iter->second.size());
	}
	for (int i = 0; i < divisores[divisores.size()-1]; i++){
		int j = 0;
		for (map<string, vector<string> >::iterator iter = seqParams.begin(); iter != seqParams.end(); iter++, j++){
			set_variable(iter->first, iter->second[(i / divisores[j])%iter->second.size()]);
		}
		set_variable("outputFile", outputFileSeq);
    individualName = get_variable("Individual");
		//cerr << "Llama al analizador" << endl << flush;
		(*parAnalyzer)(repetitions, SEQUENTIAL_ANALYSIS);
		//cerr << "Vuelve del analizador" << endl << flush;
	}
	//cerr << "Termina analyzeSeqExperiment" << endl;
}

void analyzeExperiment(const string &expFile, analyzerFunc analyzer){
	//cerr << "Llama a analyzeExperiment" << endl;
	bool parallel;
	resetParValues();
	expParin = fopen(expFile.c_str(), "r");
	if (expParin == 0){
		cout << "No se pudo abrir el fichero " << expFile << endl;
		exit(-1);
	}
	expParrestart(expParin);
	expParparse();
	fclose(expParin);
	if (errorParsing){//can be a sequential experiment
		parallel = false;
		resetSeqValues();
		expSeqin	= fopen(expFile.c_str(), "r");
		expSeqparse();
		fclose(expSeqin);
		if (errorParsing){
			cout << "Error parsing " << expFile << endl;
			exit(-1);
		}
	} else {
		parallel = true;
	}
	set_variable("expName", name_of_experiment);
	if (parallel){
		checkParCorrectValues();
		analyzeParExperiment(analyzer);
	} else {
		checkSeqCorrectValues();
		analyzeSeqExperiment(analyzer);
	}
	//cerr << "Termina analyzeExperiment" << endl;
}

//create fronts and calculate nEvaluations
void createFrontsFiles(const string &file, vector<int> &nEvaluations){
	stringstream executionLine;
	cout << "Creando para " << file << endl;
	executionLine << "./output2fronts.pl " << file << " " << file; 
	FILE *output2fronts = popen(executionLine.str().c_str(), "r");
	if (output2fronts == NULL){
		cout << "error executing ./output2fronts.pl" << endl;
		exit(-1);
	}
	if (wait(NULL) == -1){//wait termination
		cout << "Error waiting child termination" << endl;
		exit(-1);
	}

	nEvaluations.clear();
	int difEvaluations;
	fscanf(output2fronts, "%d", &difEvaluations);
	for (int i = 0; i < difEvaluations; i++){
		int nEval;
		fscanf(output2fronts, "%d", &nEval);
		nEvaluations.push_back(nEval);
	}
	//OJO: el ultimo no lo consideramos
	nEvaluations.pop_back();
	pclose(output2fronts);
}

//create fronts for all the repetitions of the file (basename) 
void createFronts(const string &file, int repetitions, vector<int> &nEvaluations){
	stringstream completeFile;
	for (int rep = 0; rep < repetitions; rep++){
		completeFile.str("");
		completeFile << file << "." << rep;
		createFrontsFiles(completeFile.str(), nEvaluations);
	}
}

void fillEvaluations(const string &file, vector<int> &nEvaluations){
	ifstream evalFile((file + ".0.evaluations").c_str());
	if (!evalFile.is_open()){
		cout << "File " << file << ".0.evaluations could not be opened" << endl;
		exit(-1);
	}
  int nEval;
	evalFile >> nEval;
	for (int i = 0; i < nEval; i++){
		int val; 
		evalFile >> val;
		nEvaluations.push_back(val);
	}
	//OJO: la ultima no la consideramos
	nEvaluations.pop_back();
	evalFile.close();
}

void parFrontGenerator(int repetitions){
	vector<int> nEvaluations;
	char *tmp = new char[get_variable("algFile").length() + 1];
	strcpy(tmp, get_variable("algFile").c_str());
	stringstream outputFile;
	outputFile.str("");
	outputFile << "experimentsResults/" << name_of_experiment << "/";
	outputFile << get_variable("expName") << "_" << basename(tmp) << "_" << get_variable("np") << "_" << get_variable("execModelName") << "_" << get_variable("scoreId") << "_" << get_variable("initPerc") << "_" << get_variable("migProb") << "_" << get_variable("numIndMig") << "_" << get_variable("maxGlobalFrontSize") << "_" << get_variable("maxFinalSolutionSize");
	if ((resultFilter == "*") || (resultFilter == outputFile.str())){
		if (generateFronts){
			createFronts(outputFile.str(), repetitions, nEvaluations);
		} else
			fillEvaluations(outputFile.str(), nEvaluations);
		resultAnalyzer(outputFile.str(), nEvaluations, repetitions, PARALLEL_ANALYSIS);
	} 
	delete(tmp);
}


void seqFrontGenerator(int repetitions){
	vector<int> nEvaluations;
	line_parser(outputFileSeq.c_str(), "OutputFile");
	if ((resultFilter == "*") || (resultFilter == line_parsed.str())){
		if (generateFronts){
			//cerr << "Llama a createFronts" << endl;
			createFronts(line_parsed.str(), repetitions, nEvaluations);
			//cerr << "Termina createFronts" << endl;
		} else
			fillEvaluations(line_parsed.str(), nEvaluations);
		//cerr << "Llama a resultAnalyzer" << endl;
		resultAnalyzer(line_parsed.str(), nEvaluations, repetitions, SEQUENTIAL_ANALYSIS);
		//cerr << "Vuelve de resultAnalyzer" << endl;
	}
}

void frontGenerator(int repetitions, int type){
	//cerr << "Front Generator llamado" << endl;
	if (type == PARALLEL_ANALYSIS){
		parFrontGenerator(repetitions);
	} else {
		seqFrontGenerator(repetitions);
	}
	//cerr << "Front Generator termina" << endl;
}

void analyzeUnion(string baseFile){
	vector<int> nEvaluations;
	ifstream evals((baseFile + ".0.evaluations").c_str());
	if (!evals.is_open()){
		cout << baseFile << ".0.evaluations could not be opened" << endl;
		exit(-1);
	}
	int nEvals;
	evals >> nEvals;
	for (int i = 0; i < nEvals; i++){
		int v;
		evals >> v;
		nEvaluations.push_back(v);
	}
	// OJO: la ultima no la consideramos
	nEvaluations.pop_back();
	evals.close();
	ifstream repFile((baseFile + ".repetitions").c_str());
	int rep;
	if (!repFile.is_open()){
		cout << baseFile << ".repetitions could not be opened" << endl;
		exit(-1);
	}
	repFile >> rep;
	repFile.close();
	resultAnalyzer(baseFile, nEvaluations, repetitions, UNION_ANALYSIS);
}

void analyzeExperimentResults(const string &expFile, const string &filter, resultAnalyzerFunc analyzer, bool create){
	generateFronts = create;
	resultFilter = filter;
	resultAnalyzer = analyzer;
	if (expFile == "UNION")
		analyzeUnion(filter);
	else
		analyzeExperiment(expFile, frontGenerator);
}
