# testrun.mak --- test for decomment
#
# WIN32: TARGET=Release\decomment.exe
# UNIX:  TARGET=Release\decomment
.SUFFIXES: .test  # activate $* for .test

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

# testrun.mak - end
