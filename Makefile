# Makefile - for decomment
#
# Project Home: https://github.com/hkuno9000/cpp-decomment
# Code license: MIT License
#-------------------------------------------------------------------------
# MAIN TARGET
TARGET=Release\decomment.exe

all:	build

#-------------------------------------------------------------------------
# COMMANDS
#
cleanall: clean
	del *.zip *.ncb *.user *.dat *.cache *.bak *.decomment $$* src\*.aps src\*.bak

clean:
	vcbuild /clean   decomment.sln

rebuild:
	vcbuild /rebuild decomment.sln

build:
	vcbuild decomment.sln

zip:
#	svn list -R | zip -@ -D cpp-decomment-src.zip
	zip cpp-decomment-src.zip Makefile decomment.sln decomment.vcproj src/* test/* -x *.aps
	zip cpp-decomment-exe.zip -j Release/decomment.exe

#.........................................................................
# TEST
#
!INCLUDE test/testrun.mak

# Makefile - end
