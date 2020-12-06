INCLUDEDIR :=
INCLUDEDIR += -Iinclude/

CC := gcc
CFLAGS := $(INCLUDEDIR) -pedantic -Wall -Wextra -Werror -Wno-misleading-indentation

OBJS := hyper_network.o hyper_file.o hyper.o 

all: clean libhyper.a
	@echo "Done!"

libhyper.a: $(OBJS)
	ar -cvq $@ $^

%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o *.a
