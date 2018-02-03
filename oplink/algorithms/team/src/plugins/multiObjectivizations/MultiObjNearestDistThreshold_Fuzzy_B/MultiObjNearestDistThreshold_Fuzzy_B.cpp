#include "MultiObjNearestDistThreshold_Fuzzy_B.h"

#include <stdlib.h>
#include <math.h>
#include <float.h>

bool MultiObjNearestDistThreshold_Fuzzy_B::init(const vector<string> &params) {
  if (params.size() != 4){
    cerr << "Error: NearestDistThreshold parameters --> shiftValue delta numGenToFuzzify k" << endl;
    return false;
  }

	shiftValue = atof(params[0].c_str());
	delta = atof(params[1].c_str());
	numGenToFuzzify = atoi(params[2].c_str());
	k = atoi(params[3].c_str());
	
	if (delta <= 0) {
		cerr << "Error: NearestDistThreshold - Parameter delta must be greater than 0" << endl;
		return false;
	}

	if (numGenToFuzzify <= 0) {
		cerr << "Error: NearestDistThreshold - Parameter numGenToFuzzify must be greater than 0" << endl;
		return false;
	}
	
	if (k <= 0) {
		cerr << "Error: NearestDistThreshold - Parameter k must be greater than 0" << endl;
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
	pThreshold = initStageValues[i];
	initStageValues[i] = initStageValues[initStageValues.size() - 1];
	initStageValues.pop_back();

	// Definition of the Fuzzy System
  // Engine
  multiObjFuzzyEngine = new fl::Engine("multiObjFuzzyEngine");

	// Input variables
	fit = new fl::InputVariable;
  fit->setName("FIT");
  fit->setRange(0.0, 1.0);
	fit->addTerm(new fl::Triangle("LOW", -0.5, 0, 0.5));
	fit->addTerm(new fl::Triangle("MEDIUM", 0, 0.5, 1));
	fit->addTerm(new fl::Triangle("HIGH", 0.5, 1, 1.5));
	multiObjFuzzyEngine->addInputVariable(fit);
	
  thrIn = new fl::InputVariable;
  thrIn->setName("THR_IN");
  thrIn->setRange(0.0, 1.0);
  thrIn->addTerm(new fl::Triangle("LOW", -0.16667, 0, 0.16667));
  thrIn->addTerm(new fl::Triangle("LOW_MED_B", 0, 0.16667, 0.33333));
  thrIn->addTerm(new fl::Triangle("LOW_MED_A", 0.16667, 0.33333, 0.5));
  thrIn->addTerm(new fl::Triangle("MEDIUM", 0.33333, 0.5, 0.66667));
  thrIn->addTerm(new fl::Triangle("MED_HIGH_A", 0.5, 0.66667, 0.83333));
  thrIn->addTerm(new fl::Triangle("MED_HIGH_B", 0.66667, 0.83333, 1)); 
  thrIn->addTerm(new fl::Triangle("HIGH", 0.83333, 1, 1.16667));
  multiObjFuzzyEngine->addInputVariable(thrIn);

	bestThrIn = new fl::InputVariable("BEST_THR_IN");
  bestThrIn->setName("BEST_THR_IN");
  bestThrIn->setRange(0.0, 1.0);
  bestThrIn->addTerm(new fl::Triangle("LOW", -0.5, 0, 0.5));
  bestThrIn->addTerm(new fl::Triangle("MEDIUM", 0, 0.5, 1));
  bestThrIn->addTerm(new fl::Triangle("HIGH", 0.5, 1, 1.5));
  multiObjFuzzyEngine->addInputVariable(bestThrIn);
	
  // Output Variables
  thrOut = new fl::OutputVariable;
  thrOut->setName("THR_OUT");
  thrOut->setRange(-0.45, 0.45);
  thrOut->addTerm(new fl::Triangle("NEG_GIANT", -0.54, -0.45, -0.36));
  thrOut->addTerm(new fl::Triangle("NEG_HUGE", -0.45, -0.36, -0.27));
  thrOut->addTerm(new fl::Triangle("NEG_HIGH", -0.36, -0.27, -0.18));
  thrOut->addTerm(new fl::Triangle("NEG_MEDIUM", -0.27, -0.18, -0.09));
  thrOut->addTerm(new fl::Triangle("NEG_LOW", -0.18, -0.09, 0));
  thrOut->addTerm(new fl::Triangle("ZERO", -0.09, 0, 0.09));
  thrOut->addTerm(new fl::Triangle("POS_LOW", 0, 0.09, 0.18));
  thrOut->addTerm(new fl::Triangle("POS_MEDIUM", 0.09, 0.18, 0.27));
  thrOut->addTerm(new fl::Triangle("POS_HIGH", 0.18, 0.27, 0.36));
  thrOut->addTerm(new fl::Triangle("POS_HUGE", 0.27, 0.36, 0.45));
  thrOut->addTerm(new fl::Triangle("POS_GIANT", 0.36, 0.45, 0.54));
  multiObjFuzzyEngine->addOutputVariable(thrOut);

  // Seven different fuzzy rules sets
  fl::RuleBlock* rules_low = new fl::RuleBlock;
  fl::RuleBlock* rules_low_med_b = new fl::RuleBlock;
  fl::RuleBlock* rules_low_med_a = new fl::RuleBlock;
  fl::RuleBlock* rules_medium = new fl::RuleBlock;
  fl::RuleBlock* rules_med_high_a = new fl::RuleBlock;
  fl::RuleBlock* rules_med_high_b = new fl::RuleBlock;
  fl::RuleBlock* rules_high = new fl::RuleBlock;

  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is LOW and BEST_THR_IN is LOW then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is LOW and BEST_THR_IN is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is LOW and BEST_THR_IN is HIGH then THR_OUT is POS_LOW", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is MEDIUM then THR_OUT is ZERO", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is LOW then THR_OUT is NEG_MEDIUM", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is LOW then THR_OUT is NEG_HIGH", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is LOW then THR_OUT is NEG_HUGE", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is LOW then THR_OUT is NEG_GIANT", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is LOW then THR_OUT is NEG_GIANT", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is LOW then THR_OUT is NEG_GIANT", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
  rules_low->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));

	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is LOW then THR_OUT is POS_MEDIUM", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is LOW and BEST_THR_IN is LOW then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is LOW and BEST_THR_IN is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is LOW and BEST_THR_IN is HIGH then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is MEDIUM then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is LOW then THR_OUT is NEG_MEDIUM", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is LOW then THR_OUT is NEG_HIGH", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is LOW then THR_OUT is NEG_HUGE", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is LOW then THR_OUT is NEG_GIANT", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is LOW then THR_OUT is NEG_GIANT", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_low_med_b->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is LOW then THR_OUT is POS_HIGH", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is LOW then THR_OUT is POS_MEDIUM", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is LOW and BEST_THR_IN is LOW then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is LOW and BEST_THR_IN is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is LOW and BEST_THR_IN is HIGH then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is MEDIUM then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is LOW then THR_OUT is NEG_MEDIUM", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is LOW then THR_OUT is NEG_HIGH", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is LOW then THR_OUT is NEG_HUGE", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is LOW then THR_OUT is NEG_GIANT", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_low_med_a->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));

	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is LOW then THR_OUT is POS_HUGE", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is LOW then THR_OUT is POS_HIGH", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is LOW then THR_OUT is POS_MEDIUM", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is LOW and BEST_THR_IN is LOW then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is LOW and BEST_THR_IN is MEDIUM then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is LOW and BEST_THR_IN is HIGH then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is MEDIUM then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is LOW then THR_OUT is NEG_MEDIUM", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is LOW then THR_OUT is NEG_HIGH", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is LOW then THR_OUT is NEG_HUGE", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_medium->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is LOW then THR_OUT is POS_GIANT", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is LOW then THR_OUT is POS_HUGE", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is LOW then THR_OUT is POS_HIGH", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is LOW then THR_OUT is POS_MEDIUM", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is LOW and BEST_THR_IN is LOW then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is LOW and BEST_THR_IN is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is LOW and BEST_THR_IN is HIGH then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is MEDIUM then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is LOW then THR_OUT is NEG_MEDIUM", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is LOW then THR_OUT is NEG_HIGH", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_med_high_a->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));

	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is LOW then THR_OUT is POS_GIANT", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is LOW then THR_OUT is POS_GIANT", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is LOW then THR_OUT is POS_HUGE", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is LOW then THR_OUT is POS_HIGH", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is LOW then THR_OUT is POS_MEDIUM", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is LOW and BEST_THR_IN is LOW then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is LOW and BEST_THR_IN is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is LOW and BEST_THR_IN is HIGH then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is MEDIUM then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is LOW then THR_OUT is NEG_MEDIUM", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_med_high_b->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));

	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is LOW then THR_OUT is POS_GIANT", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is LOW and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is LOW then THR_OUT is POS_GIANT", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is LOW then THR_OUT is POS_GIANT", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is LOW_MED_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is LOW then THR_OUT is POS_HUGE", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is MEDIUM and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is LOW then THR_OUT is POS_HIGH", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_A and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is LOW then THR_OUT is POS_MEDIUM", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is MEDIUM then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is MED_HIGH_B and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is LOW and BEST_THR_IN is LOW then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is LOW and BEST_THR_IN is MEDIUM then THR_OUT is NEG_LOW", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is LOW and BEST_THR_IN is HIGH then THR_OUT is POS_LOW", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is MEDIUM then THR_OUT is ZERO", multiObjFuzzyEngine));
	rules_high->addRule(fl::FuzzyRule::parse("if THR_IN is HIGH and FIT is HIGH then THR_OUT is ZERO", multiObjFuzzyEngine));
 	
	multiObjFuzzyEngine->addRuleBlock(rules_low);
	multiObjFuzzyEngine->addRuleBlock(rules_low_med_b);
	multiObjFuzzyEngine->addRuleBlock(rules_low_med_a);
	multiObjFuzzyEngine->addRuleBlock(rules_medium);
	multiObjFuzzyEngine->addRuleBlock(rules_med_high_a);
	multiObjFuzzyEngine->addRuleBlock(rules_med_high_b);
	multiObjFuzzyEngine->addRuleBlock(rules_high);

  multiObjFuzzyEngine->configure("Minimum", "Maximum", "Minimum", "Maximum", "Centroid");
 	
	improvements = vector< vector<long double> > (thrIn->numberOfTerms(), vector<long double>());
	memberships = vector< vector<long double> > (thrIn->numberOfTerms(), vector<long double>());
	weightedImprovements = vector<long double>(thrIn->numberOfTerms(), 0.0);
	
  return true;
}

