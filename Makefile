# Makefile - for decomment
#
# Project Home: http://code.google.com/p/cpp-decomment/
# Code license: New BSD License
#-------------------------------------------------------------------------
# MAIN TARGET
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
TARGET=Release\decomment.exe

testrun: blank.test nest.test op.test

blank.test: $(TARGET)
	@echo test [$*]
	$(TARGET) -d.     test/$*
	diff $*.decomment test/$*.decomment
	$(TARGET) -d. -b  test/$*
	diff $*.decomment test/$*.decomment-b
	$(TARGET) -d. -i  test/$*
	diff $*.decomment test/$*.decomment-i
	$(TARGET) -d. -bi test/$*
	diff $*.decomment test/$*.decomment-bi
	$(TARGET) -d. -m test/$*
	diff $*.decomment test/$*.decomment-m
	$(TARGET) -d. -mi test/$*
	diff $*.decomment test/$*.decomment-mi

nest.test op.test: $(TARGET)
	@echo test [$*]
	$(TARGET) -d.     test/$*
	diff $*.decomment test/$*.decomment
	$(TARGET) -d. -m  test/$*
	diff $*.decomment test/$*.decomment-m

# Makefile - end
