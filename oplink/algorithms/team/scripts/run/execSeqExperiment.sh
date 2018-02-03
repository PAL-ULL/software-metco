#!/bin/bash
./parseExperiment $1
./parallel_run_internal_beowulf_ssh.pl $1
