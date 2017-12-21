#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char * argv [] ) {
	
	/*
   * Read through command-line arguments for options.
   */
  
    printf( "argc = %d\n", argc );
	int i;
    for(  i = 0; i < argc; ++i ) {
        printf( "argv[ %d ] = %s\n", i, argv[ i ] );
    }

    
  
	
	return 0;
}