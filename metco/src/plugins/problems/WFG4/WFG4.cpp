/***********************************************************************************
 * AUTORES
 *   - Eduardo Manuel Segredo Gonzalez
 * 
 * FECHA
 *   Julio de 2008
 *
 * *********************************************************************************/

#include <vector>
#include <math.h>

#include "WFG4.h"

// Constantes y variables estaticas
const double PI = 3.1415926535897932384626433832795;
int WFG4::k;

// Inicializacion del problema
bool WFG4::init (const vector<string> &params) {
	// Comprueba los parametros del problema
	if (params.size() != 1) {
		cout << "Error WFG4: Incorrect parameters (number_of_objectives)" << endl;
		return false;
	}

	// Inicializa los parametros del problema
	int nObj = atoi(params[0].c_str());

	if (nObj < 2) {
	  cout << "Error WFG4: Parameter number_of_objectives must be greater than 2" << endl;
		return false;
	}

	if (nObj >= 3)
    k = 2 * (nObj - 1);
	else
	  k = 4;

  setNumberOfVar(k + l);  // Parametros de posicion + Parametros de distancia
	setNumberOfObj(nObj);   // Numero de objetivos
	return true;
}

// Evaluacion de un individuo WFG4
void WFG4::evaluate (void) {

	// Inicializa el vector y
  vector<double> y;
	for (int i = 0; i < getNumberOfVar(); i++)
	  y.push_back(getVar(i));

	// Aplica las transiciones al vector y
	y = WFG4_t1(y);
	y = WFG4_t2(y, k, getNumberOfObj());

	// Calcula los valores de las funciones objetivo
	vector<double> result = WFG4_shape(y);

	// Fija dichos valores
	for (int i = 0; i < result.size(); i++)
	  setObj(i, result[i]);

}
				
// Clonacion de un individuo WFG4
Individual* WFG4::clone (void) const {
	return new WFG4();
}

/******************************************************************************
 * Funciones auxiliares al problema
 * ****************************************************************************/

// Escala un valor al intervalo [0, 1]
double WFG4::correct_to_01 (const double &a, const double &epsilon) {

  const double min = 0.0;
	const double max = 1.0;

	const double min_epsilon = min - epsilon;
	const double max_epsilon = max + epsilon;

  if ((a <= min) && (a >= min_epsilon)) {
	  return min;
 	}
  else if ((a >= max) && (a <= max_epsilon)) {
    return max;
  }
  else {
    return a;
	}
}

// Construye un vector con los elementos v[head], ..., v[tail - 1]
vector<double> WFG4::subvector (const vector<double> &v, const int head, const int tail) {

  vector< double > result;

	for(int i = head; i < tail; i++) {
    result.push_back(v[i]);
 	}

  return result;
}

vector<double> WFG4::calculate_x (const vector<double> &t_p, const vector<short> &A) {

  vector<double> result;

	for (int i = 0; i < t_p.size() - 1; i++) {
		const double tmp1 = max<double>(t_p.back(), A[i]);
	 	result.push_back(tmp1 * (t_p[i] - 0.5) + 0.5); 
	}
 
 	result.push_back(t_p.back());
	return result;
}

vector<double>  WFG4::calculate_f (const double &D, const vector<double> &x, const vector<double> &h,
                                   const vector<double> &S) {

  vector< double > result;

  for (int i = 0; i < h.size(); i++) {
    result.push_back(D * x.back() + S[i] * h[i]);
  }

  return result;
}

/******************************************************************************
 * Transiciones genericas
 *****************************************************************************/

double WFG4::s_multi (const double &y, const int A, const double &B, const double &C) {
  const double tmp1 = fabs(y - C) / (2.0 * (floor(C - y) + C));
  const double tmp2 = (4.0 * A + 2.0) * PI * (0.5 - tmp1);
  return correct_to_01((1.0 + cos(tmp2) + 4.0 * B * pow(tmp1, 2.0)) / (B + 2.0));
}

double WFG4::r_sum (const vector<double> &y, const vector<double> &w) {

  double numerator   = 0.0;
  double denominator = 0.0;

  for(int i = 0; i < y.size(); i++) {
	  numerator   += w[i] * y[i];
	  denominator += w[i];
	}

	return correct_to_01(numerator / denominator);
}

/******************************************************************************
 * Superficies genericas
 * ****************************************************************************/

double WFG4::concave(const vector<double> &x, const int m) {

  const int M = x.size();
  double result = 1.0;

  for (int i = 1; i <= M - m; i++) {
    result *= sin(x[i - 1] * PI / 2.0);
  }

  if( m != 1 ) {
    result *= cos(x[M - m] * PI / 2.0);
  }

  return correct_to_01(result);
}

/******************************************************************************
 * Funciones propias del problema WFG4
 * ****************************************************************************/

// Primera transicion para WFG4
vector<double> WFG4::WFG4_t1 (const vector<double> &y) {

  const int n = y.size();
  vector<double> t;

  for (int i = 0; i < n; i++) {
    t.push_back(s_multi(y[i], 30, 10, 0.35));
  }

  return t;
}

// Segunda transicion para WFG4
vector<double> WFG4::WFG4_t2 (const vector<double> &y, const int k, const int M) {
														  
  const int n = y.size();
  const vector<double>  w(n, 1.0);
  vector< double > t;

  for (int i = 1; i <= M - 1; i++) {
    const int head = (i - 1) * k / (M - 1);
    const int tail = i * k / (M - 1);
    const vector<double> &y_sub = subvector(y, head, tail);
    const vector<double> &w_sub = subvector(w, head, tail);
    t.push_back(r_sum(y_sub, w_sub));
  }

  const vector<double> &y_sub = subvector(y, k, n);
  const vector<double> &w_sub = subvector(w, k, n);

  t.push_back(r_sum(y_sub, w_sub));

  return t;

}

// Superficie para WFG4
vector<double> WFG4::WFG4_shape (const vector<double> &t_p) {

  const int M = t_p.size();
  const vector<short> &A = WFG_create_A(M, false);
  const vector<double> &x = calculate_x(t_p, A);
  vector<double> h;

  for(int m = 1; m <= M; m++) {
    h.push_back(concave(x, m));
 	}

	return WFG_calculate_f(x, h);

}

/******************************************************************************
 * Funciones propias de los problemas WFG
 * ****************************************************************************/

// Construye un vector de longitud M - 1 con los valores "1,0,0,..." si
// "degenerate" es verdadero. En otro caso, lo construye con los valores
// "1,1,1,..."
vector<short> WFG4::WFG_create_A (const int M, const bool degenerate) {
    
  if (degenerate) {
    vector<short> A(M - 1, 0);
    A[0] = 1;
    return A;
  }
  else {
    return vector<short>(M - 1, 1);
  }
}

// Calcula los valores de fitness escalados para un problema WFG 
vector<double> WFG4::WFG_calculate_f (const vector< double >& x, const vector< double >& h) {
  
  const int M = h.size();
	vector<double> S; 
	
	for(int m = 1; m <= M; m++) {
    S.push_back(m * 2.0);
 	}

	return calculate_f(1.0, x, h, S);
}
