objects = ../so/Strategy.so ../so/Strategy.so.d
targets = Judge.cpp Strategy.cpp State.cpp Node.cpp mem.cpp

all: so

so:		# Make so for local test
	g++ -Wall -std=c++11 -O3 -DSLOWER -DLOG -fpic -shared $(targets) -o ../so/Strategy.so
	cp ../so/Strategy.so ../so/Strategy.2.so

debug:	# Make so with -DDEBUG and -O0 for debug
	# **Notice that output result is Strategy.so.d**
	g++ -Wall -std=c++11 -O0 -DDEBUG -DLOG -g -fpic -shared $(targets) -o ../so/Strategy.so.d
	cp ../so/Strategy.so.d ../so/Strategy.2.so.d

clean:
	rm -f $(objects)
