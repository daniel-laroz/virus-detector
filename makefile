all: AntiVirus

AntiVirus: AntiVirus.o
	gcc -m32 -g -Wall -o AntiVirus AntiVirus.o
	rm -f AntiVirus.o	
	
AntiVirus.o: AntiVirus.c
	gcc -g -Wall -m32 -c -o AntiVirus.o AntiVirus.c	

.PHONY: clean
clean:
	rm -f AntiVirus AntiVirus.o
