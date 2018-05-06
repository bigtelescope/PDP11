#include <stdio.h>
#include <stdlib.h>

#include "PDP11lib.h"

extern byte mem [64 * 1024];
extern word reg [8];

extern struct Flags flags;
extern struct sign xx;
extern struct mr ss, dd, hh, nn;

struct Command commands [] =
{
	{	  0,	0177777,	"halt",		do_halt,	NO_PARAM		},
	{010000,	0170000,	"mov",		do_mov, 	HAS_SS | HAS_DD	},
	{060000, 	0170000,	"add",		do_add,		HAS_DD | HAS_SS	},
	{077000,	0177000,	"sob",		do_sob,		HAS_NN			},
	{005000, 	0177000,	"clr",		do_clr,		HAS_DD			},
	{0110000,	0170000,	"movb",		do_movb, 	HAS_SS | HAS_DD	},
    {001400, 	 0xFF00,	"beq", 		do_beq, 	HAS_XX			},
    {000400,     0xFF00,    "br",       do_br,      HAS_XX },
	{	  0,		  0,	"unknown",	do_unknown,	NO_PARAM		}
};

void change_flag(struct P_Command PC)
{
	if(PC.B)
	{
		flags.N = (dd.res >> 7) & 1;
	}
	else 
	{
		flags.N = (dd.res >> 15) & 1;
	}

	flags.Z = (dd.res == 0);

	if(PC.B)
	{
		flags.C = (dd.res >> 8) & 1;
	}
	else
	{
		flags.C = (dd.res >> 16) & 1;
	}
}

void do_halt (struct P_Command PC)
{
	printf("\n");
	print_reg();
	exit(0);
}

void do_mov (struct P_Command PC)
{
	dd.res = ss.val;
	if(dd.space == REG)
	{
		reg[dd.ad]= dd.res;
	}
	else
	{
		w_write(dd.ad, dd.res);
	}
	printf("\n");
	change_flag(PC);
}

void do_movb(struct P_Command PC)
{
	dd.res = ss.val;
	if (dd.space == REG)
	{
		reg[dd.ad] = byte_to_word(dd.res);	
	}
	else
	{
		b_write(dd.ad, (byte)dd.res);
	}
	printf ("\n");
	change_flag(PC);
}

void do_add (struct P_Command PC)
{
	dd.res = dd.val + ss.val;
	if(dd.space == REG)
	{
		reg[dd.ad]= dd.res;
	}
	else
	{
		w_write(dd.ad, dd.res);
	}
	printf("\n");
	change_flag(PC);
}

void do_sob (struct P_Command PC)
{
	reg[nn.ad]--;
	if (reg[nn.ad] != 0)
	{
		reg[7] -= 2 * nn.val;
	}
	printf ("\n");
}


void do_clr (struct P_Command PC)
{
	dd.val = 0;
	printf ("\n");
}

void do_br(struct P_Command PC)
{
	//printf("%o ", pc);
	if(xx.sign == 1)
	{
		pc += 2 * xx.val;
	}
	else
	{
		pc += 2 * xx.val;
	}
	printf("%o ", pc);
	printf("\n");
}

void do_beq(struct P_Command PC)
{
	
	if(flags.Z == 1)
	{
		do_br(PC);
	}
	else
	{
		printf("%o\n", pc + (2 * xx.val));
	}
	//printf("%o\n", pc);
}

void do_unknown (struct P_Command PC)
{
	printf("\n");
	exit(0);
}
