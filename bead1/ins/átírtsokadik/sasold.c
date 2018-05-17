#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//aktualis ido lekeresehez kell,es a konvertalashoz
//forditas sajat nevvel : gcc -o sorfeszt sorfeszt.c
#include <string.h>
//ennek a segitsegevel toltjuk majd fel a structokat

#include <fcntl.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

#define ROW 5
#define COL 30
#define SIZE 30
#define MAX 10
#define ONE 1


//SZUKSEGES STRUCT-ok
struct Jelentkezo{
	
	char nev[SIZE];
	char cim[SIZE];
	char email[SIZE];
	char tel[SIZE];
	char datum[SIZE];
	char surgos[SIZE];
};

struct Container{

	struct Jelentkezo emberek[MAX];
	int elemszam;
	
};

//JELENTKEZO MUVELETEK
void nev_beallit(struct Jelentkezo* ember);
//nev beallitasa
void cim_beallit(struct Jelentkezo* ember);
//cim beallitasa
void email_beallit(struct Jelentkezo* ember);
//email cim beallitasa
void tel_beallit(struct Jelentkezo* ember);
//telefonszam beallitasa
void datum_beallit(struct Jelentkezo* ember);
//datum beallitasa
void surgos_beallit(struct Jelentkezo* ember);
//surgosseg beallitas
void jelentkezo_init(struct Jelentkezo* ember);
//egy jelentkezo inicializalasa
void adatok_kiirasa(struct Jelentkezo* ember);
//egy jelentkezo adatainak a kiirasa
void jelentkezok_kiirasa(struct Container* c);
//csak az ugyfelek neveit irjuk ki, elony a torlesnel/modositasnal
void surgos_jelentkezok_kiirasa(struct Container* c);
//csak azokat az ugyfeleket irjuk ki, akiknek surgos
void nem_surgos_jelentkezok_kiirasa(struct Container* c);
//csak azokat az ugyfeleket irjuk ki, akiknek nem surgos

//CONTAINER MUVELETEK
void container_init(struct Container* c);
//inicializalunk egy container
void uj_jelentkezo(struct Container* c);
//egy containerben taroljuk a jelentkezoket
void container_kiirasa(struct Container* c);
//egy container teljes kiiratasa
void jelentkezo_keres_es_modosit(struct Container* c, char nev[SIZE]);
//egy jelentkezot megkeresunk a containerben es modositjuk adatait
void modosit(struct Jelentkezo* ember);
//ezzel modositjuk a megtalalt jelentkezo adatait
void jelentkezo_keres_es_torol(struct Container* c, char nev[SIZE]);
//ezzel toroljuk a megtalalt jelentkezot
void helyreallit(struct Container* c, char nev[SIZE]);
//egy torles utan ezzel allitjuk helyre a container meretet,es elemeit
void teljes_nevsor(struct Container* c);
//a containerben talalhato jelentkezoket kiirjuk egy fajlba
void surgos_nevsor(struct Container* c);
//a containerben szereplo jelentkezok kozul kivalogatjuk a surgos eseteket
void nem_surgos_nevsor(struct Container* c);
//a containerben szereplo jelentkezok kozul kivalogatjuk a nem surgos eseteket
void fajbol_nevsor_beolvasas(char filename[SIZE]);
//ennek a fuggvenynek a segitsegevel olvassuk be a teljes/surgos/nem surgos neveket fajlbol
void iras_fajlba_osszes_adat(struct Container* c);
//ennek a fuggvenynek a segitsegevel irjuk ki az osszes adatot fajlba

void rendezes_surgosseg_szerint(struct Container* c);
//a surgoseket elorevesszuk, es a surgoseket a vegere

//BEOLVASO FUGGVENY
void beolvas_szoveg(char szoveg[SIZE]);
int beolvas_szam();

//SZAMLAZAS
//NEV-AR eloallitasa fajlba, 
void szamlazas(int arak[], struct Container* c);
//Mi mennyibe kerult
void arazas();
//Fajlbol beolvassuk a szamlat
void szamla_kiiras();

