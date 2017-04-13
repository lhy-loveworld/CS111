CC = gcc
CFLAGS  = -g -Wall
sources = Makefile lab0.c README backtrace.png breakpoint.png check.sh input_file.txt output_file.txt stdin.txt stdout.txt
TARGET = lab0
UID = 304759850

all: $(TARGET)

$(TARGET): $(TARGET).c
	 $(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean: 
	 $(RM) $(TARGET)
	 
dist:
	 tar -czf $(TARGET)-$(UID).tar.gz $(sources)
	
check:   clean all
	 @bash ./check.sh