/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    May 2010
 *
 * DESCRIPTION
 * 		Output in XML 
 ************************************************************************************************/


#ifndef __XML_H__
#define __XML_H__

#include <vector>
#include <string>

#include "Individual.h"
#include "OutputPrinter.h"

using namespace std;

class XML : public OutputPrinter {
	public:
		virtual bool init(const vector<string> &params);
		virtual void printSolution(EA *, bool end);
		virtual void printInit(EA *);
		virtual void finish();
	private:
		string getStrWithMinLength(int num, int minLength);
		ofstream outputFile;
};
#endif
