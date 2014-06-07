MKDIR=mkdir
CP=cp
CXX?=CXX
CCADMIN=CCadmin

build: dson.a

dson.a:
	$(CXX) -c dson.cpp
	ar rvs dson.a dson.o

clean:
	$(RM) dson.o
	$(RM) dson.a

clobber:

all: build clean

test:
	echo "Testing NYI."
