INCLUDEDIR :=
INCLUDEDIR += -Iinclude/

CC := gcc
CFLAGS := $(INCLUDEDIR) -Wall

OBJECTS := hyper_network.o hyper_file.o 

all: libhyper.a
	@echo "Done!"

libhyper.a: $(OBJECTS)
	ar -cvq $@ $^

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.a
