ls1=(1 2 4 8 12 16 24)
for t in ${ls1[@]}; do
	./lab2_list --threads=$t --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=$t --iterations=1000 --sync=s >> lab2b_list.csv
done