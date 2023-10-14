CC  = g++
CXX = g++

INCLUDES = Calculator.hpp setting.hpp Object.hpp AscalFrame.hpp AscalParameters.hpp ObjectKey.hpp
UTILINCLUDES = queue.hpp stack.hpp Vect2D.hpp unsortedlist.hpp
CFLAGS   = -O3 -Wall
CXXFLAGS = -O3 -std=c++17 -Wall

LDFLAGS = 
LDLIBS = 

sample_main: sample_main.o Fast_Attribute_DFA.o Attribute_DFA.o Attribute_NFA.o 

sample_main.o: sample_main.cpp Regex.hpp

Fast_Attribute_DFA.o: Attribute_DFA.hpp Fast_Attribute_DFA.cpp

Attribute_DFA.o: Attribute_DFA.hpp Attribute_DFA.cpp

Attribute_NFA.o: Attribute_NFA.hpp