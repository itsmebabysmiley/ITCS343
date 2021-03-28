#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <assert.h>
typedef int buffer_item;
#define BUFFER_SIZE 5
#define RAND_DIVISOR 1000000
#define TRUE 1
pthread_mutex_t mutex;              //the mutex lock
sem_t full, empty;                  //the semaphores
buffer_item buffer[BUFFER_SIZE];    //the buffer
int counter;                        //buffer counter
int count_w;                        //count number of wrestler entry the ring.
int count_r = 30;                   //count number of wrestler who was remove the ring
int bed = 0;                        //status to sleep or awake
pthread_t tid1, tid2;               //Thread ID
pthread_attr_t attr1, attr2;        //Set of thread attributes

//idk why rand didn't random number 0 so I put empty string at index 0.
char q_wrestler[31][20] = {"", "Stone Cold", "The Rock", "Triple H", "Shawn Michaels",
                           "Bret Hart", "The Undertaker", "John Cena", "Randy Orton",
                           "Edge", "Batista", "Brock Lesnar", "Kurt Angle", "Hulk Hogan", "Ric Flair",
                           "Yokozuna", "CM Punk", "Goldberg", "Sting", "diesel", "Roman Reigns",
                           "Drew McIntyre", "Bobby Lashley", "Seth Rollins", "Dean Ambrose",
                           "Chris Jericho", "Sheamus", "Eddie Guerrero", "AJ Styles",
                           "Big Show", "Kane"};

char moveset[3][30] = {"toss", "kick", "clotheslined"};     //fancy move
int out_w[30];                      //store wrestler who already out.
void *producer(void *pno);          //the producer thread
void *consumer(void *cno);          //the consumer thread

void initializeData()
{
    pthread_mutex_init(&mutex, NULL); //create the mutex lock
    //Create the full semaphore and initialize to 0
    sem_init(&full, 0, 0);
    //Create the empty semaphore and initialize to BUFFER_SIZE
    sem_init(&empty, 0, BUFFER_SIZE);
    pthread_attr_init(&attr1); //get the default attributes
    pthread_attr_init(&attr2);
    counter = 0;
    count_w = 0;
    //init buffer
    for (int i = 0; i < 5; i++)
    {
        int *x;
        x = &buffer[i];
        *x = -1;
    }
    //init out_w
    for (int i = 0; i < 30; i++)
    {
        int *x;
        x = &out_w[i];
        *x = -1;
    }
}
// Add an item to the buffer
int insert_item(buffer_item item)
{
    //When the buffer is not full add the item and increment the counter

    if (counter < BUFFER_SIZE)
    {
        buffer[counter] = item;         //add index of wrestler in buffer.
        int *out;
        out = &out_w[count_w];          
        *out = item;                    //also add index of wrestler to out_w.
        counter++;                      
        count_w++;                      
        if (counter == 5)               //buffer is full, set bed to sleep mode(1).
        {
            //printf("full\n");
            bed = 1;
        }
        return 0;
    }
    else
    { //Error if the buffer is full
        return -1;
    }
}
// Remove an item from the buffer
int remove_item(buffer_item *item)
{
    /* When the buffer is not empty remove the item
 and decrement the counter */

    if (counter > 0)
    {
        int move = rand() % 3;
        printf("%s is %s out of the ring from slot %d\n", q_wrestler[buffer[*item]],moveset[move],*item);
        //printf("%s was removed from ring [counter = %d]\n", q_wrestler[buffer[*item]], *item);
        buffer[*item] = -1;             //set index to -1, means that index is empty.
        counter--;
        count_r--;
        if (counter == 0)               //buffer is empty set sleep mode(0).
        {
            bed = 0;
        }
        return 0;
    }
    else
    { // Error buffer empty
        return -1;
    }
}
// Producer Thread
void *producer(void *pno)
{

    buffer_item item;
    time_t t;
    srand((unsigned) time(&t));
    while (TRUE)
    {

        if (count_w == 30)              //all wrestlers are entry the ring. terminate thread.
        {
            pthread_exit(NULL);
        }
        if (bed == 1)                   //if buffer is full sleep.
        {
            sleep(0.5);
        }
        else
        {
            item = rand() % 31;         // generate a random number(random wrestler)
            
            int temp_count = 0;
            while (temp_count <= 5)     //check ring has same wrestler.
            {
                if (item == buffer[temp_count])
                {
                    item = rand() % 31;
                    temp_count = 0;
                }
                else
                {
                    temp_count++;
                }
            }
            
            temp_count = 0;
            while (temp_count <= 31)    //check wrestler already out?
            {
                if (item == out_w[temp_count])
                {
                    item = rand() % 31;
                    temp_count = 0;
                }
                else
                {
                    temp_count++;
                }
            }
            sem_wait(&empty);           //start the lock
            pthread_mutex_lock(&mutex); 
            if (insert_item(item) == -1)        ///CRITICAL SECTION
            {
                fprintf(stderr, "ring is full\n");
            }
            else
            {
                printf("%s enters the rumble at slot %d\n", q_wrestler[item], counter);
                // printf("ring[%d] add %s (%d) [count = %d]\n", counter, q_wrestler[item], count_w, counter);
            }
            pthread_mutex_unlock(&mutex); 
            sem_post(&full);            //end the lock   
        }
    }
}
// Consumer Thread
void *consumer(void *cno)
{
    buffer_item item;
    while (TRUE)
    {
        if (count_r == 1)            //last wrestler left, terminate thread.
        {
            pthread_exit(NULL);
        }
        if (bed == 0)               //buffer is empty sleep.
        {
            sleep(0.5);
        }
        else
        {
            
            item = rand() % 5;
            while (buffer[item] == -1)  //check wrestler is already remove from the buffer(ring).
            {
                item = rand() % 5;
            }

            sem_wait(&full);            //start the lock
            pthread_mutex_lock(&mutex); 

            if (remove_item(&item) == -1)   ///CRITICAL SECTION
            {
                fprintf(stderr, "ring is empty\n");
            }
            pthread_mutex_unlock(&mutex); 
            sem_post(&empty);             //end the lock
        }
    }
}
void winner()
{
    for (int i = 0; i < 5; i++)
    {
        if (buffer[i] != -1)
        {
            printf("the winner of the rumble match is %s\n", q_wrestler[buffer[i]]);
        }
    }
}
int main(int argc, char *argv[])
{
    initializeData(); //Initialize the app
    // Create the producer threads
    pthread_create(&tid1, &attr1, producer, (void *)0);
    // Create the consumer threads
    pthread_create(&tid2, &attr2, consumer, (void *)1);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    // Exit the program
    winner();
    exit(0);
}
