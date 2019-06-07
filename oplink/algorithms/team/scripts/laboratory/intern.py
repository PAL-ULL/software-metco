
from paramiko import SSHClient
from queue import Queue
import logging
import threading
import time
from parser import Parser
import warnings

warnings.filterwarnings(action='ignore', module='.*paramiko.*')


class Intern:
    """
        Clase que representa a un becario. El que se encarga de ejecutar todo 
        lo que le mandan.
    """

    def __init__(self, experiment_list=None, machines=["localhost"], max_workers=20):
        self.experiment_list = experiment_list
        self.machines = machines
        self.max_workers = max_workers

    def run_experiments(self):
        if len(self.machines) == 1:
            # Ejecutar en la misma maquina pero paralelo si es posible
            exp_queue = Queue()
            list = [exp_queue.put(x) for x in self.experiment_list]
            self.__run_experiments(exp_queue, self.machines[0])

        else:
            exps_per_machine = len(self.experiment_list) // len(self.machines)
            print(f"Exps per machine: {exps_per_machine}")

    def __run_experiments(self, experiment_queue, machine):
        """
            Ejecuta la cola de experimentos en la maquina que le toca
        """
        threads = list()
        while not experiment_queue.empty():
            exp = experiment_queue.get()
            th = threading.Thread(
                target=self.__run_ssh_experiment, args=(exp, machine))
            threads.append(th)
            th.start()
        for index, thread in enumerate(threads):
            logging.info("Main    : before joining thread %d.", index)
            thread.join()
            logging.info("Main    : thread %d done", index)

    def __run_ssh_experiment(self, exp, machine):
        print(f"Running: {exp} in {machine}")
        ssh = SSHClient()
        ssh.load_system_host_keys()
        ssh.connect(machine)
        ssh_stdin, ssh_stdout, ssh_stderr = ssh.exec_command(exp)

        error = ssh_stderr.read()
        output = ssh_stdout.read()
        if error != b'':
            raise RuntimeError(error)


if __name__ == "__main__":
    filename = "/home/marrero/software-metco/oplink/algorithms/team/scripts/experiments/KnapsackProblem/experiment_configs/simple_exp.json"
    parser = Parser(filename)
    experiments = parser.get_experiments()
    for i in experiments:
        print(i)

    print(f"Total: {len(experiments)}")

    intern = Intern(experiments)
    intern.run_experiments()
