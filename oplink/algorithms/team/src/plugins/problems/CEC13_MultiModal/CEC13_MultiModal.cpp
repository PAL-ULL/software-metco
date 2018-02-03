#include "CEC13_MultiModal.h"

using namespace std;

// Declaration of static members
CEC2013* CEC13_MultiModal::func;

bool CEC13_MultiModal::init(const vector<string> &params) {
  if (params.size() != 1) {
    cerr << "Error initialising CEC13_MultiModal: problem ID [1..20] is required" << endl;
    exit(1);
  }

  int id = stoi(params[0]);

  if ((id < 1) || (id > 20)) {
    cerr << "Error initialising CEC13_MultiModal: problem ID [1..20] is required" << endl;
    exit(1);
  }

  setFunc(new CEC2013(id));
  setNumberOfVar(getFunc()->get_dimension());
  setNumberOfObj(1);
  return true;
}

void CEC13_MultiModal::evaluate() {
  double *x = new double[getNumberOfVar()];

  for (int i = 0; i < getNumberOfVar(); i++)
    x[i] = getVar(i);

	setObj(0, getFunc()->evaluate(x));
  
  delete [] x;
}

double CEC13_MultiModal::getMinimum(const int i) const {
  return getFunc()->get_lbound(i);
}

double CEC13_MultiModal::getMaximum(const int i) const {
  return getFunc()->get_ubound(i);
}

Individual* CEC13_MultiModal::clone() const {
	return new CEC13_MultiModal();
}
