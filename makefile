SLIB		:= libclibs.so

PTHREAD 	:= -l pthread
MATH		:= -lm

DEVELOPMENT	:= -g

CC          := gcc

SRCDIR      := src
INCDIR      := include
BUILDDIR    := objs
TARGETDIR   := bin

TESTMDIR	:= test
TESTBUILD	:= $(TESTDIR)/objs
TESTTARGET	:= $(TESTDIR)/bin

SRCEXT      := c
DEPEXT      := d
OBJEXT      := o

CFLAGS      := $(DEVELOPMENT) -Wall -Wno-unknown-pragmas -fPIC
LIB         := $(PTHREAD) $(MATH)
INC         := -I $(INCDIR) -I /usr/local/include
INCDEP      := -I $(INCDIR)

TESTFLAGS	:= -g $(DEFINES) -Wall -Wno-unknown-pragmas
TESTLIBS	:= -L ./bin -l clibs

SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

TESTMPLES	:= $(shell find $(TESTMDIR) -type f -name *.$(SRCEXT))
TESTOBJS	:= $(patsubst $(TESTMDIR)/%,$(TESTBUILD)/%,$(TESTMPLES:.$(SRCEXT)=.$(OBJEXT)))

all: directories $(SLIB)

install: $(SLIB)
	install -m 644 ./bin/libclibs.so /usr/local/lib/
	cp -R ./include/clibs /usr/local/include

uninstall:
	rm /usr/local/lib/libclibs.so
	rm -r /usr/local/include/clibs

directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)

clean:
	@$(RM) -rf $(BUILDDIR) 
	@$(RM) -rf $(TARGETDIR)
	@$(RM) -rf $(TESTBUILD)
	@$(RM) -rf $(TESTTARGET)

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

test: $(TESTOBJS)
	@mkdir -p ./test/bin
	$(CC) -I ./$(INCDIR) -L ./$(TARGETDIR) ./$(TESTBUILD)/test.o -o ./$(TESTTARGET)/test -l clibs

# compile tests
$(TESTBUILD)/%.$(OBJEXT): $(TESTMDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(TESTFLAGS) $(INC) $(TESTLIBS) -c -o $@ $<
	@$(CC) $(TESTFLAGS) $(INCDEP) -MM $(TESTMDIR)/$*.$(SRCEXT) > $(TESTBUILD)/$*.$(DEPEXT)
	@cp -f $(TESTBUILD)/$*.$(DEPEXT) $(TESTBUILD)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(TESTBUILD)/$*.$(OBJEXT):|' < $(TESTBUILD)/$*.$(DEPEXT).tmp > $(TESTBUILD)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(TESTBUILD)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(TESTBUILD)/$*.$(DEPEXT)
	@rm -f $(TESTBUILD)/$*.$(DEPEXT).tmp

.PHONY: all clean test