# Metaheuristic-based Extensible Tool for Cooperative Optimisation (METCO) 
#### Version registered at Safe Creative

## Running an experiment - New version

For running an experiment in METCO, this new version uses JSON format experiment configuration files. These files must have the following entries:

- jobs: An array of experiments to execute.

```json
{
    "jobs": [
        {
            (...)
        }
    ]
}
```

For every job in the *jobs* array, you may specify the following entries:

- Algorithms: It must be an array with the configuration of every algorithm we want to test. For example, the following snippet shows to configuration of GA:

```json
"Algorithms": [
                {
                    "Name": "MonoGA",
                    "Args": {
                        "popSize": 250,
                        "pm": 0.8,
                        "pc": 0.1,
                        "survivalSelection": 0
                    },
                    "OutputFilePattern": "Knapsack_MonoGA_250_08_01"
                },
                {
                    "Name": "MonoGA",
                    "Args": {
                        "popSize": 150,
                        "pm": 0.8,
                        "pc": 0.1,
                        "survivalSelection": 0
                    },
                    "OutputFilePattern": "Knapsack_MonoGA_150_08_01"
                }
            ],
```
Every configuration is a JSON object inside the *Algorithms* array and for every configuration, we must set:
- *Name* of the experiment as is described in METCO algorithm subfolder.
- *Args* for the algorithm in the order as it expect them.
- *OutputFilePattern* specify how the results files will be named. Every file will follow that pattern including the repetition index and the Metco Result File (.mrf) extension. Results will be placed inside the *OuputDir* directory in a folder called **raw_metco_results**.

___

- Problem to solve: Only one per job. It must be an JSON object with the following keys: 
    - *Name* of the problem as exactly it is in the problem subdirectory of METCO.
    - *Args* of the problem in the same order that the problem expect them.

```json
      "Problem": {
                "Name": "Knapsack",
                "Args": {
                    "Filename": "path_to_instance/KnapsackInstance_50_0_0.kp",
                    "Crossover": 0
                }
            }
```

- Finally, the order parameters of the execution must be specified, i.e. stop criteria, repetitions, operators, etc.

```json
            "StopCriteria": "EVALUATIONS",
            "StopLimit": 10000,
            "Steps": 1000,
            "OutputDir": "/home/username",
            "Printer": "PlainText",
            "Repetitions": 30,
            "Mutation": "Mutate_BinaryFlip_All",
            "Crossover": "Crossover_BLX_Alpha",
            "LocalSearch": "NoOp"
```
---

### Full example:

```json
{
    "jobs": [
        {
            "Algorithms": [
                {
                    "Name": "MonoGA",
                    "Args": {
                        "popSize": 250,
                        "pm": 0.8,
                        "pc": 0.1,
                        "survivalSelection": 0
                    },
                    "OutputFilePattern": "Knapsack_MonoGA_250_08_01"
                },
                {
                    "Name": "MonoGA",
                    "Args": {
                        "popSize": 150,
                        "pm": 0.8,
                        "pc": 0.1,
                        "survivalSelection": 0
                    },
                    "OutputFilePattern": "Knapsack_MonoGA_150_08_01"
                }
            ],
            "Problem": {
                "Name": "Knapsack",
                "Args": {
                    "Filename": "path_to_instance/KnapsackInstance_50_0_0.kp",
                    "Crossover": 0
                }
            },
            "StopCriteria": "EVALUATIONS",
            "StopLimit": 10000,
            "Steps": 1000,
            "OutputDir": "/home/username",
            "Printer": "PlainText",
            "Repetitions": 30,
            "Mutation": "Mutate_BinaryFlip_All",
            "Crossover": "Crossover_BLX_Alpha",
            "LocalSearch": "NoOp"
        }
    ]
}
```

## How to run the experiment

For running the experiment we must execute the python package inside the *laboratory* directory. By default, all the experiments will be running in the localhost machine. However, another machine could be set adding the address as a second parameter when running the python module.

**Make sure you can connect to the external machine using SSH without specifying any password**.

```bash

python metco_runner.py path_to_exp_config_file/config_file.json localhost

```