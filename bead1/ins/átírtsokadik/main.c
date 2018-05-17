#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

/* When a SIGUSR1 signal arrives, set this variable. */
volatile sig_atomic_t usr_interrupt = 0;

int FD1[2];
int TimePipe[2];
char* FileName = "datas.txt"; 
pid_t IdomarPid;

void synch_signal (int sig)
{
   usr_interrupt = 1;
}


void idomar_function (pid_t madarPid)
{
  kill (getppid (), SIGUSR1);
  printf("Idomar : Signal sent to parent. \n");
  char adress[255];
  close(FD1[1]);
  read(FD1[0], &adress, sizeof(adress));
close(FD1[0]);
  printf("Idomar recieved from parent : %s  \n", adress);
  kill (madarPid, SIGUSR1);
  while (!usr_interrupt);
  printf("Idomar : The bird catched the drone \n");
  exit (0);
}

void madar_function ()
{ 
   while (!usr_interrupt);
   srand(time(NULL));
  int  sec = rand() % (5 + 1 - 1) + 1;
   printf("sec: %d \n", sec);
   sleep(sec);
   close(TimePipe[0]);
   char secString[2];
   sprintf(secString, "%d", sec);
   write(TimePipe[1], &secString, sizeof(secString));
   close(TimePipe[1]);
   kill (IdomarPid, SIGUSR1);
   exit (0);
}


void writeInvoice(char sec[2], char data[255], char urgent) {
   FILE *f = fopen("invoice.txt", "w");

   int price = 200;
   if (urgent == '1') price *= 2;

   fprintf(f, "Adress: %s", data);
   fprintf(f, "Time: %s sec \n", sec);
   fprintf(f, "Price: %d $ \n", price);

   fclose(f);
}


void parent_function() {
//   printf("Parent : Adress searching started. \n");
   close(FD1[0]);
   close(TimePipe[1]);
   const int maxLines = 100;
   const int maxChars = 255;

   int urgentIndex = -1;
   int nonUrgentIndex = -1;

   char urgentLines[maxLines][maxChars];
   char nonUrgentLines[maxLines][maxChars];

// char res[255];

   FILE* file = fopen(FileName, "r"); 
   if (!file) {
    printf("File does not exist! (name should be: datas.txt) \n");
    exit(0);
   }

   char urgent;
   char line[255];
   int urgentFound = 0;
    while (fgets(line, sizeof(line), file) /* && !urgentFound */ ) {
	int len = strlen(line);
	urgent = line[len-3];
	if (urgent == '1') {
	   urgentIndex++;
           strcpy(urgentLines[urgentIndex],line);
	//   urgentFound = 1;
	}
	else {
	   nonUrgentIndex++;
           strcpy(nonUrgentLines[nonUrgentIndex],line);
	}
    }
   fclose(file);

   printf("urgentLines: %s  \n ",urgentLines[urgentIndex]);
   if (urgentIndex != -1) { 
       urgentIndex--;
       write(FD1[1], &urgentLines[urgentIndex][0], sizeof(urgentIndex));
    }
   else if (nonUrgentIndex != -1) {
       nonUrgentIndex--;
       write(FD1[1], &nonUrgentLines[nonUrgentIndex][0], sizeof(nonUrgentIndex));
   }
   else {
	puts("All adress is processed.");
	exit(0);
   }

   printf("Parent : Adress is written to the pipe. \n");
   close(FD1[1]);
  while (!usr_interrupt);
   char secString[2];
   read(TimePipe[0], &secString, sizeof(secString));
   close(TimePipe[0]);
   printf("Parent : sec arrived (%s s) \n", secString);
  // writeInvoice(secString, res, urgent);
   printf("Parent : Invoice written. \n");

}

int main (void)
{
  struct sigaction usr_action;
  sigset_t block_mask;
  pid_t idomar_id;
  pid_t madar_id;

 pipe(FD1);
 pipe(TimePipe);

  /* Establish the signal handler. */
  sigfillset (&block_mask);
  usr_action.sa_handler = synch_signal;
  usr_action.sa_mask = block_mask;
  usr_action.sa_flags = 0;
  sigaction (SIGUSR1, &usr_action, NULL);

  madar_id = fork ();
 if (madar_id == 0)
    madar_function ();       
 
  idomar_id = fork ();
  IdomarPid = idomar_id;
if (idomar_id == 0)
    idomar_function (madar_id);          


  /* Busy wait for the child to send a signal. */
  while (!usr_interrupt);
  parent_function();

  wait();
  return 0;
}
