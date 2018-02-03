/************************************************************************************************
 * AUTHORS
 *    Carlos Segura González
 *
 * DATE
 *    May 2010
 *
 * DESCRIPTION
 * 		Output with plain text
 ************************************************************************************************/


#ifndef __PLAIN_TEXT_H__
#define __PLAIN_TEXT_H__

#include <vector>

#include "Individual.h"
#include "OutputPrinter.h"

using namespace std;

class PlainText : public OutputPrinter {
	public:
		virtual bool init(const vector<string> &params);
		virtual void printSolution(EA *, bool end);
		virtual void printInit(EA *){}
		virtual void finish();
	private:
		ofstream outputFile;
};
#endif
