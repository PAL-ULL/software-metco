
import json
import itertools
import os

# Constantes de un experimento
JOBS = "jobs"
ALGS = "Algorithms"
NAME = "Name"
ARGS = "Args"
PROB = "Problem"
STOP_CRITERIA = "StopCriteria"
STOP_LIMIT = "StopLimit"
STEPS = "Steps"
OUTPUT_DIR = "OutputDir"
OUTPUT_FILE = "OutputFilePattern"
PRINTER = "Printer"
REPS = "Repetitions"
PROB_ARGS = "ProblemArgs"


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
        """
            Crea trabajos en funcion de lo obtenido en el fichero de experimento
        """
        for job in jobs:
            command = None
            # Creamos el directorio de resultados si no existe
            output_dir = job[OUTPUT_DIR] + RESULTS
            if not os.path.exists(output_dir):
                os.makedirs(output_dir)

            # Generamos los comandos de ejecucion para cada posible opcion
            for algorithm in job[ALGS]:
                print(algorithm)
                print(job[PROB])
                print(job[PROB_ARGS])

                for rep in range(job[REPS]):
                    command = f"{METCO_EXEC_} {output_dir} {METCO_PATH_} "
                    command += f"{job[PRINTER]} {algorithm[OUTPUT_FILE]}_{rep} "
                    command += f"{algorithm[NAME]} {job[PROB]} {job[STOP_CRITERIA]} "
                    command += f"{job[STOP_LIMIT]} {job[STEPS]} 0 "

                    # Parametros del algoritmo vienen aqui
                    command += " ".join(str(x)
                                        for x in algorithm[ARGS].values()) + " "

                    command += PROBLEM_SEP + " " + \
                        " ".join(str(x) for x in job[PROB_ARGS].values()) + " "
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
