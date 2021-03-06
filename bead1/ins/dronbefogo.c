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

void handler(int signumber)	// Signal handler
{
	printf("\n\tAz idomar munkara kesz!\n");
}
int dronfogas(struct ugyfel* u, int* size)
{	
	int ugyfelsorszam = -1;
	if (*size == 0){
		printf("\tnincsenek uygfelek\n");
	}else{
		int i;
		for ( i = 0; i < *size; i++) {
			if (strcmp(u[i].surgosseg,"Igen")==0) {
				ugyfelsorszam = i;
			}
		}
		int fid1 = mkfifo("f2.ftc", S_IRUSR | S_IWUSR);
		int fid2 = mkfifo("f1.ftc", S_IRUSR | S_IWUSR);
		int fid3 = mkfifo("f3.ftc", S_IRUSR | S_IWUSR);
	
		int f1, f2, f3;
	
		pid_t idomar, madar;
		int status;
		char sz[160];  // char array for reading from pipe
	
		signal(SIGTERM, handler);

		idomar = fork();
		if (idomar == 0) 
		{		    	// idomar process
			printf("\n\tAz idomár most végzi a madár körüli teendőket...\n");
			sleep(2);
			kill(getppid(), SIGTERM);
			sleep(1);
		
			int r;
		
			f1 = open("f1.ftc", O_RDONLY);
			read(f1, sz, sizeof(sz));
			printf("\n\tKövetkező megbízás: %s", sz);
			close(f1);
		
			f3 = open("f3.ftc", O_RDONLY);
			read(f3, &r, sizeof(int));
			close(f3);
			
			f2 = open("f2.ftc", O_WRONLY);
			write(f2, &r, sizeof(int));
			close(f2);
		
		} 
		else 
		{
			madar = fork();
			if (madar == 0)	// madar process
			{
				sleep(4);
				printf("\n\tA madár most fogja be a drónt...\n");
				int r = rand() % 6 + 2;
				
				f3 = open("f3.ftc", O_WRONLY);
				write(f3, &r, sizeof(int));
				close(f3);
			
				sleep(1);	
			}
			else
			{
				// szulo process (kozpont)
				pause();
				int fogadott;
				
				f1 = open("f1.ftc", O_WRONLY);
				write(f1, u[ugyfelsorszam].cim, sizeof(u[ugyfelsorszam].cim));
				close(f1);
				
				f2 = open("f2.ftc", O_RDONLY);
				read(f2, &fogadott, sizeof(int));
				printf("\n\tA madár ennyi idő alatt fogta be a drónt: %i másodperc.\n", fogadott);
				close(f2);
				

				szamla(u[ugyfelsorszam].nev, fogadott);
				int i = ugyfelsorszam;
				char* nev=u[i].nev;
				while (i < *size - 1) {
					u[i] = u[i + 1];
					i++;
				}
				(*size)--;
				printf("\n\t %s nevu ugyfel szamlaja elkeszult es torolve lett az adatbazisbol.\n", nev);
				remove("f2.ftc");
				remove("f1.ftc");
				remove("f3.ftc");
			}	
		}
	}
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
      case 9:
	    dronfogas(u, &size);
    }
  } while(option != 9);

  return 0;
}

