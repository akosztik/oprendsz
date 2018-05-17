#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

struct ugyfel {
  char date[20];
  char nev[100];
  char cim[100];
  char email[100];
  char tel[100];
  char surgosseg[5];
};

void readData(char* fname, struct ugyfel* u, int* size) {
  FILE * f = fopen(fname, "a+");
  int i = 0;
  while(fscanf(f, "%s %s %s %s %s %s", u[i].nev, u[i].cim, u[i].email, u[i].tel, u[i].surgosseg, u[i].date )!= EOF){
    i++;
  }
  *size=i;
  fclose(f);

}

void mentes(char* fname, struct ugyfel* u, int* size) {
  FILE *f = fopen(fname, "w");
  if (f < 0) {
  	perror("Nem lehet megnyitni a fajlt!\n");
  	exit(1);
  }
  FILE* temp = fopen("temp", "a+");
  if (temp < 0) {
  	perror("Nem lehet megnyitni a fajlt!\n");
  	exit(1);
  }
  int i = 0;
  while(i < *size) {
    fprintf(temp, "%s %s %s %s %s %s \n", u[i].nev, u[i].cim, u[i].email, u[i].tel, u[i].surgosseg, u[i].date );
    printf("\tugyfel: %s\n", u[i].nev);
    i++;
  }
  fclose(f);
  fclose(temp);
  remove(fname);
  rename("temp",fname);
  printf("\tMentve!\n");
}


void ugyfelAdatok(struct ugyfel* u, int id) {
  printf("\tID: %i\n", id);
  printf("\tNev: %s", u->nev);
  printf("\tCim: %s", u->cim);
  printf("\tEmail: %s", u->email);
  printf("\tTelefon: %s", u->tel);
  printf("\tSurgos: %s\n", u->surgosseg);
  printf("\tBejelentes ideje: %s\n", u->date);
}


void ugyfelAdatokRoviden(struct ugyfel* u, int id) {
  printf("\tID: %i", id);
  printf("\tNev: %s\n", u->nev);
}

void listazas(struct ugyfel* u, int* size, int mod) {
  int i = 0;
  if (mod==1){
    while (i < *size) {
      ugyfelAdatok(u, i + 1);
      u++;
      i++;
    }
  }else{
    while (i < *size) {
      ugyfelAdatokRoviden(u, i + 1);
      u++;
      i++;
    }
  }
}

void surgossegSzerint(struct ugyfel* u, int* size) {
  printf("\tA surgossegi felarasok: \n");
  int i;
  int count=0;
  for (i = 0; i < *size; i++) {
    if (strcmp(u[i].surgosseg,"Igen")==0) {
      ugyfelAdatok(&u[i], i + 1);
      count++;
    }
  }
  if (count == 0){
	printf("\tnem jelentkeztek\n");
  }
	
}

void ugyfelSzerint(struct ugyfel* u, int* size) {
  printf("\tUgyfel neve: ");
  char nev[100];
  fgets(nev, sizeof(nev), stdin);
  printf("\n\tNev szerint keresett ugyfel: \n");
  int i;
  for (i = 0; i < *size; i++) {
    if (strcmp(u[i].nev,nev)==0){
      ugyfelAdatok(&u[i], i + 1);
    }
  }
}

int getUInt(char* message, int min, int max) {
  int converted;
  char* end;
  char input[10];
  do {
    printf(message);
    fgets(input, sizeof(input), stdin);
    converted = strtol(input, &end, 10);
  } while (end == input || converted < min || converted > max);
  return converted;
}

void hozzaadas(struct ugyfel* u, int* size) {
  time_t systime = time(NULL);
  struct ugyfel c;
  printf("Szóközök helyett '_'-t használjon!\n");
  strftime(c.date, 20, "%Y.%m.%d.%H:%M:%S", localtime(&systime));
  printf("\tNev: ");
  fgets(c.nev, sizeof(c.nev), stdin);
  printf("\tCim: ");
  fgets(c.cim, sizeof(c.cim), stdin);
  printf("\tEmail: ");
  fgets(c.email, sizeof(c.email), stdin);
  printf("\tTelefon: ");
  fgets(c.tel, sizeof(c.tel), stdin);
  printf("\tSurgos (Igen/Nem): ");
  fgets(c.surgosseg, sizeof(c.surgosseg), stdin);
  u[*size] = c;
  (*size)++;
  printf("\n");

}

