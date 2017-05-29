#NAME: Hongyang Li, Zhenli Jiang
#EMAIL: hyli@g.ucla.edu, jenny921111@gmail.com
#ID: 304759850, 304878235
CC = gcc
CFLAGS  = -g -Wall -lm
sources = Makefile $(TARGET).c README ext2_fs.h
TARGET = lab3a
UID = 304759850

all: build

build: $(TARGET).c
	 $(CC) -o $(TARGET) $(TARGET).c $(CFLAGS)

clean: 
	 $(RM) $(TARGET)
	 
dist:
	 tar -czf $(TARGET)-$(UID).tar.gz $(sources)
	