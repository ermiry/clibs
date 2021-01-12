TYPE		:= development

NATIVE		:= 0

SLIB		:= libclibs.so

ll: directories $(SLIB)

directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

install: $(SLIB)
	install -m 644 ./bin/libclibs.so /usr/local/lib/
	cp -R ./include/clibs /usr/local/include

uninstall:
	rm /usr/local/lib/libclibs.so
	rm -r /usr/local/include/clibs

PTHREAD 	:= -l pthread
MATH		:= -lm

DEFINES		:= -D _GNU_SOURCE

CC          := gcc

SRCDIR      := src
INCDIR      := include

BUILDDIR    := objs
TARGETDIR   := bin

SRCEXT      := c
DEPEXT      := d
OBJEXT      := o

COVDIR		:= coverage
COVEXT		:= gcov

# common flags
# -Wconversion
COMMON		:= -Wall -Wno-unknown-pragmas \
				-Wfloat-equal -Wdouble-promotion -Wint-to-pointer-cast -Wwrite-strings \
				-Wtype-limits -Wsign-compare -Wmissing-field-initializers \
				-Wuninitialized -Wmaybe-uninitialized -Wempty-body \
				-Wunused-parameter -Wunused-but-set-parameter -Wunused-result \
				-Wformat -Wformat-nonliteral -Wformat-security -Wformat-overflow -Wformat-signedness -Wformat-truncation

# main
CFLAGS      := $(DEFINES)

ifeq ($(TYPE), development)
	CFLAGS += -g -fasynchronous-unwind-tables
else ifeq ($(TYPE), test)
	CFLAGS += -g -fasynchronous-unwind-tables -D_FORTIFY_SOURCE=2 -fstack-protector -O2
else
	CFLAGS += -D_FORTIFY_SOURCE=2 -O2
endif

# check which compiler we are using
ifeq ($(CC), g++) 
	CFLAGS += -std=c++11 -fpermissive
else
	CFLAGS += -std=c11 -Wpedantic -pedantic-errors
	# check for compiler version
	ifeq "$(GCCVGTEQ8)" "1"
    	CFLAGS += -Wcast-function-type
	else
		CFLAGS += -Wbad-function-cast
	endif
endif

ifeq ($(NATIVE), 1)
	CFLAGS += -march=native
endif

# common flags
CFLAGS += -fPIC $(COMMON)

LIB         := -L /usr/local/lib $(PTHREAD) $(MATH)

ifeq ($(TYPE), test)
	LIB += -lgcov --coverage
endif

INC         := -I $(INCDIR) -I /usr/local/include
INCDEP      := -I $(INCDIR)

SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

SRCCOVS		:= $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(SRCEXT).$(COVEXT)))

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

$(SLIB): $(OBJECTS)
	$(CC) $^ $(LIB) -shared -o $(TARGETDIR)/$(SLIB)

# compile sources
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) $(LIB) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# tests
TESTDIR		:= test
TESTBUILD	:= $(TESTDIR)/objs
TESTTARGET	:= $(TESTDIR)/bin
TESTCOVDIR	:= $(COVDIR)/test

TESTFLAGS	:= -g $(DEFINES) -Wall -Wno-unknown-pragmas -Wno-format

ifeq ($(TYPE), test)
	TESTFLAGS += -fprofile-arcs -ftest-coverage
endif

ifeq ($(NATIVE), 1)
	TESTFLAGS += -march=native
endif

TESTLIBS	:= $(PTHREAD) -L ./bin -l clibs

ifeq ($(TYPE), test)
	TESTLIBS += -lgcov --coverage
endif

TESTINC		:= -I $(INCDIR) -I ./$(TESTDIR)

TESTS		:= $(shell find $(TESTDIR) -type f -name *.$(SRCEXT))
TESTOBJS	:= $(patsubst $(TESTDIR)/%,$(TESTBUILD)/%,$(TESTS:.$(SRCEXT)=.$(OBJEXT)))

TESTCOVS	:= $(patsubst $(TESTDIR)/%,$(TESTBUILD)/%,$(TESTS:.$(SRCEXT)=.$(SRCEXT).$(COVEXT)))

