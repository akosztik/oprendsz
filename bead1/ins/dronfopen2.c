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
  char nev[100];
  char cim[100];
  char email[100];
  char tel[100];
  int surgosseg;
  time_t date;
};

void readData(char* fname, struct ugyfel* u, int* size) {
  FILE * f = fopen(fname, "a+");
  printf("\n########## %i\n", f);
  int i = 0;
  //struct tm tm;
  const char * datestring;
  while(fscanf(f, "%s %s %s %s %i %s", u[i].nev, u[i].cim, u[i].email, u[i].tel, u[i].surgosseg, &datestring )!= EOF){
    printf("%s\n", u[i].nev);
    //strptime(datestring, "%H:%M:%S", &tm);
    //u[i].date=mktime(&tm);
    i++;
  }
  *size=i;
  fclose(f);

}

void mentes(char* fname, struct ugyfel* u, int* size) {
  FILE *f = fopen(fname, "a+");
  if (f < 0) {
  	perror("Nem lehet megnyitni a fajlt!\n");
  	exit(1);
  }
  //FILE* temp = fopen("temp", "a+");
  //if (temp < 0) {
  //	perror("Nem lehet megnyitni a fajlt!\n");
  //	exit(1);
  //}
  int i = 0;
  while(i < *size) {
    //fwrite(u, sizeof(struct ugyfel), 1, f);
    fprintf(f, "%s %s %s %s %i %s \n", u[i].nev, u[i].cim, u[i].email, u[i].tel, u[i].surgosseg, ctime(&u[i].date ));
    printf("\tugyfel: %s", u[i].nev);
    i++;
  }
  fclose(f);
  //fclose(temp);
  //remove(fname);
  //rename("temp",fname);
  printf("\tMentve!\n");
}


void ugyfelAdatok(struct ugyfel* u, int id) {
  printf("\n\tID: %i\n", id);
  printf("\tNev: %s", u->nev);
  printf("\tCim: %s", u->cim);
  printf("\tEmail: %s", u->email);
  printf("\tTelefon: %s", u->tel);
  printf("\tSürgös: %i\n", u->surgosseg);
  printf("\tBejelentes ideje: %s", ctime(&(u->date)));
}

void ugyfelAdatokRoviden(struct ugyfel* u, int id) {
  printf("\n\tID: %i\n", id);
  printf("\tNev: %s", u->nev);
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
  printf("\n");
}

void surgossegSzerint(struct ugyfel* u, int* size) {
  printf("\n\tA surgossegi felarasok: \n");
  int i;
  for (i = 0; i < *size; i++) {
    if (u[i].surgosseg == 1) {
      ugyfelAdatok(&u[i], i + 1);
    }
  }
}

void ugyfelSzerint(struct ugyfel* u, int* size) {
  printf("\n\tUgyfel neve: ");
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

int  getUSurgos(char* message) {
 char input[10];
  do {
    printf(message);
    fgets(input, sizeof(input), stdin);
  } while (strcmp(input, "I\n") != 0 && strcmp(input, "H\n") != 0);
  if (strcmp(input,"I\n")==0){
    return 1;
  }else{
    return 0;
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
  struct ugyfel c;
  printf("\n\tNev: ");
  fgets(c.nev, sizeof(c.nev), stdin);
  printf("\tCim: ");
  fgets(c.cim, sizeof(c.cim), stdin);
  printf("\tEmail: ");
  fgets(c.email, sizeof(c.email), stdin);
  printf("\tTelefon: ");
  fgets(c.tel, sizeof(c.tel), stdin);
  c.surgosseg = getUSurgos("\tSurgos(I/H): ");
  c.date = time(NULL);
  u[*size] = c;
  (*size)++;
  printf("\tAdatok mentve!\n");
}

void torles(struct ugyfel* u, int* size) {
  listazas(u, size, 2);
  int at = getUInt("\n\tTorlendo id: ", 0, *size);
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
  int at = getUInt("\n\tModositando id: ", 0, *size);
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
        u[i].surgosseg = getUSurgos("\tSurgos (I/H): ");
        done = 1;
        break;
    }
  } while(!done);
  printf("\tJelentkezo frissitve!\n");
}

int main(int argc, char** argv) {
  if (argc != 2) { perror("Fajlnevet adj argumentumkent!\n"); exit(1); }

  struct ugyfel u[1000];
  int size = 0;

  readData(argv[1], u, &size);
  printf("%i\n", size);

  int option;
  do {
    printf("\n1) Uj jelentkezo bevitele\n");
    printf("2) Modositas\n");
    printf("3) Törles\n");
    printf("4) Adatok listazasa\n");
    printf("5) Szures ugyfelre\n");
    printf("6) Szures surgossegre\n");
    printf("7) Adatok mentese\n");
    printf("8) Kilepes\n");
    option = getUInt(NULL, 1, 8);
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
    }
  } while(option != 8);

  return 0;
}

