# Makefile - for decomment
#
# Project Home: https://github.com/hkuno9000/cpp-decomment
# Code license: MIT License
#-------------------------------------------------------------------------
# MAIN TARGET
TARGET=Release/decomment

all:	build

#-------------------------------------------------------------------------
# COMMANDS
#
cleanall: clean
	-rm -f *.zip *.o *.bak *.decomment

clean:
	-rm -f $(TARGET)

rebuild: clean build

build: $(TARGET)

$(TARGET): src/decomment.cpp
	if [ ! -d Release ]; then mkdir Release; fi
	$(CXX) $(CXXFLAGS) -Wall $^ -o $@

install:
	install $(TARGET) /usr/local/bin

zip:
	git archive --format zip master -o decomment-master.zip

#.........................................................................
# TEST
#
include test/testrun.mak

# GNUmakefile - end
