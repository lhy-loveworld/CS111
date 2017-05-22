#!/bin/bash
#Arthor: Hongyang Li
#This is the bash script used for the smoke_test for this lab.

echo 'Test 1 starts: unrecognizable argument'
./lab4b --foo &> /dev/null
if [ $? -eq 1 ]
then
	echo "Test 1 succeeded."
else
	echo "Test 1 failed."
fi

let errors=0

echo 'Test 2 starts: period=2 scale=C'
./lab4b --period=2 --scale=C --log="log.txt" <<-EOF
SCALE=F
PERIOD=3
START
STOP
OFF
EOF
if [ $? -ne 0 ]
then
	echo "Test 2 fails. RC != 0"
	let errors+=1
fi
if [ ! -s log.txt ]
then
	echo "Test 2 fails. Did not create a log file"
	let errors+=1
else
	for c in SCALE=F PERIOD=3 START STOP OFF SHUTDOWN
	do
		grep $c log.txt > /dev/null
		if [ $? -ne 0 ]
		then
			echo "Test 2 fails. Did not log $c command"
			let errors+=1
		else
			echo "   ... $c: OK"
		fi
	done
	egrep '[0-9][0-9]:[0-9][0-9]:[0-9][0-9] [0-9][0-9].[0-9]' log.txt > /dev/null
	if [ $? -eq 0 ] 
	then
		echo "Correct temperature report format ... OK"
	else
		echo "Did not log temperature"
		let errors+=1
	fi
fi
if [ $errors -eq 0 ]
then
	echo "Test 2 succeeded."
fi