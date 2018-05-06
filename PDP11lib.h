#ifndef PDP11LIB_H_INCLUDED
#define PDP11LIB_H_INCLUDED

typedef unsigned char byte;
typedef unsigned short int word;
typedef word adr;
typedef int dword;

extern byte mem [64 * 1024];
extern word reg [8];

extern struct Flags flags;
extern struct mr  ss, dd, hh, nn;
extern struct sign xx;

#define pc reg[7]
#define REG 1
#define MEM 0
#define NO_PARAM 0
#define HAS_SS 1
#define HAS_DD (1 << 1)
#define HAS_NN (1 << 2)
#define HAS_XX (1 << 3)


#define LO(x) ((x) & 0xFF)
#define HI(x) (((x) >> 8) & 0xFF)
#define sign(w, is_byte) (is_byte ? ((w)>>7)&1 : ((w)>>15)&1 )

word w_read  (adr a);
void w_write (adr a, word val);
byte b_read  (adr a);
void b_write (adr a, byte val);
void get_nn (word w);//ok
void get_xx (word w);

struct Flags
{
    int N;
    int Z;
    int C;
};


struct sign
{
	char val;
	char sign;
};


struct P_Command
{
	word w;       // word 
	int B;        // Byte
	word command; // opcode
	word mode_r1; //mode 1 operand 
	word r1;      // 1 operand 
	word mode_r2; // mode 2 operand
	word r2;      // 2 operand
};

struct mr 
{
	word ad;		// address
	dword val;		// value                                           ////////// put "dword"
	dword res;		// result                                          ////////// put "dword"
	word space; 	// address in mem[ ] or reg[ ]
};

struct Command
{
	word opcode;                       //opcode
	word mask;                         //mask
	char * name;                       //name of command
	void (* func)(struct P_Command PC);//do_command
	byte param;                        //parametr of commmand
};

void load_file ( );                                    //ok
void mem_dump (adr start, word n);//printing of words  //ok
void print_reg ();                                     //ok
struct P_Command create_command (word w);              //ok
//void print_command (struct P_Command c);             //ok
void run (adr pc0);                                    //ok

struct mr get_mode (word r, word mode, word b);        //ok

void change_flag(struct P_Command PC);                 //ok

void do_halt (struct P_Command PC);                    //ok
void do_mov (struct P_Command PC);                     //ok
void do_add (struct P_Command PC);                     //ok
void do_unknown (struct P_Command PC);                 //ok
void do_sob (struct P_Command PC);                     //ok
void do_clr (struct P_Command PC);                     //ok 
void do_movb(struct P_Command PC);                     //ok
void do_br(struct P_Command PC);                       //ok
void do_beq(struct P_Command PC);                      //ok
word byte_to_word(byte b);                             //ok

///#include "PDP11func.cpp"

#endif