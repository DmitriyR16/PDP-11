#include "pdp.h"

void run() {
	pc = 01000 ;
	while (1) {
		word w = w_read(pc);
		byte_func = (w>>15) & 1;
		trace("%06o %06o: ", pc, w); // 6 цифр; o - восьмеричная система;
									  // 0 в начале для красоты; 
		pc += 2;

		for(int i = 0; i < func_num ; i++) 
			if ((w & cmd[i].mask) == cmd[i].opcode) {
				trace("%s ", cmd[i].name);
				if((cmd[i].params & 1) == HAS_SS) // has_ss = 00000001
					ss = get_mr(w>>6);
				if((cmd[i].params & 2) == HAS_DD) // has_dd = 00000010
					dd = get_mr(w);
				if((cmd[i].params & 4) == HAS_NN) // has_nn = 00000100
				{
					nn = get_nn(w);
					Rn = (w>>6)&7;
				}
				if((cmd[i].params & 8) == HAS_XX) // has_xx = 00001000
					xx = (char)w;

				trace("\n");
				
				cmd[i].do_func();
				break;
			}
	}
}

struct mr get_mr(word w) {
	struct mr res;
	res.space = MEM;
	int r = w & 7;           // номер регистра 
	int mode = (w >> 3) & 7; // номер моды
	int add = w_read(pc);    // добавка к адресу в модах 6 и 7
	switch (mode) {
		case 0:   // R3
			res.adr = r;
			res.val = reg[r];
			res.space = REG;
			trace (" R%o ", r);
			break;
		case 1:   // (R3)
			res.adr = reg[r];
			if(byte_func)
				res.val = w_read(res.adr);
			else
				res.val = b_read(res.adr);
			trace (" (R%o) ", r);
			break;
		case 2:   // (R3)+
			res.adr = reg[r];
			if(byte_func && reg[r] != pc){
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
		case 3:   // @(R3)+
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
			res.adr = reg[r] + add;
			if(byte_func)
				res.val = w_read(res.adr);
			else
				res.val = b_read(res.adr);
			if(r == 7)
				trace(" %o(R%o) ", add, r);
			else	
				trace(" %o ", res.adr);
		case 7:
			res.adr = reg[r] + add;
			if(byte_func)
				res.val = w_read(res.adr);
			else
				res.val = b_read(res.adr);
			if(r == 7)
				trace(" %o ", res.adr);
			else	
				trace(" @%o(R%o) ", add, r);
		default:
			fprintf(stderr,
				"Mode %o NOT IMPLEMENTED YET!\n", mode);
			exit(1); 			

	}
	return res;
}

int get_nn(word w) {
	int res = 0;
	res = w&077;
	return res;
}

//------------------------------------------------processor commands
void do_mov() {
	if (dd.space == REG)	
  	   reg[dd.adr] = ss.val;
  	if (dd.space == MEM)
  	w_write(dd.adr, ss.val);
  	   if (dd.adr == odata)
  	   trace("  %c", mem[odata]);
  	assert(((pc>>15) & 1) == 0);
}

void do_add(){
  	if (dd.space == REG)
  	   reg[dd.adr] = ss.val + dd.val;
  	if (dd.space == MEM)
  	   w_write (dd.adr, ss.val + dd.val);
}

void do_sob(){
	reg[Rn] = reg[Rn]-1;
	if (reg[Rn] != 0)
	   pc = pc - 2*nn;

}


void do_movb() {
	if(dd.space == REG)
		reg[dd.adr] = (word)((byte)ss.val);
	if(dd.space == MEM)
		b_write (dd.adr, (byte)ss.val);
	if(dd.adr == odata)
		trace(" %c\n", mem[odata]);
	set_NZ(ss.val);
	set_C(ss.val);
}

void do_halt(){
	trace("\nR0 = %06o R2 = %06o R4 = %06o sp = %06o\
	\nR1 = %06o R3 = %06o R5 = %06o pc = %06o\n"  ,\
	reg[0], reg[2], reg[4], reg[6], reg[1], reg[3], reg[5], reg[7]);
	exit (0);
}

void do_br() {
	trace(" xx=%d \n", xx);
	pc = pc + 2*xx;
}

void do_beq(){
	if (Z)
		do_br ();
}

void do_bne() {
	if (Z != 0)
		do_br ();
}

void do_bmi() {
	if (N) 
		do_br();
}

void do_bpl() {
	if (N == 0) 
		do_br();
}

void do_tst() {
	set_NZ(dd.val);
	C = 0;
}

void do_tstb() {
	set_NZ(dd.val);	
	C = 0;
}

void do_cmp() {
	set_NZ(ss.val - dd.val);
	set_C(ss.val - dd.val);
}

void do_cmpb() {
	set_NZ(ss.val - dd.val);
	set_C(ss.val - dd.val);
}

void set_NZ(word w)
{
	Z = 0;
	if (byte_func)
	   N = (w>>7) & 1;
	else
       N = (w>>15) & 1;
    if (w == 0)
	   Z = 1;
}

void set_C (word w){
	C = (w>>15)&1;
}

//--------------------------------------------------------
void load_file (const char * file_name) {
	unsigned int start = 0, n = 0, w = 0;
	FILE * f_inp = fopen(file_name, "rb");
	assert(f_inp);
	while(2 == fscanf(f_inp,"%x %x", &start, &n)){
		for (int i = 0; i < n; i++)
	  	{
	    	fscanf(f_inp, "%x", &w);
			b_write (start+i, (byte)w);
	  	}
	}
	fclose (f_inp);	
}

//------------------------------------------------------ work with words/bytes

void b_write(Adress adr, byte b) {
	word x;
	if (adr % 2 == 0) {
		x = mem[adr];
		x = x>>8;
		mem[adr] = (x<<8) | (word)b;
	}
	else {
		x = mem[adr-1];
		mem[adr-1] = (byte)x | ((word)b<<8);
	}


}

byte b_read (Adress adr) {
	byte b;
	if (adr % 2 == 0)
		b = (byte)mem[adr];
	else {
		word w = mem[adr-1];
		b = (byte)(w >> 8);
	}
	return b;
}

word w_read (Adress adr) {
	assert(adr % 2 == 0);
	return mem [adr];
}

void w_write (Adress adr, word w) {
	assert(adr % 2 == 0);
	mem[adr] = w;
}

