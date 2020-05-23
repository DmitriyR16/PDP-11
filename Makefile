all: comp

comp:
	gcc pdp.c pdp_run.c trc_dbg.c -o run.out
