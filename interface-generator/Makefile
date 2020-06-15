PROGRAM = interface-elem
CXX     = g++

# LIBS= -lboost_regex-mt 
LIBDIRS = -L/opt/local/lib/ \

INCLUDEDIRS = \
          -I/opt/local/include \

CFLAGS = -O0 -g -Wall $(INCLUDEDIRS)
LFLAGS = $(LIBS) $(LIBDIRS)

SOURCES=$(wildcard *.cpp)
OBJECTS=$(SOURCES:.cpp=.o)

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $@ $(OBJECTS) $(LFLAGS)

.cpp.o:
	$(CXX) $(CFLAGS) -o $@ -c $<

clean:
	rm $(PROGRAM) $(OBJECTS)


