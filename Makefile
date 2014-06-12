# FLAGS and PROGS

CC = /bin/gcc
STRIP = /bin/strip
STRIPFLAGS = --strip-unneeded
FLAGS = -O2 #-D _DEBUG
CFLAGS = -fPIC -g -Wall -Iinclude -fvisibility=hidden
LDFLAGS = -ljansson
LDSOFLAGS = -ljansson -shared

# DIRS

SRCIN = src
SRCBININ = $(SRCIN)/bin
SRCTESTIN = $(SRCIN)/bin

INCIN = include

OUT = out
LIBOUT = $(OUT)/lib
BINOUT= $(OUT)/bin
INCOUT = $(OUT)/include
OBJOUT = $(OUT)/obj
OBJBINOUT = $(OBJOUT)/bin

# FILES

HEADERS_ = $(INCIN)/jansson_extension.h $(INCIN)/libjsonp.h $(INCIN)/libjsonpp.h #$(wildcard $(INCIN)/*.h)
HEADERS = $(patsubst $(INCIN)/%,$(INCOUT)/%,$(HEADERS_))

SOURCES = $(wildcard $(SRCIN)/*.c)
BINSOURCES = $(wildcard $(SRCBININ)/*.c)

OBJECTS_ = $(patsubst $(SRCIN)/%,$(OBJOUT)/%,$(SOURCES))
OBJECTS = $(OBJECTS_:.c=.o)
BINOBJECTS_ = $(patsubst $(SRCBININ)/%,$(OBJBINOUT)/%,$(BINSOURCES))
BINOBJECTS = $(BINOBJECTS_:.c=.o)

# TARGET FILES

LIBTARGET = libjsonp.so
BINTARGET = jsonp

# RULES

all: $(LIBTARGET) $(BINTARGET)

$(OBJOUT)/%.o: $(SRCIN)/%.c
	$(CC) $(FLAGS) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(OBJBINOUT)/%.o: $(SRCBININ)/%.c
	$(CC) $(FLAGS) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(OBJOUT)/%.o: $(SRCIN)/%.c
	$(CC) $(FLAGS) $(CFLAGS) $(LDFLAGS) -c $< -o $@

$(LIBTARGET): $(OBJECTS) $(HEADERS)
	$(STRIP) $(STRIPFLAGS) $(OBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) $(LDSOFLAGS) -o $(LIBOUT)/$(LIBTARGET) $(OBJECTS)

$(BINTARGET): $(OBJECTS) $(BINOBJECTS)
	$(STRIP) $(STRIPFLAGS) $(OBJECTS)
	$(STRIP) $(STRIPFLAGS) $(BINOBJECTS)
	$(CC) $(FLAGS) $(CFLAGS) $(LDFLAGS) -o $(BINOUT)/$(BINTARGET) $(OBJECTS) $(BINOBJECTS)

$(INCOUT)/%.h: $(INCIN)/%.h
	cp $< $@

.PHONY: clean

clean:
	rm -f $(LIBOUT)/$(LIBTARGET)
	rm -f $(BINOUT)/$(BINTARGET)
	rm -f $(HEADERS)
	rm -f $(OBJECTS)
	rm -f $(BINOBJECTS)