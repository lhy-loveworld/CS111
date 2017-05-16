#! /usr/bin/gnuplot

# general plot parameters
set terminal png
set datafile separator ","

# aggregate throughput (total operations per second for all threads combined)
set title "List-1: throughput vs number of threads for mutex and spin-lock"
set xlabel "Threads"
set logscale x 2
set ylabel "total operations per second for all threads"
set logscale y 10
set output 'lab2b-1.png'

# grep out only mutex/spin-locked, non-yield results
plot \
     "< grep 'list-none-m' lab2b_list.csv" using ($2):(1e9/($7)) \
	title 'mutex' with linespoints lc rgb 'red', \
     "< grep 'list-none-s' lab2b_list.csv" using ($2):(1e9/($7)) \
	title 'spin-lock' with linespoints lc rgb 'green'

# aggregate timing mutex waits
set title "List-2: mean time per mutex wait and mean time per operation for mutex-synchronized list operations"
set xlabel "Threads"
set logscale x 2
set ylabel "time (ns)"
set logscale y 10
set output 'lab2b-2.png'

# grep out only mutex, non-yield results
plot \
     "< grep 'list-none-m' lab2b_list.csv" using ($2):($8) \
	title 'wait-for-lock time' with linespoints lc rgb 'red', \
     "< grep 'list-none-m' lab2b_list.csv" using ($2):($7) \
	title 'cost per operation' with linespoints lc rgb 'green'

# how many threads/iterations we can run without failure (w/o yielding)
set title "List-3: successful iterations vs threads for each synchronization method"
set xlabel "Threads"
set logscale x 2
set xrange [0.75:]
set ylabel "Iterations per thread"
set logscale y 10
set output 'lab2b-3.png'

# grep out successful runs
plot \
     "< grep 'list-id-none' lab2b_list.csv" using ($2):($3) \
	title 'w/o sync' with points lc rgb 'green', \
     "< grep 'list-id-s' lab2b_list.csv" using ($2):($3) \
	title 'w/ spin-lock' with points lc rgb 'red', \
	"< grep 'list-id-m' lab2b_list.csv" using ($2):($3) \
	title 'w/ mutex' with points lc rgb 'yellow'

# aggregate throughput (total operations per second for all threads combined) for mutex
set title "List-4: throughput vs number of threads for mutex synchronized partitioned lists"
set xlabel "Threads"
set xrange [0.75:14]
set logscale x 2
set ylabel "total operations per second for all threads"
set logscale y 10
set output 'lab2b-4.png'

# grep out only spin-locked, non-yield results
plot \
     "< grep 'list-none-m' lab2b_list.csv | grep '1000,1,'" using ($2):(1e9/($7)) \
	title 'lists = 1' with linespoints lc rgb 'red', \
	"< grep 'list-none-m' lab2b_list.csv | grep '1000,4,'" using ($2):(1e9/($7)) \
	title 'lists = 4' with linespoints lc rgb 'green', \
     "< grep 'list-none-m' lab2b_list.csv | grep '1000,8,'" using ($2):(1e9/($7)) \
	title 'lists = 8' with linespoints lc rgb 'blue', \
	"< grep 'list-none-m' lab2b_list.csv | grep '1000,16,'" using ($2):(1e9/($7)) \
	title 'lists = 16' with linespoints lc rgb 'yellow'

# aggregate throughput (total operations per second for all threads combined) for spin-lock
set title "List-5: throughput vs number of threads for spin-lock-synchronized partitioned lists"
set xlabel "Threads"
set xrange [0.75:14]
set logscale x 2
set ylabel "total operations per second for all threads"
set logscale y 10
set output 'lab2b-5.png'

# grep out only spin-locked, non-yield results
plot \
     "< grep 'list-none-s' lab2b_list.csv | grep '1000,1,'" using ($2):(1e9/($7)) \
	title 'lists = 1' with linespoints lc rgb 'red', \
	"< grep 'list-none-s' lab2b_list.csv | grep '1000,4,'" using ($2):(1e9/($7)) \
	title 'lists = 4' with linespoints lc rgb 'green', \
     "< grep 'list-none-s' lab2b_list.csv | grep '1000,8,'" using ($2):(1e9/($7)) \
	title 'lists = 8' with linespoints lc rgb 'blue', \
	"< grep 'list-none-s' lab2b_list.csv | grep '1000,16,'" using ($2):(1e9/($7)) \
	title 'lists = 16' with linespoints lc rgb 'yellow'
