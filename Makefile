APIARY=..
all:
	Bgcc1 -Wall -quiet -std=c99 -fno-builtin -O2 -I${APIARY}/include main.c
	Bas -x -datarota=2 -lst main.lst main.s
	Bld -o aabb.out -codebase=1000 -datafloat ${APIARY}/lib/base.o main.o -L${APIARY}/lib -lmc -lc -lgcc
	Bimg aabb.out
	cp aabb.img ~/share/aaab
clean:
	rm -rf *.s *.o *.img *.out *.lst
