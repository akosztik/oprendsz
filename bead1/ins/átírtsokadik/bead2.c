#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <signal.h>


typedef struct{ 
  char* timestamp;
  char* name;
  char* address;
  char* email;
  char* phone;
  int urgent;	
}Contract;

Contract createContract(char* name,char* address,char*email,char*phone,int urgent){
  char storedtime[20];
  time_t now = time(0);
  strftime(storedtime,20,"%Y-%m-%d %H:%M:%S",localtime(&now));  
  Contract c;
  c.timestamp = malloc(sizeof(char*));
  strcpy(c.timestamp,storedtime);
  c.name = malloc(sizeof(char*));
  strcpy(c.name,name);
  c.address = malloc(sizeof(char*));
  strcpy(c.address,address);
  c.email = malloc(sizeof(char*));
  strcpy(c.email,email);
  c.phone =  malloc(sizeof(char*));
  strcpy(c.phone,phone);
  c.urgent = urgent;
  return c; 
}

typedef struct{
  Contract* array;
  int used;
  int size;
  int urgentline;
}ContractArray;

void initCArray(ContractArray *a,int initialSize){

  a->array = (Contract*)malloc(initialSize * sizeof(Contract));
  a->used = 0;
  a->urgentline = 0;
  a->size = initialSize;

}

void insertCArray(ContractArray *a,Contract element){
  if(a->used == a->size){
    a->size *= 2;
    a->array = (Contract*)realloc(a->array, a-> size * sizeof(Contract));
  }
  if(element.urgent){
    int i;
    for(i = a->used;i > a->urgentline;i--){
      a->array[i] = a->array[i-1];
    }
    a->array[a->urgentline] = element;
    a->urgentline++;
  }else{
    a->array[a->used] = element;
  }
  a->used++;
}

void freeCArray(ContractArray *a){
  free(a->array);
  a->array = NULL;
  a->used = 0;
  a->urgentline = 0;
  a->size = 0;
}



void loadFromFile(ContractArray *ca, char* fname){
  char rawline[256];
  FILE *input = fopen(fname,"r");
  if(input == NULL){
    printf("Error, file not found or not readable!\n");
  }else{
    while(fgets(rawline,256,input) != NULL){
      char *token, *line;
      char *data[8];
      int i = 0;
      line = strtok(rawline,"\n");
      token = strtok(line,";"); 
      while(token!=NULL){
        data[i] = token;
        i++;
        token=strtok(NULL,";");
      }
      Contract c = createContract(data[1],data[2],data[3],data[4],atoi(data[5]));
      strcpy(c.timestamp,data[0]);
      insertCArray(ca,c);
    }
    fclose(input);
  }
}

void saveToFile(ContractArray *ca, char* fname){
  FILE *output = fopen(fname,"w");
  int i;
  for(i = 0;i<ca->used;i++){
    fprintf(output,"%s;%s;%s;%s;%s;%i\n",
      ca->array[i].timestamp,ca->array[i].name,ca->array[i].address,ca->array[i].email,ca->array[i].phone,ca->array[i].urgent);
  }
  fclose(output);
}

void printContract(Contract c){
  char* urgent = "No";
  if(c.urgent > 0)
    urgent = "Yes";
  printf("Time: %s, Name : %s, Addres : %s, E-mail : %s, Phone number : %s, Urgent : %s\n",c.timestamp,c.name,c.address,c.email,c.phone,urgent);
}

void listAllContracts(ContractArray * ca){
  printf("All contracts: \n");
  int i;
  for(i = 0 ; i < ca->used;i++){
    printContract(ca->array[i]);
  }
}

void listContractsByUrgency(ContractArray *ca, int urgent){
  printf("Contracts of selected urgency: \n");
  int i;
  if(urgent){
    for(i = 0 ; i < ca->urgentline;i++)
      printContract(ca->array[i]);
  }else{
    for(i = ca->urgentline;i< ca->used;i++)
      printContract(ca->array[i]);
  }
}

void listContractsByCustomer(ContractArray * ca,char *name){
  printf("Contracts of selected customer: \n");
  int i;
  for(i = 0 ; i < ca->used;i++){
    if(!strcmp(ca->array[i].name,name))
      printContract(ca->array[i]);
    
  }
}

