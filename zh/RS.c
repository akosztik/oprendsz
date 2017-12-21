#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/types.h>
#include <time.h>

void gyerek1(int signumber){
  printf("toborzo vagyok: rajta vagyok!\n");
}


struct uzenet { 
     long mtype;//ez egy szabadon hasznalhato ertek, pl uzenetek osztalyozasara
     char mtext [ 1024 ]; 
};

struct ADAT {
	int db;
	char hol[100];
};

int main(int argc, char ** argv)
{
	signal(SIGUSR1, gyerek1);
	
	// Uzenetsor
	int uzenetsor, status;
	key_t kulcs; 
	kulcs = ftok(argv[0],1); 
    //printf ("A kulcs: %d\n",kulcs);
    uzenetsor = msgget( kulcs, 0600 | IPC_CREAT );
	if( uzenetsor < 0 )
	{ 
        perror("msgget"); 
        return 1; 
    }
	
	
	int pipe1[2];
	pipe(pipe1);
	
	int pipe2[2];
	pipe(pipe2);
	
	int pipe3[2];
	pipe(pipe3);
	
	srand(time(NULL));
	pid_t p;
	p = fork();
	if(p > 0)
	{
		//
		if(fork())
		{
			struct uzenet uz;
			uz.mtype = 5;
			int i = 0;
			char temp[100];
			strcat(temp, *(argv++));
			strcpy(uz.mtext, "");
			for(;i < argc - 1;++i)
			{
				strcat(uz.mtext, *argv);
				strcat(uz.mtext, " ");
				argv++;
			}
			
			
			status = msgsnd(uzenetsor, &uz, strlen (uz.mtext) + 1, 0);
			if(status >= 0)
				printf("A kapo elküldte az üzenetet!\n");
			else
				printf("A kapo nem tudta elküldeni az üzenetet!\n");
			
			pause();
			
			msgctl(uzenetsor, IPC_RMID, NULL);
			
			close(pipe1[1]);
			char* name
			read(pipe1[0], &name, sizeof(name));
			int max = -1;
			int index = -1;
			struct ADAT adatok[hany];
			for(i = 0;i < hany;++i)
			{
				read(pipe1[0], &adatok[i], sizeof(struct ADAT));
				printf("\ntoborzo: hova: %s\ndb: %d\n", adatok[i].hol, adatok[i].db);
				if(adatok[i].db >= max)
				{
					max = adatok[i].db;
					index = i;
				}
			}
			close(pipe1[0]);
			
			
			write(pipe1[1], &adatok[index], sizeof(struct ADAT));
			close(pipe1[1]);
			close(pipe1[0]);
			
			char sikeres = 0;
			read(pipe1[0], &sikeres, sizeof(sikeres));
			if(sikeres < 1)
				printf("A toborzo sikertelen volt!\n");
			else
				printf("A toborzo sikeres volt!\n");
			
			wait(&status);
			return 0;
			
		}
		// toborzo
		else
		{
			//raise(SIGUSR1);
			//printf("Lefut a tborozo\n");
			
			
			struct ADAT kapott;
			read(pipe1[0], &kapott, sizeof(struct ADAT));
			
			char* name= "marika";
			write(pipe1[1], &name, sizeof(name));
			
			char siker = 0;
			
			int sikeres = rand() % 100 + 1;
			if(sikeres <= 30)
				siker = 1;
			else
			{
				sikeres = rand() % 100 + 1;
				if(sikeres <= 30)
				siker = 1;
			}
			write(pipe1[1], &siker, sizeof(siker));
			close(pipe1[0]);
			close(pipe1[1]);
			
			exit(0);
			
		}
	}
	
	

	return 0;
}