#########################################################
#              DiffieHellman Makefile             #
#########################################################
# edited by Ethan Nanavati
CXX      = clang++
CXXFLAGS = -g3 -Wall -Wextra -Wpedantic -Wshadow
LDFLAGS  = -g3 


# DiffieHellman rule 
DiffieHellman: Main.o SecuredInfo.o
	${CXX} ${LDFLAGS} -o DiffieHellman Main.o SecuredInfo.o


# This rule builds PassengerQueue.o
SecuredInfo.o: SecuredInfo.cpp SecuredInfo.h 
	$(CXX) $(CXXFLAGS) -c SecuredInfo.cpp

# This rule builds main.o	
Main.o: Main.cpp SecuredInfo.h
	$(CXX) $(CXXFLAGS) -c Main.cpp

# remove executables, object code, and temporary files from the current folder 
# -- the executable created by unit_test is called a.out
clean: 
	rm *.o *~ a.out
