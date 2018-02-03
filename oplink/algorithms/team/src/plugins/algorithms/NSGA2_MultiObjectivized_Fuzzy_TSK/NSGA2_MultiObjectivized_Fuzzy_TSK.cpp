/***********************************************************************************
 * AUTHORS
 *   - Eduardo Segredo González
 *   - Carlos Segura González
 * 
 * DATE 
 *   May 2013
 * *********************************************************************************/


#include "NSGA2_MultiObjectivized_Fuzzy_TSK.h"
#include <float.h>


// Define una generación de búsqueda del algoritmo 
void NSGA2_MultiObjectivized_Fuzzy_TSK::runGeneration() {

	// Evaluate the initial population
	if (getGeneration() == 0) {
		multiObjectivize(population);
		rankPopulation();
	}
	// Push the children at the end of the population
	createChildPop();
	//MultiObjetivizar
	multiObjectivize(offSpring);
	//Fill External Archive
	fillExternalArchive();
	// Calculate the fitness as the rank of each element
	rankCrowdingPopulation();
}
	

// Inicializa los parámetros iniciales del algoritmo
bool NSGA2_MultiObjectivized_Fuzzy_TSK::init(const vector<string> &params) {
	
	// Check for the algorithm parameters
	if (params.size() != 5) {
		cerr << "Error NSGA-II: Incorrect parameters (pc pSize delta numGenToFuzzify k)" << endl;
    cerr << params.size() << endl;
		return false;
	}

	// Initiate the parameters
	this->pc = atof(params[0].c_str());
	setPopulationSize(atoi(params[1].c_str()));
  this->delta = atof(params[2].c_str());
  this->numGenToFuzzify = atoi(params[3].c_str());
  this->k = atoi(params[4].c_str());
	offSpring = new vector<Individual *>();

  if (delta <= 0) {
    cerr << "Error NSGA-II: Parameter delta must be greater than 0" << endl;
    return false;
  }

  if (numGenToFuzzify <= 0) {
    cerr << "Error NSGA-II: Parameter numGenToFuzzify must be greater than 0" << endl;
    return false;
  }
  
  if (k <= 0) {
    cerr << "Error NSGA-II: Parameter k must be greater than 0" << endl;
    return false;
  }

  long double value = 0;
  for (int i = 0; i <= (int)((1.0 / delta) + 1.0); i++) {
    initStageValues.push_back(value);
    value += delta;
  }
  
  initStage = true;

  cout << "initStageValues: [";
  for (int i = 0; i < initStageValues.size(); i++) {
    cout << initStageValues[i] << ", ";
  }
  cout << "]" << endl;

  cout << "initStageValues size: " << initStageValues.size() << endl;
  int r = rand();
  cout << "rand: " << r << endl;
  int i = r % initStageValues.size();
  cout << "i: " << i << endl;
  pm = initStageValues[i];
  initStageValues[i] = initStageValues[initStageValues.size() - 1];
  initStageValues.pop_back();
 
  // Definition of the Fuzzy System
  // Engine
  fuzzyEngine = new fl::Engine("fuzzyEngine");

  // Input variables
  fit = new fl::InputVariable;
  fit->setName("FIT");
  fit->setRange(0.0, 1.0);
  fit->addTerm(new fl::Triangle("LOW", -0.5, 0, 0.5));
  fit->addTerm(new fl::Triangle("MEDIUM", 0, 0.5, 1));
  fit->addTerm(new fl::Triangle("HIGH", 0.5, 1, 1.5));
  fuzzyEngine->addInputVariable(fit);

  pmIn = new fl::InputVariable;
  pmIn->setName("PM_IN");
  pmIn->setRange(0.0, 1.0);
  pmIn->addTerm(new fl::Triangle("LOW", -0.16667, 0, 0.16667));
  pmIn->addTerm(new fl::Triangle("LOW_MED_B", 0, 0.16667, 0.33333));
  pmIn->addTerm(new fl::Triangle("LOW_MED_A", 0.16667, 0.33333, 0.5));
  pmIn->addTerm(new fl::Triangle("MEDIUM", 0.33333, 0.5, 0.66667));
  pmIn->addTerm(new fl::Triangle("MED_HIGH_A", 0.5, 0.66667, 0.83333));
  pmIn->addTerm(new fl::Triangle("MED_HIGH_B", 0.66667, 0.83333, 1));
  pmIn->addTerm(new fl::Triangle("HIGH", 0.83333, 1, 1.16667));
  fuzzyEngine->addInputVariable(pmIn);

  var = new fl::InputVariable;
  var->setName("VAR");
  var->setRange(0.0, 1.0);
  var->addTerm(new fl::Triangle("LOW", -0.5, 0, 0.5));
  var->addTerm(new fl::Triangle("MEDIUM", 0, 0.5, 1));
  var->addTerm(new fl::Triangle("HIGH", 0.5, 1, 1.5));
  fuzzyEngine->addInputVariable(var);

	// Output Variables
	pmOut = new fl::OutputVariable;
  pmOut->setName("PM_OUT");
	pmOut->setRange(-0.45, 0.45);
	pmOut->addTerm(new fl::Constant("NEG_GIANT", -0.45));
	pmOut->addTerm(new fl::Constant("NEG_HUGE", -0.36));
	pmOut->addTerm(new fl::Constant("NEG_HIGH", -0.27));
	pmOut->addTerm(new fl::Constant("NEG_MEDIUM", -0.18));
	pmOut->addTerm(new fl::Constant("NEG_LOW", -0.09));
	pmOut->addTerm(new fl::Constant("ZERO", 0));
	pmOut->addTerm(new fl::Constant("POS_LOW", 0.09));
	pmOut->addTerm(new fl::Constant("POS_MEDIUM", 0.18));
	pmOut->addTerm(new fl::Constant("POS_HIGH", 0.27));
	pmOut->addTerm(new fl::Constant("POS_HUGE", 0.36));
	pmOut->addTerm(new fl::Constant("POS_GIANT", 0.45));
	fuzzyEngine->addOutputVariable(pmOut);

	// Seven different fuzzy rules sets
	fl::RuleBlock* rules_low = new fl::RuleBlock;
	fl::RuleBlock* rules_low_med_b = new fl::RuleBlock;
	fl::RuleBlock* rules_low_med_a = new fl::RuleBlock;
	fl::RuleBlock* rules_medium = new fl::RuleBlock;
	fl::RuleBlock* rules_med_high_a = new fl::RuleBlock;
	fl::RuleBlock* rules_med_high_b = new fl::RuleBlock;
	fl::RuleBlock* rules_high = new fl::RuleBlock;

	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is LOW and VAR is LOW then PM_OUT is POS_LOW", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is LOW and VAR is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is LOW and VAR is HIGH then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is MEDIUM then PM_OUT is ZERO", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is LOW then PM_OUT is NEG_MEDIUM", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is LOW then PM_OUT is NEG_HIGH", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is LOW then PM_OUT is NEG_HUGE", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is LOW then PM_OUT is NEG_GIANT", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is LOW then PM_OUT is NEG_GIANT", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is LOW then PM_OUT is NEG_GIANT", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));

	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is LOW then PM_OUT is POS_MEDIUM", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is LOW and VAR is LOW then PM_OUT is POS_LOW", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is LOW and VAR is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is LOW and VAR is HIGH then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is MEDIUM then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is LOW then PM_OUT is NEG_MEDIUM", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is LOW then PM_OUT is NEG_HIGH", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is LOW then PM_OUT is NEG_HUGE", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is LOW then PM_OUT is NEG_GIANT", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is LOW then PM_OUT is NEG_GIANT", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is LOW then PM_OUT is POS_HIGH", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is LOW then PM_OUT is POS_MEDIUM", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is LOW and VAR is LOW then PM_OUT is POS_LOW", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is LOW and VAR is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is LOW and VAR is HIGH then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is MEDIUM then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is LOW then PM_OUT is NEG_MEDIUM", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is LOW then PM_OUT is NEG_HIGH", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is LOW then PM_OUT is NEG_HUGE", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is LOW then PM_OUT is NEG_GIANT", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));

	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is LOW then PM_OUT is POS_HUGE", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is LOW then PM_OUT is POS_HIGH", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is LOW then PM_OUT is POS_MEDIUM", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is LOW and VAR is LOW then PM_OUT is POS_LOW", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is LOW and VAR is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is LOW and VAR is HIGH then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is MEDIUM then PM_OUT is ZERO", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is LOW then PM_OUT is NEG_MEDIUM", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is LOW then PM_OUT is NEG_HIGH", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is LOW then PM_OUT is NEG_HUGE", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is LOW then PM_OUT is POS_GIANT", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is LOW then PM_OUT is POS_HUGE", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is LOW then PM_OUT is POS_HIGH", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is LOW then PM_OUT is POS_MEDIUM", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is LOW and VAR is LOW then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is LOW and VAR is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is LOW and VAR is HIGH then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is MEDIUM then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is LOW then PM_OUT is NEG_MEDIUM", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is LOW then PM_OUT is NEG_HIGH", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));

	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is LOW then PM_OUT is POS_GIANT", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is LOW then PM_OUT is POS_GIANT", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is LOW then PM_OUT is POS_HUGE", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is LOW then PM_OUT is POS_HIGH", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is LOW then PM_OUT is POS_MEDIUM", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is LOW and VAR is LOW then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is LOW and VAR is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is LOW and VAR is HIGH then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is MEDIUM then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is LOW then PM_OUT is NEG_MEDIUM", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is MEDIUM then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));

	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is LOW then PM_OUT is POS_GIANT", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is LOW and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is LOW then PM_OUT is POS_GIANT", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is LOW then PM_OUT is POS_GIANT", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is LOW_MED_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is LOW then PM_OUT is POS_HUGE", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is MEDIUM and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is LOW then PM_OUT is POS_HIGH", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_A and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is LOW then PM_OUT is POS_MEDIUM", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is MED_HIGH_B and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is LOW and VAR is LOW then PM_OUT is POS_LOW", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is LOW and VAR is MEDIUM then PM_OUT is POS_LOW", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is LOW and VAR is HIGH then PM_OUT is NEG_LOW", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is MEDIUM then PM_OUT is ZERO", fuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if PM_IN is HIGH and FIT is HIGH then PM_OUT is ZERO", fuzzyEngine));
 	
	fuzzyEngine->addRuleBlock(rules_low);
	fuzzyEngine->addRuleBlock(rules_low_med_b);
	fuzzyEngine->addRuleBlock(rules_low_med_a);
	fuzzyEngine->addRuleBlock(rules_medium);
	fuzzyEngine->addRuleBlock(rules_med_high_a);
	fuzzyEngine->addRuleBlock(rules_med_high_b);
	fuzzyEngine->addRuleBlock(rules_high);

  fuzzyEngine->configure("Minimum", "Maximum", "Minimum", "", "WeightedAverage");
	
	improvements = vector< vector<long double> > (pmIn->numberOfTerms(), vector<long double>());
	memberships = vector< vector<long double> > (pmIn->numberOfTerms(), vector<long double>());
	weightedImprovements = vector<long double>(pmIn->numberOfTerms(), 0.0);

	return true;
}


