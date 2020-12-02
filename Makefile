INCLUDEDIR :=
INCLUDEDIR += -Iinclude/

CC := gcc
CFLAGS := $(INCLUDEDIR) -Wall

OBJECTS := hyper_network.o hyper_file.o 

SERVEROBJS := hyper_server.o
CLIENTOBJS := hyper_client.o

all: clean libhyper.a hyper-server hyper-client
	@echo "Done!"

hyper-server: $(SERVEROBJS) libhyper.a
	$(CC) -g $< -o $@ -L. -lhyper

hyper-client: $(CLIENTOBJS) libhyper.a
	$(CC) $< -o $@ -L. -lhyper 

libhyper.a: $(OBJECTS)
	ar -cvq $@ $^

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.a hyper-server hyper-client
