#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "PDP11lib.h"

byte mem [64 * 1024];
word reg [8];

struct Flags flags;
struct sign xx;
struct mr ss, dd, hh, nn;

extern struct Command commands[];

struct P_Command create_command(word w)
{
	struct P_Command c;
	c.w = w;
	c.B = (w >> 15);
	c.command = (w >> 12) & 15;
	c.mode_r1 = (w >> 9) & 7;
	c.r1 = (w >> 6) & 7;
	c.mode_r2 = (w >> 3) & 7;
	c.r2 = w & 7;
	return c;
}

byte b_read(adr a)
{
	return mem[a];
}

word w_read(adr a)
{
	//assert((a % 2) == 0);       // need to check?
	word w;
	w = mem[a];
	w += mem[a + 1] * 256;      // for what " * 256 " ?
	return w;
}

void b_write(adr a, byte val)
{
	mem[a] = val;
}

void w_write (adr a, word val)
{

	assert((a % 2) == 0);       // need to check?
	mem[a] = (byte) val;
	mem[a + 1] = (byte) (val >> 8);
}

void get_nn (word w)
{
	nn.ad = (w >> 6) & 07;
	nn.val = w & 077;
	printf ("R%o , %o", nn.ad, pc - 2*nn.val);
	//printf(com, "------\n%o\n------\n", w);
}

void get_xx (word w)
{
	xx.val = w & 0xff;
	//xx.sign = ((w >> 7) & 01);
}

word byte_to_word(byte b) 
{

	word w;
	if (sign(b, 1) == 0) {
		w = 0;
		w |= b;
	} else {
		w = ~0xFF;
		w |= b;
	}
	return w;
}

void print_reg ()
{
	int i = 0;
	printf("\n\n");
	printf("Print registers\n");
	//printf("\n");
	for (i = 0; i < 8; i ++)
	{
		printf("r[%d] = %o\n", i, reg[i]);
	}
	//printf("\n");
}

struct mr get_mode (word r, word mode, word b)//register, mode of this register, byte 
{
	switch(mode)
	{
		case 0:
		{
			printf("R%o", r);
			hh.ad = r;
			hh.val = reg[r];
			hh.space = REG;
			break;
		}

		case 1:
		{
			printf ("@R%o", r);
			hh.val = reg[r];
			hh.ad = w_read ((adr) reg[r]);
			hh.space = MEM;
			break;
		}

		case 2:
		{
			if (r == 7 || r == 6 || b == 0)
			{
				printf ("#%o", w_read ((adr) reg[r]));
				hh.ad = reg[r];
				hh.val = w_read ((adr) reg[r]);
				hh.space = MEM;
				reg[r] += 2;
			}
			else
			{
				printf ("(R%o)+", r);
				hh.ad =  reg[r];
				hh.val = b_read ((adr) reg[r]);
				hh.space = MEM;
				reg[r] ++;
			}
			break;
	}

	case 3:
		{
			printf ("@#%o", w_read((adr) (reg[r])));
			if (r == 7 || r == 6 || b == 0)
			{
				hh.ad = w_read ((adr) reg[r]);
				hh.val = w_read ((adr) w_read ((adr) (reg[r])));
				hh.space = MEM;
				reg[r] += 2;
			}
			else
			{
				hh.ad = w_read ((adr) reg[r]);
				hh.val = b_read ((adr) w_read ((adr) (reg[r])));
				hh.space = MEM;
				reg[r] ++;
			}
			break;
		}
		case 4:
		{
			printf ("-(R%o)", r);
			if (r == 7 || r == 6 || b == 0)
			{
				reg[r] -= 2;
				hh.ad = reg[r];
				hh.val = w_read ((adr) reg[r]);
				hh.space = MEM;
				break;
			}
			else 
			{
				reg[r] --;
				hh.ad = reg[r];
				hh.val = b_read ((adr) reg[r]);
				hh.space = MEM;
				break;
			}
		}
		case 5:
		{
			printf ("@-(R%o)", r);
			reg[r] -= 2;
			hh.ad = w_read ((adr) reg[r]);
			hh.val = w_read ((adr) w_read ((adr) (reg[r])));
			hh.space = MEM;
			break;
		}
	}
	return hh;
}

void load_file(char * FileName)
{
	int size = strlen(FileName);
	if(size == 0)
		exit(0);

	unsigned int a, b, val;
	int i = 0;
	FILE * f = fopen(FileName, "r");
	if (f == NULL) {
		perror(FileName);
		exit(1);
	}
	while(fscanf(f, "%x%x", &a, &b) == 2)
	{
		for(i = a; i < (a + b); i++)
		{
			fscanf(f, "%x", &val);
			b_write(i, val);
		}
	}
	fclose(f);
	//mem_dump(a, b);                  
}

void run(adr pc0)
{
	int i = 0;
	pc = (word)pc0;                           //can i delete (word)???
	//FILE * f = fopen("results.txt", "w");
	while(1)
	{
		word w = w_read(pc);
		pc += 2;
		struct P_Command PC = create_command(w);
		for(i = 0; ; i ++)      //or "sizeof(commands)/sizeof(struct Command)"
		{
			struct Command cmd = commands[i];
			if((w & commands[i].mask) == commands[i].opcode)
			{
				printf("%06o : %06o\t", pc - 2, w );
				printf("%s ", commands[i].name);
				if (cmd.param & HAS_SS)
				{
					//fprintf (com, " , ");
					ss = get_mode (PC.r1, PC.mode_r1, PC.B);
					printf (", ");
				}
				if (cmd.param & HAS_DD)
				{
					dd = get_mode (PC.r2, PC.mode_r2, PC.B);
				}
				/*if (cmd.param & HAS_SS)
				{
					fprintf (com, " , ");
					ss = get_mode (PC.r1, PC.mode_r1, PC.B);
				}*/
				if (cmd.param & HAS_NN)
				{
					get_nn (w);
				}
				if(cmd.param & HAS_XX)
				{
					get_xx(w);
				}
				cmd.func(PC);
				//printf("\n");
				//print_reg ();
				break;
			}
		}
	}
	//fclose(f);
}

void mem_dump(adr start, word n)
{
	assert((start % 2) == 0);
	FILE * f = fopen("adress-word", "w");
	int i;
	for(i = 0; i < n; i += 2)
	{
		printf("%06o : %06o\n", (start + i), w_read((adr)(start + i))); // try to delete (adr)???
	}
	fclose(f);
}

void testmem()
{
    byte b0, b1;
    word w;
    b0 = 0x0a;
    b1 = 0x0b;
    b_write(2, b0);
    b_write(3, b1);
    w = w_read(2);
    printf("func_testmem: %04x =  %02x%02x\n", w, b1, b0);

    w = 0x0c0d;
    w_write (4, w);
    b0 = b_read(4);
    b1 = b_read(5);
    printf("func_testmem: %04x =  %02x%02x\n", w, b1, b0);

    assert( b1 == 0x0c);
    assert( b0 == 0x0d);
}
