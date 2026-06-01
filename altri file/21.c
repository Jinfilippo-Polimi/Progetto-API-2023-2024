#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define N_LEN 20
#define A_LEN 10001



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
    expiry* head;
    struct lot_t* next;
    struct lot_t* prev;
} lot;

typedef struct ingredient_t
{
    char name[N_LEN];
    int weight;
    struct ingredient_t* next;
} ingredient;

typedef struct recipe_t
{
    char name[N_LEN];
    int weight;
    ingredient* head;
    struct recipe_t* next;
    struct recipe_t* prev;
} recipe;

typedef struct order_t
{
    char name[N_LEN];
    int weight;
    int quantity;
    int ordered_time;
    struct order_t* next;
    struct order_t* prev;
} order;



void aggiungi_ricetta(recipe* book[]);
void rimuovi_ricetta(recipe* book[], order* exit_queue, order* wait_queue);
void ordine(recipe* book[],lot* storage[], order** exit_queue, order** wait_queue, order** exit_tail, order** wait_tail, int time);
void check_wait_queue(recipe* book[], lot* storage[], order** exit_queue, order** wait_queue, order** exit_tail, order** wait_tail, int time);
int check_enough_ingredients(recipe* ptr_recipe, lot* storage[], int quantity, int time);
void make_order(recipe* ptr_recipe, lot* storage[], int quantity);
void add_to_queue(recipe* ptr_recipe, order** queue, order** tail, int quantity, int time);
order* add_to_exit_from_wait(order* ptr_wait_queue, order** exit_queue, order** wait_queue, order** exit_queue_tail, order** wait_queue_tail);
void restock(lot* storage[], int time);
order* pickup(order* exit_queue, int capacity);



unsigned long hash(char *term);
void sort_print(order* ptr);



int main()
{
    int period, capacity, time=0, i;
    char command[21];
    i=scanf("%d %d", &period, &capacity);
    i=2*i;
    recipe* book[A_LEN];
    for(i=0; i<A_LEN; i++) book[i]=NULL;
    lot* storage[A_LEN];
    for(i=0; i<A_LEN; i++) storage[i]=NULL;
    order* exit_queue=NULL, *wait_queue=NULL, *exit_tail=NULL, *wait_tail=NULL;

    while(!feof(stdin))
    {
        if(time%period==0 && time!=0) exit_queue=pickup(exit_queue, capacity);
        i=scanf("%s", command);
        i=2*i;
        if(!feof(stdin))
        {
            if(strcmp(command, "aggiungi_ricetta")==0) aggiungi_ricetta(book);
            else if(strcmp(command, "rimuovi_ricetta")==0) rimuovi_ricetta(book, exit_queue, wait_queue);
            else if(strcmp(command, "ordine")==0) ordine(book, storage, &exit_queue, &wait_queue, &exit_tail, &wait_tail, time);
            else
            {
                restock(storage, time);
                check_wait_queue(book, storage, &exit_queue, &wait_queue, &exit_tail, &wait_tail, time);
            }
            time++;
        }
    }
    return 0;
}



void aggiungi_ricetta(recipe* book[])
{
    int key, weight, dup=0, i;
    char term[N_LEN];
    recipe* ptr_recipe;
    ingredient* ptr_ingredient;

    getc(stdin);
    i=scanf("%s", term);
    i=2*i;
    key=hash(term);
    ptr_recipe=book[key];

    if(book[key]==NULL)
    {
        book[key]=malloc(sizeof(recipe));
        book[key]->next=NULL;
        book[key]->prev=NULL;
        book[key]->head=NULL;
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
            ptr_recipe->head=NULL;
            strcpy(ptr_recipe->name, term);
            ptr_recipe->weight=0;
        }
    }
    
    if(dup==0)
    {
        while(getc(stdin)!='\n')
        {
            i=scanf("%s %d", term, &weight);
            i=2*i;
            ptr_ingredient=ptr_recipe->head;
            (ptr_recipe)->head=malloc(sizeof(ingredient));
            ((ptr_recipe)->head)->next=ptr_ingredient;
            strcpy(((ptr_recipe)->head)->name, term);
            ((ptr_recipe)->head)->weight=weight;
            ptr_recipe->weight=(ptr_recipe->weight)+weight;
        }
        printf("aggiunta\n");
    }
    else
    {
        printf("ignorato\n");
        while(getc(stdin)!='\n');
    }
}



