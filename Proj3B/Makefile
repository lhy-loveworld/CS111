#NAME: Hongyang Li, Zhenli Jiang
#EMAIL: hyli@g.ucla.edu, jenny921111@gmail.com
#ID: 304759850, 304878235
sources = Makefile $(TARGET).py README
TARGET = lab3b
UID = 304759850

all: build

build:
	 cp $(TARGET).py $(TARGET)
	 chmod 777 $(TARGET)

clean:
	 $(RM) $(TARGET)
	 $(RM) $(TARGET)-$(UID).tar.gz
	 
dist:
	 tar -czf $(TARGET)-$(UID).tar.gz $(sources)
	