//FELTOLTES FUGGVENY
void feltoltes();
//a szukseges adatok feltoltese

void handler_kozpont(int signum){
	//printf("A kozpont(szulo) jelzest kapott! \n");
}

void handler_idomar(int signum){
	//printf("Az idomar(gyerek) jelzest kapott! \n");
}

void handler_sas(int signum){
	//printf("A solyom(gyerek) jelzest kapott! \n");
}


int main(){
	pid_t id1;
	pid_t id2;
	
	int kozpont_idomar[2]; // parent csak ir bele, es child(idomar) csak olvas belole
	int idomar_kozpont[2]; //child csak ir bele(a mert eredmenyt), es a parent csak kiolvassa
	int idomar_sas[2]; //idomar csak ir bele,  , es a sas kiolvassa + az elkapasok
	int sas_idomar[2]; // sas csak ir bele, es az idomar csak olvassa, az idot

	//cso[0] -> used for reading
	//cso[1] -> used for writing
	int n1, n2, n3, n4;
	n1 = pipe(kozpont_idomar);
	n2 = pipe(idomar_kozpont);
	n3 = pipe(idomar_sas);
	n4 = pipe(sas_idomar);
	if(n1==-1 || n2==-1 || n3==-1 || n4==-1){
		printf("Hiba lepett fel a letrehozas soran! \n");
		printf("Kilepes! \n");
		exit(1);
	}
	
	id1=fork();
	
	if(id1 > 0 ){
	
		id2 = fork();
		
		if(id2 == 0){
			//child2
			//feleslegeseket cso vegeit zarjuk le
			close(kozpont_idomar[1]);
			close(idomar_kozpont[0]);
			close(idomar_sas[0]);
			close(sas_idomar[1]);
			
			signal(SIGUSR1, handler_idomar);
			pause();

			printf("Idomar - A sas felkeszitese a dronfogasra....! \n");
			sleep(2);
			kill(getppid(), SIGCHLD);
			
			//elemszam eltarolasa az idomarnal
			int hossz;
			read(kozpont_idomar[0], &hossz, sizeof(hossz));
			printf("Idomar - Kiolvastam az ugyfelek letszamat! %i \n", hossz);
			
			//elemszam tovabbitasa a sasnak
			write(idomar_sas[1], &hossz, sizeof(hossz));
			printf("Idomar - Elkuldtem a sasnak az ugyfelek letszamat! \n");
			
			int j;
			for(j=0; j<hossz;j++){
				//fogadas a kozpontbol ugyfelet
				struct Jelentkezo human;
				read(kozpont_idomar[0], &human, sizeof(human));
				printf("Idomar - Kiolvastam az ugyfelet(nyugta)! \n");
				adatok_kiirasa(&human);
				sleep(1);
				
				//irni a sasnak hogy kapja el
				char szoveg[]="Kapd el!";
				write(idomar_sas[1], &szoveg, sizeof(szoveg));
				sleep(1);
				
				//fogadas a sastol az idot
				int ido;
				read(sas_idomar[0], &ido, sizeof(ido));
				printf("Idomar - A befogasi ido : %i sec \n", ido);
				sleep(1);
				
				//az idot majd tovabbitani kell a kozpontnak
				write(idomar_kozpont[1], &ido, sizeof(ido));
				printf("Idomar - Elkuldtem a kozpontnak az idot! \n");
				sleep(1);
			}
			close(idomar_sas[1]);
			close(sas_idomar[0]);
			close(kozpont_idomar[0]);
			close(idomar_kozpont[1]);
			exit(0);
			
		}else if(id2 > 0){
			//parent1
			close(kozpont_idomar[0]);
			close(idomar_kozpont[1]);
			//feleslegeseket le kell zarni
			close(idomar_sas[0]);
			close(idomar_sas[1]);
			close(sas_idomar[0]);
			close(sas_idomar[1]);
			
			
			struct Container c;
			feltoltes(&c);
			rendezes_surgosseg_szerint(&c);
			int idok[c.elemszam];
			
			printf("Kozpont - Varunk es jelzest kuldunk az idomarnak es a sasnak! \n");
			sleep(3);
			kill(id1, SIGUSR2);//child1-nek jelzes
			kill(id2, SIGUSR1);//child2-nek jelzes
			printf("Kozpont - Megvarom amig jelzest kapok az idomartol,hogy keszen all! \n");
			signal(SIGCHLD, handler_kozpont);
			pause();
			printf("Kozpont - Kezdodhet a munka! \n");
			sleep(2);
			
			//elemszam elkuldese az idomarnak
			int meret = c.elemszam;
			write(kozpont_idomar[1], &meret, sizeof(meret));
			printf("Kozpont - Elkuldtem az idomarnak az ugyfelek letszamat! \n");
			
			int szam;
			int i;
			for(i=0; i<meret; i++){
				//beleirjuk az adott ugyfelet a csobe
				write(kozpont_idomar[1], &c.emberek[i], sizeof(c.emberek[i]));
				printf("Kozpont - Elkuldtem az ugyfelet! \n");
				//kell egy kis ido, amig a gyerek dolgozik az adaton
				sleep(1);
				
				//fogadjuk az idot az idomartol
				read(idomar_kozpont[0], &szam, sizeof(szam));
				printf("Kozpont - Kiolvastam az idot! \n");
				idok[i]=szam;
				sleep(1);
			}
			
			printf("Kozpont - Vegeztunk a meloval! \n");
			printf("Kozpont - Szamla elkeszitese...! \n");
			printf("\n");
			szamlazas(idok, &c);
			arazas();
			szamla_kiiras("szamla");
			close(kozpont_idomar[1]);
			close(idomar_kozpont[0]);
		}
	}else if(id1 == 0){
		//child1
		close(idomar_sas[1]);
		close(sas_idomar[0]);
		//feleslegeseket bezarjuk
		close(kozpont_idomar[0]);
		close(kozpont_idomar[1]);
		close(idomar_kozpont[0]);
		close(idomar_kozpont[1]);
		
		
		signal(SIGUSR2, handler_sas);
		pause();
		int hossz;
		read(idomar_sas[0], &hossz, sizeof(hossz));
		printf("Sas - Kiolvastam az ugyfelek letszamat! %i \n", hossz);
		
		int k;
		int random_number = 0;
		int max=5;
		int min=1;
		char kapd[10];
		for(k=0; k<hossz; k++){
			read(idomar_sas[0], kapd, sizeof(kapd));
			printf("Sas - Kiolvastam az idomar uzenetet : %s\n", kapd);
			srand(time(NULL));
			random_number = rand()%(max-min)+min;
			printf("Sas - Befogasi ido : %i sec \n", random_number);
			write(sas_idomar[1], &random_number, sizeof(random_number));
			printf("Sas - Elkuldtem az elkapasai idot az idomarnak! \n");
		}
		
		
		close(idomar_sas[0]);
		close(sas_idomar[1]);
		exit(0);
	}
	//ide csak a szulo fog eljutni, es megvarjuk amig minden gyerek elhal,utana vegzunk a szulovel is
	pid_t wpid;
	int status = 0;
	while ((wpid = wait(&status)) > 0);
	exit(0);
}