void rimuovi_ricetta(recipe* book[], order* exit_queue, order* wait_queue)
{
    int flag=0, key, i;
    char term[N_LEN];

    getc(stdin);
    i=scanf("%s", term);
    i=2*i;
    getc(stdin);
    key=hash(term);
    recipe* ptr_recipe=book[key];
    while(ptr_recipe!=NULL && strcmp(ptr_recipe->name, term)!=0) ptr_recipe=ptr_recipe->next;
    if(ptr_recipe==NULL) printf("non presente\n");
    else
    {
        order* ptr_order=exit_queue;
        while(flag==0 && ptr_order!=NULL)
        {
            if(strcmp(ptr_order->name, term)==0) flag=1;
            else ptr_order=ptr_order->next;
        }
        ptr_order=wait_queue;
        while(flag==0 && ptr_order!=NULL)
        {
            if(strcmp(ptr_order->name, term)==0) flag=1;
            else ptr_order=ptr_order->next;
        }
        if(flag==0)
        {
            ingredient* ptr_ingredient=ptr_recipe->head, *del;
            while(ptr_ingredient!=NULL)
            {
                del=ptr_ingredient;
                ptr_ingredient=ptr_ingredient->next;
                free(del);
            }
            if(ptr_recipe==book[key])
            {
                book[key]=book[key]->next;
                if(book[key]!=NULL) book[key]->prev=NULL;
                free(ptr_recipe);
            }
            else
            {
                (ptr_recipe->prev)->next=ptr_recipe->next;
                if(ptr_recipe->next!=NULL) (ptr_recipe->next)->prev=ptr_recipe->prev;
                free(ptr_recipe);
            }
            printf("rimossa\n");
        }
        else printf("ordini in sospeso\n");
    }
}



void ordine(recipe* book[],lot* storage[], order** exit_queue, order** wait_queue, order** exit_tail, order** wait_tail, int time)
{
    char term[N_LEN];
    int quantity, key, i;
    getc(stdin);
    i=scanf("%s %d", term, &quantity);
    i=2*i;
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
            add_to_queue(ptr_recipe, exit_queue, exit_tail, quantity, time);
        }
        else add_to_queue(ptr_recipe, wait_queue, wait_tail, quantity, time);
    }
}



void check_wait_queue(recipe* book[], lot* storage[], order** exit_queue, order** wait_queue, order** exit_tail, order** wait_tail, int time)
{
    int key;
    recipe* ptr_recipe;
    order* ptr_order=*wait_queue;
    while(ptr_order!=NULL)
    {
        key=hash(ptr_order->name);
        ptr_recipe=book[key];
        while(strcmp(ptr_recipe->name, ptr_order->name)!=0) ptr_recipe=ptr_recipe->next;
        if(check_enough_ingredients(ptr_recipe, storage, ptr_order->quantity, time)==0)
        {
            make_order(ptr_recipe, storage, ptr_order->quantity);
            ptr_order=add_to_exit_from_wait(ptr_order, exit_queue, wait_queue, exit_tail, wait_tail);
        }
        else ptr_order=ptr_order->next;
    }
}



int check_enough_ingredients(recipe* ptr_recipe, lot* storage[], int quantity, int time) //FATTO
{
    int flag=0, key;
    ingredient* ptr_ingredient=ptr_recipe->head;
    lot* ptr_lot;
    expiry* del;
    while(ptr_ingredient!=NULL && flag==0)
    {
        key=hash(ptr_ingredient->name);
        ptr_lot=storage[key];
        while(ptr_lot!=NULL && strcmp(ptr_lot->name, ptr_ingredient->name)!=0) ptr_lot=ptr_lot->next;
        if(ptr_lot==NULL) flag=1;
        else
        {
            while(ptr_lot->head!=NULL && (ptr_lot->head)->expiry_time<=time)
            {
                del=ptr_lot->head;
                ptr_lot->weight=ptr_lot->weight-del->weight;
                ptr_lot->head=ptr_lot->head->next;
                free(del);
            }
            if(ptr_lot->head==NULL)
            {
                if(ptr_lot==storage[key])
                {
                    storage[key]=storage[key]->next;
                    if(storage[key]!=NULL) storage[key]->prev=NULL;
                    free(ptr_lot);
                    ptr_lot=NULL;
                }
                else
                {
                    (ptr_lot->prev)->next=ptr_lot->next;
                    if(ptr_lot->next!=NULL) (ptr_lot->next)->prev=ptr_lot->prev;
                    free(ptr_lot);
                    ptr_lot=NULL;
                }
            }
            if(ptr_lot==NULL) flag=1;
            else if(ptr_lot->weight<quantity*ptr_ingredient->weight) flag=1;
            else ptr_ingredient=ptr_ingredient->next;
        }
    }
    return flag;
}



