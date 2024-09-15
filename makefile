CLANG = -Og -Wall -Werror -pthread

ARGS1 = 9000 localhost 9001
ARGS2 = 9001 localhost 9000

all:
	gcc $(CLANG) -o s-talk main.c dataRx.c dataTx.c inhandler.c outhandler.c list.c shtdnmanager.c

build-full:
	gcc $(CLANG) -fsanitize=thread -o s-talk main.c dataRx.c dataTx.c inhandler.c outhandler.c list.c shtdnmanager.c

run1:
	./s-talk $(ARGS1)

run2:
	./s-talk $(ARGS2)

val1:
	valgrind --leak-check=full ./s-talk $(ARGS1)

val2:
	valgrind --leak-check=full --show-leak-kinds=all ./s-talk $(ARGS2)

clean:
	rm ./s-talk