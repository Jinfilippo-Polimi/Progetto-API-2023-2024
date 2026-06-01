/*
*   04 AGOSTO 2O24
*   FILIPPO JIN
*   INGEGNERIA INFORMATICA
*   PROVA FINALE DI ALGORITMI E STRUTTURE DATI 2023-2024
*   POLITECNICO DI MILANO
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define N_LEN 20
#define A_LEN 50001
#define I_LEN 11
#define E_LEN 2200
#define W_LEN 74900



typedef struct expiry_t
{
    int weight;
    int expiry_time;
    struct expiry_t* next;
} expiry;

typedef struct lot_t
{
    char name[N_LEN];
    int weight;
    int references;
    expiry* head;
    struct lot_t* next;
    struct lot_t* prev;
} lot;

typedef struct ingredient_t
{
    lot* address;
    int weight;
} ingredient;

typedef struct recipe_t
{
    char name[N_LEN];
    int weight;
    ingredient array[I_LEN];
    struct recipe_t* next;
    struct recipe_t* prev;
} recipe;

typedef struct order_t
{
    recipe* address;
    int quantity;
    int ordered_time;
} order;



void aggiungi_ricetta(recipe* book[], lot* storage[]);
void rimuovi_ricetta(recipe* book[], order exit_queue[], order wait_queue[]);
void ordine(recipe* book[],lot* storage[], order exit_queue[], order wait_queue[], int* exit_len, int* wait_len, int time);
void check_wait_queue(recipe* book[], lot* storage[], order exit_queue[], order wait_queue[], int* exit_len, int* wait_len, int time);
int check_enough_ingredients(recipe* ptr_recipe, lot* storage[], int quantity, int time);
void make_order(recipe* ptr_recipe, lot* storage[], int quantity);
void add_to_queue(recipe* ptr_recipe, order queue[], int* len, int quantity, int time);
void add_to_exit_from_wait(int position, order exit_queue[], order wait_queue[], int* exit_len, int* wait_len);
void restock(lot* storage[], int time);
void pickup(order exit_queue[], int* exit_len, int capacity);
lot* trova_lot(char term[], lot* storage[], int flag);



unsigned long hash(char *term);
int compare_ordered_time(const void* a, const void* b);
int compare_total_weight(const void* a, const void* b);



int main()
{
    int period, capacity, time=0, i, exit_len=0, wait_len=0;
    char command[21];
    recipe* book[A_LEN];
    lot* storage[A_LEN];
    order exit_queue[E_LEN], wait_queue[W_LEN];

    for(i=0; i<W_LEN; i++)
    {
        wait_queue[i].ordered_time=-1;
        if(i<A_LEN)
        {
            book[i]=NULL;
            storage[i]=NULL;
            if(i<E_LEN) exit_queue[i].ordered_time=-1;
        }
    }

    i=scanf("%d %d", &period, &capacity);
    i=i+0;
    getc(stdin);

    while(!feof(stdin))
    {
        if(time%period==0 && time!=0) pickup(exit_queue, &exit_len, capacity);
        i=scanf("%s", command);
        i=i+0;
        
        if(!feof(stdin))
        {
            if(command[2]=='g') aggiungi_ricetta(book, storage);
            else if(command[2]=='m') rimuovi_ricetta(book, exit_queue, wait_queue);
            else if(command[2]=='d') ordine(book, storage, exit_queue, wait_queue, &exit_len, &wait_len, time);
            else
            {
                restock(storage, time);
                check_wait_queue(book, storage, exit_queue, wait_queue, &exit_len, &wait_len, time);
            }
            time++;
        }
    }
    return 0;
}



void aggiungi_ricetta(recipe* book[], lot* storage[])
{
    int key, weight, dup=0, i, j;
    char term[N_LEN];
    recipe* ptr_recipe;

    getc(stdin);
    i=scanf("%s", term);
    i=i+0;
    key=hash(term);
    ptr_recipe=book[key];

    if(book[key]==NULL)
    {
        book[key]=malloc(sizeof(recipe));
        book[key]->next=NULL;
        book[key]->prev=NULL;
        strcpy(book[key]->name, term);
        book[key]->weight=0;
        ptr_recipe=book[key];
    }
    else
    {
        while(ptr_recipe->next!=NULL && strcmp(ptr_recipe->name, term)!=0) ptr_recipe=ptr_recipe->next;
        if(strcmp(ptr_recipe->name, term)==0) dup=1;
        else
        {
            ptr_recipe->next=malloc(sizeof(recipe));
            (ptr_recipe->next)->prev=ptr_recipe;
            ptr_recipe=ptr_recipe->next;
            ptr_recipe->next=NULL;
            strcpy(ptr_recipe->name, term);
            ptr_recipe->weight=0;
        }
    }
    
    if(dup==0)
    {
        for(j=0; getc(stdin)!='\n'; j++)
        {
            i=scanf("%s %d", term, &weight);
            i=i+0;

            ptr_recipe->array[j].address=(trova_lot(term, storage, 1));
            ptr_recipe->array[j].weight=weight;
            ptr_recipe->weight=ptr_recipe->weight+weight;
        }
        ptr_recipe->array[j].address=NULL;

        printf("aggiunta\n");
    }
    else
    {
        printf("ignorato\n");
        while(getc(stdin)!='\n');
    }
}



void rimuovi_ricetta(recipe* book[], order exit_queue[], order wait_queue[])
{
    int flag=0, key, i;
    char term[N_LEN];

    getc(stdin);
    i=scanf("%s", term);
    i=i+0;
    getc(stdin);
    key=hash(term);
    recipe* ptr_recipe=book[key];
    while(ptr_recipe!=NULL && strcmp(ptr_recipe->name, term)!=0) ptr_recipe=ptr_recipe->next;

    if(ptr_recipe==NULL) printf("non presente\n");
    else
    {
        for(i=0; exit_queue[i].ordered_time!=-1 && flag==0; i++)
        {
            if(exit_queue[i].address->name==ptr_recipe->name) flag=1;
        }
        for(i=0; wait_queue[i].ordered_time!=-1 && flag==0; i++)
        {
            if(wait_queue[i].address->name==ptr_recipe->name) flag=1;
        }

        if(flag==0)
        {
            for(i=0; ptr_recipe->array[i].address!=NULL; i++) ptr_recipe->array[i].address->references--;

            if(ptr_recipe==book[key])
            {
                book[key]=book[key]->next;
                if(book[key]!=NULL) book[key]->prev=NULL;
            }
            else
            {
                (ptr_recipe->prev)->next=ptr_recipe->next;
                if(ptr_recipe->next!=NULL) (ptr_recipe->next)->prev=ptr_recipe->prev;
            }
            free(ptr_recipe);
            printf("rimossa\n");
        }
        else printf("ordini in sospeso\n");
    }
}



void ordine(recipe* book[], lot* storage[], order exit_queue[], order wait_queue[], int* exit_len, int* wait_len, int time)
{
    char term[N_LEN];
    int quantity, key, i;
    getc(stdin);
    i=scanf("%s %d", term, &quantity);
    i=i+0;
    getc(stdin);

    key=hash(term);
    recipe* ptr_recipe=book[key];
    while(ptr_recipe!=NULL && strcmp(ptr_recipe->name, term)!=0) ptr_recipe=ptr_recipe->next;

    if(ptr_recipe==NULL) printf("rifiutato\n");
    else
    {
        printf("accettato\n");
        if(check_enough_ingredients(ptr_recipe, storage, quantity, time)==0)
        {
            make_order(ptr_recipe, storage, quantity);
            add_to_queue(ptr_recipe, exit_queue, exit_len, quantity, time);
        }
        else add_to_queue(ptr_recipe, wait_queue, wait_len, quantity, time);
    }
}



void check_wait_queue(recipe* book[], lot* storage[], order exit_queue[], order wait_queue[], int* exit_len, int* wait_len, int time)
{
    int i=0;

    while(i<*wait_len)
    {
        if(check_enough_ingredients(wait_queue[i].address, storage, wait_queue[i].quantity, time)==0)
        {
            make_order(wait_queue[i].address, storage, wait_queue[i].quantity);
            add_to_exit_from_wait(i, exit_queue, wait_queue, exit_len, wait_len);
            i--;
        }
        i++;
    }
    qsort(exit_queue, *exit_len, sizeof(order), compare_ordered_time);

}



int check_enough_ingredients(recipe* ptr_recipe, lot* storage[], int quantity, int time)
{
    int flag=0, i;
    expiry* del;
    for(i=0; ptr_recipe->array[i].address!=NULL && flag==0; i++)
    {
        while(ptr_recipe->array[i].address->head!=NULL && (ptr_recipe->array[i].address->head)->expiry_time<=time)
        {
            del=ptr_recipe->array[i].address->head;
            ptr_recipe->array[i].address->weight=ptr_recipe->array[i].address->weight-del->weight;
            ptr_recipe->array[i].address->head=ptr_recipe->array[i].address->head->next;
            free(del);
        }
        if(ptr_recipe->array[i].address->head==NULL && ptr_recipe->array[i].address->references==0)
        {
            if(ptr_recipe->array[i].address->prev==NULL)
            {
                ptr_recipe->array[i].address=ptr_recipe->array[i].address->next;
                if(ptr_recipe->array[i].address!=NULL) ptr_recipe->array[i].address->prev=NULL;
            }
            else
            {
                (ptr_recipe->array[i].address->prev)->next=ptr_recipe->array[i].address->next;
                if(ptr_recipe->array[i].address->next!=NULL) (ptr_recipe->array[i].address->next)->prev=ptr_recipe->array[i].address->prev;
            }
            free(ptr_recipe->array[i].address);
            ptr_recipe->array[i].address=NULL;
        }
        if(ptr_recipe->array[i].address==NULL) flag=1;
        else if(ptr_recipe->array[i].address->weight<quantity*ptr_recipe->array[i].weight) flag=1;
    }
    return flag;
}



void make_order(recipe* ptr_recipe, lot* storage[], int quantity)
{
    int dump, key, i;
    expiry* del;
    for(i=0; ptr_recipe->array[i].address!=NULL; i++)
    {
        key=hash(ptr_recipe->array[i].address->name);
        dump=quantity*ptr_recipe->array[i].weight;
        ptr_recipe->array[i].address->weight=ptr_recipe->array[i].address->weight-dump;
        while(dump>0)
        {
            if((ptr_recipe->array[i].address->head)->weight<=dump)
            {
                dump=dump-(ptr_recipe->array[i].address->head)->weight;
                del=ptr_recipe->array[i].address->head;
                ptr_recipe->array[i].address->head=(ptr_recipe->array[i].address->head)->next;
                free(del);
            }
            else
            {
                (ptr_recipe->array[i].address->head)->weight=(ptr_recipe->array[i].address->head)->weight-dump;
                dump=0;
            }
        }
        if(ptr_recipe->array[i].address->head==NULL && ptr_recipe->array[i].address->references==0)
        {
            if(ptr_recipe->array[i].address==storage[key])
            {
                storage[key]=storage[key]->next;
                if(storage[key]!=NULL) storage[key]->prev=NULL;
            }
            else
            {
                (ptr_recipe->array[i].address->prev)->next=ptr_recipe->array[i].address->next;
                if(ptr_recipe->array[i].address->next!=NULL) (ptr_recipe->array[i].address->next)->prev=ptr_recipe->array[i].address->prev;
            }
            free(ptr_recipe->array[i].address);
        }
    }
}



void add_to_queue(recipe* ptr_recipe, order queue[], int* len, int quantity, int time)
{
    queue[*len].address=ptr_recipe;
    queue[*len].ordered_time=time;
    queue[*len].quantity=quantity;
    *len=(*len)+1;
}



void add_to_exit_from_wait(int position, order exit_queue[], order wait_queue[], int* exit_len, int* wait_len)
{
    memcpy(&exit_queue[*exit_len], &wait_queue[position], sizeof(order));
    *exit_len=(*exit_len)+1;
    memmove(&wait_queue[position], &wait_queue[position+1], (*wait_len-position)*sizeof(order));
    wait_queue[*wait_len].ordered_time=-1;
    *wait_len=(*wait_len)-1;
}



void restock(lot* storage[], int time)
{
    char term[N_LEN];
    int key, weight, expiry_time;
    lot* ptr_lot;
    expiry* ptr_expiry, *temp_expiry;

    getc(stdin);
    do
    {
        key=scanf("%s %d %d", term, &weight, &expiry_time);
        key=key+0;
        if(expiry_time>time)
        {
            ptr_lot=trova_lot(term, storage, 0);
            ptr_lot->weight=ptr_lot->weight+weight;
            
            if(ptr_lot->head==NULL)
            {
                ptr_lot->head=malloc(sizeof(expiry));
                (ptr_lot->head)->next=NULL;
                (ptr_lot->head)->weight=weight;
                (ptr_lot->head)->expiry_time=expiry_time;
                ptr_expiry=ptr_lot->head;
            }
            else if(expiry_time<ptr_lot->head->expiry_time)
            {
                temp_expiry=ptr_lot->head;
                ptr_lot->head=malloc(sizeof(expiry));
                (ptr_lot->head)->next=temp_expiry;
                (ptr_lot->head)->weight=weight;
                (ptr_lot->head)->expiry_time=expiry_time;
            }
            else
            {
                temp_expiry=ptr_lot->head;
                ptr_expiry=ptr_lot->head;
                while(ptr_expiry!=NULL && (ptr_expiry)->expiry_time<expiry_time)
                {
                    temp_expiry=ptr_expiry;
                    ptr_expiry=ptr_expiry->next;
                }
                if(ptr_expiry==NULL)
                {
                    temp_expiry->next=malloc(sizeof(expiry));
                    temp_expiry=temp_expiry->next;
                    temp_expiry->next=NULL;
                    temp_expiry->weight=weight;
                    temp_expiry->expiry_time=expiry_time;
                }
                else if(ptr_expiry->expiry_time==expiry_time) ptr_expiry->weight=ptr_expiry->weight+weight;
                else if(ptr_expiry->expiry_time>expiry_time)
                {
                    temp_expiry->next=malloc(sizeof(expiry));
                    temp_expiry=temp_expiry->next;
                    temp_expiry->next=ptr_expiry;
                    temp_expiry->weight=weight;
                    temp_expiry->expiry_time=expiry_time;
                }
            }
        }
    } while (getc(stdin)!='\n');
    
    printf("rifornito\n");
}



lot* trova_lot(char term[], lot* storage[], int flag)
{
    int key;
    lot* ptr_lot;

    key=hash(term);
    ptr_lot=storage[key];
    if(storage[key]==NULL)
    {
        storage[key]=malloc(sizeof(lot));
        storage[key]->head=NULL;
        storage[key]->next=NULL;
        storage[key]->prev=NULL;
        strcpy(storage[key]->name, term);
        storage[key]->weight=0;
        storage[key]->references=flag;
        ptr_lot=storage[key];
    }
    else
    {
        while(ptr_lot->next!=NULL && strcmp(ptr_lot->name, term)!=0) ptr_lot=ptr_lot->next;
        if(strcmp(ptr_lot->name, term)!=0)
        {
            ptr_lot->next=malloc(sizeof(lot));
            (ptr_lot->next)->prev=ptr_lot;
            ptr_lot=ptr_lot->next;
            ptr_lot->head=NULL;
            ptr_lot->next=NULL;
            strcpy(ptr_lot->name, term);
            ptr_lot->weight=0;
            ptr_lot->references=flag;
        }
        else ptr_lot->references=ptr_lot->references+flag;
    }
    return ptr_lot;
}



void pickup(order exit_queue[], int* exit_len, int capacity)
{
    int i=0, j;
    while(capacity>=0 && i<*exit_len)
    {
        capacity=capacity-exit_queue[i].quantity*exit_queue[i].address->weight;
        if(capacity>=0) i++;
    }
    if(i==0) printf("camioncino vuoto\n");
    else
    {
        qsort(exit_queue, i, sizeof(order), compare_total_weight);
        for(j=0; j<i; j++)
        {
            printf("%d %s %d\n", exit_queue[j].ordered_time, exit_queue[j].address->name, exit_queue[j].quantity);
            exit_queue[j].ordered_time=-1;
        }
        qsort(exit_queue, *exit_len, sizeof(order), compare_ordered_time);
        *exit_len=(*exit_len)-i;
    }
}



unsigned long hash(char *term)
{
    unsigned long hash=5381;
    int c;
    while((c=*term++)) hash=((hash<<5)+hash)+c;
    return (hash%A_LEN);
}



int compare_ordered_time(const void* a, const void* b)
{
    order* A=(order*) a, *B=(order*) b;
    if(A->ordered_time==B->ordered_time) return 0;
    else if(A->ordered_time==-1) return 1;
    else if(B->ordered_time==-1) return -1;
    else return A->ordered_time-B->ordered_time;
}



int compare_total_weight(const void* a, const void* b)
{
    order* A=(order*) a, *B=(order*) b;
    if(B->quantity*B->address->weight!=A->quantity*A->address->weight) return ((B->quantity*B->address->weight)-(A->quantity*A->address->weight));
    else return A->ordered_time-B->ordered_time;
}