#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_QUEUE_SIZE 100
typedef struct element{/*define queue atruct*/
    int coming_minute;
    char customer_name[15];
    int operation_length;
    int transaction_amount;
}element;
struct element queue[MAX_QUEUE_SIZE];
typedef struct _list/*define linked list struct*/
{
    struct _list *next; /*refers to the next element*/
    int coming_minute;
    char customer_name[15];
    int operation_length;
    int transaction_amount;
} list;

typedef struct
{
    list *firstelement; /* linked list represents.only The first element is the sign.*/
} linkedlist;

linkedlist *producelinkedlist(); /*Create a linked list with malloc*/
void list_add(linkedlist* bl,int coming_minute, char name[15],int operation_length, int transaction_amount);/*adding element in desired linkedlist*/
void print(linkedlist* b1,FILE *outputfile);/*print linkedlist element in outputfile*/
void queue_add(struct element *queue , int coming_minute, char *name, int operation_length, int transaction_amount);/*adding element in queue*/
int queue_take(FILE *outputfile);/*takes element from the queue*/
void queue_empty(FILE *outputfile);/*print queue is empty in outputfile*/
void queue_full(FILE *outputfile);/*print queue is full in outputfile*/
int search(struct element *queue, char name[15]);/*search element in queue*/
int find_payoffice(linkedlist *b1, linkedlist *b2, linkedlist *b3, int coming_minute,int transaction_amount);/*find element's putting which payoffice*/
void addwaitingqueue(int coming_minute,char name[15], int operation_length, int transaction_amount);/*if element didn't add a payoffice, it is put waiting queue*/
int controlled_waiting_queue(linkedlist *b1, linkedlist *b2, linkedlist *b3,int customer_number,FILE *outputfile);/*find element(in waiting queue)'s putting which payoffice*/
int findmin(int i, int j, int k);/*find min number*/
int findmax(int i, int j, int k);/*find max numer*/
int front=-1;
int back=-1;
int payoffice1cout=500;/*payofiice1's first money*/
int payoffice2cout=500;/*payofiice2's first money*/
int payoffice3cout=500;/*payofiice3's first money*/
int endtime1,endtime2,endtime3;
struct element waitingqueue[20];/*define waiting queue*/

