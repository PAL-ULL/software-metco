# Metaheuristic-based Extensible Tool for Cooperative Optimisation (METCO) -- Version registered at Safe Creative

# Examples

## Adding a New Plugin


### New Algorithm

Adding a new algorithm in METCO is quite straightforward. You can follow the steps down below. For this example, we are going to use the [Simulated Annealing (SA)](https://en.wikipedia.org/wiki/Simulated_annealing) algorithm. _More references about SA algorithm can be found [here](https://www.springer.com/cda/content/document/cda_downloaddocument/9783319411910-c2.pdf?SGWID=0-0-45-1579890-p180080441) and [here](http://mathworld.wolfram.com/SimulatedAnnealing.html)_.


1. Create a new folder with the name of the algorithm in the path `local_path/software-metco/oplink/algorithms/team/src/plugins/algorithms`.
```
local_path/software-metco/oplink/algorithms/team/src/plugins/algorithm/SA.
```
2. Then, add the header and source file for the algorithm with, **exactly, the same name of the folder created**. Besides, the new class in which the algorithm is implemented must have the same name.
```
local_path/software-metco/oplink/algorithms/team/src/plugins/algorithm/SA/SA.h
local_path/software-metco/oplink/algorithms/team/src/plugins/algorithm/SA/SA.cpp
```
3. Now, inside the header file, you must include the superclasses `Individual` and `EA`: 
```cpp        
#include "Individual.h"
#include "EA.h" 
```
4. The new class must inherit from the EA superclass:

```cpp
#ifndef __SA_H__
#define __SA_H__

#include "Individual.h"
#include "EA.h"

class SA : public EA {
    (...)
};

#endif
```
5.After that, the following methods must be overwritten in the new class:

* `bool init(const vector<string>& params)`: This is the method which is in charge of initialized the algorithm with all the given parameters.
* `void runGeneration()`: In this method it must be included the code that the algorithm will repeat for each generation. Main loop.
* `void getSolution(MOFront* p)`: It may include the current population of individuals in the MultiObjectiveFront p.
* `void printInfo(ostream& os) const`: This method is only for information purpose.

6. Them, the specific features and components of the algorithm must be implemented.

Here is how the header file should look like:
```cpp
#ifndef __SA_H__
#define __SA_H__

#include "Individual.h"
#include "EA.h"

class SA : public EA {
public:
    bool init(const vector<string>& params);
    void getSolution(MOFront* p);
    void printInfo(ostream& os) const;
private:
    void runGeneration();
// Specific variables and methods for SA algorithm
private:
    void applyRandomPerturbation();
    void evaluateDifference();
    void updateTemperature();
private:
    double initialTemp;
    double temperature;
    double perturbation;
    double temperatureVariation;
    uint64_t seed;
    Individual* newIndividual; // New individual after applyRandomPerturbation
private:
    static const int PARAMS; // Number of parameters that must be given to SA
};

#endif
```
And then, we must implement this methods inside the source code file:

```cpp
#include "SA.h"
#include <algorithm>    // std::min
#include <cmath>        // isless, isgreater

// Define the static constants
const int SA::PARAMS = 3; 

// Algorithm initialization
bool SA::init(const vector<string>& params) {
    // Check number of parameters
    if (params.size() != PARAMS) {
        cout << "Parameters: seed init_temp temp_variation" << endl;
        return false;
    }
    // Only mono-objective optimization is supported
    if (getSampleInd()->getNumberOfObj() != 1) {
        cout << "Multi-Objective not supported" << endl;
        return false;
    }
    setPopulationSize(1);
    seed = static_cast<uint64_t>(atof(params[0].c_str()));
    srand(seed);
    initialTemp = atof(params[1].c_str());
    temperature = initialTemp;
    temperatureVariation = atof(params[2].c_str());
    return true;
}

void SA::runGeneration() {
    applyRandomPerturbations();
    evaluateDifference();
    updateTemperature();
}

void SA::applyRandomPerturbations() {
    double perturbation = ((double) rand () / (double) RAND_MAX);
    Individual* copy = (*population)[0]->internalClone();
    for (int j = 0; j < (*population)[0]->getNumberOfVar(); j++) {
        copy->setVar(j, getPerturbation() * (copy->getMaximum(j)
		                - copy->getMinimum(j))
			            + copy->getMinimum(j));
        if (copy->getVar(j) > copy->getMaximum(j)
        || copy->getVar(j) < copy->getMinimum(j)) {
            copy->setVar(j, (copy->getMaximum(j) - copy->getMinimum(j))
            + copy->getMinimum(j));
        }
    }
    newIndividual = copy->internalClone();;
}

void SA::evaluateDifference() {
    evaluate((*population)[0]);
    evaluate(newIndividual);
    double difference = 0.0;
    difference = (*population)[0]->getObj(0) - newIndividual->getObj(0);
    if(isless(difference, 0.0))
        (*population)[0] = newIndividual->internalClone();
    else {
        const double randomProbability = (double) rand() / (RAND_MAX);
        const double e = exp(-difference / temperature);
        const double probability = min(1.0, e);
        if(isless(randomProbability, probability))
            (*population)[0] = newIndividual->internalClone();
    }
}

void SA::updateTemperature() {
    temperature *= temperatureVariation;
}

void SA::getSolution(MOFront* p) {
    p->insert((*population)[0]);
}

void SA::printInfo(ostream& os) const {
    os << "Simulated Annealing Algorithm"  << endl;
    os << "Number of Evaluations = " << getEvaluations() << endl;
    os << "Initial Temperature = " << initialTemp << endl;
    os << "Temperature Variation = " << temperatureVariation << endl;
}
```


### New Problem

In this case, adding a new problem in METCO is even easier than an algorithm. Let's suppose that we want to create a new problem, for example, the Rastrigin function. This functions is defined like:

![equation](https://wikimedia.org/api/rest_v1/media/math/render/svg/1aa1c38ee739ca9cf4582867d74d469df4676cbc)

Where A = 10 and it is evaluated on the hypercube defined by:

![eq](https://wikimedia.org/api/rest_v1/media/math/render/svg/e3560b2d6c10fff0a26c994d45374c4dc70f98e5) for all i = 1, ..., d.

The steps that we must follow to achieve this are:

1. Create a new folder with the name of the algorithm in the path `local_path/software-metco/oplink/algorithms/team/src/plugins/problems`. In this case, it will be named _Rastrigin_.


2. Then, add the header and source file for the problem with, **exactly, the same name of the folder created**. Besides, the new class in which the problem is implemented must have the same name. For example:
```
    Rastrigin/Rastrigin.cpp
    Rastrigin/Rastrigin.h
```
3. Now, for the new problem, only include `Individual` in the header file (*.h): 
        
```cpp
#include "Individual.h"
```
4. The new class must inherit from the class named Individual which it is included in the above file. Therefore, it must look like:

```cpp
#ifndef __RASTRIGIN__
#define __RASTRIGIN__

#include "Individual.h"


class Rastrigin : public Individual {
public:
  // Constructor and destructor
  Rastrigin() {};
  ~Rastrigin() {};
 (...)
};

#endif
```
Moreover, in the new class we must overwrite the following methods:

* `bool init(const vector<string>& params)`: This is the method which is in charge of initialized the problem with all the given parameters. Inside this method, you must define the number of objectives and variables of the problem. Therefore, the following methods must be called:
    * `setNumberOfVar(NPARAM)`: Defines the number of variables of the problem. It will be a good practice to define two static vectors such as **minVar** and **maxVar** where to store the limits of each variable if, in contrast to this case, they are not constant values.
    ```cpp
        static vector<double> minVar;
        static vector<double> maxVar;
    ```
    * `setNumberOfObj(NOBJ)`: Defines the number of objectives of the problem.

    The given parameters must depend of the treated problem. In this case, the Rastrigin function takes the number of dimensions as argument so, this method will look like:

```cpp
    bool Rastrigin::init(const vector<string>& params) {
        if (params.size() != 1) {
            cerr << "Error in Rastrigin init: the parameter dim must be especificated" << endl;
            exit(-1);
        }
        double numVar = atoi(params[0].c_str());
        setNumberOfVar(numVar);
        setNumberOfObj(1);
    }
```

* `void evaluate(void)`: This method must contains the evaluation function of the problem. For every objective, the method `setObj(dimension, value)` must be call with result of the evaluation in the i-th dimension. Following with the Rastrigin example, the evaluate method will look like:

```cpp
    void Rastrigin::evaluate(){
        double evaluation = 0.0;
        for(int i = 0; i < getNumberOfVar(); i++)
            evaluation += pow(getVar(i), 2) - 10 * cos(2 * M_PI * getVar(i));
            evaluation = 10 * getNumberOfVar() + evaluation;
            setObj(0, evaluation);
    }
```

* Ranges of the search space for each dimension:

    * `double getMaximum(const int i) const`
    * `double getMinimum(const int i) const`
   
In this case, as we are defining the Rastrigin function which is evaluated on the hypercube defined by $x_{i} \in[-5.12, 5.12]$, for all i = 1, ..., d, these methods must return -5.12 and 5.12 for every value of the variable i:

```cpp

    double Rastrigin::getMaximum(const int i) const {
        return 5.12;
    }
    double Rastrigin::getMinimum(const int i){
        return -5.12;
    }
```

* `unsigned int getOptDirection(const int i) const`: This method returns the optimization direction in the dimension `i`. It must return whether **MINIMIZE** or **MAXIMIZE** constant defined in the **Individual** class.

* `Individual* clone(void) const`: Returns a new Individual cloned from the current one.

The final result of the new header file must be something like the example down below: 

```cpp
#ifndef __RASTRIGIN__
#define __RASTRIGIN__

#include "Individual.h"
#include <vector>

class Rastrigin : public Individual {
public:
  Rastrigin() {};
  ~Rastrigin() {};
  bool init (const vector<string> &params);     // Initialization method
  void evaluate (void);                         // Evaluation
  Individual* clone (void) const;               // Clone individual
  double getMaximum(const int i) const { return 5.12; };   // Ranges of the search space
  double getMinimum(const int i) const { return -5.12; };
  unsigned int getOptDirection(const int i) const { return MINIMIZE; };     // Opt direction
};

#endif
```

And finally, if we put together all the methods overwritten in the code file (*.cpp), it will look like:

```cpp
#include "Rastrigin.h"
#include <cmath>

bool Rastrigin::init(const vector<string> &params) {
  if (params.size() != 1) {
    cerr << "Error in Rastrigin init: the parameter dim must be especificated" << endl;
    exit(-1);
  }
  double numVar = atoi(params[0].c_str());
  setNumberOfVar(numVar);
  setNumberOfObj(1);
  return true;
}

Individual* Rastrigin::clone(void) const {
  return new Rastrigin();
}

void Rastrigin::evaluate(){
  double evaluation = 0.0;
  for(int i = 0; i < getNumberOfVar(); i++)
    evaluation += pow(getVar(i), 2) - 10 * cos(2 * M_PI * getVar(i));
  evaluation = 10 * getNumberOfVar() + evaluation;
  setObj(0, evaluation);
}
```
* Optionally, you can add other methods you could need, e.g. quicksort. See [Knapsack Problem](../oplink/algorithms/team/src/plugins/problems/KNAP/KNAP.h).