// Rellena un frente con las soluciones actuales
void NSGA2_MultiObjectivized_Fuzzy_TSK::getSolution(MOFront *p) {
	//Look for the best one in the original objective
	int bestIndex = 0;
	double currentBestValue = (*population)[0]->getObj(0);
	//Select the individual with best "original" objective
	for (int i = 1; i < population->size(); i++){
		if ( ((*population)[0]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < currentBestValue)){
			bestIndex = i;
			currentBestValue = (*population)[i]->getObj(0);
		} else if ( ((*population)[0]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > currentBestValue)){
			bestIndex = i;	
			currentBestValue = (*population)[i]->getObj(0);
		}
	}
	p->insert((*population)[bestIndex]);

	//for (unsigned int i = 0;  i < population->size(); i++) {
	//	p->insert((*population)[i]);
	//}
}


// Muestra la información relativa al algoritmo
void NSGA2_MultiObjectivized_Fuzzy_TSK::printInfo(ostream &os) const {
	os << "Genetic Algorithm NSGA2_MultiObjectivized_Fuzzy_TSK" << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Mutation Probability = " << pm << endl;
	os << "Crossover Probability = " << pc << endl;
	os << "Population Size = " << getPopulationSize() << endl;
}


/*******************************************************************************
 * Los siguientes métodos son específicos del NSGA-II y describen las operaciones
 * internas y el propio funcionamiento del algoritmo.
 * *****************************************************************************/

