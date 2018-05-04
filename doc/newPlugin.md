# Metaheuristic-based Extensible Tool for Cooperative Optimisation (METCO) -- Version registered at Safe Creative

# Examples

## Adding a New Plugin


### New Algorithm

Adding a new algorithm in METCO is quite straightforward. You only have to follow the simple steps.

1. Create a new folder with the name of the algorithm in the path `local_path/software-metco/oplink/algorithms/team/src/plugins/algorithms`.
2. Then, add the header and source file for the algorithm with, **exactly, the same name of the folder created**. Besides, the new class in which the algorithm is implemented must have the same name.
3. Now, inside the header file, you must include the superclasses `Individual` and `EA`: 
        
        #include "Individual.h"
        #include "EA.h" 

4. The new class must inherit from EA and the following methods must be overwritten:

    * `bool init(const vector<string>& params)`: This is the method which is in charge of initialized the algorithm with all the given parameters.`
    * `void runGeneration()`: In this method it must be included the code that the algorithm will repeat for each generation. Main loop.
    * `void getSolution(MOFront* p)`: It may include the current population of individuals in the MultiObjectiveFront p.
    * `void printInfo(ostream& os) const`: This method is only for information purpose.

5. Them, the specific features and components of the algorithm must be implemented.


### New Problem

In this case, adding a new problem in METCO is even easier than an algorithm. Let's suppose that we want to create a new problem, for example, the Rastrigin function. This functions is defined like:
    <Enter>
    $f(x) = 10d + \sum_{i=1}^{d}[x_{i}^{2} - 10cos(2\pi x_{i})]$    
    <Enter>
    <Enter>
Where A = 10 and it is evaluated on the hypercube defined by $x_{i} \in[-5.12, 5.12]$, for all i = 1, ..., d.

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
    * `setNumberOfVar(NPARAM)`: Defines the number of variables of the problem.
    * `setNumberOfObj(NOBJ)`: Defines the number of objectives of the problem.

    The given parameters must depend of the treated problem. In this case, the Rastrigin function takes the number of dimensions as argument so, this method will look like:

```cpp
    bool Rastrigin::init(const vector<string>& params) {
          if (params.size() != 1) {
            cerr << "Error in Rastrigin init: the parameter dim must be especificated" << endl;
            exit(-1);
        }
        double numVar = atoi(params[0].c_str());
        minVar.resize(numVar, -5.12);
        maxVar.resize(numVar, 5.12);
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

The final result of the new header file must be something like this example down below: 

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
  double getMaximum(const int i) const { return maxVar[i]; };   // Ranges of the search space
  double getMinimum(const int i) const { return minVar[i]; };
  unsigned int getOptDirection(const int i) const { return MINIMIZE; };     // Opt direction
private:
  // Ranges of the search space
  static vector<double> minVar;
  static vector<double> maxVar;
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
  minVar.resize(numVar, -5.12);
  maxVar.resize(numVar, 5.12);
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

vector<double> Rastrigin::minVar;
vector<double> Rastrigin::maxVar;
```

