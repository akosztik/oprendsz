#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#include <sys/ipc.h>
#include <sys/msg.h>

void handler_tanar(int signum){
	printf("A tanar(szulo) jelzest kapott! \n");
}


int main(){

	printf("Ugri burgi! \n");
	printf("\n");
	
	
	pid_t id1;
	
	//cso letrehozasa
	int tanar_tanulo[2];
	int tanulo_tanar[2];
	
	int n = pipe(tanar_tanulo);
	int m = pipe(tanulo_tanar);
	
	if(n== -1 || m == -1){
		printf("Hiba a cso letrehozasanal! Kilepek! \n");
		exit(0);
	}
	
	//[0] -> reading
	//[1] -> writing

	
	id1 = fork();
	
	if(id1 > 0){
		//parent
		printf("Tanar vagyok! \n");
		
		//lezarjuk a felesleges csovegeket
		tanar_tanulo[0];
		tanulo_tanar[1];
		
		
		//most megvarjuk, amig a tanulo jelez, hogy keszenall
		printf("Tanar : Most varok a tanulo jelzesere, hogy keszen all a tanulasra! \n");
		signal(SIGCHLD, handler_tanar);
		pause();
		
		int sec = 1;
		int i;
		for(i=1; i<=3; i++){
			write(tanar_tanulo[1], &sec, sizeof(sec));
			printf("Tanar : %i masodperc \n", sec);
			sleep(1);
			sec = sec +1;
		}
		
		
		tanar_tanulo[0];
		tanulo_tanar[1];
		
	}else if(id1 == 0){
		//child
		printf("Tanulo vagyok! \n");
		
		//lezarjuk a felesleges csovegeket
		tanar_tanulo[1];
		tanulo_tanar[0];
		
		
		//kuldunk jelet a tanarnak,hogy megvagyok
		kill(getppid(),SIGCHLD);
		
		//most kiolvassuk a masodperceket
		int j;
		int szam;
		for(j=1; j<=3; j++){
			read(tanar_tanulo[0], &szam, sizeof(szam));
			printf("Tanulo (remegve) : %i masodperc \n", szam);
		}
		
		tanar_tanulo[0];
		tanulo_tanar[1];
		exit(0);
	}
	
	//szulo bevarja, amig a gyerek terminal
	pid_t wpid;
	int status = 0;
	while ((wpid = wait(&status)) > 0);
	
	printf("\n");
	printf("Ugri-Bugri Ceg vege! \n");
}
