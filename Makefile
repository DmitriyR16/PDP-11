all: comp

comp:
	gcc pdp.c func.c trc.c -o run.out
