CC = gcc
CFLAGS  = -g -Wall -lpthread
sources = Makefile $(TARGET2_1).c $(TARGET2_2).c $(TARGET2_2).h README tests.sh lab2b_list.csv
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

clean: 
	 $(RM) $(TARGET2_1)
	 $(RM) lab2b_list.csv
	 $(RM) $(TARGET2_1)-1.png
	 $(RM) $(TARGET2_1)-2.png
	 $(RM) $(TARGET2_1)-3.png
	 $(RM) $(TARGET2_1)-4.png
	 
dist:
	 tar -czf $(TARGET)a-$(UID).tar.gz $(sources)
	