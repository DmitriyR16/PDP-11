#include <string.h>
#include "head.h"



word reg[8] = {};

const Command cmd[] =	
{
						{"HALT", 	0000000, 	0177777,		do_halt,		NO_PARAM},
						{"ADD", 	0060000, 	0170000,		do_add,			HAS_SS|HAS_DD}, 
						{"MOV", 	0010000, 	0170000,		do_mov,			HAS_SS|HAS_DD},
						{"MOVb", 	0110000, 	0170000,	  	do_movb,		HAS_SS|HAS_DD},	
						{"SOB", 	0077000, 	0177000,		do_sob,			HAS_NN},
						{"BR", 		0000400, 	0177400,	  	do_br,			HAS_XX},
						{"BEQ", 	0001400, 	0177400,	  	do_beq,			HAS_XX},
						{"BNE", 	0001000, 	0177400,	  	do_bne,			HAS_XX},
						{"BPL", 	0100000, 	0177400,     	do_bpl,       	HAS_XX},
						{"BMI", 	0100400, 	0177400,     	do_bmi,       	HAS_XX},
						{"TST", 	0005700, 	0177700,     	do_tst,       	HAS_DD},
						{"TSTb", 	0105700, 	0177700,    	do_tstb,      	HAS_DD},
						{"CMP", 	0020000, 	0170000,     	do_cmp,       	HAS_SS|HAS_DD},
						{"CMPb", 	0120000, 	0170000,    	do_cmpb,      	HAS_SS|HAS_DD},
};
const int func_num = 14;

struct mr ss, dd;
struct flags flag;

void test_mem();

int main (int argc, char * argv[]) 
{
	//test_mem();
	/*
	if(argc < 2)
	{
		printf("\033[01;31mError: \033[00mNo input file\n");
		printf("Use flag \033[01;33m-help\033[00m to get more info\n");
		fflush(0);
		return 0;
	}

	if(argc == 2)
		if(!strcmp("-help", argv[1]))
		{
			printf(
				"\n\
				Usage: ./pdp11.static [flags] input-file \n\
				\n\
				Options:\n\
				-s N	starting address, octal (default 1000)\n\
				-n	do not pass argc/argv to target\n\
				-q	quiet, don't show anything except target's output\n\
				-t	show trace to stderr\n\
				-T	show verbose trace to stderr\n\
				-o	run with ODT debugger enabled\n\"
				);
			return 0;
		}
	if(argc > 2)
	*/

	b_write(ostat, 0200);
	load_file(argv[1]); 
	run();
	return 0;
}

//

void test_mem() 
{
	
	byte b0 = 0x0a; 						// пишем байт , читаем байт
	byte b1 = 0x0b;
	word w = 0x0b0a;

	//ТЕСТ№1

	w_write (0, w);							//записываем слово
	word wres = w_read(0);					//читаем слово 
	printf("%04hx = %04hx\n",w, wres );		// hhx - half half hex (4) 
											//%02hhx - ширина 2 с ведущими нулями
	assert((w == wres) && "test_mem error test 1"); 
	
	//ТЕСТ№2 читаем слово пишем два байта

	Adress a = 4;
	w_write(a, w);
	word b0res = b_read(a);
	word b1res = b_read(a + 1);
	printf("ww/br \t %04hx = %02hhx%02hhx\n", wres, b1res, b0res);
	assert((w == wres) && "test_mem error test 2");


	//ТЕСТ№3 проверка функции b_write

	word w0 = 0x0b0a;
	word w1 = 0x0c0c;
	byte b = 0x0c;
	w_write(4, w0);

	b_write(4, b);
	b_write(5, b);
	word wres1 = w_read(4);//читаем слово  
	printf("wo/bb \t %04hx = %02hhx%02hhx\n", wres1, b, b);
	assert((w1 == wres1) && "test_mem error test 3");	
}
