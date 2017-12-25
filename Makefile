.PHONY: all clean install
default:    all
all:    
	scons --jobs=2 test 
clean:
	scons -c
install:
	scons install


