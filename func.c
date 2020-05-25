#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include "head.h"

void run()
{
	pc = 01000;			//take pointer to start of heap
	while (1) 			
	{
		word w = w_read(pc);			//read next function	
		trace("%06o %06o: ", pc, w);
		pc += 2;

		byte_func = (w >> 15) & 1;					//determine function type and it's parameters
		for(int i = 0; i < func_num; i++)
		{
			if ((w & cmd[i].mask) == cmd[i].opcode) 
			{
				trace("%s ", cmd[i].func_name);

				if((cmd[i].func_params & 1) == HAS_SS) // has_ss = 00000001
					ss = get_mr(w >> 6);

				if((cmd[i].func_params & 2) == HAS_DD) // has_dd = 00000010
					dd = get_mr(w);

				if((cmd[i].func_params & 4) == HAS_NN) // has_nn = 00000100
				{
					nn = get_nn(w);
					R_nn = (w >> 6) & 7;
				}

				if((cmd[i].func_params & 8) == HAS_XX) // has_xx = 000pXX, p <= 3
					xx = (char) w;

				trace("\n");
				
				cmd[i].do_func();
				break;
			}
		}
	}
}

struct mr get_mr(word w) 
{
	struct mr res;
	res.mem_level = MEM;
	int r = w & 7;           // reg number
	int mode = (w >> 3) & 7; // mode number
	int arg = w_read(pc);    // function argument for 6 and 7 mode, add it to adress

	switch (mode) 
	{
		case 0:  						 // R3
			res.adr = r;
			res.val = reg[r];
			res.mem_level = REG;
			trace (" R%o ", r);
			break;

		case 1:  						 // (R3)
			res.adr = reg[r];

			if(byte_func)
				res.val = w_read(res.adr);
			else
				res.val = b_read(res.adr);

			trace (" (R%o) ", r);
			break;

		case 2:  						 // (R3)+
			res.adr = reg[r];
			if(byte_func && reg[r] != pc)
			{
				res.val = b_read(res.adr);
				reg[r] -= 1;
			}				
			else
				res.val = w_read(res.adr);

			reg[r] += 2; 
			if (r == 7)
				trace(" #%o ", res.val);	//reg[7]
			else
				trace(" (R%o)+ ", r);
			break;	

		case 3:   						// @(R3)+
			res.adr = w_read(reg[r]);

			if(byte_func)
				res.val = w_read(res.adr);
			else
				res.val = b_read(res.adr);

			reg[r] += 2; 

			if (r == 7)
				trace(" @#%o ", res.adr); //reg[7]
			else 
				trace(" @(R%o)+ ", r);
			break;

		case 4: 
			if(byte_func == 1)
				reg[r] -= 1;
			else
				reg[r] -= 2;

			res.adr = reg[r];
			if(byte_func)
				res.val = w_read(res.adr);
			else
				res.val = b_read(res.adr);
			trace(" -(R%o) ", r);
			break;

		case 5: 
			reg[r] -= 2;
			res.adr = w_read(reg[r]);
			if(byte_func)
				res.val = w_read(res.adr);
			else
				res.val = b_read(res.adr);
			trace(" @-(R%o) ", r);
			break;		

		case 6:
			res.adr = reg[r] + arg;
			if(byte_func)
				res.val = w_read(res.adr);
			else
				res.val = b_read(res.adr);
			if(r == 7)
				trace(" %o ", res.adr);
			else	
				trace(" %o(R%o) ", arg, r);
			break;

		case 7:
			res.adr = reg[r] + arg;
			if(byte_func)
				res.val = w_read(res.adr);
			else
				res.val = b_read(res.adr);

			if(r == 7)
				trace(" %o ", res.adr);
			else	
				trace(" @%o(R%o) ", arg, r);
			break;

		default:
			fprintf(stderr,
				"Mode %o is not declared at PDP-11!\n", mode);
			exit(1);		
	}
	if (t == 2)
	{
		trace("\n");
		trace ("%c%c%c  ", (flag.N == 1) ? 'N' : 'n', (flag.Z == 1) ? 'Z' : 'z', (flag.C == 1) ? 'C' : 'c');
		trace("0:%06o 1:%06o 2:%06o 3:%06o 4:%06o 5:%06o sp:%06o pc:%06o\n",\
		reg[0], reg[2], reg[4], reg[6], reg[1], reg[3], reg[5], reg[7]);
	}

