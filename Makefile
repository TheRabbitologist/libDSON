MKDIR=mkdir
CP=cp
CXX?=CXX
OUTPUT?=dson.a
CCADMIN=CCadmin

build: dson.a

dson.a:
	$(CXX) -c *.cpp
	ar rvs $(OUTPUT) dson.o
	$(CXX) -o test test.o dson.o

clean:
	$(RM) *.o
	$(RM) $(OUTPUT)
	$(RM) "test"

clobber:

all: build clean

test:
	echo "Testing NYI."
