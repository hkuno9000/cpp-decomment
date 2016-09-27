# Makefile - for decomment
#
# Project Home: https://github.com/hkuno9000/cpp-decomment
# Code license: MIT License
#-------------------------------------------------------------------------
# MAIN TARGET
TARGET=Release/decomment
ifndef PREFIX
PREFIX=/usr/local
endif

all:	build

#-------------------------------------------------------------------------
# COMMANDS
#
clean:
	$(RM) *.o *~ *.bak *.tmp
	$(RM) *.decomment $(TARGET)

cleanall: clean
	$(RM) *.zip

rebuild: clean build

build: $(TARGET)

$(TARGET): src/decomment.cpp
	if [ ! -d Release ]; then mkdir Release; fi
	$(CXX) $(CXXFLAGS) -Wall $^ -o $@

install:
	install $(TARGET) $(PREFIX)/bin/decomment

zip:
	git archive --format zip master -o decomment-master.zip

#.........................................................................
# TEST
#
include test/testrun.mak

# GNUmakefile - end
