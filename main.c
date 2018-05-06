//#include "PDP11lib.h"
typedef unsigned short int adr;

void load_file(char * file);
void run(adr pc0);


int main (int argc, char** argv)
{

    load_file(argv[1]);
    run(01000);

    return 0;
}