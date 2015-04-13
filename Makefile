PROJNAME:=libuncl
VERSION:=1.0

CFLAGS=-O2 -Wall -shared --mno-cygwin
LD:=ld
LDFLAGS=-shared
PREFIX:=/usr
INCDIR:=$(PREFIX)/include
LIBDIR:=$(PREFIX)/lib

LIBNAME:=libuncl.so

all:src/$(LIBNAME)

install: all
	install -m0755 -oroot -groot -d $(INCDIR)
	install -m0644 -oroot -groot include/uncl.h $(INCDIR)
	install -m0755 -oroot -groot -d $(LIBDIR)
	install -m0644 -oroot -groot src/$(LIBNAME) $(LIBDIR)
	ldconfig

uninstall:
	rm $(INCDIR)/uncl.h
	rm $(LIBDIR)/$(LIBNAME)

src/$(LIBNAME): src/uncl.o
	$(LD) $(LDFLAGS) -o $@ $^

src/uncl.o: include/uncl.h

doc: .Doxyfile
	doxygen $^

clean:
	find . -name "*.so" | xargs rm -f
	find . -name "*.o" | xargs rm -f
	find . -name "*~" | xargs rm -f
	find . -name "tags" | xargs rm -f
	cd example && make clean

pristine: clean
	rm -rf doc

release: pristine doc
	cd .. && tar cvfz $(PROJNAME)-$(VERSION).tar.gz $(PROJNAME)
