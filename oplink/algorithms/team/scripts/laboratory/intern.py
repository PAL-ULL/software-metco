
from paramiko import SSHClient
import select
from queue import Queue
import logging
import threading
import time
from parser import Parser
import warnings
import logging

logging.getLogger("paramiko").setLevel(logging.WARNING)
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
            self.__run_experiments(self.experiment_list, self.machines[0])

        else:
            exps_per_machine = len(self.experiment_list) // len(self.machines)
            print(f"Exps per machine: {exps_per_machine}")

    def __add_experiment_to_queue(self, experiment, machine, exp_queue):
        th = threading.Thread(
            target=self.__run_ssh_experiment, args=(experiment, machine))
        th.start()
        exp_queue.put(th)

    def __run_experiments(self, experiment_list, machine, batch_size=10):
        """
            Ejecuta la cola de experimentos en la maquina que le toca
        """
        exp_queue = Queue()

        for i in experiment_list[:batch_size]:
            self.__add_experiment_to_queue(i, machine, exp_queue)
            # th = threading.Thread(
            #     target=self.__run_ssh_experiment, args=(i, machine))
            # th.start()
            # exp_queue.put(th)

        counter = batch_size
        while counter < len(self.experiment_list):  # not exp_queue.empty():
            th = exp_queue.get()
            th.join()
            exp_queue.task_done()

            self.__add_experiment_to_queue(
                self.experiment_list[counter + 1], machine, exp_queue)
            counter += 1
            print(f"{counter}/{len(self.experiment_list)} experiments performed")

        logging.info("Done")

    def __run_ssh_experiment(self, exp, machine):

        logging.info(f"\n- Running:\n{exp}\nat: {machine}")
        ssh = SSHClient()
        ssh.load_system_host_keys()
        ssh.connect(machine)

        ssh_stdin, ssh_stdout, ssh_stderr = ssh.exec_command(exp)

        error = ssh_stderr.read()
        output = ssh_stdout.read()
        if error != b'':
            logging.error(error)
            raise RuntimeError(error)