test: $(TESTOBJS)
	@mkdir -p ./$(TESTTARGET)
	@mkdir -p ./$(TESTTARGET)
	$(CC) -g $(TESTINC) -L ./$(TARGETDIR) ./$(TESTBUILD)/collections/*.o -o ./$(TESTTARGET)/collections $(TESTLIBS)
	$(CC) -g $(TESTINC) -L ./$(TARGETDIR) ./$(TESTBUILD)/utils/*.o -o ./$(TESTTARGET)/utils $(TESTLIBS)

# compile tests
$(TESTBUILD)/%.$(OBJEXT): $(TESTDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(TESTFLAGS) $(INC) $(TESTLIBS) -c -o $@ $<
	@$(CC) $(TESTFLAGS) $(INCDEP) -MM $(TESTDIR)/$*.$(SRCEXT) > $(TESTBUILD)/$*.$(DEPEXT)
	@cp -f $(TESTBUILD)/$*.$(DEPEXT) $(TESTBUILD)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(TESTBUILD)/$*.$(OBJEXT):|' < $(TESTBUILD)/$*.$(DEPEXT).tmp > $(TESTBUILD)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(TESTBUILD)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(TESTBUILD)/$*.$(DEPEXT)
	@rm -f $(TESTBUILD)/$*.$(DEPEXT).tmp

#coverage
COVOBJS		:= $(SRCCOVS) $(TESTCOVS)

test-coverage: $(COVOBJS)

coverage-init:
	@mkdir -p ./$(COVDIR)
	@mkdir -p ./$(TESTCOVDIR)

coverage: coverage-init test-coverage

# get lib coverage reports
$(BUILDDIR)/%.$(SRCEXT).$(COVEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p ./$(COVDIR)/$(dir $<)
	gcov -r $< --object-directory $(dir $@)
	mv $(notdir $@) ./$(COVDIR)/$<.gcov

# get tests coverage reports
$(TESTBUILD)/%.$(SRCEXT).$(COVEXT): $(TESTDIR)/%.$(SRCEXT)
	gcov -r $< --object-directory $(dir $@)
	mv $(notdir $@) ./$(TESTCOVDIR)

# benchmarks
BENCHDIR	:= benchmarks
BENCHBUILD	:= $(BENCHDIR)/objs
BENCHTARGET	:= $(BENCHDIR)/bin

BENCHFLAGS	:= $(DEFINES) -Wall -Wno-unknown-pragmas -O3

ifeq ($(NATIVE), 1)
	BENCHFLAGS += -march=native -mavx2
endif

BENCHLIBS	:= $(PTHREAD) $(CURL) -L ./bin -l clibs
BENCHINC	:= -I $(INCDIR) -I ./$(BENCHDIR)

BENCHS		:= $(shell find $(BENCHDIR) -type f -name *.$(SRCEXT))
BENCHOBJS	:= $(patsubst $(BENCHDIR)/%,$(BENCHBUILD)/%,$(BENCHS:.$(SRCEXT)=.$(OBJEXT)))

bench: $(BENCHOBJS)
	@mkdir -p ./$(BENCHTARGET)
	$(CC) -g $(BENCHINC) -L ./$(TARGETDIR) ./$(BENCHBUILD)/base64.o -o ./$(BENCHTARGET)/base64 $(BENCHLIBS)

# compile benchmarks
$(BENCHBUILD)/%.$(OBJEXT): $(BENCHDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(BENCHFLAGS) $(INC) $(BENCHINC) $(BENCHLIBS) -c -o $@ $<
	@$(CC) $(BENCHFLAGS) $(INCDEP) -MM $(BENCHDIR)/$*.$(SRCEXT) > $(BENCHBUILD)/$*.$(DEPEXT)
	@cp -f $(BENCHBUILD)/$*.$(DEPEXT) $(BENCHBUILD)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BENCHBUILD)/$*.$(OBJEXT):|' < $(BENCHBUILD)/$*.$(DEPEXT).tmp > $(BENCHBUILD)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BENCHBUILD)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BENCHBUILD)/$*.$(DEPEXT)
	@rm -f $(BENCHBUILD)/$*.$(DEPEXT).tmp

clear: clean-objects clean-tests clean-coverage clean-bench

clean: clear
	@$(RM) -rf $(TARGETDIR)

clean-objects:
	@$(RM) -rf $(BUILDDIR)

clean-tests:
	@$(RM) -rf $(TESTBUILD)
	@$(RM) -rf $(TESTTARGET)

clean-coverage:
	@$(RM) -rf $(COVDIR)

clean-bench:
	@$(RM) -rf $(BENCHBUILD)
	@$(RM) -rf $(BENCHTARGET)

.PHONY: all clean clear test coverage bench