// NSGA-II crowding operator 


/*************************************************************************
 * Create fronts using the non-domination rank until pSize elements have
 * been selected.
 * Each front is ordered using the local crowding distance.
 * In: population without any order with size = pSize * 2
 * Exit: population ordered using the crowded comparison operator with size pSize
 * ***********************************************************************/
void NSGA2_MultiObjectivized_Fuzzy_TSK::rankCrowdingPopulation() {
	int inserted = 0;

	vector < vector < Individual * > > fronts;
	vector < Individual *> notClassified;

	//Calculate fronts
	int differentsInFront0;
	for (int i = 0; i < offSpring->size(); i++){
		population->push_back((*offSpring)[i]);
	}
	FastNondominatedSort(population, fronts, notClassified, differentsInFront0, getPopulationSize());

	// Order each front using the local crowding distance 
	for (int i = 0; i < fronts.size(); i++)
		crowOrder(fronts[i]);

	//Create the ordered population
	population->clear();
	for (int i = 0; i < fronts.size() - 1; i++){
		for (int j = 0; j < fronts[i].size(); j++){
			population->push_back(fronts[i][j]);
		}
	}
	
	//Last front
	for (int j = 0; j < fronts[fronts.size() - 1].size(); j++){
		if (population->size() < (unsigned int) getPopulationSize()){
			population->push_back((fronts[fronts.size() - 1][j]));
		} else {
			delete (fronts[fronts.size() - 1][j]);
		}
	}

	//Delete not used individuals
	for (int i = 0; i < notClassified.size(); i++){
		delete (notClassified[i]);
	}
}


