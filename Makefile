CPPFLAGS=-std=c++11
SRCFILES=statement.cpp main.cpp atomicstatement.cpp
OBJFILES=$(SRCFILES:.cpp=.o)

%.o: %.cpp %.h
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@
main: $(OBJFILES)
	g++ $(OBJFILES)

clean:
	rm -rf *.o
	rm -rf main
