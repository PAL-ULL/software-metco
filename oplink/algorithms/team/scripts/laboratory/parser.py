
import json
import itertools
import os

# Constantes de un experimento
JOBS = "jobs"
ALG = "Algorithm"
PROB = "Problem"
STOP_CRITERIA = "StopCriteria"
STOP_LIMIT = "StopLimit"
STEPS = "Steps"
OUTPUT_DIR = "OutputDir"
OUTPUT_FILE = "OutputFilePattern"
PRINTER = "Printer"
REPS = "Repetitions"
PROB_ARGS = "ProblemArgs"

ALG_ARGS = "AlgorithmArgs"
MUTATION = "Mutation"
CROSSOVER = "Crossover"
LOCAL_SEARCH = "LocalSearch"
DECOMPOSITION = "Decomposition"

PROBLEM_SEP = "!"
LOCAL_SEARCH_SEP = "$"
MUTATION_SEP = "-"

# Fullpaths
METCO_PATH_ = "/home/marrero/software-metco/oplink/algorithms/team/src/skeleton/main"
METCO_EXEC_ = "/home/marrero/software-metco/oplink/algorithms/team/src/skeleton/main/metcoSeq"
RESULTS = "/raw_metco_results"


class Parser:

    def __init__(self, filename, **kwargs):
        if filename is None:
            raise AttributeError("Filename can not be None")

        self.instance = filename
        self.experiment_list = []

    def get_experiments(self):
        return self.__parse_instance()

    def __create_jobs(self, jobs):
        for job in jobs:
            command = None
            alg_args = []
            for arg in job[ALG_ARGS]:
                alg_args.append(job[ALG_ARGS][arg])
            prob_args = []
            for arg in job[PROB_ARGS]:
                prob_args.append(job[PROB_ARGS][arg])

            alg_params_combs = list(itertools.product(*alg_args))
            prob_params_combs = list(itertools.product(*prob_args))

            # Creamos el directorio de resultados si no existe
            output_dir = job[OUTPUT_DIR] + RESULTS
            if not os.path.exists(output_dir):
                os.makedirs(output_dir)

            for alg_comb in alg_params_combs:
                for prob_comb in prob_params_combs:
                    for rep in range(job[REPS]):

                        command = f"{METCO_EXEC_} {output_dir} {METCO_PATH_} "
                        command += f"{job[PRINTER]} {job[OUTPUT_FILE]}_{rep} "
                        command += f"{job[ALG]} {job[PROB]} {job[STOP_CRITERIA]} "
                        command += f"{job[STOP_LIMIT]} {job[STEPS]} 0 "
                        # Parametros del algoritmo vienen aqui
                        command += " ".join(str(x) for x in alg_comb) + " "
                        command += PROBLEM_SEP + " " + \
                            " ".join(str(x) for x in prob_comb) + " "
                        # Opciones que pueden estar o no
                        if MUTATION in job and CROSSOVER in job:
                            command += f"{MUTATION_SEP} {job[MUTATION]} "
                            command += f"{job[CROSSOVER]} "
                        elif MUTATION in job and CROSSOVER not in job:
                            command += f"{MUTATION_SEP} {job[MUTATION]} "
                        elif MUTATION not in job and CROSSOVER in job:
                            command += f"{MUTATION_SEP} {job[CROSSOVER]} "

                        command += f"{LOCAL_SEARCH_SEP} {job[LOCAL_SEARCH]} "
                        self.experiment_list.append(command)
        return self.experiment_list

    def __parse_instance(self):
        data = None
        with open(self.instance, "r") as read_file:
            data = json.load(read_file)

        return self.__create_jobs(data[JOBS])


if __name__ == "__main__":
    filename = "/home/marrero/software-metco/oplink/algorithms/team/scripts/experiments/KnapsackProblem/experiment_configs/sample1.json"
    parser = Parser(filename)
    experiments = parser.get_experiments()

    print(f"Total: {len(experiments)}")