// Sort and rank the population
// Population has size N, it reorders the population based on the 
void NSGA2_MultiObjectivized_Fuzzy_TSK::rankPopulation() {
	vector < vector < Individual * > > fronts;
	vector < Individual *> notClassified;
	int differentsInFront0 = 0;
	FastNondominatedSort(population, fronts, notClassified, differentsInFront0, population->size());
	population->clear();
	for (int i = 0; i < fronts.size(); i++){
		for (int j = 0; j< fronts[i].size(); j++){
			population->push_back(fronts[i][j]);
		}
	}
}


/*******************************************************************************
 * Create a child population by applying:
 *  - Selection
 *  - Crossover
 *  - Recombination
 * *****************************************************************************/
void NSGA2_MultiObjectivized_Fuzzy_TSK::createChildPop() {
	int tamPop = population->size();
	vector<int> a1;
	
	// Se genera un vector con los numeros de 0 a tamPop-1 (2 veces)
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < tamPop; j++) {
			a1.push_back(j);
		}
	}
	
	// Se reordena la primera mitad
	for (int i = 0; i < tamPop; i++) {
		int rand_value = i + (int) (((double)(tamPop-i))*rand()/(RAND_MAX+1.0));
		int tmp = a1[rand_value];
		a1[rand_value] = a1[i];
		a1[i] = tmp;
	}
	
	// Reodenamos la segunda mitad
	for (int i = tamPop; i < tamPop*2; i++) {
		int rand_value = i + (int) (((double)(2*tamPop-i))*rand()/(RAND_MAX+1.0));
		int tmp = a1[rand_value];
		a1[rand_value] = a1[i];
		a1[i] = tmp;
	}

	offSpring->clear();

	for (int i = 0; i < (tamPop*2)-((tamPop*2) % 4); i += 4){
		// Selection
		int ind1 = a1[i];
		int ind2 = a1[i+1];
		// Binary tournment
		if (ind1 < ind2){
			offSpring->push_back((*population)[ind1]->internalClone());
		} else{
			offSpring->push_back((*population)[ind2]->internalClone());
		}
		int ind3 = a1[i+2];
		int ind4 = a1[i+3];
		if (ind3 < ind4){
			offSpring->push_back((*population)[ind3]->internalClone());
		} else {
			offSpring->push_back((*population)[ind4]->internalClone());
		}
		
		// Crossover
		double vcross = rand() / (RAND_MAX + 1.0);
		if (vcross < pc) {  
			(*offSpring)[offSpring->size()-2]->crossover((*offSpring)[offSpring->size()-1]);
		}
 
		// Mutation
		(*offSpring)[offSpring->size()- 2]->mutation(pm);
		(*offSpring)[offSpring->size()- 1]->mutation(pm);
		evaluate((*offSpring)[offSpring->size()-2]);
		evaluate((*offSpring)[offSpring->size()-1]);
	}
	
	// If population is odd one more must be inserted
	if (tamPop % 2) { 
		int ind1 = a1[a1.size()- 4 ];
		int ind2 = a1[a1.size() - 3];
		Individual *tmp;
		
		// Binary tournment
		if (ind1 < ind2) {
			tmp = (*population)[ind1]->internalClone();
		} else{
			tmp = (*population)[ind2]->internalClone();
		}
		int ind3 = a1[a1.size()-2];
		int ind4 = a1[a1.size()-1];
		if (ind3 < ind4) {
			offSpring->push_back((*population)[ind3]->internalClone());
		} else {
			offSpring->push_back((*population)[ind4]->internalClone());
		}
		
		// Crossover
		double vcross = rand() / (RAND_MAX + 1.0);
		if (vcross < pc) {  
			(*offSpring)[offSpring->size()-1]->crossover(tmp);
		}
		delete(tmp);
		
		// Mutation
		(*offSpring)[offSpring->size()- 1]->mutation(pm);
		evaluate((*offSpring)[offSpring->size()-1]);
	}

  // The fuzzy logic controller infers the next value for the mutation rate pm

  //Looks for best individual (obj[0])
	Individual *best = lookForBestIndividual();
	if (best == NULL) {
	 cerr << "Error: offSpring and population has a size equal to 0" << endl;
	 exit(-1);
	}
	if (getGeneration() == 0) {
		prevBestFitness = best->getObj(0);
    cout << "Generation: " << getGeneration() << endl;
    cout << "pm: " << pm << endl;
    cout << "prevBestFitness: " << prevBestFitness << endl;
	}

	if ((getGeneration() + 1) % numGenToFuzzify == 0) {
		
		// Updates the history of the mutation rate
		pmHistory.push_back(pm);

		// Calculates the improvement and updates the history of improvements
		best = lookForBestIndividual();
		if (best == NULL) {
			cerr << "Error: offSpring and population has a size equal to 0" << endl;
			exit(-1);
		}

    cout << "Generation: " << getGeneration() << endl;
    cout << "pm: " << pm << endl;
    cout << "prevBestFitness: " << prevBestFitness << endl;
    cout << "bestFitness: " << best->getObj(0) << endl;

		long double currentImprovement = fabs(prevBestFitness - best->getObj(0));
		improvementHistory.push_back(currentImprovement);
		prevBestFitness = best->getObj(0);

    cout << "currentImprovement: " << currentImprovement << endl;

		// Calculates the diversity of the population (moment of inertia)
		long double currentMoment = 0;
		if (population != offSpring) {
			for (int i = 0; i < best->getNumberOfVar(); i++) {
				// Calculates the centroid
				long double sum = 0;
				for (int j = 0; j < population->size(); j++) {
					sum += (*population)[j]->getVar(i);
				}
				for (int j = 0; j < offSpring->size(); j++) {
					sum += (*offSpring)[j]->getVar(i);
				}
				long double centroid = sum / (long double)(population->size() + offSpring->size());

				sum = 0;
				for (int j = 0; j < population->size(); j++) {
					long double aux = (*population)[j]->getVar(i) - centroid;
					sum += (aux * aux);
				}
				for (int j = 0; j < offSpring->size(); j++) {
					long double aux = (*offSpring)[j]->getVar(i) - centroid;
					sum += (aux * aux);
				}
				currentMoment += sum;
			}
		}
		// Updates the history of moments
    momentHistory.push_back(currentMoment);

		// Calculates the improvements and the memberships
		for (int i = 0; i < pmIn->numberOfTerms(); i++) {
			long double currentMembership = (long double)pmIn->getTerm(i)->membership((fl::scalar)pm);
			if (currentMembership != 0) {
				improvements[i].push_back(currentImprovement);
				memberships[i].push_back(currentMembership);
			}
		}
		
		// Calculates the weigthed improvements
		for (int i = 0; i < pmIn->numberOfTerms(); i++) {
			long double num = 0;
			long double denom = 0;

			int lowerbound = (improvements[i].size() <= k)? 0 : improvements[i].size() - k;

			for (int j = lowerbound; j < improvements[i].size(); j++) {
					num += (improvements[i][j] * memberships[i][j] * (j - lowerbound + 1));
					denom += (memberships[i][j] * (j - lowerbound + 1));
			}
			if (denom != 0) {
				weightedImprovements[i] = (num / denom);
			}
		}

	  cout << "weightedImprovements = (";
    for (int i = 0; i < weightedImprovements.size(); i++) {
      cout << weightedImprovements[i] << ", ";
    }
    cout << ")" << endl;

		// Input variable: pmIn
		fl::scalar pmInValue = (fl::scalar)pm;
		pmIn->setInput(pmInValue);

		// Input variable: fit
		fl::scalar fitValue = (currentImprovement != 0)? (fl::scalar)getFitValue() : 0;
		fit->setInput(fitValue);
		
		// Input Variable: varValue
		fl::scalar varValue = (currentMoment != 0)? (fl::scalar)getVarValue() : 0;
		var->setInput(varValue);

		// Looks for the most suitable set of rules to be applied
		long double max = 0;
		int maxIndex = 0;
		for (int i = 0; i < weightedImprovements.size(); i++) {
			if (weightedImprovements[i] > max) {
				max = weightedImprovements[i];
				maxIndex = i;
			}
		}

    // Clean every output variable
		for (std::size_t i = 0; i < fuzzyEngine->numberOfOutputVariables(); ++i) {
			fuzzyEngine->getOutputVariable(i)->output()->clear();
		}

		// Executes the fuzzy system with the most suitable set of rules
		fuzzyEngine->getRuleBlock(maxIndex)->fireRules();

		// Output variable: pmOutValue
		fl::scalar pmOutValue;
		pmOutValue = pmOut->defuzzify();

    cout << "SET OF RULES=" << maxIndex << endl;
    cout << "PM_IN=" << pmInValue << endl;
    cout << "FIT=" << fitValue << endl;
    cout << "VAR=" << varValue << endl;
    cout << "PM_IN is " << pmIn->fuzzify(pmInValue) << endl;
    cout << "FIT is " << fit->fuzzify(fitValue) << endl;
    cout << "VAR is " << var->fuzzify(varValue) << endl;
    cout << "PM_OUT=" << pmOutValue << endl;
    cout << "PM_OUT is " << pmOut->fuzzify(pmOutValue) << endl;
    cout << "--" << endl;
	
    // Stops the initialization stage
	  if ((initStage) && (initStageValues.size() == 0)) {
		  initStage = false;
	  }
	
		// Updates the mutation rate
		if (initStage) {
			// Obtains a random value from the initStageValues vector
			int i = rand() % initStageValues.size();
			long double value = initStageValues[i];
			pm = value;
			
			// Removes the selected value for the next iteration
			initStageValues[i] = initStageValues[initStageValues.size() - 1];
			initStageValues.pop_back();
			cout << "pm actualizado en fase inicial: " << pm << endl;
		} else {
			pm += pmOutValue;
			if (pm > 1) {
				pm = 1;
			} else if (pm < 0) {
				pm = 0;
			}
			cout << "pm actualizado con pmOutValue: " << pm << endl;
		}
	}
}

