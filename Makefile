all: comp sum
	./test_1

comp:
	gcc pdp.c pdp_run.c trc_dbg.c -o test_1
sum:
	./test_1 sum.o