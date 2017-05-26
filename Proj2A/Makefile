CC = gcc
CFLAGS  = -g -Wall -lpthread
sources = Makefile $(TARGET1).c $(TARGET2_1).c $(TARGET2_2).c $(TARGET2_2).h README tests.sh $(TARGET1).csv $(TARGET2_1).csv $(TARGET1)-1.png $(TARGET1)-2.png $(TARGET1)-3.png $(TARGET1)-4.png $(TARGET1)-5.png $(TARGET2_1)-1.png $(TARGET2_1)-2.png $(TARGET2_1)-3.png $(TARGET2_1)-4.png
TARGET = lab2
TARGET1 = $(TARGET)_add
TARGET2_1 = $(TARGET)_list
TARGET2_2 = SortedList
UID = 304759850

all: build

build: $(TARGET1).c $(TARGET2_1).c $(TARGET2_2).c
	 $(CC) -o $(TARGET1) $(TARGET1).c $(CFLAGS)
	 $(CC) -o $(TARGET2_1) $(TARGET2_1).c $(TARGET2_2).c $(CFLAGS)

tests: clean build
	 @bash ./tests.sh

graphs: tests
	@gnuplot ./$(TARGET1).gp
	@gnuplot ./$(TARGET2_1).gp

clean: 
	 $(RM) $(TARGET1)
	 $(RM) $(TARGET2_1)
	 $(RM) $(TARGET1).csv
	 $(RM) $(TARGET2_1).csv
	 $(RM) $(TARGET1)-1.png
	 $(RM) $(TARGET1)-2.png
	 $(RM) $(TARGET1)-3.png
	 $(RM) $(TARGET1)-4.png
	 $(RM) $(TARGET1)-5.png
	 $(RM) $(TARGET2_1)-1.png
	 $(RM) $(TARGET2_1)-2.png
	 $(RM) $(TARGET2_1)-3.png
	 $(RM) $(TARGET2_1)-4.png
	 
dist:
	 tar -czf $(TARGET)a-$(UID).tar.gz $(sources)
	