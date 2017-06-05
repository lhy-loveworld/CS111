#NAME: Hongyang Li, Zhenli Jiang
#EMAIL: hyli@g.ucla.edu, jenny921111@gmail.com
#ID: 304759850, 304878235
sources = Makefile $(TARGET) README
TARGET = lab3b
UID = 304759850

all: build

build:
	 cp $(TARGET).py $(TARGET)

clean:
	 $(RM) $(TARGET)
	 
dist:
	 tar -czf $(TARGET)-$(UID).tar.gz $(sources)
	