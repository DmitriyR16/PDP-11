all: comp

comp:
	gcc pdp.c func.c trc_dbg.c -o run.out