void NSGA2_MultiObjectivized_Fuzzy_TSK::fillExternalArchive(){
	for (int i = 0; i < offSpring->size(); i++){
		insertInArchive((*offSpring)[i]);
	}
}
void NSGA2_MultiObjectivized_Fuzzy_TSK::multiObjectivize(vector <Individual*> *updatePop){
	int start = getSampleInd()->getNumberOfObj() - getNumMultiObjectivizationPlugins();
	for (int i = start; i < getSampleInd()->getNumberOfObj(); i++){
		getMultiObjectivizationPlugin(i - start)->convertToMultiObjective(population, updatePop, NULL, i, getGeneration());
	}
}

void NSGA2_MultiObjectivized_Fuzzy_TSK::exchangePerformed(){
	multiObjectivize(population);	
}

long double NSGA2_MultiObjectivized_Fuzzy_TSK::getFitValue() {
	long double bestImprovement = improvementHistory[improvementHistory.size() - 1];
	int lowerbound = (improvementHistory.size() <= k)? 0 : improvementHistory.size() - k;

	for (int i = lowerbound; i < improvementHistory.size() - 1; i++) {
		if (improvementHistory[i] > bestImprovement) {
			bestImprovement = improvementHistory[i];
		}
	}

	return (improvementHistory[improvementHistory.size() - 1] / bestImprovement);
}

