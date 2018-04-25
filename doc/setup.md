# Metaheuristic-based Extensible Tool for Cooperative Optimisation (METCO) -- Version registered at Safe Creative

## Setup

The steps you must follow to [start](../README.md) using METCO can be found in the [README](../README.md) file. However, here is the list of all the steps you must follow from cloned the repo to finally run an algorithm to solve a problem in METCO. 

1. First of all, download or clone this repo.
2. After that, move to `local_path/software-metco/oplink/algorithms/team/src/skeleton/main`.
3. Now run the configuration scripts:
    * `$ ./gen.sh`.
    * `$ ./configure`.
4. Run `make` to build the tool (make sure you have installed all the [dependencies](../README.md)).
5. Due to the fact that METCO allows parallel runs in multiple nodes over HPC systems or any multicore computer, you must configure a ssh connection in the machine you are going to use without using password. A good tutorial of how to achieve this can be found [here](https://www.thegeekstuff.com/2008/11/3-steps-to-perform-ssh-login-without-password-using-ssh-keygen-ssh-copy-id).

As a result of the compilation process, we will have two variations of METCO. So, from now on, we are going to discern between sequential (_metcoSeq_) and parallel (_metcoPar_) runs in METCO. This two programs can be found in the path `local_path/software-metco/oplink/algorithms/team/src/skeleton/main`.

### Sequential Runs

If your purpose is using METCO for sequential runs, _metcoSeq_ is the variation you must use. This program requires the following parameters for a single run:

* Output Path: Path where METCO is going to save the results.
* Plugin Path: Path where is metcoSeq (`(...)/src/skeleton/main/`).
* Output Printer Module: PlainText or XML.
* Output File
* Algorithm 
* Problem
* Stopping Criteria: EVALUATIONS, QUALITY and TIME.
* Stopping Criteria Value
* Print Period
* Using an External Archive
* Algorithm Parameters
* Problem Parameters
* Score Module and Parameters of the Score Module (Optional)
* Mutation and Crossover operators (Optional)
* Local Search and Local Search Parameters (Optional)
* Multiobjectivization and Parameters 

`./metcoSeq outputPath pluginPath outputPrinterModule outputFile algoritmo problema critStop critStopValue printPeriod useExternalArchive(0 | (1 maxLocalFrontSize)) [parametros_algoritmo] [! parametros_problema] [_ scoreModule paramsScoreModule] [- Mutation Crossover ] $ LocalSearch paramsLocalSearch [ + MultiObjectivizationPlugin paramsMultiObjectivization ]`

As you can see, we use some delimiters to separate the parameters in groups.

 * The **'!'** character tells METCO that the following parameters belongs to the chosen algorithm.
 * The **'_'** character determines the beginning of the score module and it's parameters. This parameters are optional.
 * After **'-'** goes the mutation and crossover operators.
 * The **'$'** are used to define, after it, a local search strategy and its parameters. It is optional as well.
 * Finally, if you are into multiobjectivization, use the **'+'** character and then define the module and its parameters.

The options of the parameters such as Output Printer, Algorithm, Problem and so on must be any option from the appropiate [directory](structure.md).


So, an example of how to solve the Rastrigin problem using the Opposition-Based-Learning Competitive Particle Swarm Optimization (OBL-CPSO) provided by METCO doing 1000 evaluations and printing solutions every 100 evaluations is:

`./metcoSeq /home/username . PlainText results.txt OBL_CPSO Rastrigin EVALUATIONS 1000 100 0 0.5 30 ! 30 $ NoOp;`


* `/home/username`: Path where you can find the file _results.txt_.
* The next parameter is `.` which tells us that we are running METCO from the same `(...)/src/skeleton/main/` directory.
*   The OBL_CPSO algorithm developed in METCO use these parameters:
    * Gamma: 0.5
    * Population size: 30
* After the '!' character goes the parameters for the problem. In this case, the Rastrigin functions only needs the number of dimensions which in this example is 30.
* Finally, `$ NoOp` indicates that we are not going to use any local search.
### Parallel Runs
_Coming soon..._
