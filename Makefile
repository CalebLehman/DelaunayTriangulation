CPP=gcc
CPPFLAGS=-O3
LINKERFLAGS=

SRCDIR=src
BUILDDIR=build

DEPS=$(SRCDIR)/defs.h $(SRCDIR)/delaunay.h $(SRCDIR)/helper.h $(SRCDIR)/io.h $(SRCDIR)/topology.h
SOURCES=$(wildcard $(SRCDIR)/*.c)
OBJECTS=$(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

$(BUILDDIR)/delaunay: $(OBJECTS)
	$(CPP) -o $@ $^ $(LINKERFLAGS)

$(OBJECTS) : $(BUILDDIR)/%.o : $(SRCDIR)/%.c $(DEPS)
	$(CPP) $(CPPFLAGS) -c -o $@ $<

.PHONY : clean
clean :
	rm -f $(BUILDDIR)/delaunay
	rm -f $(BUILDDIR)/*.o
