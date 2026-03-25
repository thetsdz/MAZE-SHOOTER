CC = gcc
CFLAGS = -Wall -Wextra -I../lib/headers
LDFLAGS = -lcunit

test: cryptage.o testCryptageCunit.o
	$(CC) -o test_runner cryptage.o testCryptageCunit.o $(LDFLAGS)
	./test_runner

cryptage.o: ../../src/cryptage.c
	$(CC) $(CFLAGS) -c ../../src/cryptage.c

testCryptageCunit.o: testCryptageCunit.c
	$(CC) $(CFLAGS) -c testCryptageCunit.c