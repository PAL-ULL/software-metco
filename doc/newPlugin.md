# Metaheuristic-based Extensible Tool for Cooperative Optimisation (METCO) -- Version registered at Safe Creative

# Examples

## Adding a New Plugin


### New Algorithm

Adding a new algorithm in METCO is quite straightforward. You only have to follow the simple steps.

1. Create a new folder with the name of the algorithm in the path `path-where-cloned/software-metco/oplink/algorithms/team/src/plugins/algorithms`.
2. Then, add the header and source file for the algorithm with, **exactly, the same the name of the folder created**. Besides, the new class in which the algorithm is implemented must have the same name.
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

1. Create a new folder with the name of the algorithm in the path `path-where-cloned/software-metco/oplink/algorithms/team/src/plugins/problems`.
2. Then, add the header and source file for the problem with, **exactly, the same the name of the folder created**. Besides, the new class in which the problem is implemented must have the same name.
3. Now, for the new problem, only include `Individual`: 
        
        #include "Individual.h"

4. The new class must inherit from Individual and the following methods must be overwritten:

    * `bool init(const vector<string>& params)`: This is the method which is in charge of initialized the problem with all the given parameters.`
    * `void evaluate(void)`: This method must contains the evaluation function of the problem. The method `setObj(dimension, value)` must be call with result of the evaluation in the i-th dimension. 
    * `double getGlobalOptimum(void) const`: If the problem has a known optimum, it should be returned from this function. In other case, you may return whatever you want and ignored this method.
    * Ranges of the search space for each dimension:

        * `double getMaximum(const int i) const`
        * `double getMinimum(const int i) const`
    * `unsigned int getOptDirection(const int i) const`: This method returns the optimization direction in the dimension `i`. It must return **MINIMIZE** or **MAXIMIZE**.
    * `Individual* clone(void) const`: Returns a new Individual cloned from the current one.

5. Finally, just add the variables and methods that you may need.