int main(int argc,char *argv[])
{
    FILE *commandfile, *processfile, *outputfile;
    char buffer[50], *tempstring;/*for getting value in files*/
    int counter, end_time,customer_number,i,j,k,cout=0;
    int coming_minute;
    char name[15];
    int operation_length;
    int transaction_amount;
    linkedlist * payoffice1 = producelinkedlist(); /* We produce a new linked list.*/
    linkedlist * payoffice2 = producelinkedlist(); /*We produce a new linked list.*/
    linkedlist * payoffice3 = producelinkedlist(); /*We produce a new linked list.*/

    if ((outputfile = fopen("output.txt", "w" ) ) == NULL ){/*open write file*/
        printf( "File could not be opened\n" );}
    for(i=0;i<20;i++){waitingqueue[i].coming_minute=0;}

    if ((processfile = fopen( "process.txt", "r" ) ) == NULL )/*open process file*/
        printf( "File could not be opened\n" );
    else
    {
        while(fgets(buffer,sizeof(buffer),processfile)!=NULL){/*read process line*/
                tempstring = strtok(buffer,",");
                counter=1;
                while(tempstring != NULL)/*line parts*/
                {
                    switch (counter){
                        case 1:
                            sscanf(tempstring,"%d",&coming_minute);
                            break;
                        case 2:
                            strcpy(name,tempstring);
                            break;
                        case 3:
                            sscanf(tempstring,"%d",&operation_length);
                            break;
                        case 4:
                            sscanf(tempstring,"%d",&transaction_amount);
                            break;
                    }
                    counter++;
                    tempstring = strtok(NULL,",");
                }
                cout++;
                queue_add(queue, coming_minute,name,operation_length,transaction_amount);/*value is added queue*/
        }
    }
    if ((commandfile = fopen("commands.txt", "r" ) ) == NULL )/*open command file*/
        printf( "File could not be opened\n" );
    else
    {
        while(fgets(buffer,sizeof(buffer),commandfile)!=NULL){/*read commands*/
            if(buffer[0]=='l'){/*if command is list*/
                int cout1=0;
                for(i=front+1;i<=back;i++){
                    fprintf(outputfile,"%d,%s,%d,%d\n",queue[i].coming_minute,queue[i].customer_name,queue[i].operation_length,queue[i].transaction_amount);/*write queue's element in outputfile*/
                    cout1++;
                }
                if(cout1==0){/*if queue is empty*/
                    fprintf(outputfile,"**********\n\n");}
                else{/*or not*/
                    fprintf(outputfile,"\n**********\n\n");}
            }
            else if((buffer[0]=='s')&&(buffer[1]=='e')){/*if command is search*/
                tempstring = strtok(buffer," ");
                counter=1;
                while(tempstring != NULL)/*get name which is wanted to search*/
                {
                    switch (counter){
                        case 2:
                            strcpy(name,(tempstring));
                            break;
                    }
                    counter++;
                    tempstring = strtok(NULL,";");
                }
                if(search(queue,name)!=-1){/*if queue have this name*/
                    j=search(queue, name);/*find this name which element in queue*/
                    fprintf(outputfile,"%d,%s,%d,%d\n",queue[j].coming_minute,queue[j].customer_name,queue[j].operation_length,queue[j].transaction_amount);/*print  searching name's other value in output file*/
                    fprintf(outputfile,"\n**********\n\n");
                }
            }
            else if((buffer[0]=='p')&&(buffer[8]!='a')){/*if command is process *number*/
                end_time=buffer[8]-'0';/*learn end time*/
                customer_number=0;
                k=0;
                while(k<=end_time){/*find elements which's coming minute is smaller the endtime*/
                    customer_number = controlled_waiting_queue(payoffice1, payoffice2, payoffice3, customer_number, outputfile);/*controlled waiting queue*/
                    k= queue_take(outputfile);/*get an element in queue*/
                    j=find_payoffice(payoffice1,payoffice2,payoffice3,queue[k].coming_minute,queue[k].transaction_amount);/*find this eleemnt is given which payoffice*/
                    if(j==1)/*if payoffice1*/
                    {
                        payoffice1cout=payoffice1cout+(queue[k].transaction_amount);/*control money*/
                        if(payoffice1cout>=0){/*if payoffice have enough money add this element in payoffice1 and print this element and payoffice in outputfile*/
                            list_add(payoffice1,queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                            customer_number++;
                            fprintf(outputfile,"%d,%s,%d,pay-office%d\n",queue[k].coming_minute,queue[k].customer_name,queue[k].transaction_amount,j);}
                         else{/*payoffice don't have enough money add this element in waiting queue*/
                            payoffice1cout=payoffice1cout-(queue[k].transaction_amount);
                            addwaitingqueue(queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                            continue;
                        }
                    }
                    else if (j==2)/*if payoffice2*/
                    {
                        payoffice2cout=payoffice2cout+(queue[k].transaction_amount);/*control money*/
                        if(payoffice2cout>=0){/*if payoffice have enough money add this element in payoffice2 and print this element and payoffice in outputfile*/
                            list_add(payoffice2,queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                            customer_number++;
                            fprintf(outputfile,"%d,%s,%d,pay-office%d\n",queue[k].coming_minute,queue[k].customer_name,queue[k].transaction_amount,j);}
                         else{/*payoffice don't have enough money add this element in waiting queue*/
                            payoffice2cout=payoffice2cout-((queue+k)->transaction_amount);
                            addwaitingqueue(queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                            continue;
                        }
                    }
                    else if(j==3)/*if payoffice3*/
                    {
                        payoffice3cout=payoffice3cout+(queue[k].transaction_amount);/*control money*/
                        if(payoffice3cout>=0){/*if payoffice have enough money add this element in payoffice3 and print this element and payoffice in outputfile*/
                            list_add(payoffice3,queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                            customer_number++;
                            fprintf(outputfile,"%d,%s,%d,pay-office%d\n",queue[k].coming_minute,queue[k].customer_name,queue[k].transaction_amount,j);}
                         else{/*payoffice don't have enough money add this element in waiting queue*/
                            payoffice3cout=payoffice3cout-(queue[k].transaction_amount);
                            addwaitingqueue(queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                            continue;
                        }
                    }
                }
                fprintf(outputfile,"\n[End Time:%d | customer number:%d]\n",end_time,customer_number);/*print end time and customer number in output fle*/
                fprintf(outputfile,"\n**********\n\n");


            }
            else if((buffer[0]=='p')&&(buffer[8]=='a')){/*if command is process all*/
                customer_number=0;
                k=0;
                while(k<back){/*deploy all element in procesfile*/
                    customer_number = controlled_waiting_queue(payoffice1,payoffice2,payoffice3,customer_number,outputfile);/*controlled waiting queue*/
                    k= queue_take(outputfile);/*get an element in queue*/
                    j=find_payoffice(payoffice1,payoffice2,payoffice3,queue[k].coming_minute,queue[k].transaction_amount);/*find this eleemnt is given which payoffice*/
                    if(j==1)/*if payoffice1*/
                    {
                        payoffice1cout=payoffice1cout+(queue[k].transaction_amount);/*control money*/
                        if(payoffice1cout>=0){/*if payoffice have enough money add this element in payoffice1 and print this element and payoffice in outputfile*/
                        list_add(payoffice1,queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                        customer_number++;
                        fprintf(outputfile,"%d,%s,%d,pay-office%d\n",queue[k].coming_minute,queue[k].customer_name,queue[k].transaction_amount,j);
                        }
                        else{/*payoffice don't have enough money add this element in waiting queue*/
                            payoffice1cout=payoffice1cout-(queue[k].transaction_amount);
                            addwaitingqueue(queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                            continue;
                        }
                    }
                    else if (j==2)/*if payoffice2*/
                    {
                        payoffice2cout=payoffice2cout+(queue[k].transaction_amount);/*control money*/
                        if(payoffice2cout>=0){/*if payoffice have enough money add this element in payoffice2 and print this element and payoffice in outputfile*/
                            list_add(payoffice2,queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                            customer_number++;
                            fprintf(outputfile,"%d,%s,%d,pay-office%d\n",queue[k].coming_minute,queue[k].customer_name,queue[k].transaction_amount,j);
                        }
                        else{/*payoffice don't have enough money add this element in waiting queue*/
                            payoffice2cout=payoffice2cout-(queue[k].transaction_amount);
                            addwaitingqueue(queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                            continue;
                        }
                    }
                    else if(j==3)/*if payoffice2*/
                    {
                        payoffice3cout=payoffice3cout+(queue[k].transaction_amount);/*control money*/
                        if(payoffice3cout>=0){/*if payoffice have enough money add this element in payoffice2 and print this element and payoffice in outputfile*/
                            list_add(payoffice3, queue[k].coming_minute, queue[k].customer_name, queue[k].operation_length, queue[k].transaction_amount);
                            customer_number++;
                            fprintf(outputfile,"%d,%s,%d,pay-office%d\n",queue[k].coming_minute,queue[k].customer_name,queue[k].transaction_amount,j);
                        }
                        else{/*payoffice don't have enough money add this element in waiting queue*/
                            payoffice3cout=payoffice3cout-((queue+k)->transaction_amount);
                            addwaitingqueue(queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                            continue;
                        }
                    }
                    else{/*if don't find any payoffice, this element is added in waiting queue*/
                            addwaitingqueue(queue[k].coming_minute,queue[k].customer_name,queue[k].operation_length,queue[k].transaction_amount);
                            continue;
                    }
                }
                fprintf(outputfile,"\n[End Time:%d | customer number:%d]\n",findmax(endtime1,endtime2,endtime3),customer_number);/*print end time and customer number in outputfile*/
                fprintf(outputfile,"\n**********\n\n");

            }
            else if((buffer[0]=='s')&&(buffer[5]=='l'))/*if command is show logs*/
            {
                fprintf(outputfile,"pay-office1:\n");/*print payoffice1's element*/
                print(payoffice1,outputfile);/*print payoffice1's element*/
                fprintf(outputfile,"\npay-office2:\n");
                print(payoffice2,outputfile);/*print payoffice2's element*/
                fprintf(outputfile,"\npay-office3:\n");
                print(payoffice3,outputfile);/*print payoffice3's element*/

            }


        }
    }
    fclose(processfile);/*close processfile*/
    fclose(commandfile);/*close commandfile*/
    fclose(outputfile);/*close outputfile*/
    return 0;
}
void queue_full(FILE *outputfile)/*print queue is full in outputfile and exit program*/
{
    fprintf(outputfile,"Queue is full!\n"); exit(-1);
}
void queue_empty(FILE *outputfile)/*print queue is empty in outputfile and exit program*/
{
   fprintf(outputfile,"Queue is empty!\n"); exit(-2);
}
void queue_add(struct element *queue, int coming_minute, char name[15], int operation_length, int transaction_amount)
{
    back++;/*incease back*/
    queue[back].coming_minute = coming_minute;/*value is added in queue*/
    strcpy(queue[back].customer_name,name);
    queue[back].operation_length = operation_length;
    queue[back].transaction_amount = transaction_amount;
}

int queue_take(FILE *outputfile)/*takes element from the queue*/
{
    if(front==back)/*controlled queue is empty or not*/
        queue_empty(outputfile);
    return ++front;/*increase front*/
}
int search(struct element *queue, char name[15])/*search name in queue*/
{
    int i;
    for(i=front;i<=back;i++){
        if(strcmp(queue[i].customer_name,name)==0){/*if find this name in queue break*/
            break;
        }
    }
    if(i<=back){return i;}/*return this name's place*/
    else{return -1;}
}

linkedlist *producelinkedlist()
{
     linkedlist* bl = (linkedlist*)malloc(sizeof(linkedlist)); /* a new linked list draws up in memory, we're taking about the address.*/
     bl->firstelement = NULL;
     return bl;
}

void list_add(linkedlist* bl,int coming_minute, char name[15],int operation_length, int transaction_amount)
{
     list *r = (list*)malloc(sizeof(list)); /* we construct a new element.*/
     r->next = NULL;
     r->coming_minute = coming_minute;/*add values in linkedlist*/
     strcpy(r->customer_name,name);
     r->operation_length=operation_length;
     r->transaction_amount=transaction_amount;
     list *p = NULL; /* linked list's last element*/
     p = (list *)bl->firstelement; /* Let the starting element from the linked list.*/
    if (p==NULL) /*our list is empty?*/
    {
        bl->firstelement = r;
        return;
    }
    while(p!=NULL) /*try to access the last element.*/
    {
        if (p->next==NULL)
            break;
        p = (list*)p->next; /*jump between nodes*/
    }
    p->next = r; /* We're writing our own address pointer of the last node.*/
}

void print(linkedlist* bl,FILE *outputfile)/*print linkedlist element in outputfile*/
{
    list *r = NULL;
    r=(list *)bl->firstelement;
    while(r!=NULL)
    {
        fprintf(outputfile,"%d,%s,%d,%d\n",r->coming_minute,r->customer_name,r->operation_length,r->transaction_amount);
        r = (list*) r->next;
    }
}

int find_payoffice(linkedlist *b1, linkedlist *b2, linkedlist *b3, int coming_minute, int transaction_amount)
{
    int min;
    if(b1->firstelement==NULL)
    {
        return 1;
    }
    list *r = NULL;/*find payoffice1's last element*/
    r=(list *)b1->firstelement;
    while(r!=NULL)
    {
         if (r->next==NULL)
            break;
        r = (list*) r->next;
    }
    list *p = NULL;/*payoffice2*/
    p=(list *)b2->firstelement;
    while(p!=NULL)
    {
        if (p->next==NULL)
            break;
        p = (list*) p->next;
    }
    list *q = NULL;/*payoffice3*/
    q=(list *)b3->firstelement;
    while(q!=NULL)
    {
        if (q->next==NULL)
            break;
        q = (list*) q->next;
    }
    int k=(r->coming_minute)+(r->operation_length);/*control coming minute and another customer's process end time to payoffice1*/
    if((k==coming_minute)&&((payoffice1cout+transaction_amount)>=0))
    {
        endtime1=k;
        return 1;
    }
    else if(b2->firstelement==NULL){
        return 2;
    }
    int j=(p->coming_minute)+(p->operation_length);/*control coming minute and another customer's process end time to payoffice2*/
    if((j==coming_minute)&&((payoffice2cout+transaction_amount)>=0))
    {
        endtime2=j;
        return 2;
    }
    else if(b3->firstelement==NULL){
        return 3;
    }
    int i=(q->coming_minute)+(q->operation_length);/*control coming minute and another customer's process end time to payoffice3*/
    if((i==coming_minute)&&((payoffice3cout+transaction_amount)>=0)){
        endtime3=i;
        return 3;
    }
    /*if endtime is bigger than coming minute actions being taken by the end time*/
    k=r->operation_length;
    j=p->operation_length;
    i=q->operation_length;
    list *r1 = NULL;
    r1=(list *)b1->firstelement;
    while(r1->next!=r)/*find the previous node*/
    {
        r1 = (list*) r1->next;
    }
    list *p1 = NULL;
    p1=(list *)b2->firstelement;
    while(p1->next!=p)/*find the previous node*/
    {
        p1 = (list*) p1->next;
    }
    list *q1 = NULL;
    q1=(list *)b3->firstelement;
    while(q1->next!=q)/*find the previous node*/
    {
        q1 = (list*) q1->next;
    }
    k = k + r1->operation_length + r1->coming_minute;
    j = j + p1->operation_length + p1->coming_minute;
    i = i + q1->operation_length + q1->coming_minute;

    if((k>coming_minute)&&(j>coming_minute)&&(i>coming_minute)){/*find this element's putting payoffice*/
        min=findmin(k,j,i);
        if((min==k)&&((payoffice1cout+transaction_amount)>=0)){endtime1=k;return 1;}
        else if ((min==j)&&((payoffice2cout+transaction_amount)>=0)){endtime2=j;return 2;}
        else if((min==i)&&((payoffice3cout+transaction_amount)>=0)){endtime3=i;return 3;}
    }
    return 0;
}
void addwaitingqueue(int coming_minute,char name[15], int operation_length, int transaction_amount)/*adding waiting queue*/
{
    int i=0;
    while(i<20)/*find empty waitingqueue elemnt*/
    {
        if(waitingqueue[i].coming_minute==0)
           break;

        else
            i++;
    }
    (waitingqueue+i)->coming_minute = coming_minute;/*add values in waiting queue*/
    strcpy(waitingqueue[i].customer_name,name);
    waitingqueue[i].operation_length = operation_length;
    (waitingqueue+i)->transaction_amount = transaction_amount;
}
int controlled_waiting_queue(linkedlist *b1, linkedlist *b2, linkedlist *b3,int customer_number,FILE *outputfile)
{
    int i=0,j;
    while(i<20){
        if(waitingqueue[i].coming_minute!=0)/*waiting queue's full elements*/
        {
            j=find_payoffice(b1,b2,b3,waitingqueue[i].coming_minute,waitingqueue[i].transaction_amount);/*find this element is given which payoffice*/
                    if(j==1)/*if payoffice1*/
                    {
                        payoffice1cout=payoffice1cout+(waitingqueue[i].transaction_amount);/*controled money*/
                        if(payoffice1cout>=0){/*if payoffice have enough money add this element in payoffice1 and print this element and payoffice in outputfile*/
                            list_add(b1,waitingqueue[i].coming_minute,waitingqueue[i].customer_name,waitingqueue[i].operation_length,waitingqueue[i].transaction_amount);
                            fprintf(outputfile,"%d,%s,%d,pay-office%d\n",waitingqueue[i].coming_minute,waitingqueue[i].customer_name,waitingqueue[i].transaction_amount,j);
                            customer_number++;
                            waitingqueue[i].coming_minute=0;
                        }
                        else{/*payoffice don't have enough money add this element in waiting queue*/
                            payoffice1cout=payoffice1cout-(waitingqueue[i].transaction_amount);
                            addwaitingqueue(waitingqueue[i].coming_minute,waitingqueue[i].customer_name,waitingqueue[i].operation_length,waitingqueue[i].transaction_amount);
                            continue;
                        }
                    }
                    else if (j==2)/*if payoffice2*/
                    {
                        payoffice2cout=payoffice2cout+(waitingqueue[i].transaction_amount);/*controled money*/
                        if(payoffice2cout>=0){/*if payoffice have enough money add this element in payoffice2 and print this element and payoffice in outputfile*/
                            list_add(b2,waitingqueue[i].coming_minute,waitingqueue[i].customer_name,waitingqueue[i].operation_length,waitingqueue[i].transaction_amount);
                            fprintf(outputfile,"%d,%s,%d,pay-office%d\n",waitingqueue[i].coming_minute,waitingqueue[i].customer_name,waitingqueue[i].transaction_amount,j);
                            customer_number++;
                            waitingqueue[i].coming_minute=0;
                        }
                        else{/*payoffice don't have enough money add this element in waiting queue*/
                            payoffice2cout=payoffice2cout-(waitingqueue[i].transaction_amount);
                            addwaitingqueue(waitingqueue[i].coming_minute,waitingqueue[i].customer_name,waitingqueue[i].operation_length,waitingqueue[i].transaction_amount);
                            continue;
                        }
                    }
                    else if(j==3)/*if payoffice3*/
                    {
                        payoffice3cout=payoffice3cout+(waitingqueue[i].transaction_amount);/*controled money*/
                        if(payoffice3cout>=0){/*if payoffice have enough money add this element in payoffice2 and print this element and payoffice in outputfile*/
                            list_add(b3,waitingqueue[i].coming_minute,waitingqueue[i].customer_name,waitingqueue[i].operation_length,waitingqueue[i].transaction_amount);
                            fprintf(outputfile,"%d,%s,%d,pay-office%d\n",waitingqueue[i].coming_minute,waitingqueue[i].customer_name,waitingqueue[i].transaction_amount,j);
                            customer_number++;
                            waitingqueue[i].coming_minute=0;
                        }
                        else{/*payoffice don't have enough money add this element in waiting queue*/
                            payoffice3cout=payoffice3cout-(waitingqueue[i].transaction_amount);
                            addwaitingqueue(waitingqueue[i].coming_minute,waitingqueue[i].customer_name,waitingqueue[i].operation_length,waitingqueue[i].transaction_amount);
                            continue;
                        }
                    }
                    i++;
                    continue;
            }
            else
                break;
    }
    return customer_number;
}
int findmin(int i, int j, int k)/*find min number*/
{
    int min=i;
    if(min> j){min = j;}
    if(min>k){min =k;}
    return min;
}
int findmax(int i, int j, int k)/*find max number*/
{
    int max=i;
    if(max< j){max = j;}
    if(max<k){max =k;}
    return max;
}
