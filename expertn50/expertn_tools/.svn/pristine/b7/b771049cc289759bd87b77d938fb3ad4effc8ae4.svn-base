#!/usr/bin/env python
import subprocess
import time
import datetime
import sys

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'


def run_n_times(n,commands):
    for item in range(n):
        print bcolors.WARNING + " ----------------- " +str(item+1) + " von " + str(n) + " ----------------- " + bcolors.ENDC
        subprocess.Popen(commands).wait()


if __name__ == "__main__":
    if len(sys.argv)<3:
        print "Parameter fehlt:"
        print "run_n_times.py n filename paramter ..."
        print "n = number of times"
        print "filename = file to execute"
    else:        
        run_n_times(int(sys.argv[1]),sys.argv[2:])        
