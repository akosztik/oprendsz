#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //fork
#include <sys/wait.h> //waitpid
#include <errno.h> 


int main()
{
 int status;
 int notacommonvalue=1;
 printf("The value is %i before forking \n",notacommonvalue);
 
 pid_t  child=fork(); //forks make a copy of variables
 if (child<0){perror("The fork calling was not succesful\n"); exit(1);} 
 if (child>0) //the parent process, it can see the returning value of fork - the child variable!
 {
    waitpid(child,&status,0); 
    //waits the end of child process PID number=child, the returning value will be in status
    //0 means, it really waits for the end of child process - the same as wait(&status)
    printf("The value is %i in parent process (remain the original) \n",notacommonvalue);
 
 }
 else //child process
 {
    notacommonvalue=5; //it changes the value of the copy of the variable
    printf("The value is %i in child process \n",notacommonvalue);
 }
 return 0;
}

/*
folyamatok:

parent folyamat (eredeti) visszatérési értéke a gyerekfolyamat PID-je, új: child visszatérési értéke 0
dinamikus statikus változók is hozzátartoznak a folyamatokhoz. a gyerekfolyamat foglal a heapen helyet, és nem átadja a szülő mutatót.
a gyerek folyamat változói nem érintkeznek a szülő változókkal többé, miután copizódott nem fér a szülő folymat változóihoz.
HF: az egészet átírni mutatóra, dinamikus memóriába mutat akkor vajon közös lesz e?
*/