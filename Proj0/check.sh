#!/bin/bash
#Arthor: Hongyang Li
#This is the bash script used for the smoke_test for this lab.
#There are 8 test cases in this script.

#Test 1 is to call the excutable without any arguments.
echo 'Test 1: stdin to stdout'
./lab0 < stdin.txt > stdout.txt
if [ $? -eq 0 ]
then
	cmp -s stdin.txt stdout.txt
	if [ $? -eq 0 ]
	then
		echo "Test 1 succeeded."
	else
		echo "Test 1 failed."
	fi
else
	echo "Test 1 failed."
fi

#Test 2 is to set an input path.
echo 'Test 2: file to stdout'
./lab0 --input=input_file.txt > stdout.txt
if [ $? -eq 0 ]
then
	cmp -s input_file.txt stdout.txt
	if [ $? -eq 0 ]
	then
		echo "Test 2 succeeded."
	else
		echo "Test 2 failed."
	fi
else
	echo "Test 2 failed."
fi

#Test 3 is to set an output path.
echo 'Test 3: stdin to file'
./lab0 --output=output_file.txt < stdin.txt
if [ $? -eq 0 ]
then
	cmp -s output_file.txt stdin.txt
	if [ $? -eq 0 ]
	then
		echo "Test 3 succeeded."
	else
		echo "Test 3 failed."
	fi
else
	echo "Test 3 failed."
fi

#Test 4 is to set both input and output paths.
echo 'Test 4: file to file'
./lab0 --input=input_file.txt --output=output_file.txt
if [ $? -eq 0 ]
then
	cmp -s output_file.txt input_file.txt
	if [ $? -eq 0 ]
	then
		echo "Test 4 succeeded."
	else
		echo "Test 4 failed."
	fi
else
	echo "Test 4 failed."
fi

#Test 5 is to check the error using an unrecognizable argument
echo 'Test 5: unrecognizable argument'
./lab0 --foo &> /dev/null
if [ $? -eq 1 ]
then
	echo "Test 5 succeeded."
else
	echo "Test 5 failed."
fi

#Test 6 is to check the error openning an input file.
echo 'Test 6: unable to open input file'
./lab0 --input=JUST_YOUR_IMAGINATION &> /dev/null
if [ $? -eq 2 ]
then
	echo "Test 6 succeeded."
else
	echo "Test 6 failed."
fi

#Test 7 is to check the error creating an output file.
echo 'Test 7: unable to create output file'
touch CANT_TOUCH_THIS
chmod 444 CANT_TOUCH_THIS
./lab0 --output=CANT_TOUCH_THIS &> /dev/null
if [ $? -eq 3 ]
then
	echo "Test 7 succeeded."
else
	echo "Test 7 failed."
fi
rm -f CANT_TOUCH_THIS

#Test 8 is to check catching segmentation fault.
echo 'Test 8: segmentation fault catching'
./lab0 -s -c &> /dev/null
if [ $? -eq 4 ]
then
	echo "Test 8 succeeded."
else
	echo "Test 8 failed."
fi