void torles(struct ugyfel* u, int* size) {
  listazas(u, size, 2);
  int at = getUInt("\tTorlendo id: ", 0, *size);
  if (at == 0) { return; }
  int i = at - 1;
  while (i < *size - 1) {
    u[i] = u[i + 1];
    i++;
  }
  (*size)--;
  printf("\tTorolve!\n");
}

void teljesitettkuldetes(struct ugyfel* u, int* size, int id) {
 
  if (id == -1) { return; }
  int i = id;
  while (i < *size - 1) {
    u[i] = u[i + 1];
    i++;
  }
  (*size)--;
}


void szerkesztes(struct ugyfel* u, int* size) {
  listazas(u, size, 2);
  int at = getUInt("\tModositando id: ", 0, *size);
  if (at == 0) { return; }
  int i = at - 1;
  ugyfelAdatok(u, at);
  printf("\n\tModositando attibutum:\n");
  printf("\t1) Nev\n");
  printf("\t2) Cim\n");
  printf("\t3) Email\n");
  printf("\t4) Telefon\n");
  printf("\t5) Surgosseg\n");
  int option;
  int done = 0;
  printf("\tSzóközök helyett '_'-t használjon!\n");
  do {
    option = getUInt(NULL, 1, 5);
    switch (option) {
      case 1:
        printf("\n\tUj nev: ");
        fgets(u[i].nev, sizeof(u[i].nev), stdin);
        done = 1;
        break;
      case 2:
        printf("\n\tUj cim: ");
        fgets(u[i].cim, sizeof(u[i].cim), stdin);
        done = 1;
        break;
      case 3:
        printf("\n\tUj email: ");
        fgets(u[i].email, sizeof(u[i].email), stdin);
        done = 1;
        break;
      case 4:
        printf("\n\tUj telefon: ");
        fgets(u[i].tel, sizeof(u[i].tel), stdin);
        done = 1;
        break;
      case 5:
        printf("\n\tSurgos (Igen/Nem): ");
        fgets(u[i].surgosseg, sizeof(u[i].surgosseg), stdin); 
		done = 1;
        break;
    }
  } while(!done);
  printf("\n\tJelentkezo frissitve!\n");
}

void szamla(char* cimzett, int ar)
{
	FILE* f;
	f = fopen("szamla.txt", "w");
	
	fprintf(f, "%sMegbízás teljesítve!\nÖsszesen %i000 Ft lesz.\n\n", cimzett, ar);
}

