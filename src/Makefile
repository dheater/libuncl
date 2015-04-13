CFLAGS=-O2 -Wall -fPIC
LD:=ld
LDFLAGS=-shared

LIBNAME:=libuncl.so

all:$(LIBNAME)

$(LIBNAME): uncl.o
	$(LD) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o *.so tags *~
