ls1=(1 2 4 8 12 16 24)
for t in ${ls1[@]}; do
	./lab2_list --threads=$t --iterations=1000 --sync=m >> lab2b_list.csv
	./lab2_list --threads=$t --iterations=1000 --sync=s >> lab2b_list.csv
done

ls2=(1 4 8 12 16)
ls3=(1 2 4 8 16)
ls4=(10 20 40 80)
for t in ${ls2[@]}; do
	for i in ${ls3[@]}; do
		./lab2_list --threads=$t --iterations=$i --yield=id --lists=4 >> lab2b_list.csv
	done
	for i in ${ls4[@]}; do
		./lab2_list --threads=$t --iterations=$i --yield=id --lists=4 --sync=s >> lab2b_list.csv
		./lab2_list --threads=$t --iterations=$i --yield=id --lists=4 --sync=m >> lab2b_list.csv
	done
done

ls5=(1 2 4 8 12)
ls6=(1 4 8 16)
for t in ${ls5[@]}; do
	for l in ${ls6[@]}; do
		./lab2_list --threads=$t --iterations=1000 --lists=$l --sync=m >> lab2b_list.csv
		./lab2_list --threads=$t --iterations=1000 --lists=$l --sync=s >> lab2b_list.csv
	done
done