//-------------JELENTKEZO MUVELETEK---------------//
void nev_beallit(struct Jelentkezo* ember){
	printf("Add meg a neved : ");
	beolvas_szoveg(ember->nev);
}

void cim_beallit(struct Jelentkezo* ember){
	printf("Add meg a cimed : ");
	beolvas_szoveg(ember->cim);
}

void email_beallit(struct Jelentkezo* ember){
	printf("Add meg az email cimed : ");
	beolvas_szoveg(ember->email);
}

void tel_beallit(struct Jelentkezo* ember){
	printf("Add meg a telefonszamod : ");
	beolvas_szoveg(ember->tel);
}

void datum_beallit(struct Jelentkezo* ember){
	time_t timer;
    char buffer[SIZE];
    struct tm* tm_info;
    time(&timer);
    tm_info = localtime(&timer);
    strftime(ember->datum, SIZE, "%Y,%m,%d, %H,%M,%S", tm_info);
}
void surgos_beallit(struct Jelentkezo* ember){
	printf("Surgos az eset? (igen/nem): ");
	beolvas_szoveg(ember->surgos);
}

void jelentkezo_init(struct Jelentkezo* ember){
	nev_beallit(ember);
	cim_beallit(ember);
	email_beallit(ember);
	tel_beallit(ember);
	datum_beallit(ember);
	surgos_beallit(ember);
}

