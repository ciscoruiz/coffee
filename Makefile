.PHONY: all clean install
default:    all
all:    
	scons --jobs=4 test
compile:    
	scons test
clean:
	scons -c
install:
	scons install

