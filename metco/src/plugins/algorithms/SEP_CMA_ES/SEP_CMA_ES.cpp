#include "SEP_CMA_ES.h"

// Algorithm initialization
bool SEP_CMA_ES::init(const vector<string>& params) {
	// Check number of parameters
	if (params.size() != 3) {
		cout << "Parametros: lambda mu sigma" << endl;
		return false;
	}

  // Only mono-objective optimization is supported
	if (getSampleInd()->getNumberOfObj() != 1) {
		cout << "Multi-Objective not supported by SEP_CMA_ES" << endl;
		return false;
	}

	setPopulationSize(atoi(params[0].c_str()));
	lambda = getPopulationSize();
	N = getSampleInd()->getNumberOfVar();
	mu = atoi(params[1].c_str());
	//mu = floor(lambda / 2.0);
	sigma = atof(params[2].c_str());
	initSigma = sigma;

  best = getSampleInd()->internalClone();
	best->getOptDirection(0) == MINIMIZE ? best->setObj(0, DBL_MAX) : best->setObj(0, DBL_MIN);

	// memory allocation
	// m*n
	arx = vector<vector<double>>(lambda, vector<double>(N));
	arz = vector<vector<double>>(lambda, vector<double>(N));
	// n
	diagD = vector<double>(N);
	diagC = vector<double>(N);
	pc = vector<double>(N);
	ps = vector<double>(N);
	xmean = vector<double>(N);
	xold = vector<double>(N);
	// lambda, mu, nvectors
  weights = vector<double>(mu);
	arfitness = vector<double>(lambda);
	arindex = vector<int>(lambda);
	arr_tmp = vector<sortedvals>(lambda);

	generator = default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
	uniformDist = uniform_real_distribution<double>(0.0, 1.0);
	normalDist = normal_distribution<double>(0.0, 1.0);

	for (int i = 0; i < N; i++) {
		diagD[i] = 1;
		diagC[i] = 1;
		ps[i] = 0;
		pc[i] = 0;
	}

	double sum_weights = 0;
	for (int i = 0; i < mu; i++) {
		weights[i] = log(double(mu + 0.5)) - log(double(1 + i));
		sum_weights = sum_weights + weights[i];
	}

	mueff = 0;
	for (int i = 0; i < mu; i++) {
		weights[i] = weights[i] / sum_weights;
		mueff = mueff + weights[i] * weights[i];
	}
	mueff = 1.0 / mueff;

	//for (int i = 0; i < N; i++)
	//	xmean[i] = getSampleInd()->getMinimum(i) + (getSampleInd()->getMaximum(i) - getSampleInd()->getMinimum(i)) * uniformDist(generator);

	c1 = 2.0 / (pow((N + 1.3), 2.0) + mueff);
	cmu = minv(1.0 - c1, 2.0 * (mueff - 2.0 + 1.0 / mueff) / (pow(N + 2.0, 2.0) + mueff));
	ccov1_sep = minv(1, c1 * (N + 1.5) / 3.0);
	ccovmu_sep = minv(1.0 - ccov1_sep, cmu * (N + 1.5) / 3);
	chiN = sqrt(double(N))*(1.0 - 1.0 / (4.0*N) + 1 / (21.0*N*N));
	cs = (mueff + 2.0) / (N + mueff + 5.0);
	damps = 1.0 + 2 * maxv(0, sqrt((mueff - 1.0) / (N + 1.0)) - 1.0) + cs;
	ccum = (4.0 + mueff/double(N)) / (N+4.0 + 2*mueff/double(N));

	return true;
}

void SEP_CMA_ES::runGeneration() {
  if (getGeneration() == 0) {
		sortPopulation();
		for (int j = 0; j < N; j++) {
      xmean[j] = 0;
   		for (int i = 0; i < mu; i++) {
				xmean[j] += weights[i] * (*population)[i]->getVar(j);
      }
    }
  }

  for (int i = 0; i < lambda; i++) {// O(lambda*m*n)
		for (int k = 0; k < N; k++)	{ // O(n)
			// Resampling for avoiding unfeasible values
			do {
				arz[i][k] = normalDist(generator);
				arx[i][k] = xmean[k] + sigma * diagD[k] * arz[i][k];
				(*population)[i]->setVar(k, arx[i][k]);
			} while (((*population)[i]->getVar(k) > (*population)[i]->getMaximum(k)) || ((*population)[i]->getVar(k) < (*population)[i]->getMinimum(k)));
		}

		//arfitness[i] = MyFunc(FuncId, N, &arx[i*N], &gt);
		evaluate((*population)[i]);
	  arfitness[i] = (*population)[i]->getObj(0);
		
    // Updates the best individual
		if ((((*population)[i]->getOptDirection(0) == MINIMIZE) && ((*population)[i]->getObj(0) < best->getObj(0))) ||
        (((*population)[i]->getOptDirection(0) == MAXIMIZE) && ((*population)[i]->getObj(0) > best->getObj(0)))) {
			delete best;
			best = (*population)[i]->internalClone();
		}
  }

	mysort();

	for (int i = 0; i < N; i++) {
		xold[i] = xmean[i];
		xmean[i] = 0;
	}

	for (int i = 0; i < mu; i++) {
		//double* cur_x = &arx[arindex[i] * N];
    vector<double> cur_x = arx[arindex[i]];
		for (int j = 0; j < N; j++)
			xmean[j] += weights[i] * cur_x[j];
	}

	double norm_ps = 0;
	for (int i = 0; i < N; i++) {
		ps[i] = (1 - cs) * ps[i] + sqrt(cs*(2 - cs)*mueff) * (1. / diagD[i]) * (xmean[i] - xold[i]) / sigma;
		norm_ps += ps[i] * ps[i];
	}
	norm_ps = sqrt(norm_ps);

	for (int i = 0; i < N; i++)
		pc[i] = (1 - ccum)*pc[i] + sqrt(ccum*(2 - ccum)*mueff)*(xmean[i] - xold[i]) / sigma;

	for (int i = 0; i < N; i++) {
		double val = 0;
		for (int j = 0; j < mu; j++)
			val += weights[j] * arz[arindex[j]][i] * arz[arindex[j]][i];
		diagC[i] = (1 - ccov1_sep - ccovmu_sep) * diagC[i] + ccov1_sep * pc[i] * pc[i] + ccovmu_sep * (diagC[i] * val);
	}

	sigma = sigma * exp((cs / damps)*(norm_ps / chiN - 1));

	for (int i = 0; i < N; i++)
		diagD[i] = sqrt(diagC[i]);
}

