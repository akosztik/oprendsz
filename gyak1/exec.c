#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //fork
#include <sys/wait.h> //waitpid
#include <errno.h> 


int main()
{
 int status;
 
 pid_t  child=fork(); //forks make a copy of variables
 if (child<0){perror("The fork calling was not succesful\n"); exit(1);} 
 if (child>0) //the parent process, it can see the returning value of fork - the child variable!
 {
    waitpid(child,&status,0); 
    printf("The end of parent process\n");
 }
 else //child process
 {
    //to start a program, like printing out a string (in parameter) 5 times (parameter)
    char * cmd="./write";
    char * arg[]={"./write","Operating Systems","5",NULL}; 
    printf("./write program will be called by execv function\n");
    execv(cmd,arg);
    printf("It never returns to child process (except on error)- the content of the whole child process will be changed to the new one");
 }
 return 0;
}

//http://reptile.web.elte.hu/oprendszer/
//http://bakod.web.elte.hu/oprendszer/Posix2.pdf
//kill hívás
//http://bakod.web.elte.hu/oprendszer/

// mi az exec? -->fork utáni állapotban használják: amikor a gyerek létrejön és ez indít egy új folyamatot. 
// pl: shell vizsgálja a beírt parancsot ha oké indítja az execcel a parancsot, fork exec - az alapváza.
//Így alapból lefut-e? rendben van? exec csalad 4-6 tagjának utána nézni