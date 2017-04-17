CC = gcc
CFLAGS  = -g -Wall
sources = Makefile $(TARGET).c README
TARGET = lab1a
UID = 304759850

all: $(TARGET)

$(TARGET): $(TARGET).c
	 $(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean: 
	 $(RM) $(TARGET)
	 
dist:
	 tar -czf $(TARGET)-$(UID).tar.gz $(sources)
	