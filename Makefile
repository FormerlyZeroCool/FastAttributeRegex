CC  = g++
CXX = g++

INCLUDES = Calculator.hpp setting.hpp Object.hpp AscalFrame.hpp AscalParameters.hpp ObjectKey.hpp
UTILINCLUDES = queue.hpp stack.hpp Vect2D.hpp unsortedlist.hpp
CFLAGS   = -O3 -Wall
CXXFLAGS = -O3 -std=c++17 -Wall

LDFLAGS = 
LDLIBS = 
default: sample_main regex.a

run: sample_main
	./sample_main example.txt testinput.c

sample_main: sample_main.o Fast_Attribute_DFA.o Attribute_DFA.o Attribute_NFA.o 

regex.a: Fast_Attribute_DFA.o Attribute_DFA.o Attribute_NFA.o
	ar rcs $@ $^ 

sample_main.o: sample_main.cpp Regex.hpp

Fast_Attribute_DFA.o: Attribute_DFA.hpp Fast_Attribute_DFA.cpp

Attribute_DFA.o: Attribute_DFA.hpp Attribute_DFA.cpp

Attribute_NFA.o: Attribute_NFA.hpp

clean:
	rm -f *.o regex.a sample_main