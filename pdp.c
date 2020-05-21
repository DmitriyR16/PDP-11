#include "pdp.h"
#include <string.h>


word reg[8] = {};
struct mr ss, dd;
const Command cmd[] =	{{0177777, 	0000000, 	"HALT",		do_halt,		NO_PARAM},
						{0170000, 	0060000, 	"ADD",		do_add,			HAS_SS|HAS_DD}, 
						{0170000, 	0010000, 	"MOV",		do_mov,			HAS_SS|HAS_DD},
						{0170000, 	0110000, 	"MOVb",	  	do_movb,		HAS_SS|HAS_DD},	
						{0177000, 	0077000, 	"SOB",		do_sob,			HAS_NN},
						{0177400, 	0000400, 	"BR",	  	do_br,			HAS_XX},
						{0177400, 	0001400, 	"BEQ",	  	do_beq,			HAS_XX},
						{0177400, 	0001000, 	"BNE",	  	do_bne,			HAS_XX},
						{0177400, 	0100000, 	"BPL",     	do_bpl,       	HAS_XX},
						{0177400, 	0100400, 	"BMI",     	do_bmi,       	HAS_XX},
						{0177700, 	0005700, 	"TST",     	do_tst,       	HAS_DD},
						{0177700, 	0105700, 	"TSTb",    	do_tstb,      	HAS_DD},
						{0170000, 	0020000, 	"CMP",     	do_cmp,       	HAS_SS|HAS_DD},
						{0170000, 	0120000, 	"CMPb",    	do_cmpb,      	HAS_SS|HAS_DD},
	};
const int func_num = 14;
struct flags; 

void test_mem();

int main (int argc, char * argv[]) {
	//test_mem();
/*	if(argc < 2){
		printf("\033[01;31mError: \033[00mNo input file\n");
		printf("Use flag \033[01;33m-help\033[00m to get more info\n");
		fflush(0);
		return 0;
	}
	if(argc == 2)
		if(!strcmp("-help", argv[1])){
			printf("\n\
Usage: ./pdp11.static [flags] input-file \n\
\n\
Options:\n\
	-s N	starting address, octal (default 1000)\n\
	-n	do not pass argc/argv to target\n\
	-q	quiet, don't show anything except target's output\n\
	-t	show trace to stderr\n\
	-T	show verbose trace to stderr\n\
	-o	run with ODT debugger enabled\n\
");
			return 0;
		}
	if(argc > 2)*/
	load_file(argv[1]); 
	run();
	return 0;
}

void test_mem() {
	
	byte b0 = 0x0a; // пишем байт , читаем байт
	byte b1 = 0x0b;
	word w = 0x0b0a;

	//ТЕСТ№1
	w_write (0, w);//записываем слово
	word wres = w_read(0);//читаем слово 
	printf("%04hx = %04hx\n",w, wres ); // hhx - half half hex (4) 
	//%02hhx - ширина 2 с ведущими нулями
	assert(w == wres); 
	
	//ТЕСТ№2 читаем слово пишем два байта

	Adress a = 4;
	w_write(a, w);
	word b0res = b_read(a);
	word b1res = b_read(a+1);
	printf("ww/br \t %04hx = %02hhx%02hhx\n", wres, b1res, b0res );
	assert (w == wres);


	//ТЕСТ№3 проверка функции b_write

	word w0 = 0x0b0a;
	word w1 = 0x0c0c;
	byte b = 0x0c;
	w_write(4, w0);

	b_write(4,b);
	b_write(5,b);
	word wres1 = w_read(4);//читаем слово  
	printf("wo/bb \t %04hx = %02hhx%02hhx\n", wres1, b, b);
	assert(w1 == wres1);	
}
