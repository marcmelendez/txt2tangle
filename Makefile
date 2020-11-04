all:
	./txt2tangle txt2tangledoc.txt
	./txt2tangle txt2tangle.txt
	gcc -Wall -O2 -o txt2tangle txt2tangle.c

first:
	gcc -Wall -O2 -o txt2tangle txt2tangle.c
	./txt2tangle txt2tangledoc.txt
