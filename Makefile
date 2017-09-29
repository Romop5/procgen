SRCFILES=statement.cpp main.cpp atomicstatement.cpp
OBJFILES=$(SRCFILES:.cpp=.o)

%.o: %.cpp %.h
	$(CXX) -c $.cpp $(CPPFLAGS) $(CXXFLAGS)
main: $(OBJFILES)
	g++ $(OBJFILES)

clean:
	rm -rf *.o
	rm -rf main
