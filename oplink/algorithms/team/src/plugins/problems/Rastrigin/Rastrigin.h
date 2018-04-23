/*
 * @Author: Alejandro Marrero 
 * @Contact: alu0100825008@ull.edu.es 
 * @Date: 2018-04-13 09:15:09 
 * @Last Modified by: mikey.zhaopeng
 * @Last Modified time: 2018-04-13 09:16:53
 */


#ifndef __RASTRIGIN__
#define __RASTRIGIN__

#include "Individual.h"


class Rastrigin : public Individual {
public:
  // Constructor and destructor
  Rastrigin() {};
  ~Rastrigin() {};
	// Initialization method
	bool init (const vector<string> &params);
	// Evaluation
	void evaluate (void);
	// Clone
	Individual* clone (void) const;
  // Ranges
  // Gets the value of the global optimum
  double getGlobalOptimum(void) const { return globalOptimum; };
	double getMaximum(const int i) const { return maxVar[i]; };
	double getMinimum(const int i) const { return minVar[i]; };
  unsigned int getOptDirection(const int i) const { return MINIMIZE; };
private:
  static bool firstInit;
  static int numVar;
  static vector<double> minVar;
  static vector<double> maxVar;
};

#endif
