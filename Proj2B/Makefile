CC = gcc
CFLAGS  = -g -Wall -lpthread
sources = Makefile $(TARGET2_1).c $(TARGET2_2).c $(TARGET2_2).h README tests.sh lab2b_list.csv lab2b-1.png lab2b-2.png lab2b-3.png lab2b-4.png lab2b-5.png profile.sh profile.out
TARGET = lab2
TARGET2_1 = $(TARGET)_list
TARGET2_2 = SortedList
UID = 304759850

all: build

build: $(TARGET2_1).c $(TARGET2_2).c
	 $(CC) -o $(TARGET2_1) $(TARGET2_1).c $(TARGET2_2).c $(CFLAGS)

tests: clean build
	 @bash ./tests.sh

graphs: tests
	 @gnuplot ./lab2b.gp

profile:
	 @bash ./profile.sh

clean: 
	 $(RM) $(TARGET2_1)
	 $(RM) lab2b_list.csv
	 $(RM) lab2b-1.png
	 $(RM) lab2b-2.png
	 $(RM) lab2b-3.png
	 $(RM) lab2b-4.png
	 $(RM) lab2b-5.png
	 
dist:
	 tar -czf $(TARGET)a-$(UID).tar.gz $(sources)
	