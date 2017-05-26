#!/bin/bash -x
for t in `seq 1 5`;
do
	i=1
	for j in `seq 1 3`;
	do
		let i*=10
		for k in `seq 1 9`;
		do
			let it=i*k
			./lab2_add --threads=$t --iterations=${it} >> lab2_add.csv
		done
	done
done

ls1=(2 4 8 12)
ls2=(10 20 40 80 100 1000 10000 100000)
for t in ${ls1[@]}; do
	for i in ${ls2[@]}; do
		./lab2_add --threads=$t --iterations=$i --yield >> lab2_add.csv
	done
done

ls3=(10 100 1000 10000)
for t in ${ls1[@]}; do
	for i in ${ls3[@]}; do
		./lab2_add --threads=$t --iterations=$i --yield >> lab2_add.csv
		./lab2_add --threads=$t --iterations=$i --yield --sync=m >> lab2_add.csv
		./lab2_add --threads=$t --iterations=$i --yield --sync=c >> lab2_add.csv
	done
	./lab2_add --threads=$t --iterations=10 --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=$t --iterations=100 --yield --sync=s >> lab2_add.csv
	./lab2_add --threads=$t --iterations=1000 --yield --sync=s >> lab2_add.csv
done

ls4=(1 2 4 8 12)
for t in ${ls4[@]}; do
	./lab2_add --threads=$t --iterations=10000 >> lab2_add.csv
	./lab2_add --threads=$t --iterations=10000 --sync=m >> lab2_add.csv
	./lab2_add --threads=$t --iterations=10000 --sync=s >> lab2_add.csv
	./lab2_add --threads=$t --iterations=10000 --sync=c >> lab2_add.csv
done

ls5=(10 100 1000 10000 20000)
for i in ${ls5[@]}; do
	./lab2_list --threads=1 --iterations=$i >> lab2_list.csv
done

ls6=(1 10 100 1000)
for t in ${ls1[@]}; do
	for i in ${ls6[@]}; do
		./lab2_list --threads=$t --iterations=$i >> lab2_list.csv
	done
done

ls7=(1 2 4 8 16 32)
for t in ${ls1[@]}; do
	for i in ${ls7[@]}; do
		./lab2_list --threads=$t --iterations=$i --yield=i >> lab2_list.csv
		./lab2_list --threads=$t --iterations=$i --yield=d >> lab2_list.csv
		./lab2_list --threads=$t --iterations=$i --yield=il >> lab2_list.csv
		./lab2_list --threads=$t --iterations=$i --yield=dl >> lab2_list.csv
	done
done

./lab2_list --threads=12 --iterations=32 --yield=i >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=d >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=il >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=dl >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=i --sync=m >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=d --sync=m >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=il --sync=m >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=dl --sync=m >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=i --sync=s >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=d --sync=s >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=il --sync=s >> lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=dl --sync=s >> lab2_list.csv

ls8=(1 2 4 8 12 16 24)
for t in ${ls8[@]}; do
	./lab2_list --threads=$t --iterations=1000 --sync=m >> lab2_list.csv
	./lab2_list --threads=$t --iterations=1000 --sync=s >> lab2_list.csv
done