void inputContract(ContractArray *ca){
  static char name[41],address[81],email[41],phone[21],urgentstr[10];
  int urgent = 0;
  printf("Enter your name (maximum of 40 characters): ");
  fgets(name,41,stdin);
  printf("Enter your address (maximum of 80 characters): ");
  fgets(address,81,stdin);
  printf("Enter your e-mail (maximum of 40 characters): ");
  fgets(email,41,stdin);
  printf("Enter your phone number (maximum of 20 characters): ");
  fgets(phone,21,stdin);
  printf("Is your contract urgent? (Type Y for yes, and N for No) :");
  fgets(urgentstr,10,stdin);
  if(strcmp(name,"\n") && strcmp(address,"\n") && strcmp(email,"\n") && strcmp(phone,"\n") && strcmp(urgentstr,"\n")){
    if(!strcmp(strtok(urgentstr,"\n"),"Y"))
    urgent = 1;
    Contract c = createContract(strtok(name,"\n"),strtok(address,"\n"),strtok(email,"\n"),strtok(phone,"\n"),urgent);
    insertCArray( ca,c);
    printf("Contract created with the following specifications : \n");
    printContract(c);
  }else
    printf("Couldn't create contract, some information is missing. Please try again.\n");
}

void listContracts(ContractArray *ca){
  char selector[10],userinput[41];  
  printf("How do you want to list the current contracts?\n");
  printf("'A' : List all contracts, 'N' : List contracts of selected customer, 'U' : List contracts of selected urgency\n:: ");
  fgets(selector,10,stdin);
  if(!strcmp(selector,"N\n")){
    printf("Enter the customers name you are searching for :"); 
    fgets(userinput,41,stdin);
    listContractsByCustomer(ca,strtok(userinput,"\n"));
  }else if(!strcmp(selector,"U\n")){
    printf("Do you want to see urgent or non-urgent contracts?\n'U' : urgent, 'N' : non-urgent\n:: ");
    fgets(userinput,41,stdin);
    if(!strcmp(userinput,"U\n")){
      listContractsByUrgency(ca,1);
    }else if(!strcmp(userinput,"N\n")){
      listContractsByUrgency(ca,0);
    }else
      printf("Invalid input\n");
  }else if(!strcmp(selector,"A\n")){
    listAllContracts(ca);
  }else
    printf("Invalid input\n");
}

int removeContract(ContractArray *ca, char timestamp[20]){
  int i,j,result;
  result = 0;
  for(i = 0; i<ca->used;i++){
    if(!strcmp(timestamp,ca->array[i].timestamp)){
      result = 1;
      for(j = i;j<ca->used-1;j++)
        ca->array[j] = ca->array[j+1];
        ca->used--;
    }
  }
  return result;
}

void modifyContract(ContractArray *ca){
  printf("Enter submission date of the contract you want to modify (the format is 'yyyy-mm-dd hh-mm-ss') :");
  char date[20];
  fgets(date,20,stdin);
  if(strtok(date,"\n")){
    int result = removeContract(ca,strtok(date,"\n"));
    if(result){
      printf("Modifiable contract found, enter new data\n");
      getchar();
      inputContract(ca);
    }else{
      printf("Contract not found\n");
    }
  }else
    printf("Invalid input\n");
 
}

void deleteContract(ContractArray *ca){
  printf("Enter submission date of the contract you want to delete (the format is 'yyyy-mm-dd hh:mm:ss') :");
  char date[20];
  fgets(date,20,stdin);
  if(strtok(date,"\n")){
    int result = removeContract(ca,strtok(date,"\n"));
    if(result){
      printf("Contract successfully removed\n");
      getchar();
    }else{
      printf("Contract not found\n");
    }
  }else
    printf("Invalid input\n");
}

void makeReceipt(Contract c, int duration){
  int cost;
  char fname [100],storedtime[20];
  time_t now = time(0);
  strftime(storedtime,20,"%Y-%m-%d %H:%M:%S",localtime(&now));  
  strcpy(fname,"./receipts/");
  strcat(fname, c.name);
  strcat(fname, "-");
  strcat(fname,storedtime);
  FILE *output = fopen(fname,"w");
  fprintf(output,"Name: %s\nAddress: %s\nE-mail: %s\nPhone number: %s\n",c.name,c.address,c.email,c.phone);
  fprintf(output,"Duration of drone removal : %i\n",duration);
  fprintf(output,"Date of drone removal: %s\n",storedtime);
  cost = duration*500 + c.urgent*1000;
  fprintf(output,"Total cost : %i\n",cost);
  fprintf(output,"--Base fee (500 * time needed) : %i\n",duration*500);
  fprintf(output,"--Urgency fee : %i\n",c.urgent*1000);
  fclose(output);
}

