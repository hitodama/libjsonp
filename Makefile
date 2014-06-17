# FLAGS and PROGS

CC = clang #cc
LD = ld
STRIP = strip
STRIPFLAGS = --strip-unneeded
FLAGS = -std=c99 -O2 #-D _DEBUG
DFLAGS = -O0 -D _DEBUG
CFLAGS = -fPIC -g -Wall -Iinclude -fvisibility=hidden 
LDFLAGS = -ljansson -lm
LDSOFLAGS = $(LDFLAGS) -shared

# DIRS

SRCIN = src
SRCBININ = $(SRCIN)/bin
SRCTESTIN = $(SRCIN)/test

INCIN = include

OUT = out
LIBOUT = $(OUT)/lib
BINOUT = $(OUT)/bin
TESTOUT = $(OUT)/test
INCOUT = $(OUT)/include
OBJOUT = $(OUT)/obj
OBJBINOUT = $(OBJOUT)/bin
OBJTESTOUT = $(OBJOUT)/test

# FILES

HEADERS_ = $(INCIN)/jansson_extension.h $(INCIN)/libjsonp.h $(INCIN)/libjsonpp.h #$(wildcard $(INCIN)/*.h)
HEADERS = $(patsubst $(INCIN)/%,$(INCOUT)/%,$(HEADERS_))

SOURCES = $(wildcard $(SRCIN)/*.c)
OBJECTS_ = $(patsubst $(SRCIN)/%,$(OBJOUT)/%,$(SOURCES))
OBJECTS = $(OBJECTS_:.c=.o)

BINSOURCES = $(wildcard $(SRCBININ)/*.c)
BINOBJECTS_ = $(patsubst $(SRCBININ)/%,$(OBJBINOUT)/%,$(BINSOURCES))
BINOBJECTS = $(BINOBJECTS_:.c=.o)

TESTSOURCES = $(wildcard $(SRCTESTIN)/*.c)
TESTOBJECTS_ = $(patsubst $(SRCTESTIN)/%,$(OBJTESTOUT)/%,$(TESTSOURCES))
TESTOBJECTS = $(TESTOBJECTS_:.c=.o)

# TARGET FILES

LIBTARGET = libjsonp.so
BINTARGET = jsonp
TESTTARGET = jsonp_test

# RULES

all: mkout $(LIBOUT)/$(LIBTARGET) $(BINOUT)/$(BINTARGET) 

$(OBJOUT)/%.o: $(SRCIN)/%.c
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@
	$(STRIP) $(STRIPFLAGS) $@

$(OBJBINOUT)/%.o: $(SRCBININ)/%.c
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@
	$(STRIP) $(STRIPFLAGS) $@

$(OBJTESTOUT)/%.o: $(SRCTESTIN)/%.c
	$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@
	$(STRIP) $(STRIPFLAGS) $@

$(LIBOUT)/$(LIBTARGET): $(OBJECTS) $(HEADERS)
	$(CC) $(FLAGS) $(CFLAGS) $(LDSOFLAGS) -o $(LIBOUT)/$(LIBTARGET) $(OBJECTS)
	#$(STRIP) $(STRIPFLAGS) $(LIBOUT)/$(LIBTARGET)

$(BINOUT)/$(BINTARGET): $(OBJECTS) $(BINOBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) $(LDFLAGS) -o $(BINOUT)/$(BINTARGET) $(OBJECTS) $(BINOBJECTS)

$(TESTOUT)/$(TESTTARGET): $(OBJECTS) $(TESTOBJECTS)
	$(CC) $(DFLAGS) $(CFLAGS) $(LDFLAGS) -o $(TESTOUT)/$(TESTTARGET) $(OBJECTS) $(TESTOBJECTS)

$(INCOUT)/%.h: $(INCIN)/%.h
	cp $< $@

.PHONY: clean mkout test install

install:
	@echo "Sorry Makefiles are little beyond me. Please contribute a more elaborate build system!"

uninstall:
	@echo "Sorry Makefiles are little beyond me. Please contribute a more elaborate build system!"

test: mkout $(TESTOUT)/$(TESTTARGET)
	$(TESTOUT)/$(TESTTARGET)

mkout:
	mkdir -p $(OUT)
	mkdir -p $(BINOUT)
	mkdir -p $(TESTOUT)
	mkdir -p $(INCOUT)
	mkdir -p $(OBJOUT)
	mkdir -p $(LIBOUT)
	mkdir -p $(OBJBINOUT)
	mkdir -p $(OBJTESTOUT)

clean:
	$(RM) -dfr $(OUT)