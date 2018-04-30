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

In this case, adding a new problem in METCO is even easier than an algorithm.

1. Create a new folder with the name of the algorithm in the path `local_path/software-metco/oplink/algorithms/team/src/plugins/problems`.
2. Then, add the header and source file for the problem with, **exactly, the same name of the folder created**. Besides, the new class in which the problem is implemented must have the same name. For example:
```
    Rastrigin/Rastrigin.cpp
    Rastrigin/Rastrigin.h
```
3. Now, for the new problem, only include `Individual`: 
        
        #include "Individual.h"

4. The new class must inherit from the class named Individual which it is included in the above file and the following methods must be overwritten:

    * `bool init(const vector<string>& params)`: This is the method which is in charge of initialized the problem with all the given parameters. Inside this method, you must define the number of objectives and variables of the problem. Therefore, the following methods must be called:
        * `setNumberOfVar(NPARAM)`: Defines the number of variables of the problem.
        * `setNumberOfObj(NOBJ)`: Defines the number of objectives of the problem.

    The given parameters must depend of the treated problem. However, let's suppose that we are including a new problem, a Rastrigin function for example. This function takes the number of dimensions as argument so, this method will look like:

    ```
    bool Rastrigin::init(const vector<string>& params) {
        double numVar = atoi(params[0].c_str());
        minVar.resize(numVar, -5.12);
        maxVar.resize(numVar, 5.12);
        setNumberOfVar(numVar);
        setNumberOfObj(1);
    }
    ```

    * `void evaluate(void)`: This method must contains the evaluation function of the problem. For every objective, the method `setObj(dimension, value)` must be call with result of the evaluation in the i-th dimension. Following with the Rastrigin example, the evaluate method will look like:

    **Rastrigin equation:** <Enter>
    
    $f(x) = 10d + \sum_{i=1}^{d}[x_{i}^{2} - 10cos(2\pi x_{i})]$    
    <Enter>
    <Enter>
    ```
    void Rastrigin::evaluate(){
        double evaluation = 0.0;
        for(int i = 0; i < getNumberOfVar(); i++)
            evaluation += pow(getVar(i), 2) - 10 * cos(2 * M_PI * getVar(i));
            evaluation = 10 * numVar + evaluation;
            setObj(0, evaluation);
    }
    ```

    * Ranges of the search space for each dimension:

        * `double getMaximum(const int i) const`
        * `double getMinimum(const int i) const`
   
    For instance, if we consider the Rastrigin function which is evaluated on the hypercube defined by $x_{i} \in[-5.12, 5.12]$, for all i = 1, ..., d, these methods must return -5.12 and 5.12 for every value of the variable i:
    ```
    double Rastrigin::getMaximum(const int i) const {
        return 5.12;
    }
    double Rastrigin::getMinimum(const int i){
        return -5.12;
    }
    ```
    * `unsigned int getOptDirection(const int i) const`: This method returns the optimization direction in the dimension `i`. It must return **MINIMIZE** or **MAXIMIZE**.
    * `Individual* clone(void) const`: Returns a new Individual cloned from the current one.

5. Finally, just add the variables and methods that you may need.