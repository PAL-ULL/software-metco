import logging
from modules.intern import Intern
from modules.parser import Parser
from datetime import datetime
import sys
import os

LOG_FILE = "metco_runner"
LOG_DIR = "./log/"
EXT = ".log"
NUM_ARGS = 2


def main(experiment_file):
    now = datetime.now()
    now_str = now.strftime("%d_%m_%Y_%H_%M_%S")
    log_filename = LOG_DIR + LOG_FILE + "_" + now_str + EXT
    print(f"Logs at: {log_filename}")
    logging.basicConfig(filename=log_filename, level=logging.INFO)
    logging.info('Started')

    if not os.path.exists(experiment_file):
        raise FileExistsError()

    parser = Parser(experiment_file)
    experiments = parser.get_experiments()
    print(f"Total: {len(experiments)}")

    intern = Intern(experiments)
    intern.run_experiments()

    logging.info('Finished')


if __name__ == '__main__':
    if len(sys.argv) != NUM_ARGS:
        raise AttributeError(
            "No enough args given. Experiment filename needed")

    if not os.path.exists(LOG_DIR):
        os.makedirs(LOG_DIR)

    main(sys.argv[1])