void adatok_kiirasa(struct Jelentkezo* ember){
	printf("Nev: %s \n", ember->nev);
	printf("Cim: %s \n", ember->cim);
	printf("Email: %s \n", ember->email);
	printf("Telefonszam: %s \n", ember->tel);
	printf("Datum: %s \n", ember->datum);
	printf("Surgos: %s \n", ember->surgos);
}

void jelentkezok_kiirasa(struct Container* c){
	int db = c->elemszam;
	int i;
	for(i=0; i < db ; i++){
		printf("%s \n", c->emberek[i].nev);
	}
}

void surgos_jelentkezok_kiirasa(struct Container* c){
	int db = c->elemszam;
	int i;
	for(i=0; i<db ; i++){
		if(strcmp(c->emberek[i].surgos, "igen")==0){
			printf("%s \n", c->emberek[i].nev);
		}
	}
}

void nem_surgos_jelentkezok_kiirasa(struct Container* c){
	int db = c->elemszam;
	int i;
	for(i=0; i<db ; i++){
		if(strcmp(c->emberek[i].surgos, "nem")==0){
			printf("%s \n", c->emberek[i].nev);
		}
	}
}

//-----------CONTAINER MUVELETEK--------------//
void container_init(struct Container* c){
	c->elemszam=0;
}

void uj_jelentkezo(struct Container* c){
	int db = c->elemszam;
	if(db < MAX){
		struct Jelentkezo uj;
		jelentkezo_init(&uj);
		c->emberek[db]=uj;
		c->elemszam=c->elemszam+1;
		
		teljes_nevsor(c);
		surgos_nevsor(c);
		nem_surgos_nevsor(c);
		iras_fajlba_osszes_adat(c);
		
		return;
	}
	printf("A container megtelt! \n");
}

void container_kiirasa(struct Container* c){
	int db = c->elemszam;
	int i;
	
	for(i=0; i < db; i++){
		adatok_kiirasa(&(c->emberek[i]));
	}
	printf("\n");
}


void jelentkezo_keres_es_modosit(struct Container* c, char nev[SIZE]){
	int i=0;
	int db = c->elemszam;
	int talalt=0;
	int result;
	
	while(talalt == 0 && i < db){
		result = strcmp(c->emberek[i].nev, nev);

		if(result==0){
			modosit(&(c->emberek[i]));
			talalt=1;
		}
		i=i+1;
	}
	
	if(talalt == 0){
		printf("Ilyen nev nincs a listan! \n");
	}
	
	teljes_nevsor(c);
	surgos_nevsor(c);
	nem_surgos_nevsor(c);
	iras_fajlba_osszes_adat(c);
}

void modosit(struct Jelentkezo* ember){
	printf("Nev modositasa -> 1 \n");
	printf("Cim modositasa -> 2 \n");
	printf("Email modositasa -> 3 \n");
	printf("Telefonszam modositasa -> 4 \n");
	printf("Surgosseg modositasa -> 5 \n");
	
	int szam;
	szam = beolvas_szam();
	switch(szam){
		case 1 : 
			nev_beallit(ember);
			break;
		case 2:
			cim_beallit(ember);
			break;
		case 3:
			email_beallit(ember);
			break;
		case 4:
			tel_beallit(ember);
			break;
		case 5:
			surgos_beallit(ember);
			break;
		default:
			printf("Csak a megadottak kozott valaszthatsz!");
	}
}

