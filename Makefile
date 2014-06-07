MKDIR=mkdir
CP=cp
CXX?=CXX
OUTPUT?=dson.a
CCADMIN=CCadmin

build: dson.a

dson.a:
	$(CXX) -c *.cpp
	ar rvs $(OUTPUT) *.o

clean:
	$(RM) *.o
	$(RM) $(OUTPUT)

clobber:

all: build clean

test:
	echo "Testing NYI."
