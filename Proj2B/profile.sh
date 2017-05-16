#!/bin/bash
rm -f ./raw.gperf
LD_PRELOAD=/u/cs/grad/patnaiku/lib/libprofiler.so.0 CPUPROFILE=raw.gperf ./lab2_list --threads=12 --iterations=1000 --sync=s
pprof --text ./lab2_list ./raw.gperf > profile.out
pprof --list=test_run ./lab2_list ./raw.gperf >> profile.out
rm -f ./raw.gperf