void jelentkezo_keres_es_torol(struct Container* c, char nev[SIZE]){
	int i=0;
	int db = c->elemszam;
	int talalt=0;
	int result;
	
	while(talalt == 0 && i < db){
		result = strcmp(c->emberek[i].nev, nev);

		if(result == 0){
			helyreallit(c, nev);
			talalt=1;
		}
		i=i+1;
	}
	
	if(talalt == 0){
		printf("Ilyen nev nincs a listan! \n");
	}
	
	teljes_nevsor(c);
	surgos_nevsor(c);
	nem_surgos_nevsor(c);
	iras_fajlba_osszes_adat(c);
}

void helyreallit(struct Container* c, char nev[SIZE]){
	int i,j;
	j=0;
	
	for(i=0; i < c->elemszam; i++){
		
		if(strcmp(c->emberek[i].nev, nev) != 0){
			c->emberek[j]=c->emberek[i];
			j=j+1;
		}
	}
	c->elemszam = c->elemszam-1;
}

void teljes_nevsor(struct Container* c){
	int g;
	g=open("teljes_nevsor", O_WRONLY|O_TRUNC|S_IRUSR|S_IWUSR);
	if(g < 0){
		printf("Hiba...Kilepes... \n");
		exit(1);
	}
	
	int n = c->elemszam;
	int i, j;
	char newline='\n';
	
	for(i=0; i < n; i++){
		write(g,c->emberek[i].nev,strlen(c->emberek[i].nev));
		write(g,&newline, sizeof(newline));
	}
	close(g);
}

void surgos_nevsor(struct Container* c){
	int g;
	g=open("surgos_nevsor", O_WRONLY|O_TRUNC|S_IRUSR|S_IWUSR);
	if(g < 0){
		printf("Hiba...Kilepes... \n");
		exit(1);
	}
	
	int n = c->elemszam;
	int i, j, result;
	char newline='\n';
	
	for(i=0; i < n; i++){
		
		result = strcmp(c->emberek[i].surgos,"igen");
		if(result == 0){
			write(g,c->emberek[i].nev,strlen(c->emberek[i].nev));
			write(g,&newline, sizeof(newline));
		}
	}
	close(g);
}

void nem_surgos_nevsor(struct Container* c){
	int g;
	g=open("nem_surgos_nevsor", O_WRONLY|O_TRUNC|S_IRUSR|S_IWUSR);
	if(g < 0){
		printf("Hiba a megnyitasnal... Kilepes... \n");
		exit(1);
	}
	
	int n = c->elemszam;
	int i, result;
	char newline='\n';
	
	for(i=0; i < n; i++){
		result = strcmp(c->emberek[i].surgos,"nem");
		if(result == 0){
			write(g,c->emberek[i].nev,strlen(c->emberek[i].nev));
			write(g,&newline, sizeof(newline));
		}
	}
	close(g);
}

void iras_fajlba_osszes_adat(struct Container* c){
	int g;
	g=open("osszes_adat", O_WRONLY|O_TRUNC|S_IRUSR|S_IWUSR);
	if(g < 0){
		printf("Hiba a megnyitasnal... Kilepes... \n");
		exit(1);
	}
	
	int db = c->elemszam;
	int i;

	char newline='\n';
	for(i=0; i < db ; i++){

		write(g,c->emberek[i].nev,strlen(c->emberek[i].nev));
		write(g,&newline, sizeof(newline));
		
		write(g,c->emberek[i].cim,strlen(c->emberek[i].cim));
		write(g,&newline, sizeof(newline));
		
		write(g,c->emberek[i].email,strlen(c->emberek[i].email));
		write(g,&newline, sizeof(newline));
		
		write(g,c->emberek[i].tel,strlen(c->emberek[i].tel));
		write(g,&newline, sizeof(newline));
		
		write(g,c->emberek[i].datum,strlen(c->emberek[i].datum));
		write(g,&newline, sizeof(newline));
		
		write(g,c->emberek[i].surgos,strlen(c->emberek[i].surgos));
		write(g,&newline, sizeof(newline));
	}
	close(g);
}

