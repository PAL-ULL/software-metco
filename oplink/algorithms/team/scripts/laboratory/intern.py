
from paramiko import SSHClient
import select
from queue import Queue
import logging
import threading
import time
from parser import Parser
import warnings
import logging

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
            # exp_queue = Queue()
            # list = [exp_queue.put(x) for x in self.experiment_list]
            # self.__run_experiments(exp_queue, self.machines[0])
            offset = 10
            for i in range(0, len(self.experiment_list), offset):
                print(f"Running {i + offset}/{len(self.experiment_list)}")
                self.__run_experiments(
                    self.experiment_list[i:offset], self.machines[0])

        else:
            exps_per_machine = len(self.experiment_list) // len(self.machines)
            print(f"Exps per machine: {exps_per_machine}")

    def __run_experiments(self, experiment_list, machine):
        """
            Ejecuta la cola de experimentos en la maquina que le toca
        """
        exp_queue = Queue()
        threads = []
        for i in experiment_list:
            th = threading.Thread(
                target=self.__run_ssh_experiment, args=(i, machine))
            th.start()
            exp_queue.put(th)

        while not exp_queue.empty():
            th = exp_queue.get()
            th.join()
            exp_queue.task_done()

        exp_queue.join()
        logging.info("Done")

    def __run_ssh_experiment(self, exp, machine):

        logging.info(f"Running: {exp} in {machine}")
        ssh = SSHClient()
        ssh.load_system_host_keys()
        ssh.connect(machine)

        ssh_stdin, ssh_stdout, ssh_stderr = ssh.exec_command(exp)

        error = ssh_stderr.read()
        output = ssh_stdout.read()
        if error != b'':
            logging.error(error)
            raise RuntimeError(error)
