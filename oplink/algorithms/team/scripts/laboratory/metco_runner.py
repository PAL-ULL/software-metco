import logging
from intern import Intern
from parser import Parser
import datetime

LOG_FILE = "metco_runner"
EXT = ".log"


def main():
    date = datetime.date.today().strftime("%d-%m-%Y-%H-%M-%S")
    log_filename = LOG_FILE + "_" + date + EXT
    print(f"Logs at: {log_filename}")
    logging.basicConfig(filename=log_filename, level=logging.INFO)
    logging.info('Started')

    filename = "/home/marrero/software-metco/oplink/algorithms/team/scripts/experiments/KnapsackProblem/experiment_configs/sample2.json"
    parser = Parser(filename)
    experiments = parser.get_experiments()
    print(f"Total: {len(experiments)}")

    intern = Intern(experiments)
    intern.run_experiments()

    logging.info('Finished')


if __name__ == '__main__':
    main()
