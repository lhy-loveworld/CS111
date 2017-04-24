CC = gcc
CFLAGS  = -g -Wall -lmcrypt
sources = Makefile $(TARGET1).c $(TARGET2).c README my.key
TARGET = lab1b
TARGET1 = $(TARGET)-server
TARGET2 = $(TARGET)-client
UID = 304759850

all: $(TARGET)

$(TARGET): $(TARGET)-$(TARGET1).c $(TARGET)-$(TARGET2).c
	 $(CC) -o $(TARGET1) $(TARGET1).c $(CFLAGS)
	 $(CC) -o $(TARGET2) $(TARGET2).c $(CFLAGS)

$(TARGET1): $(TARGET1).c
	 $(CC) -o $(TARGET1) $(TARGET1).c $(CFLAGS)

$(TARGET2): $(TARGET)-$(TARGET2).c
	 $(CC) -o $(TARGET2) $(TARGET2).c $(CFLAGS)

clean: 
	 $(RM) $(TARGET1)
	 $(RM) $(TARGET2)
	 
dist:
	 tar -czf $(TARGET)-$(UID).tar.gz $(sources)
	