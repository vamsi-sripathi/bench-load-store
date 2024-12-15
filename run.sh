#!/bin/bash

taskset -c 4 ./s2l-nt.out  134217728 2M 0 1 8 134217728 2>&1 | tee nt-1gib-2m-0-1-8-134217728.log #1GiB
taskset -c 4 ./s2l-rfo.out 134217728 2M 0 1 8 134217728 2>&1 | tee rfo-1gib-2m-0-1-8-134217728.log #1GiB

taskset -c 4 ./s2l-nt.out  134217728 2M 1 0 8 134217728 2>&1 | tee nt-1gib-2m-1-0-8-134217728.log #1GiB
taskset -c 4 ./s2l-rfo.out 134217728 2M 1 0 8 134217728 2>&1 | tee rfo-1gib-2m-1-0-8-134217728.log #1GiB

taskset -c 4 ./s2l-nt.out  134217728 2M 1 1 8 134217728 2>&1 | tee nt-1gib-2m-1-1-8-134217728.log #1GiB
taskset -c 4 ./s2l-rfo.out 134217728 2M 1 1 8 134217728 2>&1 | tee rfo-1gib-2m-1-1-8-134217728.log #1GiB