//--------FELTOLTES-----------------//
void feltoltes(struct Container* c){
	printf("\n");
	printf("NE-SAS-OLD KFT ADATBESZERZES \n");
	printf("\n");
	printf("\n");
	
	int vege=0;
	int szam;
	char nev[SIZE];
	container_init(c);
	
	do{
		printf("Valaszd ki mit szeretnel tesztelni! \n");
		printf("Uj jelentkezo letrehozasa -> 1 \n");
		printf("Egy jelentkezo modositasa -> 2 \n");
		printf("Egy jelentkezo torlese -> 3 \n");
		printf("Teljes nevsor kiirasa -> 4 \n");
		printf("Surgos nevsor kiirasa -> 5 \n");
		printf("Nem surgos nevsor kiirasa -> 6 \n");
		printf("Az osszes adat kiirasa -> 7 \n");
		printf("NE-SAS-OLD KFT futtatasa -> 8 \n");
		printf("Vegleges kilepes a programbol -> 0");
		printf("\n");
		printf("\n");
		printf("\n");
		printf("Add meg a megfelelo szamot a teszteleshez: ");
		szam = beolvas_szam();
		
		switch(szam){
			case 1:
				uj_jelentkezo(c);
				printf("\n");
				break;
			case 2:
				jelentkezok_kiirasa(c);
				printf("Add meg a nevet,akit modositani akarsz: ");
				beolvas_szoveg(nev);
				jelentkezo_keres_es_modosit(c, nev);
				printf("\n");
				break;
			case 3:
				jelentkezok_kiirasa(c);
				printf("Add meg a nevet,akit torolni akarsz: ");
				beolvas_szoveg(nev);
				jelentkezo_keres_es_torol(c, nev);
				printf("\n");
				break;
			case 4:
				printf("--------- \n");
				printf("TELJES NEVSOR \n");
				//teljes_nevsor(&c);
				fajbol_nevsor_beolvasas("teljes_nevsor");
				printf("--------- \n");
				break;
			case 5:
				printf("--------- \n");
				printf("SURGOS NEVSOR \n");
				//surgos_jelentkezok_kiirasa(&c);
				fajbol_nevsor_beolvasas("surgos_nevsor");
				printf("--------- \n");
				break;
			case 6:
				printf("--------- \n");
				printf("NEM SURGOS NEVSOR \n");
				//nem_surgos_jelentkezok_kiirasa(&c);
				fajbol_nevsor_beolvasas("nem_surgos_nevsor");
				printf("--------- \n");
				break;
			case 7:
				printf("--------- \n");
				printf("AZ OSSZES UGYFEL ADATAI \n");
				//container_kiirasa(&c);
				fajbol_nevsor_beolvasas("osszes_adat");
				printf("--------- \n");
				break;
			case 0:
				printf("--------- \n");
				printf("KILEPES A PROGRAMBOL \n");
				exit(0);
			case 8:
				vege=1;
				break;
			default: 
				printf("Csak a megadottak kozott valaszthatsz! \n");
		}
		
	}while(vege != 1);
}