void MultiObjNearestDistThreshold_Fuzzy_B::convertToMultiObjective(vector<Individual *> *population, vector<Individual *> *offSpring, vector<Individual *> *reference, int ordObj, int numberGeneration) {

  //Looks for best individual (obj[0])
	Individual *best = lookForBestIndividual(population, offSpring);
	if (best == NULL) {
		cerr << "Error: offSpring and population has a size equal to 0" << endl;
		exit(-1);
	}
	if (numberGeneration == 0) {
		prevBestFitness = best->getObj(0);
    cout << "Generation: " << numberGeneration << endl;
    cout << "pThreshold: " << pThreshold << endl;
    cout << "prevBestFitness: " << prevBestFitness << endl;
	}
	
	// Updates the population
	if (!updateObjectiveValue(population, offSpring, best, ordObj)) {
		cerr << "Updating population: objective value less than 0 after shifting" << endl << flush;
		exit(-1);
	}

  // Updates the offSpring
	if (population != offSpring) {
		if (!updateObjectiveValue(offSpring, population, best, ordObj)) {
			cerr << "Updating offSpring: objective value less than 0 after shifting" << endl << flush;
			exit(-1);
		}
	}

	// Calculates the new threshold value
	if ((numberGeneration + 1) % numGenToFuzzify == 0) {
		
		// Updates the history of the threshold
		thresholdHistory.push_back(pThreshold);
		
		// Calculates the improvement and updates the history of improvements
		best = lookForBestIndividual(population, offSpring);
		if (best == NULL) {
			cerr << "Error: offSpring and population has a size equal to 0" << endl;
			exit(-1);
		}

    cout << "Generation: " << numberGeneration << endl;
    cout << "pThreshold: " << pThreshold << endl;
    cout << "prevBestFitness: " << prevBestFitness << endl;
    cout << "bestFitness: " << best->getObj(0) << endl;

		long double currentImprovement = fabs(prevBestFitness - best->getObj(0));
		//if (currentImprovement != 0) {
		improvementHistory.push_back(currentImprovement);
		prevBestFitness = best->getObj(0);
		//}

    cout << "currentImprovement: " << currentImprovement << endl;

		// Calculates the improvements and the memberships
		for (int i = 0; i < thrIn->numberOfTerms(); i++) {
			long double currentMembership = (long double)thrIn->getTerm(i)->membership((fl::scalar)pThreshold);
			//if ((currentMembership != 0) && (currentImprovement != 0)) {
			if (currentMembership != 0) {
				improvements[i].push_back(currentImprovement);
				memberships[i].push_back(currentMembership);
			}
		}
		
		// Calculates the weigthed improvements
		for (int i = 0; i < thrIn->numberOfTerms(); i++) {
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

		// Input variable: thrIn
		fl::scalar thrInValue = (fl::scalar)pThreshold;
		thrIn->setInput(thrInValue);

		// Input variable: fit
		fl::scalar fitValue = (currentImprovement != 0)? (fl::scalar)getFitValue() : 0;
		fit->setInput(fitValue);
		
		// Input Variable: bestThrIn
		fl::scalar bestThrInValue = getBestThrInValue();
		bestThrIn->setInput(bestThrInValue);

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
    for (std::size_t i = 0; i < multiObjFuzzyEngine->numberOfOutputVariables(); ++i) {
      multiObjFuzzyEngine->getOutputVariable(i)->output()->clear();
    }

    // Executes the fuzzy system with the most suitable set of rules
    multiObjFuzzyEngine->getRuleBlock(maxIndex)->fireRules();

		// Output variable: thrOutValue
		fl::scalar thrOutValue;
		thrOutValue = thrOut->defuzzify();

    cout << "SET OF RULES=" << maxIndex << endl;
    cout << "THR_IN=" << thrInValue << endl;
    cout << "FIT=" << fitValue << endl;
    cout << "BEST_THR_IN=" << bestThrInValue << endl;
    cout << "THR_IN is " << thrIn->fuzzify(thrInValue) << endl;
    cout << "FIT is " << fit->fuzzify(fitValue) << endl;
    cout << "BEST_THR_IN is " << bestThrIn->fuzzify(bestThrInValue) << endl;
    cout << "THR_OUT=" << thrOutValue << endl;
    cout << "THR_OUT is " << thrOut->fuzzify(thrOutValue) << endl;
    cout << "--" << endl;
	
    // Stops the initialization stage
    if ((initStage) && (initStageValues.size() == 0)) {
    	initStage = false;
    }
	
		// Updates the threshold
		if (initStage) {
			// Obtains a random value from the initStageValues vector
			int i = rand() % initStageValues.size();
			long double value = initStageValues[i];
			pThreshold = value;
			
			// Removes the selected value for the next iteration
			initStageValues[i] = initStageValues[initStageValues.size() - 1];
			initStageValues.pop_back();
			cout << "pThreshold actualizado en fase inicial: " << pThreshold << endl;
		} else {
			pThreshold += thrOutValue;
			if (pThreshold > 1) {
				pThreshold = 1;
			} else if (pThreshold < 0) {
				pThreshold = 0;
			}
			cout << "pThreshold actualizado con thrOutValue: " << pThreshold << endl;
		}
	}
}

Individual* MultiObjNearestDistThreshold_Fuzzy_B::lookForBestIndividual(vector<Individual *> *population, vector<Individual *> *offSpring) {
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

long double MultiObjNearestDistThreshold_Fuzzy_B::getFitValue() {
	long double bestImprovement = improvementHistory[improvementHistory.size() - 1];
	int lowerbound = (improvementHistory.size() <= k)? 0 : improvementHistory.size() - k;

	for (int i = lowerbound; i < improvementHistory.size() - 1; i++) {
		if (improvementHistory[i] > bestImprovement) {
			bestImprovement = improvementHistory[i];
		}
	}

	return (improvementHistory[improvementHistory.size() - 1] / bestImprovement);
}

long double MultiObjNearestDistThreshold_Fuzzy_B::getBestThrInValue() {
	long double bestImprovement = improvementHistory[improvementHistory.size() - 1];
	long double bestThrIn = thresholdHistory[thresholdHistory.size() - 1];
	int lowerbound = (improvementHistory.size() <= k)? 0 : improvementHistory.size() - k;

	for (int i = lowerbound; i < improvementHistory.size() - 1; i++) {
		if (improvementHistory[i] > bestImprovement) {
			bestImprovement = improvementHistory[i];
			bestThrIn = thresholdHistory[i];
		}
	}

	return (bestThrIn);
}

bool MultiObjNearestDistThreshold_Fuzzy_B::updateObjectiveValue(vector<Individual *> *population, vector<Individual *> *offSpring, Individual *best, int ordObj) {
  for (unsigned int i = 0; i < population->size(); i++) {
    double minDist = DBL_MAX;
		if ((*population)[i]->getObj(0) - shiftValue < 0){
			return false;
		}
    // Checks if the condition of the threshold is satisfied
    if ( (pThreshold == 0) ||
         ((best->getInternalOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) - shiftValue >= (best->getObj(0) - shiftValue)*pThreshold)) ||
         ((best->getInternalOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) - shiftValue <= (best->getObj(0) - shiftValue)/pThreshold)) ) {

      // Respect the own population
      for (unsigned int j = 0; j < population->size(); j++) {
        if (i != j) {
          double actualDist = 0;
          for (unsigned int k = 0; k < (*population)[i]->getNumberOfVar(); k++) {
            actualDist += ((*population)[i]->getVar(k) - (*population)[j]->getVar(k)) * ((*population)[i]->getVar(k) - (*population)[j]->getVar(k));
          }
          if (actualDist < minDist)
            minDist = actualDist;
        }
      }

      // Respect the offSpring
			if (population != offSpring){
      	for (unsigned int j = 0; j < offSpring->size(); j++) {
	        double actualDist = 0;
 	       	for (unsigned int k = 0; k < (*population)[i]->getNumberOfVar(); k++) {
 	        	actualDist += ((*population)[i]->getVar(k) - (*offSpring)[j]->getVar(k)) * ((*population)[i]->getVar(k) - (*offSpring)[j]->getVar(k));
 	       	}
       		if (actualDist < minDist)
          	minDist = actualDist;
      	}
			}

      (*population)[i]->setObj(ordObj, sqrt(minDist));

    } else {
      (*population)[i]->setObj(ordObj, 0);
    }
  }
	return true;
}
