CC = gcc
CFLAGS  = -g -Wall -lmraa -lm
sources = Makefile $(TARGET).c README
TARGET = lab4c
UID = 304759850
platform = $(shell uname -r | grep 'edison')

all: build

build: $(TARGET).c

ifeq ($(platform),)
	 $(CC) -o $(TARGET)_tcp $(TARGET).c $(CFLAGS) -DDUMMY
	 $(CC) -o $(TARGET)_tls $(TARGET).c $(CFLAGS) -DDUMMY
else
	 $(CC) -o $(TARGET)_tcp $(TARGET).c $(CFLAGS)
	 $(CC) -o $(TARGET)_tls $(TARGET).c $(CFLAGS)
endif

check: clean build
	 @bash ./tests.sh

clean: 
	 $(RM) $(TARGET)_tcp
	 $(RM) $(TARGET)_tls
	 $(RM) $(TARGET)-$(UID).tar.gz
	 
dist:
	 tar -czf $(TARGET)-$(UID).tar.gz $(sources)
	