void SEP_CMA_ES::sortPopulation() {
	sort(population->begin(), population->end(), sortByObj0);
}

void SEP_CMA_ES::getSolution(MOFront* p) {
	p->insert(best);
}

void SEP_CMA_ES::printInfo(ostream& os) const {
	os << "SEP Covariance Matrix Adaptation Evolution Strategy"  << endl;
	os << "Number of Evaluations = " << getEvaluations() << endl;
	os << "Population Size (lambda) = " << getPopulationSize() << endl;
	os << "mu = " << mu << endl;
	os << "Initial sigma = " << initSigma << endl;
	os << "Current sigma = " << sigma << endl;
}

double SEP_CMA_ES::minv(double a, double b)
{
	if (a < b)	return a;
	else		return b;
}

double SEP_CMA_ES::maxv(double a, double b)
{
	if (a > b)	return a;
	else		return b;
}

void SEP_CMA_ES::mysort() {
	for(int i = 0; i < lambda; i++) {
		arr_tmp[i].value = arfitness[i];
		arr_tmp[i].id = i;
	}

	//qsort(arr, sz, sizeof(sortedvals), compare);
  sort(arr_tmp.begin(), arr_tmp.end(), compare);

	for(int i = 0; i < lambda; i++) {
		arfitness[i] = arr_tmp[i].value;
		arindex[i] = arr_tmp[i].id;
	}
}

/*double* SEP_CMA_ES::getRestartInfo() {
  double *data = new double[1];

	//cout << "Gathering information at SEP_CMA_ES::getRestartInfo" << endl;

  data[0] = sigma;
	//cout << "sigma: " << sigma << endl;

  //xmean
  //for (int i = 0; i < N; i++) {
	//	data[idx++] = xmean[i];
  //}
  
  //diagD
  //for (int i = 0; i < N; i++) {
	//	data[idx++] = diagD[i];
  //}
  
  //diagC
  //for (int i = 0; i < N; i++) {
	//	data[idx++] = diagC[i];
  //}

  //ps
  //for (int i = 0; i < N; i++) {
	//	data[idx++] = ps[i];
  //}

  //pc
  //for (int i = 0; i < N; i++) {
	//	data[idx++] = pc[i];
		//cout << "pc[" << i << "]: " << pc[i] << " ";
  //}
  //cout << endl;

  return data;
}*/

/*void SEP_CMA_ES::setRestartInfo(double *data) {
	
  //cout << "Restoring information at SEP_CMA_ES::setRestartInfo" << endl;
 
	sigma = data[0];

  //cout << "sigma: " << sigma << endl;

  //xmean
  //for (int i = 0; i < N; i++) {
	//	xmean[i] = data[idx++];
  //}
  
  //diagD
  //for (int i = 0; i < N; i++) {
	//	diagD[i] = data[idx++];
  //}
  
  //diagC
  //for (int i = 0; i < N; i++) {
	//	diagC[i] = data[idx++];
  //}

  //ps
  //for (int i = 0; i < N; i++) {
	//	ps[i] = data[idx++];
  //}

  //pc
  //for (int i = 0; i < N; i++) {
	//	pc[i] = data[idx++];
	//	cout << "pc[" << i << "]: " << pc[i] << " ";
  //}
  //cout << endl;
}*/

bool sortByObj0(const Individual* i1, const Individual* i2) {
	return (i1->getInternalOptDirection(0) == MINIMIZE) ?
	       (i1->getObj(0) < i2->getObj(0)) : (i1->getObj(0) > i2->getObj(0));
}

bool compare(const sortedvals i1, const sortedvals i2) {
	return (i1.value < i2.value);
}

