#include <stdio.h>
#include <stdlib.h>
//call it with some parameters from the command line 

int main(int argc,char ** argv) 
//char** means an array of character arrays = array of strings
{
 int i;
 printf("Number of command line arguments are: %i\n",argc);
 for (i=0;i<argc;i++){
  printf("%i. argument is %s\n",i,argv[i]);
 }
 return 0;
}

/*fordítása: gcc arg.c vagy cc arg.c
cc -o alma arg.c
./a.out
 l a listázás

https://stackoverflow.com/questions/14690159/is-ascii-code-7-bit-or-8-bit , szöveg vége charcter a nulla character,
Fradi: char * sz="Fradi";
|72 69 99 89 88 0| in memory

-a karaktereket bináris bitesorozatként tárolja.

ha |72 69 0 89 88 0| --> will mean : Fr

sz[2]=0;
sz[2]='0';--> ASCI nullat tesz bele

char nev[20]; //csak memoriat foglalok a tömbnek= memória szemét/ami épp ott van,
char * nev1; // malloc --> memoria alloc (hány bitot foglalsz void *-ot ad vissza, (char *) malloc

HF: szócsere: string sdk --> házi nálam : replacer.c

---------------------------------------------

*/

