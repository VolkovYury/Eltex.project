all: jsonEditor.o
	gcc -Wall -Wextra -Werror jsonEditor.o -o jsonEditor -ljson-c
jsonEditor.o: jsonEditor.c
	gcc jsonEditor.c -c
clean:
	rm -rf jsonEditor.o jsonEditor
