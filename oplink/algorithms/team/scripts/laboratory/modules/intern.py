
from paramiko import SSHClient
import select
from queue import Queue
import logging
import threading
import time
from .parser import Parser
import warnings
import logging

logging.getLogger("paramiko").setLevel(logging.WARNING)
warnings.filterwarnings(action='ignore', module='.*paramiko.*')


class Intern:
    """
        Clase que representa a un becario.
        El que se encarga de ejecutar todo lo que le mandan.
    """

    def __init__(self, experiment_list=None, machine="localhost", max_workers=10):
        self.experiment_list = experiment_list
        self.machine = machine
        self.max_workers = max_workers

    def run_experiments(self):
        # Interfaz para ejecutar los experimentos creados
        self.__run_experiments(self.experiment_list,
                               self.machine, self.max_workers)

    def __add_experiment_to_queue(self, experiment, machine, exp_queue):
        th = threading.Thread(
            target=self.__run_ssh_experiment, args=(experiment, machine))
        th.start()
        exp_queue.put(th)

    def __run_experiments(self, experiment_list, machine, workers=10):
        """
            Ejecuta los experimentos de la lista en la maquina que se pasa
            como parametro en grupos de batch_size inicialmente.
            Cuando un experimento acaba, otro lo sustituye automaticamente.
        """
        exp_queue = Queue()

        for i in experiment_list[:workers]:
            self.__add_experiment_to_queue(i, machine, exp_queue)

        counter = workers
        while counter < len(experiment_list) - 1:
            th = exp_queue.get()
            th.join()
            exp_queue.task_done()

            self.__add_experiment_to_queue(
                experiment_list[counter + 1], machine, exp_queue)
            counter += 1
            print(f"{counter}/{len(experiment_list) - 1} experiments performed")

        logging.info("Done")

    def __run_ssh_experiment(self, exp, machine):
        """
            Ejecuta el experimento sobre SSH en la maquina especificada
        """
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
