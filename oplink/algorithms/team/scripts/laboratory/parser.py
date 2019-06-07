
import json

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

METCO_PATH_ = "/home/marrero/software-metco/oplink/algorithms/team/src/skeleton/main/"
METCO_EXEC_ = "/home/marrero/software-metco/oplink/algorithms/team/src/skeleton/main/metcoSeq"


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
            for rep in range(job[REPS]):
                command = f".{METCO_EXEC_} {job[OUTPUT_DIR]} {METCO_PATH_} "
                command += f"{job[PRINTER]} {job[OUTPUT_FILE]}_{rep} "
                command += f"{job[ALG]} {job[PROB]} {job[STOP_CRITERIA]} "
                command += f"{job[STOP_LIMIT]} {job[STEPS]} 0 "
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
    for i in experiments:
        print(i)

    print(f"Total: {len(experiments)}")
