CC = gcc
CFLAGS = -Wall -Wextra -I../../lib/headers
# Par défaut pour Linux :
LDFLAGS = -lcunit 

# DECOMMENTE CETTE LIGNE SI TU ES SOUS WINDOWS (MSYS2 / MinGW) :
# LDFLAGS = -lcunit -lws2_32

test: reseau.o testReseauCunit.o
	$(CC) -o test_runner_reseau reseau.o testReseauCunit.o $(LDFLAGS)
	./test_runner_reseau

reseau.o: ../../src/reseau.c
	$(CC) $(CFLAGS) -c ../../src/reseau.c

testReseauCunit.o: testReseauCunit.c
	$(CC) $(CFLAGS) -c testReseauCunit.c

clean:
	rm -f *.o test_runner_reseau test_runner_reseau.exe