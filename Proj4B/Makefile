CC = gcc
CFLAGS  = -g -Wall -lmraa -lm
sources = Makefile $(TARGET).c README tests.sh
TARGET = lab4b
UID = 304759850
platform = $(shell uname -r | grep 'edison')

all: build

build: $(TARGET).c

ifeq ($(platform),)
	 $(CC) -o $(TARGET) $(TARGET).c $(CFLAGS) -DDUMMY
else
	 $(CC) -o $(TARGET) $(TARGET).c $(CFLAGS)
endif

check: clean build
	 @bash ./tests.sh

clean: 
	 $(RM) $(TARGET)
	 
dist:
	 tar -czf $(TARGET)-$(UID).tar.gz $(sources)
	