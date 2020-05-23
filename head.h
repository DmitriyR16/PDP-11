#ifndef HEADh
#define HEADh

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NO_PARAM 0
#define HAS_SS 1
#define HAS_DD 2
#define HAS_NN 4
#define HAS_XX 8

#define ostat 0177564
#define odata 0177566
#define REG 10
#define MEM 11

#define MEMSIZE (64 * 1024)
#define pc reg[7]

typedef unsigned char byte; 		// 8 bit
typedef unsigned short int word; 	// 16 bit
typedef word Adress;

byte byte_func;
int nn;
int xx;
int R_nn;
extern const int func_num;

word mem[MEMSIZE];
extern word reg[]; 			//Массив регистров 


typedef struct 
{
	char * func_name;
	word opcode;
	word mask;
	void (*do_func)(void);
	char func_params;
} Command;

extern const Command cmd[];

struct mr {
	word adr; 			// адрес аргумента
	word val; 			// значение аргумента
	word space; 		// Тип памяти, с которой мы работаем: 
};
extern struct mr ss, dd;

struct flags
{
	unsigned char N : 1;
	unsigned char Z : 1;
	unsigned char C : 1;
};
extern struct flags flag; 

void do_mov();
void do_add();
void do_movb();
void do_halt();
void do_sob();
void do_br();
void do_beq();
void do_bne();
void do_bpl();
void do_bmi();
void do_cmp();
void do_cmpb();
void do_tst();
void do_tstb();
void set_C(word w);
void set_NZ (word w); // можно добавить второй аргумент функции  byte byte_func = 0

int get_nn(word w);
struct mr get_mr(word w); // можно добавить второй аргумент функции  byte byte_func

void b_write(Adress adr, byte b); // пишем байт b по адресу adr
byte b_read (Adress adr); // читаем байт по адресу adr;
void w_write(Adress adr, word w); // пишем слово w по адресу adr
word w_read (Adress adr); // читаем слово по адресу adr

void run();
void load_file (const char * file_name);
void trace(const char *  format, ...);
#endif