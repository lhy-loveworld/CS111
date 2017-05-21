#!/bin/bash
#Arthor: Hongyang Li
#This is the bash script used for the smoke_test for this lab.

echo 'Test starts: unrecognizable argument'
./lab4b --foo &> /dev/null
if [ $? -eq 1 ]
then
	echo "Test succeeded."
else
	echo "Test failed."
fi

echo 'Test starts: period=2 scale=C'
./lab4b --period=2 --scale=C --log="log.txt" <<-EOF
SCALE=F
PERIOD=3
START
STOP
OFF
EOF
if [ $? -neq 0 ]
then
	echo "Test fails. RC != 0"
fi

if [ ! -s log.txt ]
then
	echo "did not create a log file"
else
	for c in SCALE=F PERIOD=3 START STOP OFF SHUTDOWN
	do
		grep $c log.txt > /dev/null
		if [ $? -ne 0 ]
		then
			echo "DID NOT LOG $c command"
		else
			echo "   ... $c: OK"
		fi
	done
	egrep '[0-9][0-9]:[0-9][0-9]:[0-9][0-9] [0-9][0-9].[0-9]' LOGFILE > /dev/null
	if [ $? -eq 0 ] 
	then
		echo "... OK"
	else
		echo "DID NOT LOG TEMPERATURE"
	fi
fi