long double NSGA2_MultiObjectivized_Fuzzy_TSK::getVarValue() {
	long double bestMoment = momentHistory[momentHistory.size() - 1];
	int lowerbound = (momentHistory.size() <= k)? 0 : momentHistory.size() - k;

	for (int i = lowerbound; i < momentHistory.size() - 1; i++) {
		if (momentHistory[i] > bestMoment) {
			bestMoment = momentHistory[i];
		}
	}

	return (momentHistory[momentHistory.size() - 1] / bestMoment);
}

Individual* NSGA2_MultiObjectivized_Fuzzy_TSK::lookForBestIndividual() {
  Individual *best = NULL;
  if (population->size() > 0){
    best = (*population)[0];
  } else if (offSpring->size() > 0){
    best = (*offSpring)[0];
  } else {
    return NULL;
  }

  int direction = best->getInternalOptDirection(0);
  for (int i = 0; i < population->size(); i++){
    if ( ( (direction == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0)) ) ||
         ( (direction == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0))) ){
      best = (*population)[i];
    }
  }

	if (population != offSpring){	
  	for (int i = 0; i < offSpring->size(); i++){
   	 if ( ( (direction == MINIMIZE) && ((*offSpring)[i]->getObj(0) < best->getObj(0)) ) ||
   	      ( (direction == MAXIMIZE) && ((*offSpring)[i]->getObj(0) > best->getObj(0))) ){
   	   best = (*offSpring)[i];
   	 }
  	}
	}
	return best;
}
