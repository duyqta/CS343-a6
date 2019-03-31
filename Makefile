NUMFLAVOURS:=4 # Flavour constant

CXX = u++					# compiler
CXXFLAGS = -g -multi -O2 -std=c++11 -Wall -Wextra -MMD -DNUMFLAVOURS="${NUMFLAVOURS}" # compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS = soda.o config.o bank.o printer.o nameserver.o truck.o bottlingplant.o # list of object files
EXEC = soda

DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"

#############################################################

.PHONY : all clean

all : ${EXEC}					# build all executables

${EXEC} : ${OBJECTS}
	${CXX} ${CXXFLAGS} $^ -o $@

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXEC}