void make_order(recipe* ptr_recipe, lot* storage[], int quantity) //FATTO
{
    int dump, key;
    ingredient* ptr_ingredient=ptr_recipe->head;
    lot* ptr_lot;
    expiry* del;
    while(ptr_ingredient!=NULL)
    {
        key=hash(ptr_ingredient->name);
        ptr_lot=storage[key];
        while(strcmp(ptr_lot->name, ptr_ingredient->name)!=0) ptr_lot=ptr_lot->next;
        dump=quantity*ptr_ingredient->weight;
        ptr_lot->weight=ptr_lot->weight-dump;
        while(dump>0)
        {
            if((ptr_lot->head)->weight<=dump)
            {
                dump=dump-(ptr_lot->head)->weight;
                del=ptr_lot->head;
                ptr_lot->head=(ptr_lot->head)->next;
                free(del);
            }
            else
            {
                (ptr_lot->head)->weight=(ptr_lot->head)->weight-dump;
                dump=0;
            }
        }
        if(ptr_lot->head==NULL)
        {
            if(ptr_lot==storage[key])
            {
                storage[key]=storage[key]->next;
                if(storage[key]!=NULL) storage[key]->prev=NULL;
                free(ptr_lot);
            }
            else
            {
                (ptr_lot->prev)->next=ptr_lot->next;
                if(ptr_lot->next!=NULL) (ptr_lot->next)->prev=ptr_lot->prev;
                free(ptr_lot);
            }
        }
        ptr_ingredient=ptr_ingredient->next;
    }
}



void add_to_queue(recipe* ptr_recipe, order** queue, order** tail, int quantity, int time)
{
    if(*queue==NULL)
    {
        *queue=malloc(sizeof(order));
        *tail=*queue;
        (*tail)->prev=NULL;
    }
    else
    {
        (*tail)->next=malloc(sizeof(order));
        ((*tail)->next)->prev=*tail;
        (*tail)=(*tail)->next;
    }
    (*tail)->next=NULL;
    strcpy((*tail)->name, ptr_recipe->name);
    (*tail)->ordered_time=time;
    (*tail)->quantity=quantity;
    (*tail)->weight=ptr_recipe->weight;
}



order* add_to_exit_from_wait(order* ptr_wait_queue, order** exit_queue, order** wait_queue, order** exit_queue_tail, order** wait_queue_tail)
{
    order* ptr_exit_queue=*exit_queue, *next_in_wait_queue=ptr_wait_queue->next;
    if(*exit_queue==NULL)
    {
        if(ptr_wait_queue==*wait_queue)
        {
            if(*wait_queue_tail==*wait_queue) *wait_queue_tail=(*wait_queue_tail)->prev;
            *wait_queue=(*wait_queue)->next;
            if(*wait_queue!=NULL) (*wait_queue)->prev=NULL;
        }
        else
        {
            if(ptr_wait_queue==*wait_queue_tail) (*wait_queue_tail)=(*wait_queue_tail)->prev;
            (ptr_wait_queue->prev)->next=ptr_wait_queue->next;
            if(ptr_wait_queue->next!=NULL) (ptr_wait_queue->next)->prev=ptr_wait_queue->prev;
        }
        *exit_queue=ptr_wait_queue;
        *exit_queue_tail=*exit_queue;
        ptr_wait_queue->prev=NULL;
        ptr_wait_queue->next=NULL;
    }
    else
    {
        while(ptr_exit_queue->next!=NULL && ptr_exit_queue->ordered_time<ptr_wait_queue->ordered_time) ptr_exit_queue=ptr_exit_queue->next;
        if(ptr_wait_queue->ordered_time<ptr_exit_queue->ordered_time)
        {
            if(ptr_exit_queue==*exit_queue)
            {
                if(ptr_wait_queue==*wait_queue)
                {
                    if(*wait_queue_tail==*wait_queue) *wait_queue_tail=(*wait_queue_tail)->prev;
                    *wait_queue=(*wait_queue)->next;
                    if(*wait_queue!=NULL) (*wait_queue)->prev=NULL;
                }
                else
                {
                    if(ptr_wait_queue==*wait_queue_tail) (*wait_queue_tail)=(*wait_queue_tail)->prev;
                    (ptr_wait_queue->prev)->next=ptr_wait_queue->next;
                    if(ptr_wait_queue->next!=NULL) (ptr_wait_queue->next)->prev=ptr_wait_queue->prev;
                }
                ptr_wait_queue->prev=NULL;
                ptr_wait_queue->next=*exit_queue;
                (*exit_queue)->prev=ptr_wait_queue;
                *exit_queue=ptr_wait_queue;
            }
            else
            {
                if(ptr_wait_queue==*wait_queue)
                {
                    if(*wait_queue_tail==*wait_queue) *wait_queue_tail=(*wait_queue_tail)->prev;
                    *wait_queue=(*wait_queue)->next;
                    if(*wait_queue!=NULL) (*wait_queue)->prev=NULL;
                }
                else
                {
                    if(ptr_wait_queue==*wait_queue_tail) (*wait_queue_tail)=(*wait_queue_tail)->prev;
                    (ptr_wait_queue->prev)->next=ptr_wait_queue->next;
                    if(ptr_wait_queue->next!=NULL) (ptr_wait_queue->next)->prev=ptr_wait_queue->prev;
                }
                ptr_wait_queue->next=ptr_exit_queue;
                (ptr_exit_queue->prev)->next=ptr_wait_queue;
                ptr_wait_queue->prev=ptr_exit_queue->prev;
                ptr_exit_queue->prev=ptr_wait_queue;
            }
        }
        else
        {
            if(ptr_wait_queue==*wait_queue)
            {
                if(*wait_queue_tail==*wait_queue) *wait_queue_tail=(*wait_queue_tail)->prev;
                *wait_queue=(*wait_queue)->next;
                if(*wait_queue!=NULL) (*wait_queue)->prev=NULL;
            }
            else
            {
                if(ptr_wait_queue==*wait_queue_tail) (*wait_queue_tail)=(*wait_queue_tail)->prev;
                (ptr_wait_queue->prev)->next=ptr_wait_queue->next;
                if(ptr_wait_queue->next!=NULL) (ptr_wait_queue->next)->prev=ptr_wait_queue->prev;
            }
            ptr_exit_queue->next=ptr_wait_queue;
            ptr_wait_queue->prev=ptr_exit_queue;
            ptr_wait_queue->next=NULL;
            *exit_queue_tail=ptr_wait_queue;
        }
    }
    return next_in_wait_queue;
}



