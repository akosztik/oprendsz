#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/types.h>
#include <time.h>

void gyerek1(int signumber){
  printf("Üzenet a felderítőtől: rajta vagyok!\n");
}

void gyerek2(int signumber){
  printf("Második gyerek kész!\n");
}

struct uzenet { 
     long mtype;//ez egy szabadon hasznalhato ertek, pl uzenetek osztalyozasara
     char mtext [ 1024 ]; 
};

struct ADAT {
	int nehezseg;
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
		// kapo
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
			int hany = 0;
			read(pipe1[0], &hany, sizeof(hany));
			int max = -1;
			int index = -1;
			struct ADAT adatok[hany];
			for(i = 0;i < hany;++i)
			{
				read(pipe1[0], &adatok[i], sizeof(struct ADAT));
				printf("A felderítő talált: %s\nNehézség: %d\n", adatok[i].hol, adatok[i].nehezseg);
				if(adatok[i].nehezseg >= max)
				{
					max = adatok[i].nehezseg;
					index = i;
				}
			}
			close(pipe1[0]);
			
			
			write(pipe2[1], &adatok[index], sizeof(struct ADAT));
			close(pipe2[1]);
			close(pipe2[0]);
			
			char sikeres = 0;
			read(pipe3[0], &sikeres, sizeof(sikeres));
			if(sikeres < 1)
				printf("A szallito sikertelen volt!\n");
			else
				printf("A szallito sikeres volt!\n");
			
			wait(&status);
			return 0;
			
		}
		// szallito
		else
		{
			//raise(SIGUSR1);
			//printf("Lefut a szallito\n");
			
			struct ADAT lopo;
			read(pipe2[0], &lopo, sizeof(struct ADAT));
			
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
			write(pipe3[1], &siker, sizeof(siker));
			close(pipe3[0]);
			close(pipe3[1]);
			
			exit(0);
			
		}
	}
	// felderito
	else
	{
		sleep(1);
		//printf("Lefut a felderito\n");
		//raise(SIGUSR2);
		char celpont[100];
		struct uzenet uz;
		status = msgrcv(uzenetsor, &uz, 1024, 5, 0);
		if(status >= 0)
		{
			printf("A felderítő megkapta az uzenetet!\n");
			printf("%d számú felderítő célpontja: %s\n", getpid(), uz.mtext);
			
			kill(getppid(), SIGUSR1);
			srand(time(NULL));
			int talal = rand() % 3 + 1;
			struct ADAT adatok[talal];
			int i = 0;
			close(pipe1[0]);
			write(pipe1[1], &talal, sizeof(talal));
			char* helyek[3];
			helyek[0] = "Belváros";
			helyek[1] = "Margit híd";
			helyek[2] = "Deák tér";
			for(;i < talal;++i)
			{
				adatok[i].nehezseg = rand() % 3;
				strcpy(adatok[i].hol, helyek[adatok[i].nehezseg]);
				write(pipe1[1], &(adatok[i]), sizeof(struct ADAT));
			}
			
			close(pipe1[1]);
			
		}
		else
			printf("A szallito nem kapta meg az uzenetet!\n");
		
		
		exit(0);
	}
	return 0;
}