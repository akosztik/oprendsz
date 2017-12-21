#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

struct tob
{
	char typ[20];
	char street[20];
	int hn;
};

char names[5][10];

void handler(int x)
{
	printf("signal arrived\n");
}

//0			1					2
//roadshow Fekete_Rigo_utca 53 2
int main (int argc,char* argv[]) 
{
	
	strcpy(names[0],"Balogh");
	strcpy(names[1],"Kovacs");
	strcpy(names[2],"Mezga");
	strcpy(names[3],"Toth");
	strcpy(names[4],"Varga");

	key_t key;
	int smid;
	char* sm;
	printf("itt\n");
	struct tob t;
	strcpy(t.typ,argv[1]);
	strcpy(t.street,argv[2]);
	//t.hn = atoi(argv[3]);
	
	

	key = ftok(argv[0],1); //nevezd át a fájlt
	smid=shmget(key,500,IPC_CREAT|S_IRUSR|S_IWUSR);
	sm = shmat(smid,NULL,0);
	
	//int tobNum = atoi(argv[4]);
	int i;
	pid_t pid;
	pid_t ppid = getpid();
	signal(SIGUSR1,handler);

	pid = fork();
	if(pid == 0) //gyerek
	{
		sleep(1);
		struct tob ct;
		srand(time(NULL));
		int r=rand()%5;
		//names[r]
		memcpy(&ct,sm,sizeof(struct tob));
		printf("nev: %s\n",names[r]);
		printf("\ntype: %s street: %s\n",ct.typ,ct.street);
		printf("pid: %i\n",getpid());
		kill(ppid,SIGUSR1);
		exit(0);
	}
	else //szülő
	{
		memcpy(sm,&t,sizeof(struct tob));
		shmdt(sm);
		wait(NULL);
		shmctl(smid,IPC_RMID,NULL);
	}
	

	return 0;

}