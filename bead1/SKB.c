#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


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
	char* tmp=malloc(sizeof(char)* max);
	printf(question);
	fgets (var, max, stdin);
	//if ((strlen(tmp)>0) && (tmp[strlen (tmp) - 1] == '\n'))
	//	tmp[strlen (tmp) - 1] = '\0';
	//printf("your answer was: %s", var);

}

struct Customer* getCustomerData(){
	struct Customer* tempCustomer = malloc(sizeof(struct Customer));
	int var;
	getDataString("\nEnter customer name( max 30 digits ): ", 30, tempCustomer->name);
	getDataString("Enter email( max 30 digits ): ", 30,tempCustomer->email);
	getDataString("Enter phone number ( max 12 digits ): ", 12,tempCustomer->phone);
    char* tourChoice= "________________\nChoose tour:\n________________\n1 - Add Parlament\n2 - Hosok tere\n3 - Var\nEnter a selection: ";
	tempCustomer->tour=getDataInt(tourChoice, 1, 3);
	var=getDataInt("\nEnter number of guests, yourself included( max 100 ): ", 1, 100);
	tempCustomer->guests=var;
	time_t now = time(&now);
	tempCustomer->reservationTime=gmtime(&now);
	printf ( "\nCurrent time and date: %s",asctime(tempCustomer->reservationTime) );
	tempCustomer->price=(var-1)*0.85+1;
	printf ( "\nprice: %g",tempCustomer->price );
	return tempCustomer;
}
bool exists(struct Customer* customer)
{
	bool exists=false;
	if( access( "customers", F_OK ) != -1 ){
	FILE* f=fopen("customers", "r");
	struct Customer* registeredCustomer=malloc(sizeof(struct Customer));
    while(!feof(f) && !exists){
		fread(registeredCustomer, sizeof(struct Customer), 1, f);
		//printf ( "\nreg mail: %s",registeredCustomer->email );
		//printf ( "\nreg tour: %d",registeredCustomer->tour );
		if (strcmp(registeredCustomer->email,customer->email) == 0 && registeredCustomer->tour==customer->tour){
			printf ( "\nEXISTS" );
			exists=true;
		}
	}
	fclose(f);
	free(registeredCustomer);
	}
		
	return exists;
}
struct Customer* getDataCheckingUser(int selector){
	struct Customer* tempCustomer = malloc(sizeof(struct Customer));
	
	getDataString("\nEnter email: ", 30,tempCustomer->email);
	char* tourChoice= "________________\nChoose tour:\n________________\n1 - Add Parlament\n2 - Hosok tere\n3 - Var\nEnter a selection: ";
	tempCustomer->tour=getDataInt(tourChoice, 1, 3);
	time_t now = time(&now);
	struct tm* time=gmtime(&now);
	printf("\n................checking customer data......................\n");
	if (exists(tempCustomer)){
		if (selector==0){
			int var;
			getDataString("\nEnter customer name: ", 30, tempCustomer->name);
			getDataString("Enter phone number (12 digits ): ", 12,tempCustomer->phone);
			var=getDataInt("Enter number of guests, yourself included: ", 1, 100);
			tempCustomer->guests=var;
			tempCustomer->price=(var-1)*0.85+1;
			tempCustomer->reservationTime=time;
		}
		
	}else{
		printf("\nThe customer was not registered for this tour.\n");
		tempCustomer=NULL;
	}
	return tempCustomer;
}

void add(struct Customer* customer){
	FILE* file = fopen("customers", "a");
    fwrite(customer, sizeof(struct Customer), 1, file);
    printf("\nThe registration was stored for the given tour.\n");
    fclose(file);
}

void delete(struct Customer* customer){
	if( access( "customers", F_OK ) != -1 ){
	FILE* readable = fopen("customers", "r");
    FILE* out = fopen("modif", "w");
	struct Customer* readInCust = malloc(sizeof(struct Customer));
	fread(readInCust, sizeof(struct Customer), 1, readable);
	 while (!feof(readable)){
		 if (strcmp(readInCust->email,customer->email) != 0 || readInCust->tour != customer->tour){
			fwrite(readInCust, sizeof(struct Customer), 1, out);
			//printf ( "\nsavingtotmp: %s for %d . tour",readInCust->email,readInCust->tour);
		 }else{
			 printf ( "\ndeleting: %s 's registration for %d . tour",readInCust->name,readInCust->tour);
		 }
		 fread(readInCust, sizeof(struct Customer), 1, readable);
	 }
	 fclose(readable);
	 fclose(out);
	 free(readInCust);
	}
	 remove("customers");
	 rename("modif","customers");
}

void addNewCustomer(struct Customer* customer){
	printf("\n................checking customer data......................\n");
	
	if (!exists(customer)){
		printf ( "\ncustomer is not registered for this tour" );
		add(customer);
	}else{
		printf("\nThe customer is already registered for this tour.\n");
	}
	free(customer);
}

void modifyCustomer(struct Customer* customer){
	
	if (customer != NULL){
		delete(customer);
		add(customer);
	}
	free(customer);
	
}

void deleteCustomer(struct Customer* customer){
	if (customer != NULL){
		delete(customer);
	}
	free(customer);
}
void list(int mode){
	if( access( "customers", F_OK ) != -1 ){
	FILE* f = fopen("customers", "r");
	struct Customer* readInCust = malloc(sizeof(struct Customer));
	fread(readInCust, sizeof(struct Customer), 1, f);
	 while (!feof(f)){
		 if (mode!=0 && readInCust->tour == mode){
			printf ( "%s with %d guests, contact: %s ",readInCust->name,readInCust->guests, readInCust->email );
		 }else{
			printf ( "%s for %d . tour with %d number of guests registered ,contact: %d ",readInCust->name, readInCust->tour,readInCust->guests ,readInCust->phone );
		 }
		 fread(readInCust, sizeof(struct Customer), 1, f);
	 }
	 fclose(f);
	 free(readInCust);
	}
}
void listTour(int tour){
	printf("\n********************registered for tournumber: %d \n",tour);
	list(tour);

	
}

void listAllCustomers(){
	printf("\n********************registered customers: \n");
	list(0);
}


int main(int argc,char** argv)
{
		printf("******************************\nBeerBike Tours\n******************************");
        int choice;
		char* menu="\n__________________________\nChoose from the menu: \n__________________________\n1 - Add new Customer\n2 - Modify Customerdata\n3 - Delete Customer\n4 - List Customers of a tour\n5 - List Customers\n6 - Quit\nEnter a selection: ";
        char* tourChoice= "\n________________\nChoose tour:\n________________\n1 - Add Parlament\n2 - Hosok tere\n3 - Var\nEnter a selection: ";
		choice=getDataInt(menu,1,6);
		while(choice != 6)
        {
                switch (choice) {
                        case 1:
                                addNewCustomer(getCustomerData());
                                break;
                        case 2:
                                modifyCustomer(getDataCheckingUser(0));
                                break;
                        case 3:
                                deleteCustomer(getDataCheckingUser(1));
                                break;
                        case 4:
                                listTour(getDataInt(tourChoice,1,3));
                                break;
                        case 5:
                                listAllCustomers();
                                break;
						case 6: 
								startSK();
								break;
                }
				choice=getDataInt(menu,1,6);
        }
		free(choice)
        return 0;
}