void handler(){
  printf("Eagle tamer ready to work.\n");
}
void eagleStuff(){}
void tamerStuff(){}

void executeContracts(ContractArray *ca){
  time_t t;
  pid_t tamer,eagle;
  srand((unsigned) time(&t));
  char status;
  int parentpipe[2], childpipe[2], eaglepipe[2],random;
  pipe(parentpipe); 
  pipe(childpipe);
  pipe(eaglepipe);

  tamer = fork();
  if(tamer == 0){ //Tamer
    int eagletime;
    signal(SIGUSR2,tamerStuff);
    sleep(rand() % 2 +1);
    kill(getppid(),SIGUSR1);
    char recv[81];
    read(parentpipe[0],recv,sizeof(recv));
    while(strcmp(recv,"end")){
      printf("Eagle tamer is currently at : %s\n",recv);
      write(eaglepipe[1],"a",sizeof(char));
      kill(0,SIGUSR2);
      pause();
      read(eaglepipe[0],&eagletime,sizeof(int));
      write(childpipe[1],&eagletime,sizeof(int));
      read(parentpipe[0],recv,sizeof(recv));
    }
    write(eaglepipe[1],0,sizeof(char));
  }else{
    eagle = fork(); 
    if(eagle == 0){ //Eagle
      signal(SIGUSR2,eagleStuff);
      signal(SIGUSR1,SIG_IGN);
      read(eaglepipe[0],&status,sizeof(int));
      while(status){
        random = rand() %5 + 1;
        sleep(random);
        write(eaglepipe[1],&random,sizeof(int));
        kill(tamer,SIGUSR2);
        pause();
        read(eaglepipe[0],&status,sizeof(int));
      }
    }else{ //Center

      signal(SIGUSR1,handler);
      signal(SIGUSR2,SIG_IGN);
      pause();
      char message[81], timestamp[20];
      int waittime;
      while(ca->used > 0){
        strcpy(message,ca->array[0].address);
        write(parentpipe[1],message,sizeof(message));
        read(childpipe[0],&waittime,sizeof(int));
        printf("job finished in : %i minutes\n",waittime);
        makeReceipt(ca->array[0],waittime);
        removeContract(ca,ca->array[0].timestamp);
      }
      write(parentpipe[1],"end",sizeof("end"));
      close(parentpipe[0]);
      close(parentpipe[1]);
      close(childpipe[0]);
      close(childpipe[1]);
      close(eaglepipe[0]);
      close(eaglepipe[1]);
      kill(eagle,SIGKILL);
      kill(tamer,SIGKILL);
    }
  
  }
}


int menu(ContractArray *ca,char* userinput){
  
  char *selector;
  int result = 1;
  printf("What would you like to do?\n");
  printf("'N' : Create new contract, 'M' : Modify contract, 'D' : Delete contract, 'L' : List contracts,'E' : Execute contracts, 'Q' : Quit\n:: ");
  fgets(userinput,10,stdin);
  selector = userinput;
  if(!strcmp(selector,"N\n")){
    inputContract(ca);
  }else if(!strcmp(selector,"M\n")){
    modifyContract(ca);
  }else if(!strcmp(selector,"D\n")){
    deleteContract(ca);
  }else if(!strcmp(selector,"L\n")){
    listContracts(ca);
  }else if(!strcmp(selector,"E\n")){
    executeContracts(ca);
  }else if(!strcmp(selector,"Q\n")){
    result = 0;
  }else{
    printf("Invalid input. Please note that the menu is case sensitive\n");
  }
  return result;
}

int main(int argc,char* argv[]){
  ContractArray ca;
  char userinput[1000];
  initCArray(&ca,5);
  loadFromFile(&ca,"data");
  while(menu(&ca,userinput)){}
  saveToFile(&ca,"data");
}
