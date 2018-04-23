# Metaheuristic-based Extensible Tool for Cooperative Optimisation (METCO) -- Version registered at Safe Creative

## Setup

The steps you must follow to [start](../README.md) using METCO can be found in the [README](../README.md) file. However, here is the list of all the steps you must follow from cloned the repo to finally run an algorithm to solve a problem in METCO. 

1. First of all, download or clone this repo.
2. After that, move to `path_where_cloned/software-metco/oplink/algorithms/team/src/skeleton/main`.
3. Now run the configuration scripts:
    * `$ ./gen.sh`.
    * `$ ./configure`.
4. Run `make` to build the tool (make sure you have installed all the [dependencies](../README.md)).
5. Due to the fact that METCO allows parallel runs in multiple nodes over HPC systems or any multicore computer, you must configure a ssh connection in the machine you are going to use without using password. A good tutorial of how to achieve this can be found [here](https://www.thegeekstuff.com/2008/11/3-steps-to-perform-ssh-login-without-password-using-ssh-keygen-ssh-copy-id).

As a result of the compilation process, we will have two variations of METCO. So, from now on, we are going to discern between sequential (_metcoSeq_) and parallel (_metcoPar_) runs in METCO. This two programs can be found in the path `path_where_cloned/software-metco/oplink/algorithms/team/src/skeleton/main`.

### Sequential Runs

If your purpose is using METCO for sequential runs, _metcoSeq_ is the variation you must use. This program requires the following parameters for a single run:

* Output Path
* Plugin Path
* Output Printer Module
* Output File
* Algorithm 
* Problem
* Stopping Criteria
* Stopping Criteria Value
* Print Period
* Using an Externa Archive
* Algorithm Parameters
* Problem Parameters
* Score Module and Parameters of the Score Module (Optional)
* Mutation and Crossover operators (Optional)
* Local Search and Local Search Parameters (Optional)
* Multiobjectivization and Parameters 


### Parallel Runs
Coming soon...