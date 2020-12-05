INCLUDEDIR :=
INCLUDEDIR += -Iinclude/

CC := gcc
CFLAGS := $(INCLUDEDIR) -pedantic -Wall -Wextra -Werror -Wno-misleading-indentation

OBJECTS := hyper_network.o hyper_file.o hyper.o 

SERVEROBJS := hyper_server.o
CLIENTOBJS := hyper_client.o

all: clean libhyper.a hyper-server hyper
	@echo "Done!"

hyper-server: $(SERVEROBJS) libhyper.a
	$(CC) -g $< -o $@ -L. -lhyper

hyper: $(CLIENTOBJS) libhyper.a
	$(CC) $< -o $@ -L. -lhyper 

libhyper.a: $(OBJECTS)
	ar -cvq $@ $^

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.a hyper-server hyper
