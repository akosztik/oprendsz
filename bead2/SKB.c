#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>


struct Customer{
        char name[30];
        char email[30];
        char phone[12];
        int tour;
        int guests;
        struct tm* reservationTime;
		double price;
};
void clear(){
	while (getchar()!='\n');
}
int getDataInt(char* question, int startofMenu, int endofMenu)
{
	int ans;
	do{
		printf(question);
		scanf("%d", &ans);
		clear();
		//printf("your answer was: %d", ans);
	}while((ans<startofMenu) || (ans>endofMenu));
	return ans;
}
void getDataString(char* question, int max, char* var)
{	
	//char* tmp=malloc(sizeof(char)* max);
	printf(question);
	fgets (var, max, stdin);
	//if ((strlen(tmp)>0) && (tmp[strlen (tmp) - 1] == '\n'))
	//	tmp[strlen (tmp) - 1] = '\0';
	//printf("your answer was: %s", var);

}

struct Customer getCustomerData(){
	struct Customer tempCustomer;
	int var;
	getDataString("\nEnter customer name( max 30 digits ): ", 30, tempCustomer.name);
	getDataString("Enter email( max 30 digits ): ", 30,tempCustomer.email);
	getDataString("Enter phone number ( max 12 digits ): ", 12,tempCustomer.phone);
    char* tourChoice= "________________\nChoose tour:\n________________\n1 - Add Parlament\n2 - Hosok tere\n3 - Var\nEnter a selection: ";
	tempCustomer.tour=getDataInt(tourChoice, 1, 3);
	var=getDataInt("\nEnter number of guests, yourself included( max 100 ): ", 1, 100);
	tempCustomer.guests=var;
	time_t now = time(&now);
	tempCustomer.reservationTime=gmtime(&now);
	printf ( "\nCurrent time and date: %s",asctime(tempCustomer.reservationTime) );
	tempCustomer.price=(var-1)*0.85*3000+3000;
	printf ( "\nprice: %g",tempCustomer.price );
	return tempCustomer;
}
bool exists(struct Customer customer)
{
	bool exists=false;
	int f=open("customers", O_RDONLY|O_CREAT);	
	if (f < 0) {
  		perror("Nem lehet megnyitni a fajlt!\n");
  		exit(1);
  	}
	struct Customer registeredCustomer;
    	while(sizeof(registeredCustomer) == read(f, &registeredCustomer, sizeof(registeredCustomer)) && !exists){
		printf ( "\nreg mail: %s",registeredCustomer.email );
		printf ( "\nreg tour: %d",registeredCustomer.tour );
		if (strcmp(registeredCustomer.email,customer.email) == 0 && registeredCustomer.tour==customer.tour){
			printf ( "\nEXISTS" );
			exists=true;
		}
	}
	close(f);		
	return exists;
}
struct Customer getDataCheckingUser(int selector){
	struct Customer tempCustomer;
	
	getDataString("\nEnter email: ", 30,tempCustomer.email);
	char* tourChoice= "________________\nChoose tour:\n________________\n1 - Add Parlament\n2 - Hosok tere\n3 - Var\nEnter a selection: ";
	tempCustomer.tour=getDataInt(tourChoice, 1, 3);
	time_t now = time(&now);
	struct tm* time=gmtime(&now);
	printf("\n................checking customer data......................\n");
	if (exists(tempCustomer)){
		if (selector==0){
			int var;
			getDataString("\nEnter customer name: ", 30, tempCustomer.name);
			getDataString("Enter phone number (12 digits ): ", 12,tempCustomer.phone);
			var=getDataInt("Enter number of guests, yourself included: ", 1, 100);
			tempCustomer.guests=var;
			tempCustomer.price=(var-1)*0.85*3000+3000;
			tempCustomer.reservationTime=time;
		}
		
	}else{
		printf("\nThe customer was not registered for this tour.\n");
		tempCustomer.tour=0;
	}
	return tempCustomer;
}

int add(struct Customer customer){
	int addedGuests=0;
    int f = open("customers", O_RDWR|O_CREAT|O_TRUNC);
    if (f < 0) {
  	perror("Nem lehet megnyitni a fajlt!\n");
  	exit(1);
    }
	write(f, &customer,sizeof(struct Customer));
    printf("\nThe registration was stored for the given tour.\n");
    close(f);
	addedGuests=customer.guests;
	return addedGuests;

}

