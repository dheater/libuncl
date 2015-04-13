CFLAGS =-O2 -Wall
LDFLAGS=-L../src -luncl

PROGRAMS:=userver uclient
all: $(PROGRAMS)

userver: userver.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

uclient: uclient.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(PROGRAMS) *.o tags *~
