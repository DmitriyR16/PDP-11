all: comp

comp:
	gcc pdp.c pdp_run.c trc_dbg.c -o test_1