int delete(struct Customer customer){
	int deletedGuests=0;
	int readable = open("customers", O_RDWR|O_CREAT);
    	if (readable < 0) {
  		perror("Nem lehet megnyitni a fajlt!\n");
  		exit(1);
    	}
	int out  = open("modif", O_RDWR|O_CREAT|O_TRUNC);
   	if (out  < 0) {
  		perror("Nem lehet megnyitni a fajlt!\n");
  		exit(1);
    	}	
	struct Customer readInCust;
	while(sizeof(readInCust) == read(readable, &readInCust, sizeof(readInCust))){
		if (strcmp(readInCust.email,customer.email) != 0 || readInCust.tour != customer.tour){
			write(out,&readInCust, sizeof(struct Customer));
			printf ( "\nsavingtotmp: %s for %d . tour",readInCust.email,readInCust.tour);
		
		 }else{
			 printf ( "\ndeleting: %s 's registration for %d . tour",readInCust.name,readInCust.tour);
			 deletedGuests=readInCust.guests;
		 }
	 }
	 close(readable);
	 close(out);
	 remove("customers");
	 rename("modif","customers");
	 sleep(1);
	 chmod("customers", S_IRWXU|S_IRWXG|S_IROTH|S_IWOTH);
	 return deletedGuests;
}

void  addNewCustomer(struct Customer customer,int* diff, int* route){
	printf("\n................checking customer data......................\n");
	
	if (!exists(customer)){
		printf ( "\ncustomer is not registered for this tour" );
		int differencToAdd=add(customer);
		*diff=differencToAdd;
		*route=customer.tour;
		printf ( "\nnew customers for the tour %d : %d guests ",customer.tour,differencToAdd);
	
	}else{
		printf("\nThe customer is already registered for this tour.\n");
	}
	
}

void modifyCustomer(struct Customer customer,int *diff, int *route){
	if (customer.tour!=0){
		int differencToAdd=-delete(customer)+add(customer);
		*diff=differencToAdd;
		*route=customer.tour;	
	}
}

void deleteCustomer(struct Customer customer,int *diff, int *route){
	if (customer.tour!=0){
		int differencToAdd=delete(customer);
		*diff=-differencToAdd;
		*route=customer.tour;
		printf ( "\ndelete customers for the tour %d : %d ",customer.tour,differencToAdd);
	}
}

void list(int mode){
	int f=open("customers", O_RDWR|O_CREAT);	
	if (f < 0) {
  		perror("Nem lehet megnyitni a fajlt!\n");
  		exit(1);
  	}
	struct Customer readInCust;
	while(sizeof(readInCust) == read(f, &readInCust, sizeof(readInCust))){
		 if (mode!=0 && readInCust.tour == mode){
			printf ( "%s with %d guests, contact: %s ",readInCust.name,readInCust.guests, readInCust.email );
		 }else{
			printf ( "%s for %d . tour with %d number of guests registered ,contact: %d ",readInCust.name, readInCust.tour,readInCust.guests ,readInCust.phone );
		 }
	 }
	 close(f);
}
	

void getCustomerCountOfATour(int mode, int *count){
	int f=open("customers", O_RDWR|O_CREAT);	
	if (f < 0) {
  		perror("Nem lehet megnyitni a fajlt!\n");
  		exit(1);
  	}
	int index=0;
	struct Customer readInCust;
	while(sizeof(readInCust) == read(f, &readInCust, sizeof(readInCust))){
		 if (readInCust.tour == mode){
			index++;
		 }
	}
	*count=index;
	close(f);

}
struct Customer getCustomersOfATour(int f,int mode){
		
	struct Customer readInCust;
	while(sizeof(readInCust) == read(f, &readInCust, sizeof(readInCust))){
		if (readInCust.tour == mode){
			printf("*** getCustomersOfATour id:%s is ready!\n", readInCust.name);
			return readInCust;
		 }
	}
	return readInCust;

	
}

void listTour(int tour){
	printf("\n********************registered for tournumber: %d \n",tour);
	list(tour);
}