	return res;
}

//------------------------------------------------ loading input file

void load_file (const char * file_name) 
{
	unsigned int start = 0, n = 0, w = 0;

	FILE * f_inp = fopen(file_name, "rb");
	if(f_inp == NULL)
	{
		perror(file_name);
		exit(7);
	}

	while(fscanf(f_inp, "%x %x", &start, &n) == 2)
	{
		for (int i = 0; i < n; i++)
	  	{
	    	fscanf(f_inp, "%x", &w);
			b_write (start + i, (byte) w);
	  	}
	}
	fclose (f_inp);	
}

//------------------------------------------------ getting arguments

int get_nn(word w) 
{
	int res = 0;
	res = w & 077;
	return res;
}

//------------------------------------------------ processor commands

void do_mov() 
{
	if (dd.mem_level == REG)	
  	   reg[dd.adr] = ss.val;
  	if (dd.mem_level == MEM)
  		w_write(dd.adr, ss.val);
  	if (dd.adr == odata)
  	   trace("  %c", mem[odata]);
  	assert((((pc >> 15) & 1) == 0) && "do_mov error");
}

void do_add()
{
  	if (dd.mem_level == REG)
  	   reg[dd.adr] = ss.val + dd.val;
  	if (dd.mem_level == MEM)
  	   w_write (dd.adr, ss.val + dd.val);
}

void do_sob()
{
	trace("R%d = %o, %o", R_nn, reg[R_nn], pc - 2 * nn);
	reg[R_nn] -= 1;
	if (reg[R_nn] != 0)
	   pc = pc - (2 * nn);

}


void do_movb() 
{
	if(dd.mem_level == REG)
		reg[dd.adr] = (word) ((byte) ss.val);
	if(dd.mem_level == MEM)
		b_write (dd.adr, (byte) ss.val);
	if(dd.adr == odata)
		printf("%c", mem[odata]);
	set_NZ(ss.val);
	set_C(ss.val);
}

void do_halt()
{
	printf("\n======================== halted ==============================\n");
	trace("\n");
	for(int i = 0; i <= 7; i++)
		trace("R%d = %06o         ", i, reg[i]);
	trace("\n");
	exit (0);
}

//--------------------------------------------- branch functions

void do_br()
{
	trace(" xx=%d \n", xx);
	pc = pc + (2 * xx);
}

void do_beq()
{
	if (flag.Z == 1)
		do_br ();
}

void do_bne() 
{
	if (flag.Z == 0)
		do_br ();
}

void do_bmi() 
{
	if (flag.N == 1) 
		do_br();
}

void do_bpl() 
{
	if (flag.N == 0) 
		do_br();
}

void do_tst() 
{
	set_NZ(dd.val);
	flag.C = 0;
}

void do_tstb() 
{
	set_NZ(dd.val);	
	flag.C = 0;
}

void do_cmp()
{
	set_NZ(ss.val - dd.val);
	set_C(ss.val - dd.val);
}

void do_cmpb() {
	set_NZ(ss.val - dd.val);
	set_C(ss.val - dd.val);
}

//-------------------------------------------------------- setting flags

void set_NZ(word w)
{
	flag.Z = 0;
	if (byte_func)
	   flag.N = (w >> 7) & 1;
	else
       flag.N = (w >> 15) & 1;
    if (w == 0)
	   flag.Z = 1;
}

void set_C (word w)
{
	flag.C = (w >> 15) & 1;
}

//------------------------------------------------------ work with words/bytes in memory

void b_write(Adress adr, byte b)
{
	word x;
	if (adr % 2 == 0) 
	{
		x = mem[adr];
		x = x >> 8;
		mem[adr] = (x << 8) | ((word) b);
	}
	else
	{
		x = mem[adr - 1];
		mem[adr - 1] = ((byte) x) | ((word) (b << 8));
	}


}

byte b_read (Adress adr) 
{
	byte b;
	if (adr % 2 == 0)
		b = (byte) mem[adr];
	else 
	{
		word w = mem[adr - 1];
		b = (byte) (w >> 8);
	}
	return b;
}

word w_read (Adress adr) 
{
	assert((adr % 2 == 0) && "w_read error");
	return mem [adr];
}

void w_write (Adress adr, word w)
{
	assert((adr % 2 == 0) && "w_write error");
	mem[adr] = w;
}

