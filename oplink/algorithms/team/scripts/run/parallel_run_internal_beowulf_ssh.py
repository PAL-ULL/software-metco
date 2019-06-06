#!/usr/bin/python3
import sys
import re
from queue import Queue


ARGS = 2
MACHINE = "Machinefile:"
NOEXP = "Name_of_experiment:"
PREACTION = "Preaction:"
TMP_FILES = 'experimentsTmpFiles'
EXP_LIST = "experiment_list"


def parse_experiment_config_file(filename):
    machinefile = None
    name_of_experiment = None
    preaction = None
    with open(filename) as file:
        lines = file.readlines()
    for line in lines:
        # TODO
        pass
    return name_of_experiment, machinefile, preaction


def get_nodes_or_tasks(file, result_queue):
    with open(file) as f:
        lines = f.readlines()
    for line in lines:
        result_queue.put(line)


if __name__ == "__main__":
    if len(sys.argv) != ARGS:
        raise AttributeError("Experiment file not found")

    machines = Queue()
    tasks = Queue()
    finished = Queue()
    machinefile, name_of_experiment, preaction = parse_experiment_config_file(
        sys.argv[1])
    if machinefile is None:
        raise Exception("Machinefile not found")
    if name_of_experiment is None:
        raise("Name_of_experimento not found")

    experiment_file = f'{TMP_FILES}/{name_of_experiment}/{EXP_LIST}'

    # Obtenemos los nodos y las tareas
    get_nodes_or_tasks(machinefile, machines)
    if machines.empty:
        raise Exception("Machines non specified")
    get_nodes_or_tasks(experiment_file, tasks)
    if tasks.empty:
        raise Exception("Tasks non specified")