void restock(lot* storage[], int time) //FATTO
{
    char term[N_LEN];
    int key, weight, expiry_time;
    lot* ptr_lot;
    expiry* ptr_expiry, *temp_expiry;

    getc(stdin);
    do
    {
        key=scanf("%s %d %d", term, &weight, &expiry_time);
        key=0;
        if(expiry_time>time)
        {
            key=hash(term);
            ptr_lot=storage[key];
            if(storage[key]==NULL)
            {
                storage[key]=malloc(sizeof(lot));
                storage[key]->head=NULL;
                storage[key]->next=NULL;
                storage[key]->prev=NULL;
                strcpy(storage[key]->name, term);
                storage[key]->weight=weight;
                ptr_lot=storage[key];
            }
            else
            {
                while(ptr_lot->next!=NULL && strcmp(ptr_lot->name, term)!=0) ptr_lot=ptr_lot->next;
                if(strcmp(ptr_lot->name, term)==0) ptr_lot->weight=ptr_lot->weight+weight;
                else
                {
                    ptr_lot->next=malloc(sizeof(lot));
                    (ptr_lot->next)->prev=ptr_lot;
                    ptr_lot=ptr_lot->next;
                    ptr_lot->head=NULL;
                    ptr_lot->next=NULL;
                    strcpy(ptr_lot->name, term);
                    ptr_lot->weight=weight;
                }
            }
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



order* pickup(order* exit_queue, int capacity)
{
    order* ptr=exit_queue, *temp;
    if(ptr!=NULL)
    {
        while(ptr!=NULL && capacity>=0)
        {
            capacity=capacity-((ptr->quantity)*(ptr->weight));
            if(capacity>=0) ptr=ptr->next;
        }
        if(ptr==exit_queue) printf("camioncino vuoto\n");
        else
        {
            if(ptr!=NULL)
            {
                temp=ptr->prev;
                ptr->prev=NULL;
                temp->next=NULL;
            }
            sort_print(exit_queue);
        }
    }
    else printf("camioncino vuoto\n");
    return ptr;
}



unsigned long hash(char *term)
{
    unsigned long hash=5381;
    int c;
    while((c=*term++)) hash=((hash<<5)+hash)+c;
    return (hash%A_LEN);
}



void sort_print(order* ptr)
{
    int max;
    order* check=ptr, *ptr_max=NULL;
    
    while(ptr!=NULL)
    {
        max=-1;
        check=ptr;
        while(check!=NULL)
        {
            if(check->weight*check->quantity>max)
            {
                max=check->weight*check->quantity;
                ptr_max=check;
            }
            check=check->next;
        }
        printf("%d %s %d\n", ptr_max->ordered_time, ptr_max->name, ptr_max->quantity);
        if(ptr_max==ptr)
        {
            ptr=ptr_max->next;
            if(ptr!=NULL) ptr->prev=NULL;
            free(ptr_max);
        }
        else
        {
            (ptr_max->prev)->next=ptr_max->next;
            if(ptr_max->next!=NULL) (ptr_max->next)->prev=ptr_max->prev;
            free(ptr_max);
        }
    }
}