void rendezes_surgosseg_szerint(struct Container* c){
	struct Jelentkezo surgos_tomb[10];
	int db1=0;
	
	struct Jelentkezo nem_surgos_tomb[10];
	int db2=0;
	
	int hossz = c->elemszam;

	int i;
	for(i=0; i < hossz; i++){
		if(strcmp(c->emberek[i].surgos, "igen")==0){
			surgos_tomb[db1] = c->emberek[i];
			db1=db1+1;
		}else if(strcmp(c->emberek[i].surgos, "nem")==0){
			nem_surgos_tomb[db2] = c->emberek[i];
			db2=db2+1;
		}
	}
	
	int n;
	for(n=0; n < db1; n++){
		c->emberek[n]=surgos_tomb[n];
	}
	
	int m;
	for(m=0; m < db2; m++){
		c->emberek[m+n]=nem_surgos_tomb[m];
	}
}
//SZAMLAZAS, NEV-AR eloallitasa fajlba
void arazas(){
	printf("1 sec -> 100 Ft \n");
	printf("2 sec -> 200 Ft \n");
	printf("3 sec -> 300 Ft \n");
	printf("4 sec -> 400 Ft \n");
	printf("5 sec -> 500 Ft \n");
	printf("\n");
}
void szamlazas(int idok[], struct Container* c){
	int g;
	g=open("szamla", O_WRONLY|O_TRUNC|S_IRUSR|S_IWUSR);
	if(g < 0){
		printf("Hiba a megnyitasnal... Kilepes... \n");
		exit(1);
	}
	
	int db = c->elemszam;
	int i;

	char newline='\n';
	char vesszo[]=",";
	int szam;
	
	char szoveg[3];
	char sec[4];
	char zero[]="\0";
	for(i=0; i < db ; i++){
		write(g,c->emberek[i].nev,strlen(c->emberek[i].nev));
		write(g, vesszo, strlen(vesszo));
		
		szam=idok[i];

		switch(szam){
			case 1:
				strcpy(sec, "1sec");
				write(g, sec, sizeof(sec));
				write(g, vesszo, strlen(vesszo));
				strcpy(szoveg, "100");
				write(g, szoveg, sizeof(szoveg));
				break;
			case 2:
				strcpy(sec, "2sec");
				write(g, sec, sizeof(sec));
				write(g, vesszo, strlen(vesszo));
				strcpy(szoveg, "200");
				write(g, szoveg, sizeof(szoveg));
				break;
			case 3:
				strcpy(sec, "3sec");
				write(g, sec, sizeof(sec));
				write(g, vesszo, strlen(vesszo));
				strcpy(szoveg, "300");
				write(g, szoveg, sizeof(szoveg));
				break;
			case 4:
				strcpy(sec, "4sec");
				write(g, sec, sizeof(sec));
				write(g, vesszo, strlen(vesszo));
				strcpy(szoveg, "400");
				write(g, szoveg, sizeof(szoveg));
				break;
			case 5:
				strcpy(sec, "5sec");
				write(g, sec, sizeof(sec));
				write(g, vesszo, strlen(vesszo));
				strcpy(szoveg, "500");
				write(g, szoveg, sizeof(szoveg));
				break;
		}

		write(g,&newline, sizeof(newline));
	}
	close(g);
}

void szamla_kiiras(char filename[SIZE]){
	int g;
	g=open(filename,O_RDONLY);
	if(g < 0){
		printf("Hiba a megnyitasnal... Kilepes... \n");
		exit(1);
	}
	
	char cc;
	while(read(g, &cc, sizeof(cc))){
		printf("%c", cc);
	}
	close(g);
}

//--------BEOLVAS:SZOVEG ES SZAM------------------//
void beolvas_szoveg(char szoveg[SIZE]){
	fgets(szoveg, SIZE, stdin);
	szoveg[strlen(szoveg)-1] = 0;
	//az strlen fuggveny nem nezi a 0-at a string vegen
	//a stringet beolvassuk a tomb-be, majd az utolso
	//elem utan beszurunk egy 0-at, hogy tudjuk hogy ott van a vege
}

int beolvas_szam(){
	int szam;
	scanf("%d", &szam);
	getc(stdin);
	
	return szam;
}

void fajbol_nevsor_beolvasas(char filename[SIZE]){
	int g;
	g=open(filename,O_RDONLY);
	if (g<0){
		printf("Hiba...Kilepes... \n");
		exit(1);
	}
	
	char cc;
	while(read(g, &cc, sizeof(cc))){
		printf("%c", cc);
	}
	close(g);
}
