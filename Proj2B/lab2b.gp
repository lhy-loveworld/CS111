#! /usr/bin/gnuplot

# general plot parameters
set terminal png
set datafile separator ","

# aggregate throughput (total operations per second for all threads combined)
set title "List-1: throughput vs number of threads for mutex and spin-lock"
set xlabel "Threads"
set logscale x 10
set ylabel "total operations per second for all threads"
set logscale y 10
set output 'lab2b-1.png'

# grep out only single threaded, un-protected, non-yield results
plot \
     "< grep 'list-none-m' lab2b_list.csv" using ($2):(1e9/($7)) \
	title 'mutex' with linespoints lc rgb 'red', \
     "< grep 'list-none-s' lab2b_list.csv" using ($2):(1e9/($7)) \
	title 'spin-lock' with linespoints lc rgb 'green'