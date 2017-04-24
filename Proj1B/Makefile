CC = gcc
CFLAGS  = -g -Wall
sources = Makefile $(TARGET1).c $(TARGET2).c README my.key
TARGET = lab1b
TARGET1 = $(TARGET)-server
TARGET2 = $(TARGET)-client
UID = 304759850

all: $(TARGET)

$(TARGET): $(TARGET1).c $(TARGET2).c
	 $(CC) $(CFLAGS) -o $(TARGET1) $(TARGET1).c
	 $(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).c

$(TARGET1): $(TARGET1).c
	 $(CC) $(CFLAGS) -o $(TARGET1) $(TARGET1).c

$(TARGET2): $(TARGET2).c
	 $(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).c

clean: 
	 $(RM) $(TARGET1)
	 $(RM) $(TARGET2)
	 
dist:
	 tar -czf $(TARGET)-$(UID).tar.gz $(sources)
	