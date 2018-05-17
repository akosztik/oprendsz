#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

void handler(int signumber){
  printf("elkezdhetjük a tanulást %i \n",signumber);
}

int main(){

    
  signal(SIGTERM,handler); //handler = SIG_IGN - ignore the signal (not SIGKILL,SIGSTOP), 
  
  int pipec[2];
  if (pipe(pipec) == -1) 
  {
    perror("Hiba a pipe nyitaskor!");
    exit(EXIT_FAILURE);
  }
                           //handler = SIG_DFL - back to default behavior 
  int status;
  pid_t child=fork();
  //if (child < 0) {
  //	perror("Forkolasi hiba!");
  //	exit(EXIT_FAILURE);
  //}
  if (child>0) //szulo folyamat
  { 
    close(pipec[0]);
    pause(); //waits till a signal arrive 
    //printf("jelzes a gyerektol megerkezett\n");
    
    int i;
    for (i=1;i<4;i++){
      write(pipec[1], &i, sizeof(int));
	sleep(1);
    }
    close(pipec[1]);
    waitpid(child,&status,0); // gyerekfolyamat végére vár
    printf("%i: végeztem\n",getppid());
  }
  else //gyerek
  {
    close(pipec[1]);
    
    kill(getppid(),SIGTERM);
    
    printf("\t%i: elküldtem a szignalt a szulonek\n", child);
    int ido;
    int i;
    for (i=0;i<3;i++){

      read(pipec[0], &ido, sizeof(int));
      printf("\t%i: countdown: %i\n",child, ido);
    } 
    close(pipec[0]);	
    if(ido==3){
	printf("\t%i: ugrottam\n",child);
    }
    //sleep(3);
    //kill(getppid(),SIGTERM); 
    //1. parameter the pid number of process, we send the signal
    // 		if -1, then eacho of the processes of the same uid get the signal
    // 		we kill our bash as well! The connection will close
    //2. parameter the name or number of signal
    printf("%i: vegeztem\n",child);  
  }
  return 0;
}