void handler(int signumber)
{
	printf("\n\tAz idomár munkára kész!\n");
}
int dronfogas(struct ugyfel* u, int* size)
{	
	int ugyfelsorszam = -1;
	if (*size == 0){
		printf("\tnincsenek ugyfelek\n");
	}else{
		int i;
		for ( i = 0; i < *size; i++) {
			if (strcmp(u[i].surgosseg,"Igen")==0) {
				ugyfelsorszam = i;
			}
		}
		int pipe1[2];
		int pipe2[2];
		int pipe3[2];
		
		if (pipe(pipe1) == -1) 
		{
			perror("Hiba a pipe nyitaskor!");
			exit(EXIT_FAILURE);
		}
	
		if (pipe(pipe2) == -1) 
		{
			perror("Hiba a pipe nyitaskor!");
			exit(EXIT_FAILURE);
		}
		if (pipe(pipe3) == -1) 
		{
			perror("Hiba a pipe nyitaskor!");
			exit(EXIT_FAILURE);
		}
		pid_t idomar, madar, mainpid;
		signal(SIGTERM, handler);
		mainpid = getpid();
		if (mainpid>0){
			
			pause(); //szignálra vár
			close(pipe1[0]);
			close(pipe2[1]);
			close(pipe3[0]);
			close(pipe3[1]);
			
			write(pipe1[1], u[ugyfelsorszam].cim, sizeof(u[ugyfelsorszam].cim));
			close(pipe1[1]);	
			int fogadott;
				
			read(pipe2[0], &fogadott, sizeof(int));
			printf("\n\tPID: %i: A madár ennyi idő alatt fogta be a drónt: %i másodperc.\n",getppid(), fogadott);
			close(pipe2[0]);
		}		
		idomar = fork();		
		if (idomar == 0) // idomar-child process
		{
			close(pipe1[1]);
			close(pipe2[0]);
			close(pipe3[1]);	 
			
			printf("\n\tPID: %i: Az idomár most végzi a madár körüli teendőket...",idomar);
			sleep(2);
			kill(mainpid, SIGTERM);
			sleep(2);
		        
            char pipestring[100];
			read(pipe1[0], pipestring, sizeof(pipestring));
			printf("\n\tPID: %i: Következő megbízás: %s",idomar, pipestring);
			close(pipe1[0]);
			int ido;
			
			read(pipe3[0], &ido, sizeof(int));
			close(pipe3[0]);
			
			write(pipe2[1], &ido, sizeof(int));
			close(pipe2[1]);
			
			madar= fork();
			if (madar==0){
                //madar-child process forked from parent process
				
				close(pipe1[1]);
				close(pipe1[0]);
				close(pipe2[1]);
				close(pipe2[0]);
				close(pipe3[0]);
				printf("\n\tPID: %i: A madár most fogja be a drónt...\n",madar);
				int r = rand() % 6 + 2;
				
				write(pipe3[1], &r, sizeof(int));
				close(pipe3[1]);
			}else if (madar<0){
				perror("Forkolasi hiba!");
				exit(EXIT_FAILURE);
			}
		
		}else if (idomar<0){
		    perror("Forkolasi hiba!");
			exit(EXIT_FAILURE);
		}
		
		mainpid = getpid();
		if (mainpid>0){
			
			pause(); //szignálra vár
			close(pipe1[0]);
			close(pipe2[1]);
			close(pipe3[0]);
			close(pipe3[1]);
			
			write(pipe1[1], u[ugyfelsorszam].cim, sizeof(u[ugyfelsorszam].cim));
			close(pipe1[1]);	
			int fogadott;
				
			read(pipe2[0], &fogadott, sizeof(int));
			printf("\n\tPID: %i: A madár ennyi idő alatt fogta be a drónt: %i másodperc.\n",getppid(), fogadott);
			close(pipe2[0]);
				
			//int status2;
			//waitpid(idomar, &status2,0);
				

			szamla(u[ugyfelsorszam].nev, fogadott);
			int i = ugyfelsorszam;
			char* nev=u[i].nev;
			while (i < *size - 1) {
					u[i] = u[i + 1];
					i++;
			}
			(*size)--;
			printf("\n\tPID: %i: ugyfel szamlaja elkeszult: %s\n",getppid(), nev);
		}
			
	}
	return ugyfelsorszam;
}
	
int main(int argc, char** argv) {
  if (argc != 2) { perror("Fajlnevet adj argumentumkent!\n"); exit(1); }

  struct ugyfel u[1000];
  int size = 0;
  printf("\tAdatok beolvasása\n");
  readData(argv[1], u, &size);

  int option;
  do {
    printf("\n1) Uj jelentkezo bevitele\n");
    printf("2) Modositas\n");
    printf("3) Törles\n");
    printf("4) Adatok listazasa\n");
    printf("5) Szures ugyfelre\n");
    printf("6) Szures surgossegre\n");
    printf("7) Adatok mentese\n");
    printf("8) Dronbefogas\n");
	printf("9) Kilepes\n");
    option = getUInt(NULL, 1, 9);
    switch (option) {
      case 1:
        hozzaadas(u, &size);
        break;
      case 2:
        szerkesztes(u, &size);
        break;
      case 3:
        torles(u, &size);
        break;
      case 4:
        listazas(u, &size, 1);
        break;
      case 5:
        ugyfelSzerint(u, &size);
        break;
      case 6:
        surgossegSzerint(u, &size);
        break;
      case 7:
        mentes(argv[1], u, &size);
        break;
      case 8:
		teljesitettkuldetes(u, &size, dronfogas(u, &size));
		break;
    }
  } while(option != 9);

  return 0;
}

