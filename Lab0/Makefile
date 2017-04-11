CC = gcc
CFLAGS  = -g -Wall
sources = Makefile lab0.c README backtrace.png breakpoint.png
TARGET = lab0

all: $(TARGET)

$(TARGET): $(TARGET).c
	 $(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean: 
	 $(RM) $(TARGET)
	 
dist:
	 @tar -czf lab0-304759850.tar.gz $(sources)
	
check:
	 @