void listAllCustomers(){
	printf("\n********************registered customers: \n");
	list(0);
}
void startSK(struct Customer u[],int routeToDrive,int count) {
  int sum=0;
  int i;
  for (i=0;i<count;i++){
	  sum+=u[i].guests;
  }
  printf("\n****osszesutas: %d \n",sum);

  int parentToChild[2];       // [0]parent------------child[1]
  int childToParent[2];       // [0]child------------parent[1]
  if (pipe(parentToChild) < 0 || pipe(childToParent) < 0) {
    perror("Pipe megnyitasi hiba!");
    exit(EXIT_FAILURE);
  }

  pid_t pid = fork();   //lemasolodik a childfolyamatba minden, ami felette van
  if (pid < 0) {
    perror("Forkolasi hiba!");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    //child process
    close(parentToChild[1]);    
    close(childToParent[0]);
    printf("*** SBD id:%d is ready!\n", getpid());
    sleep(1);
    kill(getppid(), SIGUSR1); //sor vetelezese
    sleep(1);
    printf("*** SBD id:%d got the data:\n", getpid());
    printf("\nTourList:");
    for(i = 0; i < count; i++) {
	  struct Customer c;
      read(parentToChild[0], &c, sizeof(struct Customer));
      printf ( "\n%s with  %d ,contact: %s,route: %d ",c.name, c.guests,c.phone ,c.tour );
    }
    int price;
    read(parentToChild[0], &price, sizeof(int));
    printf("\nprice: %d\n", price);
    int r;
    read(parentToChild[0], &r, sizeof(int));
    close(parentToChild[0]);
    printf("\nTour: %d\n", r);
    int beer = 5 * sum;
    write(childToParent[1], &beer, sizeof(int));
    int income = 3000 * sum;
    write(childToParent[1], &income, sizeof(int));
    close(childToParent[1]);
    printf("\nSBD id:%d sent back the data\n", getpid());
    sleep(1);
    printf("\nBIKETOUR STARTED!\n");
    sleep(1);
    for(i = 0; i < 3; i++) {
      printf("\t\t ...\n");
      sleep(1);
    }
    printf("\nVege a turanak!\n");
    sleep(1);
    kill(getppid(), SIGUSR2); //tura vege
    raise(SIGTERM);
  }
  else {
    //parent process4

    close(parentToChild[0]);    // [0]parent (X)--------child[1]
    close(childToParent[1]);    // [0]child--------(X) parent[1]
    pause();
    int i;
    for(i = 0; i < count; i++) {  
      write(parentToChild[1], &u[i], sizeof(struct Customer));
    }
	
	sleep(1);
    int price = 3000;
    write(parentToChild[1], &price, sizeof(int));
    write(parentToChild[1], &routeToDrive, sizeof(int));
    close(parentToChild[1]);
    printf("\t\tTour data sent to SBD id:%d .\n", pid);
    int beer;
    read(childToParent[0], &beer, sizeof(int));
    int income;
    read(childToParent[0], &income, sizeof(int));
    close(childToParent[0]);
    printf("\t\tSBD id:%d 's tourdata:\n", pid);
    printf("\t\tBeer: %d liter\n", beer);
    printf("\t\tIncome: %d Ft \n", income);
    sleep(1);
    pause();
  }
}

void handler(int sigNumber) { }


int checkCustomerNumbers(int numbersOfEachRoute[3], int max){
		int i;
		//printf("\t\tcustomernumber required %d \n",max);
		int ind=0;
		for (i=0;i<3;i++){
			if (numbersOfEachRoute[i]>max){
				//printf("\t\tcustomernumber of route %d: %d \n",i+1, numbersOfEachRoute[i]);
				max=numbersOfEachRoute[i];
				ind=i+1;
			}
		}
		if (max<=8){return 0;
		}else{ return ind; }
}	
int main(int argc,char** argv)
{
		signal(SIGUSR1, handler);
		signal(SIGUSR2, handler);
	
		printf("******************************\nBeerBike Tours\n******************************");
        int choice;
		int routes[]={0,0,0};
		char* menu="\n__________________________\nChoose from the menu: \n__________________________\n1 - Add new Customer\n2 - Modify Customerdata\n3 - Delete Customer\n4 - List Customers of a tour\n5 - List Customers\n6 - Quit\nEnter a selection: ";
        char* tourChoice= "\n________________\nChoose tour:\n________________\n1 - Add Parlament\n2 - Hosok tere\n3 - Var\nEnter a selection: ";
		choice=getDataInt(menu,1,6);
		int diff=0;
		int route=0;
		while(choice != 6)
        {
                switch (choice) {
                        case 1:
                                addNewCustomer(getCustomerData(),&diff,&route);
								routes[route-1]+=diff;
                                break;
                        case 2:
                                modifyCustomer(getDataCheckingUser(0),&diff,&route);
								routes[route-1]+=diff;
                                break;
                        case 3:
                                deleteCustomer(getDataCheckingUser(1),&diff,&route);
								routes[route-1]+=diff;
                                break;
                        case 4:
                                listTour(getDataInt(tourChoice,1,3));
								break;
								
                        case 5:
                                listAllCustomers();
                                break;
						
                }
				
				int minimumrequired=8;
				printf("\t\tmin: %d  \n", minimumrequired);
				int routeToDrive=checkCustomerNumbers(routes,minimumrequired);
				printf("\t\tRoute: %d , if route is 0 we not reached minimum customer number \n", routeToDrive);
				if (routeToDrive!=0){
						int count;
						getCustomerCountOfATour(routeToDrive, &count);
						struct Customer customersOfATour[count];
						int j;
						int f=open("customers", O_RDWR|O_CREAT);	
							if (f < 0) {
								perror("Nem lehet megnyitni a fajlt!\n");
								exit(1);
							}
						for (j=0;j<count;j++){
							customersOfATour[j]=getCustomersOfATour(f,routeToDrive);
						}
						close(f);
						printf("\n*** %d tura indithato!\n", routeToDrive);
						sleep(1);
						startSK(customersOfATour,routeToDrive,count);
						int i;
						  for(i = 0; i < count; i++) {
							customersOfATour[i];
							deleteCustomer( customersOfATour[i],&diff,&route); 
							routes[route-1]+=diff;
						}
						
				}
				sleep(5);
				choice=getDataInt(menu,1,6);
        }
		
        return 0;
}